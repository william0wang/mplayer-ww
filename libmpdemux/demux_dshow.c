/*
 * DirectShow media demuxer for win32
 * copyright (c) 2010 William Wang <william0wang@gmail.com>
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "ebml.h"

#include "mp_msg.h"
#include "help_mp.h"

#include "sub/ass_mp.h"
#include "sub/vobsub.h"
#include "sub/subreader.h"
#include "sub/sub.h"

#include "libavutil/common.h"

#include "libavutil/lzo.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/avstring.h"

#include "loader/com.h"
#include "loader/wine/winbase.h"

#ifdef WIN32_LOADER
#include "loader/ldt_keeper.h"
#endif

#define DSN_API_VERSION 7

typedef struct
{
	uint32_t width : 12;
	uint32_t height : 12;
	uint32_t aspectX : 8;
	uint32_t aspectY : 8;
	uint32_t avgtimeperframe : 20;
	uint32_t reserved : 4;
	uint32_t haveVideo;
	char *videoDecoder;
} dshow_video_info_t;

typedef struct
{
	uint32_t nSamplesPerSec;
	uint32_t nChannels;
	uint32_t wBitsPerSample;
	uint32_t wFormatTag;
	uint32_t reserved;
	uint32_t haveAudio;
	char *audioDecoder;
	char *demuxer;
} dshow_audio_info_t;

typedef struct {
	unsigned char *pData;
	int iLen;
	int64_t i_pts;
} dshow_pkg_t;

typedef int (__stdcall *GrabSampleCallbackRoutine)
(unsigned char *pData, int32_t iLen, int64_t i64TimeStamp);

typedef void* (__stdcall *TInitDShowGraphFromFile)
(const char *szFileName,
 GUID MediaType,
 uint32_t dwVideoID,
 uint32_t dwAudioID,
 GrabSampleCallbackRoutine pVideoCallback,
 GrabSampleCallbackRoutine pAudioCallback,
 dshow_video_info_t *pVideoInfo,
 dshow_audio_info_t *pAudioInfo);

typedef int (__stdcall *TGraphOperate)(void *pdgi);
typedef double (__stdcall *TGetGraphDuration)(void *pdgi);
typedef int (__stdcall *TSeekGraph)(void *pdgi, int64_t timestamp);
typedef unsigned int (WINAPI *funcDSGetApiVersion)(void);

#ifdef WIN32_LOADER
    ldt_fs_t* ldt_fs = NULL;
#endif

extern const const GUID MEDIASUBTYPE_YV12;

TGraphOperate StartGraph, StopGraph, DestroyGraph;
TGetGraphDuration GetGraphDuration;
TSeekGraph SeekGraph;
TInitDShowGraphFromFile InitDShowGraphFromFile=NULL;
funcDSGetApiVersion DSGetApiVersion;

int force_dshow_demux = 0;
int open_with_dshow_demux = 0;
static HMODULE hLib = NULL;
static HANDLE hWaitVSend = 0, hWaitASend = 0;
static HANDLE hWaitVDec = 0, hWaitADec = 0;
static dshow_pkg_t g_vpkg;
static dshow_pkg_t g_apkg;
static int b_stopped = 0;
static double duration;
static double stream_pos;
static double last_vpts;
static int b_firstframe = 1;
static int b_firstaframe = 1;
static int b_vfinished = 0;
static int b_afinished = 0;
static int wait_time = 50;
static void *g_pdgi = 0;
static unsigned char *apkg_buf = NULL;
static int apkg_buf_size = 0;
static dshow_video_info_t VideoInfo;
static dshow_audio_info_t AudioInfo;
extern char *filename;

static int __stdcall VideoCallBackProc(unsigned char *pData, int32_t iLen, int64_t i64TimeStamp)
{
	if(b_stopped)
		return 1;

	if(!pData && !iLen && !i64TimeStamp) {
		b_vfinished = 1;
		SetEvent(hWaitVDec);
		return 1;
	}

	if (b_firstframe)
		b_firstframe = 0;
	else
		WaitForSingleObject(hWaitVSend,INFINITE);

	if (pData) {
		g_vpkg.i_pts = i64TimeStamp;
		g_vpkg.pData = pData;
		g_vpkg.iLen = (int)iLen;
	} else {
		g_vpkg.pData = NULL;
		g_vpkg.iLen = 0;
	}

	SetEvent(hWaitVDec);
	return 1;
}

static int __stdcall AudioCallBackProc(unsigned char *pData, int32_t iLen, int64_t i64TimeStamp)
{
	if(b_stopped)
		return 1;

	if(!pData && !iLen && !i64TimeStamp) {
		b_afinished = 1;
		SetEvent(hWaitADec);
		return 1;
	}

	if (b_firstaframe)
		b_firstaframe = 0;
	else
		WaitForSingleObject(hWaitASend,INFINITE);

	if (pData) {
		g_apkg.i_pts = i64TimeStamp;
		g_apkg.iLen = (int)iLen;
		if(!apkg_buf)
			apkg_buf = (unsigned char *)malloc(g_apkg.iLen+8);
		else if(apkg_buf_size < g_apkg.iLen)
			apkg_buf = (unsigned char *)realloc(apkg_buf, g_apkg.iLen+8);
		apkg_buf_size = g_apkg.iLen;
		memcpy(apkg_buf, pData, g_apkg.iLen);
		g_apkg.pData = apkg_buf;
	} else {
		g_apkg.pData = NULL;
		g_apkg.iLen = 0;
	}

	SetEvent(hWaitADec);
	return 1;
}

static demuxer_t* demux_dshow_open (demuxer_t *demuxer)
{
	sh_video_t *sh_video;
	sh_audio_t *sh_audio;
	stream_pos = 0;
	last_vpts = 0;
	b_stopped = 0;
	b_vfinished = 0;
	b_afinished = 0;
	b_firstframe = 1;
	b_firstaframe = 1;
	g_vpkg.pData = NULL;
	g_apkg.pData = NULL;
	apkg_buf = NULL;
	apkg_buf_size = 0;

	if(AudioInfo.demuxer)
		demux_info_add(demuxer, "DShow Demuxer", AudioInfo.demuxer);

	if(VideoInfo.haveVideo) {
		if (VideoInfo.avgtimeperframe < 10000)
			VideoInfo.avgtimeperframe = 400000;

		sh_video = new_sh_video(demuxer, 0);

		if (demuxer->video->id == -1) demuxer->video->id = 0;
		if (demuxer->video->id == 0)
		demuxer->video->sh = sh_video;
		sh_video->ds = demuxer->video;

		sh_video->disp_w = VideoInfo.width;
		sh_video->disp_h = VideoInfo.height;
		sh_video->format = mmioFOURCC('Y', 'V', '1', '2');

		sh_video->frametime = (float)(VideoInfo.avgtimeperframe/1E7);
		sh_video->fps = 1.0 / sh_video->frametime;

		sh_video->bih = calloc (1, sizeof (BITMAPINFOHEADER));
		sh_video->bih->biSize = sizeof (BITMAPINFOHEADER);
		sh_video->bih->biWidth = VideoInfo.width;
		sh_video->bih->biHeight = VideoInfo.height;
		sh_video->bih->biBitCount = 12;
		sh_video->bih->biSizeImage = sh_video->bih->biWidth * sh_video->bih->biHeight * sh_video->bih->biBitCount/8;
		sh_video->bih->biCompression = sh_video->format;
		if(VideoInfo.videoDecoder)
			demux_info_add(demuxer, "DShow Video", VideoInfo.videoDecoder);

		if(VideoInfo.aspectX > 1 && VideoInfo.aspectY > 1)
			sh_video->aspect = (float)VideoInfo.width*(float)VideoInfo.aspectX/((float)VideoInfo.height*(float)VideoInfo.aspectY);
	}

	if(AudioInfo.haveAudio) {
		sh_audio = new_sh_audio_aid(demuxer, 0, 0, NULL);
		demuxer->audio->id = 0;
		demuxer->audio->sh = sh_audio;
		sh_audio->ds = demuxer->audio;
		sh_audio->wf = malloc (sizeof (WAVEFORMATEX));
		memset(sh_audio->wf, 0, sizeof (WAVEFORMATEX));

		if(AudioInfo.audioDecoder) {
			if(AudioInfo.wFormatTag == 0xfffe && !strcmp(AudioInfo.audioDecoder, "Microsoft DTV-DVD Audio Decoder"))
				AudioInfo.wFormatTag = 0x3;
			demux_info_add(demuxer, "DShow Audio", AudioInfo.audioDecoder);
		}
		sh_audio->format = sh_audio->wf->wFormatTag = AudioInfo.wFormatTag;
		sh_audio->channels = sh_audio->wf->nChannels = AudioInfo.nChannels;
		sh_audio->samplerate = sh_audio->wf->nSamplesPerSec = AudioInfo.nSamplesPerSec;
		sh_audio->wf->wBitsPerSample = AudioInfo.wBitsPerSample;
		sh_audio->wf->nBlockAlign = sh_audio->channels * sh_audio->wf->wBitsPerSample / 8;
		sh_audio->wf->nAvgBytesPerSec = sh_audio->wf->nBlockAlign * sh_audio->samplerate;
        sh_audio->i_bps = sh_audio->wf->nAvgBytesPerSec;

	}

	duration = GetGraphDuration(g_pdgi);

	hWaitVSend = CreateEventA(NULL, FALSE, FALSE, NULL);
	hWaitASend = CreateEventA(NULL, FALSE, FALSE, NULL);
	hWaitVDec = CreateEventA(NULL, FALSE, FALSE, NULL);
	hWaitADec = CreateEventA(NULL, FALSE, FALSE, NULL);

	StartGraph(g_pdgi);

    return demuxer;
}

static int init_dshow()
{
	char *ext;
	wait_time = 50;
	ext = strrchr(filename, '.');
	if(ext) {
		if(!stricmp(ext, ".bik") || !stricmp(ext, ".avs"))
			return 0;
		if(!stricmp(ext, ".wmv") || !stricmp(ext, ".asf"))
			wait_time = 1;
	}

#ifdef WIN32_LOADER
	ldt_fs = Setup_LDT_Keeper();
#endif
	hLib = LoadLibraryA("dshownative.dll");
	if(!hLib) {
    	mp_msg(MSGT_DEMUX, MSGL_V, "dshow demuxer: failed to load dshownative.dll\n");
		return 0;
	}
	InitDShowGraphFromFile = (TInitDShowGraphFromFile)GetProcAddress(hLib,"InitDShowGraphFromFile");
	StartGraph = (TGraphOperate)GetProcAddress(hLib,"StartGraph");
	StopGraph = (TGraphOperate)GetProcAddress(hLib,"StopGraph");
	DestroyGraph = (TGraphOperate)GetProcAddress(hLib,"DestroyGraph");
	SeekGraph = (TSeekGraph)GetProcAddress(hLib,"SeekGraph");
	GetGraphDuration = (TGetGraphDuration)GetProcAddress(hLib,"GetGraphDuration");
	DSGetApiVersion = (funcDSGetApiVersion) GetProcAddress(hLib, "DSGetApiVersion");

	if(!InitDShowGraphFromFile || !StartGraph || !StopGraph || !DSGetApiVersion ||
			!DestroyGraph || !SeekGraph || !GetGraphDuration) {
		FreeLibrary(hLib);
		hLib = NULL;
		return 0;
	}

    if (DSGetApiVersion() != DSN_API_VERSION) {
		FreeLibrary(hLib);
		hLib = NULL;
        mp_msg(MSGT_DECVIDEO, MSGL_V, "dshow demuxer: Incompatible API version\n");
        return 0;
    }

	if (!(g_pdgi = InitDShowGraphFromFile(filename, MEDIASUBTYPE_YV12, 0, 0, VideoCallBackProc,
											AudioCallBackProc, &VideoInfo, &AudioInfo))) {
		FreeLibrary(hLib);
		hLib = NULL;
    	mp_msg(MSGT_DEMUX, MSGL_V, "dshow demuxer: failed to render %s\n", filename);
		return 0;
	}

	return 1;
}

static void demux_dshow_close (demuxer_t *demuxer)
{
 	b_stopped = 1;
	SetEvent(hWaitVSend);
	SetEvent(hWaitASend);
	if(g_pdgi) {
		StopGraph(g_pdgi);
		DestroyGraph(g_pdgi);
	}
	g_pdgi = NULL;
	SetEvent(hWaitVDec);
	SetEvent(hWaitADec);
	CloseHandle(hWaitVSend);
	CloseHandle(hWaitASend);
	CloseHandle(hWaitVDec);
	CloseHandle(hWaitADec);
	if(apkg_buf)
		free(apkg_buf);
	apkg_buf = NULL;
	if(hLib)
		FreeLibrary(hLib);
	hLib = NULL;
#ifdef WIN32_LOADER
	Restore_LDT_Keeper(ldt_fs);
#endif
	return;
}

static int demux_dshow_fill_buffer(demuxer_t *demuxer, demux_stream_t *ds)
{
	demux_packet_t *dp;
	sh_video_t *sh_video;
	float new_ftime, new_fps, diff;

	if(b_stopped) return 0;

    /* Video */
    if (ds == demuxer->video) {
		if(b_vfinished) return 0;
		if(WaitForSingleObject(hWaitVDec, 5000) == WAIT_TIMEOUT)
			return 0;
		if (!g_vpkg.pData || g_vpkg.iLen < 1) {
			SetEvent(hWaitVSend);
			return 0;
		}
		sh_video = demuxer->video->sh;
        dp = new_demux_packet(g_vpkg.iLen);
        dp->pts = (float)(g_vpkg.i_pts / 1E7);
        memcpy(dp->buffer, g_vpkg.pData, g_vpkg.iLen);
		SetEvent(hWaitVSend);
		new_ftime = dp->pts - last_vpts;
		if(last_vpts > 0 && new_ftime > 0) {
			new_fps = 1.0/ new_ftime;
			diff = new_fps - sh_video->fps;
			if(diff > 0.9 || diff < -0.9) {
				sh_video->frametime = new_ftime;
				sh_video->fps = new_fps;
			}
		}
		if(last_vpts < 0)
			last_vpts = 0;
		else
			stream_pos = last_vpts = dp->pts;
        ds_add_packet(ds, dp);
    }

    /* Audio */
	if (ds == demuxer->audio) {
		if(b_afinished) return 0;
		if(WaitForSingleObject(hWaitADec, wait_time) == WAIT_TIMEOUT)
			return 0;
		if (!g_apkg.pData || g_apkg.iLen < 1) {
			SetEvent(hWaitASend);
			return 0;
		}
        dp = new_demux_packet(g_apkg.iLen);
        dp->pts = (float)(g_apkg.i_pts / 1E7);
        memcpy(dp->buffer, g_apkg.pData, g_apkg.iLen);
		SetEvent(hWaitASend);
		if(!VideoInfo.haveVideo)
			stream_pos = dp->pts;
        ds_add_packet(ds, dp);
	}

    return 1;
}

