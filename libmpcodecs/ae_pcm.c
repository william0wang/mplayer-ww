/*
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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "m_option.h"
#include "mp_msg.h"
#include "libmpdemux/aviheader.h"
#include "libaf/af_format.h"
#include "libaf/reorder_ch.h"
#include "libmpdemux/ms_hdr.h"
#include "stream/stream.h"
#include "libmpdemux/muxer.h"
#include "ae_pcm.h"
#include <io.h>

static int ao_pipe = 0;
static int pipe_only = 0;

#define WAV_ID_RIFF 0x46464952 /* "RIFF" */
#define WAV_ID_WAVE 0x45564157 /* "WAVE" */
#define WAV_ID_FMT  0x20746d66 /* "fmt " */
#define WAV_ID_DATA 0x61746164 /* "data" */
#define WAV_ID_PCM  0x0001

struct WaveHeader
{
	uint32_t riff;
	uint32_t file_length;
	uint32_t wave;
	uint32_t fmt;
	uint32_t fmt_length;
	uint16_t fmt_tag;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t block_align;
	uint16_t bits;
	uint32_t data;
	uint32_t data_length;
};

m_option_t pcmopts_conf[] = {
	{"pipe", &ao_pipe, CONF_TYPE_INT, 0, 0, 0, NULL},
	{NULL, NULL, 0, 0, 0, 0, NULL}
};

static int bind_pcm(audio_encoder_t *encoder, muxer_stream_t *mux_a)
{
	mux_a->h.dwScale=1;
	mux_a->h.dwRate=encoder->params.sample_rate;
	mux_a->wf=malloc(sizeof(*mux_a->wf));
	mux_a->wf->wFormatTag=0x1; // PCM
	mux_a->wf->nChannels=encoder->params.channels;
	mux_a->h.dwSampleSize=2*mux_a->wf->nChannels;
	mux_a->wf->nBlockAlign=mux_a->h.dwSampleSize;
	mux_a->wf->nSamplesPerSec=mux_a->h.dwRate;
	mux_a->wf->nAvgBytesPerSec=mux_a->h.dwSampleSize*mux_a->wf->nSamplesPerSec;
	mux_a->wf->wBitsPerSample=16;
	mux_a->wf->cbSize=0; // FIXME for l3codeca.acm

	encoder->input_format = (mux_a->wf->wBitsPerSample==8) ? AF_FORMAT_U8 : AF_FORMAT_S16_LE;
	encoder->min_buffer_size = 16384;
	encoder->max_buffer_size = mux_a->wf->nAvgBytesPerSec;

	return 1;
}

static int encode_pcm(audio_encoder_t *encoder, uint8_t *dest, void *src, int nsamples, int max_size)
{
	max_size = FFMIN(nsamples, max_size);
	if (encoder->params.channels == 5 || encoder->params.channels == 6 ||
		    encoder->params.channels == 8) {
		max_size -= max_size % (encoder->params.channels * 2);
		reorder_channel_copy_nch(src, AF_CHANNEL_LAYOUT_MPLAYER_DEFAULT,
		                         dest, AF_CHANNEL_LAYOUT_WAVEEX_DEFAULT,
		                         encoder->params.channels,
		                         max_size / 2, 2);
	}
	else
	memcpy(dest, src, max_size);

	if (ao_pipe) {
		max_size = write(ao_pipe, dest, max_size);
	}
	return max_size;
}

static int set_decoded_len(audio_encoder_t *encoder, int len)
{
	return len;
}

static int close_pcm(audio_encoder_t *encoder)
{
	if (ao_pipe) {
		_commit(ao_pipe);
		close(ao_pipe);
		ao_pipe = 0;
	}
	return 1;
}

static int get_frame_size(audio_encoder_t *encoder)
{
	return 0;
}

int mpae_init_pcm(audio_encoder_t *encoder)
{
	encoder->params.samples_per_frame = encoder->params.sample_rate;
	encoder->params.bitrate = encoder->params.sample_rate * encoder->params.channels * 2 * 8;

	encoder->decode_buffer_size = encoder->params.bitrate / 8;
	encoder->bind = bind_pcm;
	encoder->get_frame_size = get_frame_size;
	encoder->set_decoded_len = set_decoded_len;
	encoder->encode = encode_pcm;
	encoder->close = close_pcm;

	if (ao_pipe) {
		struct WaveHeader wavhdr;
		wavhdr.riff = le2me_32(WAV_ID_RIFF);
		wavhdr.wave = le2me_32(WAV_ID_WAVE);
		wavhdr.fmt = le2me_32(WAV_ID_FMT);
		wavhdr.fmt_length = le2me_32(16);
		wavhdr.fmt_tag = le2me_16(WAV_ID_PCM);
		wavhdr.channels = le2me_16(encoder->params.channels);
		wavhdr.sample_rate = le2me_32(encoder->params.sample_rate);
		wavhdr.bits = le2me_16(16);
		wavhdr.block_align = le2me_16(wavhdr.channels * (16 / 8));
		wavhdr.bytes_per_second = le2me_32(encoder->params.sample_rate * wavhdr.block_align);
		
		wavhdr.data = le2me_32(WAV_ID_DATA);
		wavhdr.data_length=le2me_32(0x7ffff000);
		wavhdr.file_length = wavhdr.data_length + sizeof(wavhdr) - 8;

		write(ao_pipe, &wavhdr, sizeof(wavhdr));
		mp_msg(MSGT_MENCODER, MSGL_INFO, "Audio data will be piped out through fd %d\n", ao_pipe); 
	}
	return 1;
}
