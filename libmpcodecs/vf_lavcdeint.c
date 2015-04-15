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
#include <inttypes.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"
#include "av_helpers.h"



static int
vf_open(vf_instance_t *vf, char *args)
{
  /* We don't have any args */
  (void) args;

  mp_msg(MSGT_VFILTER,MSGL_FATAL, "lavcdeint has been removed, please use '-vf pp=fd' (same filter) or '-vf yadif'\n");


  return 0;
}


const vf_info_t vf_info_lavcdeint = {
    "libavcodec's deinterlacing filter",
    "lavcdeint",
    "Joe Rabinoff",
    "libavcodec's internal deinterlacer, in case you don't like "
      "the builtin ones (invoked with -pp or -npp)",
    vf_open,
    NULL
};


//===========================================================================//
