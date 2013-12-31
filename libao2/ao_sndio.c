/*
 * Copyright (c) 2008 Alexandre Ratchov <alex@caoua.org>
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
#include <sys/types.h>
#include <poll.h>
#include <errno.h>
#include <sndio.h>
#include <stdlib.h>

#include "config.h"
#include "mp_msg.h"
#include "mixer.h"
#include "help_mp.h"

#include "libaf/af_format.h"

#include "audio_out.h"
#include "audio_out_internal.h"

static ao_info_t info = {
    "sndio audio output",
    "sndio",
    "Alexandre Ratchov <alex@caoua.org>",
    ""
};

LIBAO_EXTERN(sndio)

static struct sio_hdl *hdl;
static struct pollfd *pfds;
static struct sio_par par;
static int delay, vol, havevol;
static int prepause_delay;

/*
 * control misc parameters (only the volume for now)
 */
static int control(int cmd, void *arg)
{
    ao_control_vol_t *ctl = arg;

    switch (cmd) {
    case AOCONTROL_GET_VOLUME:
        if (!havevol)
            return CONTROL_FALSE;
        ctl->left = ctl->right = vol * 100 / SIO_MAXVOL;
        break;
    case AOCONTROL_SET_VOLUME:
        if (!havevol)
            return CONTROL_FALSE;
        sio_setvol(hdl, ctl->left * SIO_MAXVOL / 100);
        break;
    default:
        return CONTROL_UNKNOWN;
    }
    return CONTROL_OK;
}

/*
 * call-back invoked whenever the the hardware position changes
 */
static void movecb(void *addr, int delta)
{
    delay -= delta * (int)(par.bps * par.pchan);
}

/*
 * call-back invoked whenever the volume changes
 */
static void volcb(void *addr, unsigned newvol)
{
    vol = newvol;
}

/*
 * open device and setup parameters
 * return: 1 = success, 0 = failure
 */
static int init(int rate, int channels, int format, int flags)
{
    int bpf;

    hdl = sio_open(SIO_DEVANY, SIO_PLAY, 0);
    if (hdl == NULL) {
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: can't open sndio\n");
        return 0;
    }
    sio_initpar(&par);
    par.bits = af_fmt2bits(format);
    par.sig = (format & AF_FORMAT_SIGN_MASK) == AF_FORMAT_SI;
    if (par.bits > 8)
        par.le = (format & AF_FORMAT_END_MASK) == AF_FORMAT_LE;
    par.rate = rate;
    par.pchan = channels;
    par.appbufsz = par.rate * 250 / 1000;    /* 250ms buffer */
    par.round = par.rate * 10 / 1000;        /*  10ms block size */
    if (!sio_setpar(hdl, &par)) {
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: couldn't set params\n");
        goto err_out;
    }
    if (!sio_getpar(hdl, &par)) {
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: couldn't get params\n");
        goto err_out;
    }
    if (par.bps != SIO_BPS(par.bits)) {
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: unsupported format\n");
        goto err_out;
    }
    pfds = calloc(sio_nfds(hdl), sizeof(*pfds));
    if (pfds == NULL) {
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: couldn't allocate poll fds\n");
        goto err_out;
    }
    bpf = par.bps * par.pchan;
    ao_data.format = af_bits2fmt(8 * par.bps);
    ao_data.format |= par.sig ? AF_FORMAT_SI : AF_FORMAT_US;
    if (par.bits > 8)
        ao_data.format |= par.le ? AF_FORMAT_LE : AF_FORMAT_BE;
    ao_data.channels = par.pchan;
    ao_data.bps = bpf * par.rate;
    ao_data.buffersize = par.bufsz * bpf;
    ao_data.outburst = par.round * bpf;
    ao_data.samplerate = rate;
    havevol = sio_onvol(hdl, volcb, NULL);
    sio_onmove(hdl, movecb, NULL);

    /*
     * prepare the device to start. It will start as soon there's enough
     * data in the buffer to not underrun
     */
    delay = 0;
    if (!sio_start(hdl)) {
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: init: couldn't start\n");
        goto err_out;
    }
    return 1;
err_out:
    free(pfds);
    pfds = NULL;
    sio_close(hdl);
    hdl = NULL;
    return 0;
}

/*
 * close device
 */
static void uninit(int immed)
{
    if (hdl)
        sio_close(hdl);
    hdl = NULL;
    free(pfds);
    pfds = NULL;
}

/*
 * stop playing and prepare to restart
 */
static void reset(void)
{
    if (!sio_stop(hdl))
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: reset: couldn't stop\n");
    delay = 0;
    if (!sio_start(hdl))
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: reset: couldn't start\n");
}

/*
 * refresh the "delay" counter: call sio_revents() which keeps track of
 * the hardware position
 */
static void refresh(void)
{
    int n;

    n = sio_pollfd(hdl, pfds, POLLOUT);
    while (poll(pfds, n, 0) < 0 && errno == EINTR)
        ; /* nothing */
    sio_revents(hdl, pfds);
}

/*
 * return the number of bytes available in the device buffer
 */
static int get_space(void)
{
    refresh();
    return par.bufsz * par.pchan * par.bps - delay;
}

/*
 * delay in seconds between first and last sample in the buffer
 */
static float get_delay(void)
{
    refresh();
    return (float)delay / (par.bps * par.pchan * par.rate);
}

/*
 * submit the given number of bytes to the device
 */
static int play(void *data, int len, int flags)
{
    int n;

    n = sio_write(hdl, data, len);
    delay += n;
    return n;
}

/*
 * pause playing
 */
static void audio_pause(void)
{
    /*
     * sndio can't pause, so just stop
     */
    prepause_delay = delay;
    if (!sio_stop(hdl))
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: pause: couldn't stop\n");
    delay = 0;
}

/*
 * resume playing after audio_pause()
 */
static void audio_resume(void)
{
    /*
     * prepare to start; then refill the buffer with the number of bytes
     * audio_pause() consumed (this will trigger start)
     */
    if (!sio_start(hdl))
        mp_msg(MSGT_AO, MSGL_ERR, "ao2: resume: couldn't start\n");
    mp_ao_resume_refill(&audio_out_sndio,
                        par.bufsz * par.pchan * par.bps - prepause_delay);
}
