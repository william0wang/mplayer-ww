/*
 * Sony Playstation Portable PMP demuxer
 * copyright (c) 2008 - 2009 William Wang <william0wang@gmail.com>
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

#define minimum_buffer_size    262144

struct pmp_signature_struct
{
    unsigned int pmpm;
    unsigned int version;
};

struct pmp_video_struct
{
    unsigned int format;
    unsigned int number_of_frames;
    unsigned int width;
    unsigned int height;
    unsigned int scale;
    unsigned int rate;
};

struct pmp_audio_struct
{
    unsigned int format;
    unsigned int number_of_streams;
    unsigned int maximum_number_of_frames;
    unsigned int scale;
    unsigned int rate;
    unsigned int stereo;
};

struct pmp_header_struct
{
    struct pmp_signature_struct signature;
    struct pmp_video_struct video;
    struct pmp_audio_struct audio;
};

struct pmp_stream_struct
{
    struct pmp_header_struct header;

    unsigned int *packet_index;
    unsigned int  packet_start;
    unsigned int  maximum_packet_size;
};

typedef struct
{
    uint8_t number_of_audio_frames;
    unsigned int  first_delay;
    unsigned int  last_delay;
    unsigned int  video_length;
} pmp_packet_header_t;

typedef struct
{
    struct pmp_stream_struct stream;
    double video_length;
    unsigned int  current_video_frame;
    unsigned int  current_audio_frame;
    unsigned int  current_audio_packet;
    unsigned int  buffer_size;
    unsigned int  first_packet;
    unsigned int  first_packet_position;
    unsigned int *packet_header;
} pmp_demuxer_t;

static int pmp_aid=0;

static void pmp_demux_safe_constructor(pmp_demuxer_t *p)
{
    p->current_video_frame = 0;
    p->current_audio_frame = 0;
    p->current_audio_packet = 0;
    p->stream.packet_index = 0;
}

static void pmp_demux_close(pmp_demuxer_t *p)
{
    if (p->stream.packet_index)
        free(p->stream.packet_index);
    
    if(p->packet_header)
        free(p->packet_header);
    
    pmp_demux_safe_constructor(p);
}

static int check_header_v1(struct pmp_header_struct header)
{
    if (header.video.format  < 0 || header.video.format  > 1)
        return 0;
    if (header.video.number_of_frames == 0) return 0;
    if (header.video.width == 0) return 0;
    if (header.video.height == 0) return 0;
    if (header.video.scale == 0) return 0;
    if (header.video.rate == 0) return 0;

    if (header.video.rate  < header.video.scale) return 0;
    if (header.video.scale > 0xffffff) return 0;

    if ((header.video.width  % 8) != 0) return 0;
    if ((header.video.height % 8) != 0) return 0;
    if (header.video.width > 720) return 0;
    if (header.video.height > 512) return 0;

    if (header.audio.format < 0 || header.audio.format > 1) return 0;
    if (header.audio.number_of_streams == 0) return 0;
    if (header.audio.maximum_number_of_frames == 0) return 0;
    if (header.audio.format == 0 && header.audio.scale != 1152) return 0;
    if (header.audio.format == 1 && header.audio.scale != 1024) return 0;
    if (header.audio.rate != 44100 && header.audio.rate != 48000) return 0;
    if (header.audio.stereo != 1) return 0;
    return 1;
}

static int open_audio(demuxer_t *demuxer, pmp_demuxer_t *p, int anum, int aid)
{
    sh_audio_t *sh_audio = new_sh_audio_aid(demuxer, anum, aid, NULL);
    demuxer->audio->sh = sh_audio;
    sh_audio->ds = demuxer->audio;
    sh_audio->wf = malloc (sizeof (WAVEFORMATEX));
    memset(sh_audio->wf, 0, sizeof (WAVEFORMATEX));
    sh_audio->samplerate = sh_audio->wf->nSamplesPerSec = (uint32_t) p->stream.header.audio.rate;
    sh_audio->wf->nAvgBytesPerSec = 16000;
    sh_audio->channels = sh_audio->wf->nChannels = 2;
    sh_audio->wf->nBlockAlign = p->stream.header.audio.scale;
    if(p->stream.header.audio.format == 1)
        sh_audio->format = sh_audio->wf->wFormatTag = 0xff;
    else
        sh_audio->format = sh_audio->wf->wFormatTag = 0x0055;
}

static demuxer_t* demux_pmp_open (demuxer_t *demuxer)
{
    stream_t *s = demuxer->stream;
    pmp_demuxer_t *p;
    p = calloc (1, sizeof (pmp_demuxer_t));
    pmp_demux_safe_constructor(p);

    stream_seek(s, s->start_pos);
    if(stream_eof(s)) return NULL;

    int header_len = sizeof(struct pmp_header_struct);

    if (stream_read(s, &p->stream.header, header_len) != header_len)
        return NULL;
    if (p->stream.header.signature.pmpm != 0x6d706d70)
        return NULL;
    if (p->stream.header.signature.version != 1)
        return NULL;
    if(!check_header_v1(p->stream.header))
        return NULL;

    int number_of_frames = sizeof(unsigned int) * p->stream.header.video.number_of_frames;

    p->stream.packet_index = malloc(number_of_frames);
    if (!p->stream.packet_index) {
        pmp_demux_close(p);
        return NULL;
    }

    if (stream_read(s, p->stream.packet_index, number_of_frames) != number_of_frames) {
        pmp_demux_close(p);
        return NULL;
    }

    p->stream.packet_start  = header_len + number_of_frames;
    p->stream.maximum_packet_size = p->stream.packet_index[0] >> 1;

    unsigned int i = 0;
    unsigned int packet_size = 0;

    for ( ; i < p->stream.header.video.number_of_frames; i++) {
        if ((p->stream.packet_index[i] >> 1) > p->stream.maximum_packet_size)
            p->stream.maximum_packet_size = p->stream.packet_index[i] >> 1;
        packet_size += p->stream.packet_index[i] >> 1;
    }

    p->buffer_size = minimum_buffer_size;
    if (p->stream.maximum_packet_size > p->buffer_size)
        p->buffer_size = p->stream.maximum_packet_size;

    p->packet_header = malloc(sizeof(unsigned int) * (p->stream.header.audio.maximum_number_of_frames * p->stream.header.audio.number_of_streams + 3));
    if (!p->packet_header) {
        pmp_demux_close(p);
        return NULL;
    }

    p->first_packet = 0;
    p->first_packet_position = p->stream.packet_start;

    double video_frames = p->stream.header.video.number_of_frames;
    float video_scale = p->stream.header.video.scale;
    float video_rate = p->stream.header.video.rate;

    p->video_length = (video_frames * video_scale) / video_rate;

    demuxer->priv = p;

    sh_video_t *sh_video = new_sh_video(demuxer, 0);

    if (demuxer->video->id == -1) demuxer->video->id = 0;
    if (demuxer->video->id == 0)
    demuxer->video->sh = sh_video;
    sh_video->ds = demuxer->video;

    sh_video->disp_w = p->stream.header.video.width;
    sh_video->disp_h = p->stream.header.video.height;
    if(p->stream.header.video.format == 0)
        sh_video->format = mmioFOURCC('m', 'p', '4', 'v');
    else
        sh_video->format = mmioFOURCC('H', '2', '6', '4');

    sh_video->frametime = video_scale / video_rate;
    sh_video->fps = 1.0 / sh_video->frametime;

    sh_video->bih = calloc (1, sizeof (BITMAPINFOHEADER));
    sh_video->bih->biSize = sizeof (BITMAPINFOHEADER);
    sh_video->bih->biWidth = p->stream.header.video.width;
    sh_video->bih->biHeight = p->stream.header.video.height;
    sh_video->bih->biBitCount = 24;
    sh_video->bih->biSizeImage = sh_video->bih->biWidth * sh_video->bih->biHeight * sh_video->bih->biBitCount/8;
    sh_video->bih->biCompression = sh_video->format;
    sh_video->aspect = (float)sh_video->bih->biWidth / (float)sh_video->bih->biHeight;

    if (p->stream.header.audio.number_of_streams > 0)
    {
        int i = 0;
        while(i < p->stream.header.audio.number_of_streams) {
            open_audio(demuxer, p, i, i);
            ++i;
        }
        demuxer->audio->id = 0;
        demuxer->audio->sh = demuxer->a_streams[0];
    }
    return demuxer;
}

static void demux_pmp_close (demuxer_t *demuxer)
{
    pmp_demuxer_t *pmp_d = (pmp_demuxer_t *) demuxer->priv;
    if(pmp_d)
    {
        pmp_demux_close(pmp_d);
        free(pmp_d);
    }
}

static unsigned int get_packet_position(pmp_demuxer_t *pmp_d , int audio)
{
    unsigned int first_packet  = pmp_d->first_packet;
    unsigned int first_packet_position = pmp_d->first_packet_position;
    unsigned int packet = 0;

    if(audio)
        packet = pmp_d->current_audio_frame;
    else
        packet = pmp_d->current_video_frame;

    if (packet >= first_packet) {
        while (first_packet != packet) {
            first_packet_position += pmp_d->stream.packet_index[first_packet] >> 1;
            first_packet ++;
        }
    } else {
        while (first_packet != packet) {
            first_packet --;
            first_packet_position -= pmp_d->stream.packet_index[first_packet] >> 1;
        }
    }
    return first_packet_position;
}

static int demux_pmp_fill_buffer(demuxer_t *demuxer, demux_stream_t *ds)
{
    pmp_packet_header_t header;
    pmp_demuxer_t *p = demuxer->priv;
    sh_video_t* sh_video = demuxer->video->sh;

    /* Video */
    if (ds == demuxer->video) {
        if(p->current_video_frame > p->stream.header.video.number_of_frames)
            return 0;

        stream_t *s = demuxer->stream;
        unsigned int framepos = get_packet_position(p , 0);
        stream_seek(s, framepos);
        stream_read(s, &header.number_of_audio_frames, 1);
        stream_read(s, &header.first_delay, 4);
        stream_read(s, &header.last_delay, 4);
        stream_read(s, &header.video_length, 4);
        unsigned int videopos = framepos + 1 + 4 + 4 + 4 + 4*header.number_of_audio_frames* p->stream.header.audio.number_of_streams;
        unsigned int len = header.video_length;
        stream_seek(s, videopos);
        demux_packet_t *dp = new_demux_packet(len);
        len = stream_read(s, dp->buffer, len);
        resize_demux_packet(dp, len);
        dp->pts = (float)(p->current_video_frame) / sh_video->fps;
        dp->pos = demuxer->filepos;
        dp->flags = (p->stream.packet_index[p->current_video_frame] & 1 )? 1: 0;
        ds_add_packet(ds, dp);
        p->current_video_frame++;
    }

    /* Audio */
    if (ds == demuxer->audio) {
        if(p->current_audio_frame > p->stream.header.video.number_of_frames)
            return 0;

        sh_audio_t *sh_audio = ds->sh;
        stream_t *s = demuxer->stream;
        unsigned int framepos = get_packet_position(p , 1);
        stream_seek(s, framepos);
        stream_read(s, &header.number_of_audio_frames, 1);
        stream_read(s, &header.first_delay, 4);
        stream_read(s, &header.last_delay, 4);
        stream_read(s, &header.video_length, 4);

        unsigned int audiopos = framepos + 1 + 4 + 4 + 4 + 4*header.number_of_audio_frames* p->stream.header.audio.number_of_streams + header.video_length;
        unsigned int len = 0;

        if(p->stream.header.audio.format == 1) {
            int padding = 7;
            uint8_t aac_hdr[7] = {0xff, 0xf9, 0, 0x80, 0, 0, 0xfc};
            unsigned int sr_index = (p->stream.header.audio.rate == 48000) ? 3 : 4;
            aac_hdr[2] = 0x40 | (sr_index<<2);
            stream_read(s, &len, 4);
            if(pmp_aid > 0) {
                audiopos += len;
                int j = 1;
                while(j < header.number_of_audio_frames*pmp_aid) {
                    stream_read(s, &len, 4);
                    audiopos += len;
                    ++j;
                }
                stream_read(s, &len, 4);
            }
            aac_hdr[4] = (len >> 3) + 1;
            aac_hdr[5] = (len << 5) - 1;
            stream_seek(s, audiopos);
            demux_packet_t *dp = new_demux_packet(len + padding);
            memcpy(dp->buffer, aac_hdr, padding);
            len = stream_read(s, &dp->buffer[padding], len);
            audiopos += len;
            resize_demux_packet(dp, len + padding);
            dp->pts = (float)p->current_audio_frame / sh_video->fps;
            dp->pos = demuxer->filepos;
            ds_add_packet(ds, dp);
            int i= 1;
            unsigned int audio_length;
            while(i < header.number_of_audio_frames) {
                stream_seek(s, framepos + 1 + 4 + 4 + 4 + 4*i + 4*pmp_aid*header.number_of_audio_frames);
                stream_read(s, &audio_length, 4);
                aac_hdr[4] = (audio_length >> 3) + 1;
                aac_hdr[5] = (audio_length << 5) - 1;
                stream_seek(s, audiopos);
                demux_packet_t *dp = new_demux_packet(audio_length + padding);
                memcpy(dp->buffer, aac_hdr, padding);
                audio_length = stream_read(s, &dp->buffer[padding], audio_length);
                audiopos += audio_length;
                resize_demux_packet(dp, audio_length + padding);
                dp->pts = (float)p->current_audio_frame / sh_video->fps +
                    (float)(i * p->stream.header.audio.scale)/(float)p->stream.header.audio.rate;
                dp->pos = demuxer->filepos;
                ds_add_packet(ds, dp);
                ++i;
            }
        } else {
            int i = 0;
            unsigned int audio_length;
            while(i < header.number_of_audio_frames) {
                i++;
                stream_read(s, &audio_length, 4);
                len += audio_length;
            }
            stream_seek(s, audiopos);
            demux_packet_t *dp = new_demux_packet(len);
            len = stream_read(s, dp->buffer, len);
            resize_demux_packet(dp, len);
            dp->pts = (float)p->current_audio_frame / sh_video->fps;
            ds_add_packet(ds, dp);
        }
        p->current_audio_frame++;
    }

    return 1;
}