static int demux_dshow_control(demuxer_t *demuxer, int cmd, void *arg)
{
    switch(cmd)
    {
    case DEMUXER_CTRL_GET_TIME_LENGTH:
        *((double *)arg) = duration;
        return DEMUXER_CTRL_OK;
    case DEMUXER_CTRL_GET_PERCENT_POS:
        *((int *)arg) = (int)(stream_pos*100/duration);
        return DEMUXER_CTRL_OK;
    default:
        return DEMUXER_CTRL_NOTIMPL;
    }
}

static void demux_dshow_seek(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags)
{
	int64_t time64;
    if(!demuxer->video->sh)
        return;
 	b_stopped = 1;
	last_vpts = -1;

    if (flags&SEEK_ABSOLUTE) stream_pos=0;
    if (flags&SEEK_FACTOR) rel_seek_secs *= duration;

    stream_pos += rel_seek_secs;
    if (stream_pos < 0) stream_pos = 0;

	time64 =  (int64_t)(stream_pos*1E7);
	SetEvent(hWaitVSend);
	SetEvent(hWaitASend);
	SetEvent(hWaitVDec);
	SetEvent(hWaitADec);
	SeekGraph(g_pdgi, time64);
	b_stopped = 0;
}

static int dshow_check_file(demuxer_t *demuxer)
{
    if (!demuxer->filename)
		return 0;

    mp_msg(MSGT_DEMUX, MSGL_V, "dshow demuxer: Checking for DirectShow media\n");

	if(!init_dshow())
		return 0;

    return DEMUXER_TYPE_DSHOW;
}

