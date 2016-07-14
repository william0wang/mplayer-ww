/*
 * VIDIX Drivers Registry Handler.
 * Copyright (C) 2007 Benjamin Zores <ben@geexbox.org>
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "vidix.h"
#include "drivers.h"
#include "libavutil/common.h"
#include "mpbswap.h"
#include "mp_msg.h"
#include "config.h"

extern const VDXDriver cyberblade_drv;
extern const VDXDriver ivtv_drv;
extern const VDXDriver mach64_drv;
extern const VDXDriver mga_drv;
extern const VDXDriver mga_crtc2_drv;
extern const VDXDriver nvidia_drv;
extern const VDXDriver pm2_drv;
extern const VDXDriver pm3_drv;
extern const VDXDriver radeon_drv;
extern const VDXDriver rage128_drv;
extern const VDXDriver s3_drv;
extern const VDXDriver sh_veu_drv;
extern const VDXDriver sis_drv;
extern const VDXDriver unichrome_drv;

static const VDXDriver * const all_drivers[] = {
#ifdef CONFIG_VIDIX_DRV_CYBERBLADE
  &cyberblade_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_IVTV
  &ivtv_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_MACH64
  &mach64_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_MGA
  &mga_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_MGA_CRTC2
  &mga_crtc2_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_NVIDIA
  &nvidia_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_PM2
  &pm2_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_PM3
  &pm3_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_RADEON
  &radeon_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_RAGE128
  &rage128_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_S3
  &s3_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_SH_VEU
  &sh_veu_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_SIS
  &sis_drv,
#endif
#ifdef CONFIG_VIDIX_DRV_UNICHROME
  &unichrome_drv,
#endif
  NULL
};

void vidix_register_all_drivers (void)
{
}

static int vidix_probe_driver (VDXContext *ctx, const VDXDriver *drv,
                               unsigned int cap, int verbose)
{
  vidix_capability_t vid_cap;

  if (verbose)
    mp_msg(MSGT_VO, MSGL_STATUS, "[vidixlib] PROBING: %s\n", drv->name);

  if (!drv->probe || drv->probe (verbose, PROBE_NORMAL) != 0)
    return 0;

  if (!drv->get_caps || drv->get_caps (&vid_cap) != 0)
    return 0;

  if ((vid_cap.type & cap) != cap)
  {
    if (verbose)
      mp_msg(MSGT_VO, MSGL_STATUS, "[vidixlib] Found %s but has no required capability\n",
              drv->name);
     return 0;
  }

  if (verbose)
    mp_msg(MSGT_VO, MSGL_STATUS, "[vidixlib] %s probed o'k\n", drv->name);

  ctx->drv = drv;
  return 1;
}

static void vidix_list_drivers (void)
{
  const VDXDriver *drv;
  int i = 0;

  mp_msg(MSGT_VO, MSGL_STATUS, "[vidixlib] Available VIDIX drivers:\n");

  while ((drv = all_drivers[i++]))
  {
    vidix_capability_t cap;
    drv->get_caps (&cap);
    mp_msg(MSGT_VO, MSGL_STATUS, "[vidixlib]  * %s - %s\n", drv->name, cap.name);
  }
}

int vidix_find_driver (VDXContext *ctx, const char *name,
                       unsigned int cap, int verbose)
{
  const VDXDriver *drv;
  int i = 0;

  if (name && !strcmp (name, "help"))
  {
    vidix_list_drivers ();
    ctx->drv = NULL;
    return 0;
  }

  while ((drv = all_drivers[i++]))
  {
    if (name) /* forced driver */
    {
      if (!strcmp (drv->name, name))
      {
        if (vidix_probe_driver (ctx, drv, cap, verbose))
          return 1;
        else
        {
          ctx->drv = NULL;
          return 0;
        }
      }
    }
    else /* auto-probe */
    {
      if (vidix_probe_driver (ctx, drv, cap, verbose))
        return 1;
    }
  }

  if (verbose)
    mp_msg(MSGT_VO, MSGL_STATUS, "[vidixlib] No suitable driver can be found.\n");
  ctx->drv = NULL;
  return 0;
}