static int demux_pmp_control(demuxer_t *demuxer, int cmd, void *arg)
{
    pmp_demuxer_t *pmp_d = demuxer->priv;

    switch(cmd)
    {
    case DEMUXER_CTRL_GET_TIME_LENGTH:
        *((double *)arg) = pmp_d->video_length;
        return DEMUXER_CTRL_OK;
    case DEMUXER_CTRL_GET_PERCENT_POS:
        *((int *)arg) = pmp_d->current_video_frame * 100 / pmp_d->stream.header.video.number_of_frames;
        return DEMUXER_CTRL_OK;
    case DEMUXER_CTRL_SWITCH_AUDIO:
        if(pmp_d->stream.header.audio.number_of_streams < 2)
            return DEMUXER_CTRL_NOTIMPL;
        int id = *(int*)arg;
        if(id < 0) {
            ++pmp_aid;
            pmp_aid %= pmp_d->stream.header.audio.number_of_streams;
        } else {
            pmp_aid = id % pmp_d->stream.header.audio.number_of_streams;
        }
        demuxer->audio->id = pmp_aid;
        *(int*)arg = demuxer->audio->id;
        return DEMUXER_CTRL_OK;
    default:
        return DEMUXER_CTRL_NOTIMPL;
    }
}

static void demux_pmp_seek(demuxer_t *demuxer,float rel_seek_secs,float audio_delay,int flags)
{
    pmp_demuxer_t *pmp_d = demuxer->priv;
    sh_video_t *sh_video = demuxer->video->sh;

    if(!sh_video)
        return;

    double video_pos = (double)pmp_d->current_video_frame / sh_video->fps;
    double duration = (double)pmp_d->stream.header.video.number_of_frames / sh_video->fps;

    if (flags&SEEK_ABSOLUTE) video_pos=0;
    if (flags&SEEK_FACTOR) rel_seek_secs *= duration;

    video_pos += rel_seek_secs;
    if (video_pos < 0) video_pos = 0;

    pmp_d->current_video_frame = FFMIN(video_pos * sh_video->fps, pmp_d->stream.header.video.number_of_frames);
    while(!(pmp_d->stream.packet_index[pmp_d->current_video_frame] & 1)) {
        if(pmp_d->current_video_frame >= pmp_d->current_audio_frame)
            ++pmp_d->current_video_frame;
        else
            --pmp_d->current_video_frame;
        if(pmp_d->current_video_frame <= 0 || pmp_d->current_video_frame >= pmp_d->stream.header.video.number_of_frames)
            break;
    }

    pmp_d->current_audio_frame = pmp_d->current_video_frame;
    sh_video->num_frames_decoded = pmp_d->current_video_frame;

}

static int pmp_check_file(demuxer_t *demuxer)
{
    mp_msg(MSGT_DEMUX, MSGL_V, "PMP: Checking for PSP media format\n");

    if (!demuxer->filename) return 0;

    stream_t *s = demuxer->stream;

    stream_seek(s, s->start_pos);
    if(stream_eof(s)) return NULL;

    struct pmp_header_struct header;

    int header_len = sizeof(struct pmp_header_struct);

    if (stream_read(s, &header, header_len) != header_len)
        return 0;

    if (header.signature.pmpm != 0x6d706d70)
        return 0;

    if (header.signature.version != 1)
        return 0;

    if(!check_header_v1(header))
        return 0;

    return DEMUXER_TYPE_PMP;
}

const demuxer_desc_t demuxer_desc_pmp = {
  "PMP demuxer",
  "pmp",
  "PSP media",
  "William Wang",
  "",
  DEMUXER_TYPE_PMP,
  1,
  pmp_check_file,
  demux_pmp_fill_buffer,
  demux_pmp_open,
  demux_pmp_close,
  demux_pmp_seek,
  demux_pmp_control
};
