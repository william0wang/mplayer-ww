/*
 * Copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "config.h"
#include "af.h"
#include "libavutil/rational.h"
#include "libswresample/swresample.h"
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
#include "libavutil/mem.h"
#include "libavutil/common.h"

// Data for specific instances of this filter
typedef struct af_resample_s{
    struct SwrContext *swrctx;
    uint8_t *in[1];
    uint8_t *tmp[1];
    int in_alloc;
    int tmp_alloc;

    int filter_length;
    int linear;
    int phase_shift;
    double cutoff;

    int ctx_out_rate;
    int ctx_in_rate;
    int ctx_filter_size;
    int ctx_phase_shift;
    int ctx_linear;
    double ctx_cutoff;
}af_resample_t;


// Initialization and runtime control
static int control(struct af_instance_s* af, int cmd, void* arg)
{
  af_resample_t* s   = (af_resample_t*)af->setup;
  af_data_t *data= (af_data_t*)arg;
  int out_rate, test_output_res; // helpers for checking input format

  switch(cmd){
  case AF_CONTROL_REINIT:
    if((af->data->rate == data->rate) || (af->data->rate == 0))
        return AF_DETACH;

    af->data->nch    = data->nch;
    if (af->data->nch > AF_NCH) af->data->nch = AF_NCH;
    af->data->format = AF_FORMAT_S16_NE;
    af->data->bps    = 2;
    af->mul = (double)af->data->rate / data->rate;
    af->delay = af->data->nch * s->filter_length / FFMIN(af->mul, 1); // *bps*.5

    if (s->ctx_out_rate != af->data->rate || s->ctx_in_rate != data->rate || s->ctx_filter_size != s->filter_length ||
        s->ctx_phase_shift != s->phase_shift || s->ctx_linear != s->linear || s->ctx_cutoff != s->cutoff) {
        swr_free(&s->swrctx);
        if((s->swrctx=swr_alloc()) == NULL) return AF_ERROR;
        av_opt_set_int(s->swrctx, "out_sample_rate", af->data->rate, 0);
        av_opt_set_int(s->swrctx, "in_sample_rate", data->rate, 0);
        av_opt_set_int(s->swrctx, "filter_size", s->filter_length, 0);
        av_opt_set_int(s->swrctx, "phase_shift", s->phase_shift, 0);
        av_opt_set_int(s->swrctx, "linear_interp", s->linear, 0);
        av_opt_set_double(s->swrctx, "cutoff", s->cutoff, 0);
        av_opt_set_sample_fmt(s->swrctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
        av_opt_set_sample_fmt(s->swrctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
        av_opt_set_int(s->swrctx, "in_channel_count", af->data->nch, 0);
        av_opt_set_int(s->swrctx, "out_channel_count", af->data->nch, 0);
        if(swr_init(s->swrctx) < 0) return AF_ERROR;
        s->ctx_out_rate    = af->data->rate;
        s->ctx_in_rate     = data->rate;
        s->ctx_filter_size = s->filter_length;
        s->ctx_phase_shift = s->phase_shift;
        s->ctx_linear      = s->linear;
        s->ctx_cutoff      = s->cutoff;
    }

    // hack to make af_test_output ignore the samplerate change
    out_rate = af->data->rate;
    af->data->rate = data->rate;
    test_output_res = af_test_output(af, (af_data_t*)arg);
    af->data->rate = out_rate;
    return test_output_res;
  case AF_CONTROL_COMMAND_LINE:{
    s->cutoff= 0.0;
    sscanf((char*)arg,"%d:%d:%d:%d:%lf", &af->data->rate, &s->filter_length, &s->linear, &s->phase_shift, &s->cutoff);
    if(s->cutoff <= 0.0) s->cutoff= FFMAX(1.0 - 6.5/(s->filter_length+8), 0.80);
    return AF_OK;
  }
  case AF_CONTROL_RESAMPLE_RATE | AF_CONTROL_SET:
    af->data->rate = *(int*)arg;
    return AF_OK;
  }
  return AF_UNKNOWN;
}

// Deallocate memory
static void uninit(struct af_instance_s* af)
{
    if(af->data)
        free(af->data->audio);
    free(af->data);
    if(af->setup){
        af_resample_t *s = af->setup;
        swr_free(&s->swrctx);
        av_free(s->in[0]);
        av_free(s->tmp[0]);
        free(s);
    }
}

// Filter data through filter
static af_data_t* play(struct af_instance_s* af, af_data_t* data)
{
  af_resample_t *s = af->setup;
  int ret;
  int8_t *in = (int8_t*)data->audio;
  int8_t *out;
  int chans   = data->nch;
  int in_len  = data->len;
  int out_len = in_len * af->mul + 10;

  if(AF_OK != RESIZE_LOCAL_BUFFER(af,data))
      return NULL;

  av_fast_malloc(&s->tmp[0], &s->tmp_alloc, FFALIGN(out_len,32));
  if(s->tmp[0] == NULL) return NULL;

  out= (int8_t*)af->data->audio;

  out_len= FFMIN(out_len, af->data->len);

  av_fast_malloc(&s->in[0], &s->in_alloc, FFALIGN(in_len,32));
  if(s->in[0] == NULL) return NULL;

  memcpy(s->in[0], in, in_len);

  ret = swr_convert(s->swrctx, &s->tmp[0], out_len/chans/2, &s->in[0], in_len/chans/2);
  if (ret < 0) return NULL;
  out_len= ret*chans*2;

  memcpy(out, s->tmp[0], out_len);

  data->audio = af->data->audio;
  data->len   = out_len;
  data->rate  = af->data->rate;
  return data;
}

static int af_open(af_instance_t* af){
  af_resample_t *s = calloc(1,sizeof(af_resample_t));
  af->control=control;
  af->uninit=uninit;
  af->play=play;
  af->mul=1;
  af->data=calloc(1,sizeof(af_data_t));
  s->filter_length= 16;
  s->cutoff= FFMAX(1.0 - 6.5/(s->filter_length+8), 0.80);
  s->phase_shift= 10;
//  s->setup = RSMP_INT | FREQ_SLOPPY;
  af->setup=s;
  return AF_OK;
}

af_info_t af_info_lavcresample = {
  "Sample frequency conversion using libavcodec",
  "lavcresample",
  "Michael Niedermayer",
  "",
  AF_FLAGS_REENTRANT,
  af_open
};
