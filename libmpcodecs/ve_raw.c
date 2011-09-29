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
#include <string.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "codec-cfg.h"
#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"

#include "stream/stream.h"
#include "libmpdemux/muxer.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

#include "m_option.h"
#include "libavutil/rational.h"

//===========================================================================//

struct vf_priv_s {
    muxer_stream_t* mux;
};
#define mux_v (vf->priv->mux)

typedef struct {
	int format;
	int width;
	int height;
	int interlace;
	int fps_num;
	int fps_den;
	int pixelaspect_num;
	int pixelaspect_den;
	int frame_count;
} YUV_INFO;

static int pipefd;
static int framesize = 0;

m_option_t rawvidopts_conf[] =
{
	/* Standard things mencoder should be able to treat directly */
	{"pipe", &pipefd, CONF_TYPE_INT, 0, 0, 0, NULL},
	{NULL, 0, 0, 0, 0, 0, NULL}
};

static int set_format(struct vf_instance *vf, unsigned int fmt) {
    if (!force_fourcc)
        mux_v->bih->biCompression = fmt;

    mux_v->bih->biPlanes = 1;
    if (IMGFMT_IS_RGB(fmt)) {
	if (IMGFMT_RGB_DEPTH(fmt) < 8 && !(fmt&128))
	    mux_v->bih->biBitCount = IMGFMT_RGB_DEPTH(fmt);
	else
	    mux_v->bih->biBitCount = (IMGFMT_RGB_DEPTH(fmt)+7)&(~7);
	return 1;
    }
    if (IMGFMT_IS_BGR(fmt)) {
	if (IMGFMT_BGR_DEPTH(fmt) < 8 && !(fmt&128))
	    mux_v->bih->biBitCount = IMGFMT_BGR_DEPTH(fmt);
	else
	    mux_v->bih->biBitCount = (IMGFMT_BGR_DEPTH(fmt)+7)&(~7);
	return 1;
    }
    switch (fmt) {
    case IMGFMT_NV12:
    case IMGFMT_NV21:
    case IMGFMT_HM12:
	mux_v->bih->biPlanes = 2;
	mux_v->bih->biBitCount = 12;
	break;
    case IMGFMT_I420:
    case IMGFMT_IYUV:
    case IMGFMT_YV12:
    case IMGFMT_411P:
	mux_v->bih->biPlanes = 3;
	mux_v->bih->biBitCount = 12;
	break;
    case IMGFMT_444P:
	mux_v->bih->biPlanes = 3;
	mux_v->bih->biBitCount = 24;
	break;
    case IMGFMT_422P:
	mux_v->bih->biPlanes = 3;
	mux_v->bih->biBitCount = 16;
	break;
    case IMGFMT_IF09:
	mux_v->bih->biPlanes = 4;
    case IMGFMT_YVU9:
	mux_v->bih->biBitCount = 9;
	break;
    case IMGFMT_UYVY:
    case IMGFMT_YUY2:
	mux_v->bih->biBitCount = 16;
	break;
    case IMGFMT_Y8:
        mux_v->bih->biBitCount = 8;
        break;
    default:
	mp_msg(MSGT_MENCODER, MSGL_INFO, MSGTR_MPCODECS_OutputWithFourccNotSupported, fmt);
	mux_v->bih->biCompression = 0;
	return 0;
    }
    return 1;
}


static int config(struct vf_instance *vf,
        int width, int height, int d_width, int d_height,
	unsigned int flags, unsigned int outfmt)
{
    int ret;
    AVRational pixelaspect = av_div_q((AVRational){d_width, d_height}, (AVRational){width, height});
    mux_v->bih->biWidth = width;
    mux_v->bih->biHeight = height;
    mux_v->aspect = (float)d_width/d_height;
    ret = set_format(vf, outfmt);
    if (!ret) return 0;

    mux_v->bih->biSizeImage = mux_v->bih->biWidth*mux_v->bih->biHeight*mux_v->bih->biBitCount/8;

	if (pipefd && !framesize)	 {
		YUV_INFO yuv_info = {
			0,
			mux_v->bih->biWidth,
			mux_v->bih->biHeight,
			'p',
			mux_v->h.dwRate,
			mux_v->h.dwScale,
			pixelaspect.num, pixelaspect.den,
			0
		};
		int len = sizeof(yuv_info);
		write(pipefd, &len, sizeof(len));
		write(pipefd, &yuv_info, len);
		framesize = mux_v->bih->biSizeImage;
	}

    return 1;
}

static int control(struct vf_instance *vf, int request, void *data) {
    return CONTROL_UNKNOWN;
}

static int query_format(struct vf_instance *vf, unsigned int fmt) {
    if (IMGFMT_IS_RGB(fmt) || IMGFMT_IS_BGR(fmt))
	return VFCAP_CSP_SUPPORTED | VFCAP_CSP_SUPPORTED_BY_HW;
    switch (fmt) {
    case IMGFMT_NV12:
    case IMGFMT_NV21:
    case IMGFMT_HM12:
    case IMGFMT_I420:
    case IMGFMT_IYUV:
    case IMGFMT_YV12:
    case IMGFMT_411P:
    case IMGFMT_444P:
    case IMGFMT_422P:
    case IMGFMT_UYVY:
    case IMGFMT_YUY2:
    case IMGFMT_YVU9:
    case IMGFMT_IF09:
    case IMGFMT_Y8:
	return VFCAP_CSP_SUPPORTED | VFCAP_CSP_SUPPORTED_BY_HW;
    }

    return 0;
}

static int put_image(struct vf_instance *vf, mp_image_t *mpi, double pts) {
    if (pipefd) {
	char nulldata = 0;
	mux_v->buffer = &nulldata;
	muxer_write_chunk(mux_v, 1, 0x10, MP_NOPTS_VALUE, MP_NOPTS_VALUE);
	//framesize = mpi->width*mpi->height*mux_v->bih->biBitCount/8;
	write(pipefd, mpi->planes[0], framesize);
    } else {
    mux_v->buffer = mpi->planes[0];
    muxer_write_chunk(mux_v, mpi->width*mpi->height*mux_v->bih->biBitCount/8, 0x10, pts, pts);
    }
    return 1;
}

//===========================================================================//

static void uninit(struct vf_instance *vf)
{
    if (pipefd) {
#if defined(__MINGW32__) || defined(__CYGWIN__)
		_commit(pipefd);
#endif
		close(pipefd);
		pipefd = 0;
    }
}

static int vf_open(vf_instance_t *vf, char* args){
    vf->config = config;
    vf->default_caps = VFCAP_CONSTANT;
    vf->control = control;
    vf->query_format = query_format;
    vf->put_image = put_image;
    vf->default_caps = 0;
    vf->priv = malloc(sizeof(struct vf_priv_s));
    memset(vf->priv, 0, sizeof(struct vf_priv_s));
    vf->priv->mux = (muxer_stream_t*)args;
    vf->uninit = uninit;

    mux_v->bih = calloc(1, sizeof(*mux_v->bih));
    mux_v->bih->biSize = sizeof(*mux_v->bih);
    mux_v->bih->biWidth = 0;
    mux_v->bih->biHeight = 0;

    return 1;
}

const vf_info_t ve_info_raw = {
    "raw encoder",
    "raw",
    "jwe21@cam.ac.uk",
    "Based on rawrgb",
    vf_open
};

//===========================================================================//