static int dshow_check_file_preferred(demuxer_t *demuxer)
{
	if(!force_dshow_demux && !open_with_dshow_demux)
		return 0;

	open_with_dshow_demux = 0;

    if (!demuxer->filename)
		return 0;

    mp_msg(MSGT_DEMUX, MSGL_V, "dshow pref demuxer: Checking for DirectShow media\n");

	if(!init_dshow())
		return 0;

    return DEMUXER_TYPE_DSHOW_PREFERRED;
}

const demuxer_desc_t demuxer_desc_dshow = {
  "DirectShow demuxer",
  "dshow",
  "DirectShow media",
  "William Wang",
  "Requires binary dll",
  DEMUXER_TYPE_DSHOW,
  0,
  dshow_check_file,
  demux_dshow_fill_buffer,
  demux_dshow_open,
  demux_dshow_close,
  demux_dshow_seek,
  demux_dshow_control
};

const demuxer_desc_t demuxer_desc_dshow_preferred = {
  "DirectShow preferred demuxer",
  "dshowpref",
  "DirectShow media",
  "William Wang",
  "Requires binary dll",
  DEMUXER_TYPE_DSHOW_PREFERRED,
  1,
  dshow_check_file_preferred,
  demux_dshow_fill_buffer,
  demux_dshow_open,
  demux_dshow_close,
  demux_dshow_seek,
  demux_dshow_control
};
