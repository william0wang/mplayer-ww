/*
 * MPlayer Skin for WW version
 * Copyright (C) 2009 William Wang <wan0eve@126.com>
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
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <windows.h>
#include <windowsx.h>

#include "mp_msg.h"
#include "cpudetect.h"
#include "resource.h"
#include "winstuff.h"
#include "bitmap.h"
#include "path.h"
#include "skin.h"
#include "input/input.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libiniparser/iniparser.h"

static int desktopbpp;
static char skin_name[256];
static dictionary *skin_ini = NULL;
static RECT save_rect = {0,0,-1,-1};
extern float percent_value;
extern int show_controlbar;
extern int skinned_player;
extern int have_audio;
extern int show_status;
extern int show_status2;
extern int seek_realtime;
extern int is_asf_format;
extern int ExitGuiThread;
extern char* filename;
extern char status_text_timer2[64];
char status[128];
int seek_dropping = 0;
int volume_dropping = 0;
int volpercent = 0;
int seekpercent = 0;
int bSeeking = 0;
static DWORD last_seek = 0;
static int need_reset_rgn = 0;

extern int is_audio_muted();

/* returns the bits per pixel of the desktop */
/* the format is always in BGR byte order */
static int GetDesktopBitsPerPixel(void)
{
    HWND desktop=GetDesktopWindow();
    HDC dc=GetDC(desktop);
    int bpp=GetDeviceCaps(dc, BITSPIXEL);
    ReleaseDC(desktop, dc);
    return bpp;
}

/* reads a complete image as is into image buffer */
static image_t *pngRead(const char *skindir, unsigned char *fname)
{
    txSample bmp;
    image_t *bf;
    FILE *fp;
    char *filename = NULL;

    if(!fname || !stricmp(fname, "NULL")) return 0;

    filename = calloc(1, strlen(skindir) + strlen(skin_name) + strlen(fname) + 9);
    sprintf(filename, "%s\\%s\\%s.png", skindir, skin_name, fname);
    if(!(fp = fopen(filename, "rb"))) {
        free(filename);
        return 0;
    }
    fclose(fp);

    if(bpRead(filename, &bmp) != 0) {
    	free(filename);
    	return 0;
	}

    free(filename);
    bf = calloc(1, sizeof(image_t));

    bf->width = bmp.Width; bf->height = bmp.Height;
    bf->size = bf->width * bf->height * desktopbpp / 8;
    if (desktopbpp == 32)
      bf->data = bmp.Image;
    else {
      const uint8_t *src[4] = { bmp.Image, NULL, NULL, NULL};
      int src_stride[4] = { 4 * bmp.Width, 0, 0, 0 };
      uint8_t *dst[4] = { NULL, NULL, NULL, NULL };
      int dst_stride[4];
      enum PixelFormat out_pix_fmt;
      struct SwsContext *sws;
      if      (desktopbpp == 16) out_pix_fmt = PIX_FMT_RGB555;
      else if (desktopbpp == 24) out_pix_fmt = PIX_FMT_RGB24;
      av_image_fill_linesizes(dst_stride, out_pix_fmt, bmp.Width);
      sws = sws_getContext(bmp.Width, bmp.Height, PIX_FMT_RGB32,
                           bmp.Width, bmp.Height, out_pix_fmt,
                           SWS_POINT, NULL, NULL, NULL);
      bf->data = malloc(bf->size);
      dst[0] = bf->data;
      sws_scale(sws, src, src_stride, 0, bmp.Height, dst, dst_stride);
      sws_freeContext(sws);
      free(bmp.Image);
    }
    return bf;
}

/* frees all skin images */
static void freeimages(image_t *img)
{
	if(img && img->data) free(img->data);
}

/* render image data to bitmap */
static int render_bitmag(bitmap_t *bitmap, HWND hWnd)
{
    HDC hdc;
    BITMAPINFO binfo;
    if(!bitmap || !bitmap->img) return 0;
    binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    binfo.bmiHeader.biWidth = bitmap->img->width;
    binfo.bmiHeader.biHeight = -bitmap->img->height;
    binfo.bmiHeader.biPlanes = 1;
    binfo.bmiHeader.biSizeImage = bitmap->img->width * bitmap->img->height * (desktopbpp / 8);
    binfo.bmiHeader.biXPelsPerMeter = 0;
    binfo.bmiHeader.biYPelsPerMeter = 0;
    binfo.bmiHeader.biClrUsed = 0;
    binfo.bmiHeader.biClrImportant = 0;
    binfo.bmiHeader.biBitCount = desktopbpp;
    binfo.bmiHeader.biCompression = BI_RGB;
    hdc = GetDC(hWnd);
    if(!hdc) {
    	freeimages(bitmap->img);
    	return 0;
    }
    bitmap->bmp = CreateDIBSection(hdc, &binfo, DIB_RGB_COLORS, &bitmap->data, NULL, 0);
    if(!bitmap->bmp) {
    	freeimages(bitmap->img);
    	ReleaseDC(hWnd,hdc);
        return 0;
    }
    memcpy(bitmap->data, bitmap->img->data, binfo.bmiHeader.biSizeImage);
    freeimages(bitmap->img);
    ReleaseDC(hWnd,hdc);

    return 1;
}

static inline int init_bitmap(bitmap_t *bitmap)
{
	bitmap->data = NULL;
	bitmap->img = NULL;
	bitmap->exist = 0;
	bitmap->show = 1;
	bitmap->center = 0;

    bitmap->top = -1;
    bitmap->left = -1;
    bitmap->right = -1;
    bitmap->width = -1;
    bitmap->height = -1;

	return 0;
}

/* load logo file */
int loadlogo(logo_t *logo, HWND hWnd)
{
	if(!render_bitmag(&logo->logo, hWnd))
        return 0;

    logo->is_ok = 1;
	return 1;
}

void freelogo(logo_t *logo)
{
	if(!logo) return;

    logo->is_ok = 0;
    if(logo->logo.img)
		free(logo->logo.img);
	if(logo->logo.bmp)
		DeleteObject(logo->logo.bmp);
}

/* load skin files */
int loadskin(skin_t *skin, HWND hWnd)
{
	BITMAP bm;
	int is_inner = 0;
	char *skindir = NULL;
	HINSTANCE hInstance = GetModuleHandle(0);

    if(!show_controlbar || !hWnd) 
    	return 0;

 	skindir = get_path("skin");
	skin->background_left.width = 0;
	skin->background_right.width = 0;
	skin->background_center.width = 0;
	skin->background_center.left = 0;

    if (!skin_ini) {
		skin->background.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_BACKGROUND), IMAGE_BITMAP, 0, 0, 0);
		skin->background_left.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_BKGL), IMAGE_BITMAP, 0, 0, 0);
		skin->background_right.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_BKGR), IMAGE_BITMAP, 0, 0, 0);
		skin->seekbar.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_SEEKBAR), IMAGE_BITMAP, 0, 0, 0);
		skin->seekbar_foregnd.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_SEEKBARF), IMAGE_BITMAP, 0, 0, 0);
		skin->seekbar_thumb.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_SEEKBART), IMAGE_BITMAP, 0, 0, 0);
		skin->volumebar.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_VOLUMEBAR), IMAGE_BITMAP, 0, 0, 0);
		skin->volumebar_foregnd.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_VOLUMEBARF), IMAGE_BITMAP, 0, 0, 0);
		skin->volumebar_thumb.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_VOLUMEBART), IMAGE_BITMAP, 0, 0, 0);

		skin->background.img = calloc(1, sizeof(image_t));
		GetObject(skin->background.bmp, sizeof(BITMAP), &bm);
		skin->background.img->width = bm.bmWidth;
		skin->background.img->height = bm.bmHeight;
		skin->background_height = skin->background.img->height;

		skin->background_left.img = calloc(1, sizeof(image_t));
		GetObject(skin->background_left.bmp, sizeof(BITMAP), &bm);
		skin->background_left.width = skin->background_left.img->width = bm.bmWidth;
		skin->background_left.img->height = bm.bmHeight;

		skin->background_right.img = calloc(1, sizeof(image_t));
		GetObject(skin->background_right.bmp, sizeof(BITMAP), &bm);
		skin->background_right.width = skin->background_right.img->width = bm.bmWidth;
		skin->background_right.img->height = bm.bmHeight;

		skin->seekbar.img = calloc(1, sizeof(image_t));
		GetObject(skin->seekbar.bmp, sizeof(BITMAP), &bm);
		skin->seekbar.img->width = bm.bmWidth;
		skin->seekbar.img->height = bm.bmHeight;
	    skin->seekbar.top = 2;
	    skin->seekbar.left = 60;
	    skin->seekbar.right = 75;
	    skin->seekbar.height = 13;

		skin->seekbar_foregnd.img = calloc(1, sizeof(image_t));
		GetObject(skin->seekbar_foregnd.bmp, sizeof(BITMAP), &bm);
		skin->seekbar_foregnd.img->width = bm.bmWidth;
		skin->seekbar_foregnd.img->height = bm.bmHeight;

		skin->seekbar_thumb.img = calloc(1, sizeof(image_t));
		GetObject(skin->seekbar_thumb.bmp, sizeof(BITMAP), &bm);
		skin->seekbar_thumb.img->width = bm.bmWidth;
		skin->seekbar_thumb.img->height = bm.bmHeight;
	    skin->seekbar_thumb.width = skin->seekbar_thumb.img->width/4;
	    skin->seekbar_thumb.height = skin->seekbar_thumb.img->height;
	    skin->seekbar_thumb.top = 3;
	    skin->seekbar_thumb.state = 0;
	    skin->seekbar_thumb.show = 1;

		skin->volumebar.img = calloc(1, sizeof(image_t));
		GetObject(skin->volumebar.bmp, sizeof(BITMAP), &bm);
		skin->volumebar.img->width = bm.bmWidth;
		skin->volumebar.img->height = bm.bmHeight;
	    skin->volumebar.top = 7;
	    skin->volumebar.right = 4;
	    skin->volumebar.height = 12;
	    skin->volumebar.width = skin->volumebar.img->width;

		skin->volumebar_foregnd.img = calloc(1, sizeof(image_t));
		GetObject(skin->volumebar_foregnd.bmp, sizeof(BITMAP), &bm);
		skin->volumebar_foregnd.img->width = bm.bmWidth;
		skin->volumebar_foregnd.img->height = bm.bmHeight;

		skin->volumebar_thumb.img = calloc(1, sizeof(image_t));
		GetObject(skin->volumebar_thumb.bmp, sizeof(BITMAP), &bm);
		skin->volumebar_thumb.img->width = bm.bmWidth;
		skin->volumebar_thumb.img->height = bm.bmHeight;
	    skin->volumebar_thumb.width = skin->volumebar_thumb.img->width/4;
	    skin->volumebar_thumb.height = skin->volumebar_thumb.img->height;
	    skin->volumebar_thumb.top = 2;
	    skin->volumebar_thumb.state = 1;

		skin->btn_play.exist = 0;
		skin->btn_prev.exist = 0;
		skin->btn_next.exist = 0;
		skin->btn_back.exist = 0;
		skin->btn_forward.exist = 0;

	    skin->status.top = 2;
	    skin->status.left = 1;
	    skin->status.right = -1;
	    skin->status.width = 50;
	    skin->status.height = 12;
    } else {
		if(!render_bitmag(&skin->background, hWnd)) {
			skin->background.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_BACKGROUND), IMAGE_BITMAP, 0, 0, 0);
			skin->background.img = calloc(1, sizeof(image_t));
			GetObject(skin->background.bmp, sizeof(BITMAP), &bm);
			skin->background.img->width = bm.bmWidth;
			skin->background.img->height = bm.bmHeight;
			++is_inner;
	    }
		skin->background_height = skin->background.img->height;

		if(render_bitmag(&skin->background_left, hWnd)) {
			skin->background_left.width = skin->background_left.img->width;
		}

		if(render_bitmag(&skin->background_right, hWnd)) {
			skin->background_right.width = skin->background_right.img->width;
		}

		if(skin->background_center.show && render_bitmag(&skin->background_center, hWnd)) {
			skin->background_center.width = skin->background_center.img->width;
		}

		if(!render_bitmag(&skin->seekbar, hWnd)) {
			skin->seekbar.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_SEEKBAR), IMAGE_BITMAP, 0, 0, 0);
			skin->seekbar.img = calloc(1, sizeof(image_t));
			GetObject(skin->seekbar.bmp, sizeof(BITMAP), &bm);
			skin->seekbar.img->width = bm.bmWidth;
			skin->seekbar.img->height = bm.bmHeight;
			++is_inner;
	    }

		if(!render_bitmag(&skin->seekbar_foregnd, hWnd)) {
			skin->seekbar_foregnd.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_SEEKBARF), IMAGE_BITMAP, 0, 0, 0);
			skin->seekbar_foregnd.img = calloc(1, sizeof(image_t));
			GetObject(skin->seekbar_foregnd.bmp, sizeof(BITMAP), &bm);
			skin->seekbar_foregnd.img->width = bm.bmWidth;
			skin->seekbar_foregnd.img->height = bm.bmHeight;
			++is_inner;
	    }

		if(!render_bitmag(&skin->seekbar_thumb, hWnd)) {
			skin->seekbar_thumb.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_SEEKBART), IMAGE_BITMAP, 0, 0, 0);
			skin->seekbar_thumb.img = calloc(1, sizeof(image_t));
			GetObject(skin->seekbar_thumb.bmp, sizeof(BITMAP), &bm);
			skin->seekbar_thumb.img->width = bm.bmWidth;
			skin->seekbar_thumb.img->height = bm.bmHeight;
			++is_inner;
	    }
	    skin->seekbar_thumb.width = skin->seekbar_thumb.img->width/4;
	    skin->seekbar_thumb.height = skin->seekbar_thumb.img->height;

	    skin->seekbar.top = iniparser_getint(skin_ini , "seekbar:top", -1);
	    skin->seekbar.left = iniparser_getint(skin_ini , "seekbar:left", -1);
	    skin->seekbar.right = iniparser_getint(skin_ini , "seekbar:right", -1);
	    skin->seekbar.width = iniparser_getint(skin_ini , "seekbar:width", 0);
	    skin->seekbar.height = iniparser_getint(skin_ini , "seekbar:height", -1);
	    skin->seekbar.center = iniparser_getint(skin_ini , "seekbar:center", 0);
	    skin->seekbar.show = iniparser_getint(skin_ini , "seekbar:zoom", 0);
	    skin->seekbar_thumb.top = iniparser_getint(skin_ini , "seekbar_thumb:top", -1);
	    skin->seekbar_thumb.show = iniparser_getint(skin_ini , "seekbar_thumb:show", 0);
	    skin->seekbar_thumb.state = 0;

		if(!render_bitmag(&skin->volumebar, hWnd)) {
			skin->volumebar.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_VOLUMEBAR), IMAGE_BITMAP, 0, 0, 0);
			skin->volumebar.img = calloc(1, sizeof(image_t));
			GetObject(skin->volumebar.bmp, sizeof(BITMAP), &bm);
			skin->volumebar.img->width = bm.bmWidth;
			skin->volumebar.img->height = bm.bmHeight;
			++is_inner;
	    }

		if(!render_bitmag(&skin->volumebar_foregnd, hWnd)) {
			skin->volumebar_foregnd.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_VOLUMEBARF), IMAGE_BITMAP, 0, 0, 0);
			skin->volumebar_foregnd.img = calloc(1, sizeof(image_t));
			GetObject(skin->volumebar_foregnd.bmp, sizeof(BITMAP), &bm);
			skin->volumebar_foregnd.img->width = bm.bmWidth;
			skin->volumebar_foregnd.img->height = bm.bmHeight;
			++is_inner;
	    }
	    skin->volumebar.width = skin->volumebar.img->width;

		if(!render_bitmag(&skin->volumebar_thumb, hWnd)) {
			skin->volumebar_thumb.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_VOLUMEBART), IMAGE_BITMAP, 0, 0, 0);
			skin->volumebar_thumb.img = calloc(1, sizeof(image_t));
			GetObject(skin->volumebar_thumb.bmp, sizeof(BITMAP), &bm);
			skin->volumebar_thumb.img->width = bm.bmWidth;
			skin->volumebar_thumb.img->height = bm.bmHeight;
			++is_inner;
	    }
	    skin->volumebar_thumb.width = skin->volumebar_thumb.img->width/4;
	    skin->volumebar_thumb.height = skin->volumebar_thumb.img->height;
	    skin->volumebar_thumb.state = 1;

		skin->volumebar.center = iniparser_getint(skin_ini , "volumebar:center", 0);
	    skin->volumebar.top = iniparser_getint(skin_ini , "volumebar:top", -1);
	    skin->volumebar.left = iniparser_getint(skin_ini , "volumebar:left", -1);
	    skin->volumebar.right = iniparser_getint(skin_ini , "volumebar:right", -1);
	    skin->volumebar.height = iniparser_getint(skin_ini , "volumebar:height", -1);
	    skin->volumebar_thumb.top = iniparser_getint(skin_ini , "volumebar_thumb:top", -1);

		if(is_inner > 5 && !skin->background_left.width) {
			skin->background_left.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_BKGL), IMAGE_BITMAP, 0, 0, 0);
			skin->background_left.img = calloc(1, sizeof(image_t));
			GetObject(skin->background_left.bmp, sizeof(BITMAP), &bm);
			skin->background_left.width = skin->background_left.img->width = bm.bmWidth;
			skin->background_left.img->height = bm.bmHeight;
		}

		if(is_inner > 5 && !skin->background_right.width) {
			skin->background_right.bmp = LoadImage(hInstance, MAKEINTRESOURCE(IDB_SKIN_BKGR), IMAGE_BITMAP, 0, 0, 0);
			skin->background_right.img = calloc(1, sizeof(image_t));
			GetObject(skin->background_right.bmp, sizeof(BITMAP), &bm);
			skin->background_right.width = skin->background_right.img->width = bm.bmWidth;
			skin->background_right.img->height = bm.bmHeight;
		}

		if(skin->btn_play.show && render_bitmag(&skin->btn_pause, hWnd)) {
			skin->btn_pause.exist = 1;
	    } else {
			skin->btn_pause.exist = 0;
	    }

		if(skin->btn_pause.exist && render_bitmag(&skin->btn_play, hWnd)) {
		    skin->btn_play.top = iniparser_getint(skin_ini , "btn_play:top", -1);
		    skin->btn_play.left = iniparser_getint(skin_ini , "btn_play:left", -1);
		    skin->btn_play.right = iniparser_getint(skin_ini , "btn_play:right", -1);
		    skin->btn_play.center = iniparser_getint(skin_ini , "btn_play:center", 1);
		    skin->btn_play.width = skin->btn_play.img->width/4;
		    skin->btn_play.height = skin->btn_play.img->height;
			skin->btn_play.exist = 1;
		    skin->btn_play.state = 0;
	    } else {
			skin->btn_play.exist = 0;
	    }

		if(skin->btn_prev.show && render_bitmag(&skin->btn_prev, hWnd)) {
			skin->btn_prev.top = iniparser_getint(skin_ini , "btn_prev:top", -1);
		    skin->btn_prev.left = iniparser_getint(skin_ini , "btn_prev:left", -1);
		    skin->btn_prev.right = iniparser_getint(skin_ini , "btn_prev:right", -1);
		    skin->btn_prev.center = iniparser_getint(skin_ini , "btn_prev:center", 1);
		    skin->btn_prev.width = skin->btn_prev.img->width/4;
		    skin->btn_prev.height = skin->btn_prev.img->height;
		    skin->btn_prev.state = 0;
			skin->btn_prev.exist = 1;
	    } else {
			skin->btn_prev.exist = 0;
	    }

		if(skin->btn_next.show && render_bitmag(&skin->btn_next, hWnd)) {
		    skin->btn_next.top = iniparser_getint(skin_ini , "btn_next:top", -1);
		    skin->btn_next.left = iniparser_getint(skin_ini , "btn_next:left", -1);
		    skin->btn_next.right = iniparser_getint(skin_ini , "btn_next:right", -1);
		    skin->btn_next.center = iniparser_getint(skin_ini , "btn_next:center", 1);
		    skin->btn_next.width = skin->btn_next.img->width/4;
		    skin->btn_next.height = skin->btn_next.img->height;
		    skin->btn_next.state = 0;
			skin->btn_next.exist = 1;
	    } else {
			skin->btn_next.exist = 0;
	    }

		if(skin->btn_forward.show && render_bitmag(&skin->btn_forward, hWnd)) {
		    skin->btn_forward.top = iniparser_getint(skin_ini , "btn_forward:top", -1);
		    skin->btn_forward.left = iniparser_getint(skin_ini , "btn_forward:left", -1);
		    skin->btn_forward.right = iniparser_getint(skin_ini , "btn_forward:right", -1);
		    skin->btn_forward.center = iniparser_getint(skin_ini , "btn_forward:center", 1);
		    skin->btn_forward.width = skin->btn_forward.img->width/4;
		    skin->btn_forward.height = skin->btn_forward.img->height;
		    skin->btn_forward.state = 0;
			skin->btn_forward.exist = 1;
	    } else {
			skin->btn_forward.exist = 0;
	    }

		if(skin->btn_back.show && render_bitmag(&skin->btn_back, hWnd)) {
		    skin->btn_back.top = iniparser_getint(skin_ini , "btn_back:top", -1);
		    skin->btn_back.left = iniparser_getint(skin_ini , "btn_back:left", -1);
		    skin->btn_back.right = iniparser_getint(skin_ini , "btn_back:right", -1);
		    skin->btn_back.center = iniparser_getint(skin_ini , "btn_back:center", 1);
		    skin->btn_back.width = skin->btn_back.img->width/4;
		    skin->btn_back.height = skin->btn_back.img->height;
		    skin->btn_back.state = 0;
			skin->btn_back.exist = 1;
	    } else {
			skin->btn_back.exist = 0;
	    }

		if(skin->btn_stop.show && render_bitmag(&skin->btn_stop, hWnd)) {
		    skin->btn_stop.top = iniparser_getint(skin_ini , "btn_stop:top", -1);
		    skin->btn_stop.left = iniparser_getint(skin_ini , "btn_stop:left", -1);
		    skin->btn_stop.right = iniparser_getint(skin_ini , "btn_stop:right", -1);
		    skin->btn_stop.center = iniparser_getint(skin_ini , "btn_stop:center", 1);
		    skin->btn_stop.width = skin->btn_stop.img->width/4;
		    skin->btn_stop.height = skin->btn_stop.img->height;
		    skin->btn_stop.state = 0;
			skin->btn_stop.exist = 1;
	    } else {
			skin->btn_stop.exist = 0;
	    }

		if(skin->btn_open.show && render_bitmag(&skin->btn_open, hWnd)) {
		    skin->btn_open.top = iniparser_getint(skin_ini , "btn_open:top", -1);
		    skin->btn_open.left = iniparser_getint(skin_ini , "btn_open:left", -1);
		    skin->btn_open.right = iniparser_getint(skin_ini , "btn_open:right", -1);
		    skin->btn_open.center = iniparser_getint(skin_ini , "btn_open:center", 0);
		    skin->btn_open.width = skin->btn_open.img->width/4;
		    skin->btn_open.height = skin->btn_open.img->height;
		    skin->btn_open.state = 0;
			skin->btn_open.exist = 1;
	    } else {
			skin->btn_open.exist = 0;
	    }

		if(skin->btn_mute.show && render_bitmag(&skin->btn_mute, hWnd)) {
		    skin->btn_mute.top = iniparser_getint(skin_ini , "btn_mute:top", -1);
		    skin->btn_mute.left = iniparser_getint(skin_ini , "btn_mute:left", -1);
		    skin->btn_mute.right = iniparser_getint(skin_ini , "btn_mute:right", -1);
		    skin->btn_mute.center = iniparser_getint(skin_ini , "btn_mute:center", 0);
		    skin->btn_mute.width = skin->btn_mute.img->width/4;
		    skin->btn_mute.height = skin->btn_mute.img->height;
		    skin->btn_mute.state = 0;
			skin->btn_mute.exist = 1;
	    } else {
			skin->btn_mute.exist = 0;
	    }

		if(skin->btn_mute.exist && render_bitmag(&skin->btn_muted, hWnd)) {
		    skin->btn_muted.width = skin->btn_muted.img->width/4;
		    skin->btn_muted.height = skin->btn_muted.img->height;
			skin->btn_mute.exist = 1;
	    } else {
			skin->btn_mute.exist = 0;
	    }

		if(skin->btn_equalizer.show && render_bitmag(&skin->btn_equalizer, hWnd)) {
		    skin->btn_equalizer.top = iniparser_getint(skin_ini , "btn_equalizer:top", -1);
		    skin->btn_equalizer.left = iniparser_getint(skin_ini , "btn_equalizer:left", -1);
		    skin->btn_equalizer.right = iniparser_getint(skin_ini , "btn_equalizer:right", -1);
		    skin->btn_equalizer.center = iniparser_getint(skin_ini , "btn_equalizer:center", 0);
		    skin->btn_equalizer.width = skin->btn_equalizer.img->width/4;
		    skin->btn_equalizer.height = skin->btn_equalizer.img->height;
		    skin->btn_equalizer.state = 0;
			skin->btn_equalizer.exist = 1;
	    } else {
			skin->btn_equalizer.exist = 0;
	    }

		if(skin->btn_screenshot.show && render_bitmag(&skin->btn_screenshot, hWnd)) {
		    skin->btn_screenshot.top = iniparser_getint(skin_ini , "btn_screenshot:top", -1);
		    skin->btn_screenshot.left = iniparser_getint(skin_ini , "btn_screenshot:left", -1);
		    skin->btn_screenshot.right = iniparser_getint(skin_ini , "btn_screenshot:right", -1);
		    skin->btn_screenshot.center = iniparser_getint(skin_ini , "btn_screenshot:center", 0);
		    skin->btn_screenshot.width = skin->btn_screenshot.img->width/4;
		    skin->btn_screenshot.height = skin->btn_screenshot.img->height;
		    skin->btn_screenshot.state = 0;
			skin->btn_screenshot.exist = 1;
	    } else {
			skin->btn_screenshot.exist = 0;
	    }

		if(skin->btn_playlist.show && render_bitmag(&skin->btn_playlist, hWnd)) {
		    skin->btn_playlist.top = iniparser_getint(skin_ini , "btn_playlist:top", -1);
		    skin->btn_playlist.left = iniparser_getint(skin_ini , "btn_playlist:left", -1);
		    skin->btn_playlist.right = iniparser_getint(skin_ini , "btn_playlist:right", -1);
		    skin->btn_playlist.center = iniparser_getint(skin_ini , "btn_playlist:center", 0);
		    skin->btn_playlist.width = skin->btn_playlist.img->width/4;
		    skin->btn_playlist.height = skin->btn_playlist.img->height;
		    skin->btn_playlist.state = 0;
			skin->btn_playlist.exist = 1;
	    } else {
			skin->btn_playlist.exist = 0;
	    }

		if(skin->btn_fullscreen.show && render_bitmag(&skin->btn_fullscreen, hWnd)) {
		    skin->btn_fullscreen.top = iniparser_getint(skin_ini , "btn_fullscreen:top", -1);
		    skin->btn_fullscreen.left = iniparser_getint(skin_ini , "btn_fullscreen:left", -1);
		    skin->btn_fullscreen.right = iniparser_getint(skin_ini , "btn_fullscreen:right", -1);
		    skin->btn_fullscreen.center = iniparser_getint(skin_ini , "btn_fullscreen:center", 0);
		    skin->btn_fullscreen.width = skin->btn_fullscreen.img->width/4;
		    skin->btn_fullscreen.height = skin->btn_fullscreen.img->height;
		    skin->btn_fullscreen.state = 0;
			skin->btn_fullscreen.exist = 1;
	    } else {
			skin->btn_fullscreen.exist = 0;
	    }

		iniparser_freedict(skin_ini);
	}

	free(skindir);
    skin->is_ok = 1;
	InvalidateRect(hWnd, NULL, TRUE);
	return 1;
}

int loadappskin(skin_t *skin, HWND hWnd)
{
    char *skindir = NULL;

    if(!skin->enable_appskin || !hWnd)
    	return 0;

    skindir = get_path("skin");

	skin->title_left.width = 0;
	skin->title_right.width = 0;
	skin->bottom_left.width = 0;
	skin->bottom_left.height = 0;
	skin->bottom_right.width = 0;
	skin->bottom_right.height = 0;

    if (!skin_ini) {
		skin->enable_appskin = 0;
    } else {
		if(!render_bitmag(&skin->title, hWnd))
			skin->enable_appskin = 0;

		if(skin->enable_appskin && render_bitmag(&skin->title_left, hWnd))
			skin->title_left.width = skin->title_left.img->width;

		if(skin->enable_appskin && render_bitmag(&skin->title_right, hWnd))
			skin->title_right.width = skin->title_right.img->width;

		if(!skin->enable_appskin || !render_bitmag(&skin->border_left, hWnd))
			skin->enable_appskin = 0;

		if(!skin->enable_appskin || !render_bitmag(&skin->border_right, hWnd))
			skin->enable_appskin = 0;

		if(!skin->enable_appskin || !render_bitmag(&skin->border_bottom, hWnd))
			skin->enable_appskin = 0;

		if(skin->enable_appskin && render_bitmag(&skin->bottom_left, hWnd)) {
			skin->bottom_left.width = skin->bottom_left.img->width;
			skin->bottom_left.height = skin->bottom_left.img->height;
		}

		if(skin->enable_appskin && render_bitmag(&skin->bottom_right, hWnd)) {
			skin->bottom_right.width = skin->bottom_right.img->width;
			skin->bottom_right.height = skin->bottom_right.img->height;
		}

		if(skin->enable_appskin && render_bitmag(&skin->btn_close, hWnd)) {
		    skin->btn_close.top = iniparser_getint(skin_ini , "btn_close:top", -1);
		    skin->btn_close.left = iniparser_getint(skin_ini , "btn_close:left", -1);
		    skin->btn_close.right = iniparser_getint(skin_ini , "btn_close:right", -1);
		    skin->btn_close.width = skin->btn_close.img->width/4;
		    skin->btn_close.height = skin->btn_close.img->height;
		    skin->btn_close.state = 0;
			skin->btn_close.exist = 1;
		} else {
			skin->enable_appskin = 0;
		}

		if(skin->enable_appskin && render_bitmag(&skin->btn_menu, hWnd)) {
		    skin->btn_menu.top = iniparser_getint(skin_ini , "btn_menu:top", -1);
		    skin->btn_menu.left = iniparser_getint(skin_ini , "btn_menu:left", -1);
		    skin->btn_menu.right = iniparser_getint(skin_ini , "btn_menu:right", -1);
		    skin->btn_menu.width = skin->btn_menu.img->width/4;
		    skin->btn_menu.height = skin->btn_menu.img->height;
		    skin->btn_menu.state = 0;
			skin->btn_menu.exist = 1;
		} else {
			skin->btn_menu.exist = 0;
		}

		if(skin->enable_appskin && render_bitmag(&skin->btn_min, hWnd)) {
		    skin->btn_min.top = iniparser_getint(skin_ini , "btn_min:top", -1);
		    skin->btn_min.left = iniparser_getint(skin_ini , "btn_min:left", -1);
		    skin->btn_min.right = iniparser_getint(skin_ini , "btn_min:right", -1);
		    skin->btn_min.width = skin->btn_min.img->width/4;
		    skin->btn_min.height = skin->btn_min.img->height;
		    skin->btn_min.state = 0;
			skin->btn_min.exist = 1;
		} else {
			skin->btn_min.exist = 0;
		}

		if(skin->enable_appskin && render_bitmag(&skin->btn_max, hWnd)) {
		    skin->btn_max.top = iniparser_getint(skin_ini , "btn_max:top", -1);
		    skin->btn_max.left = iniparser_getint(skin_ini , "btn_max:left", -1);
		    skin->btn_max.right = iniparser_getint(skin_ini , "btn_max:right", -1);
		    skin->btn_max.width = skin->btn_max.img->width/4;
		    skin->btn_max.height = skin->btn_max.img->height;
		    skin->btn_max.state = 0;
			skin->btn_max.exist = 1;
		} else {
			skin->btn_max.exist = 0;
		}
	}

	free(skindir);
    skin->border_ok = 1;
	return skin->enable_appskin;
}

void freeskin(skin_t *skin)
{
	if(!skin) return;

    skin->is_ok = 0;
    if(!show_controlbar)
	    return;
	if(skin->background.img)
		free(skin->background.img);
	if(skin->background_center.img)
		free(skin->background_center.img);
	if(skin->background_left.img)
		free(skin->background_left.img);
	if(skin->background_right.img)
		free(skin->background_right.img);

	if(skin->seekbar.img)
		free(skin->seekbar.img);
	if(skin->seekbar_foregnd.img)
		free(skin->seekbar_foregnd.img);
	if(skin->seekbar_thumb.img)
		free(skin->seekbar_thumb.img);

	if(skin->volumebar.img)
		free(skin->volumebar.img);
	if(skin->volumebar_foregnd.img)
		free(skin->volumebar_foregnd.img);
	if(skin->volumebar_thumb.img)
		free(skin->volumebar_thumb.img);

	if(skin->btn_play.img)
		free(skin->btn_play.img);
	if(skin->btn_pause.img)
		free(skin->btn_pause.img);
	if(skin->btn_prev.img)
		free(skin->btn_prev.img);
	if(skin->btn_next.img)
		free(skin->btn_next.img);
	if(skin->btn_back.img)
		free(skin->btn_back.img);
	if(skin->btn_forward.img)
		free(skin->btn_forward.img);
	if(skin->btn_stop.img)
		free(skin->btn_stop.img);

	if(skin->btn_open.img)
		free(skin->btn_open.img);
	if(skin->btn_mute.img)
		free(skin->btn_mute.img);
	if(skin->btn_muted.img)
		free(skin->btn_muted.img);
	if(skin->btn_equalizer.img)
		free(skin->btn_equalizer.img);
	if(skin->btn_screenshot.img)
		free(skin->btn_screenshot.img);
	if(skin->btn_playlist.img)
		free(skin->btn_playlist.img);
	if(skin->btn_fullscreen.img)
		free(skin->btn_fullscreen.img);

	if(skin->background.bmp)
		DeleteObject(skin->background.bmp);
	if(skin->background_center.bmp)
		DeleteObject(skin->background_center.bmp);
	if(skin->background_left.bmp)
		DeleteObject(skin->background_left.bmp);
	if(skin->background_right.bmp)
		DeleteObject(skin->background_right.bmp);

	if(skin->seekbar.bmp)
		DeleteObject(skin->seekbar.bmp);
	if(skin->seekbar_foregnd.bmp)
		DeleteObject(skin->seekbar_foregnd.bmp);
	if(skin->seekbar_thumb.bmp)
		DeleteObject(skin->seekbar_thumb.bmp);

	if(skin->volumebar.bmp)
		DeleteObject(skin->volumebar.bmp);
	if(skin->volumebar_foregnd.bmp)
		DeleteObject(skin->volumebar_foregnd.bmp);
	if(skin->volumebar_thumb.bmp)
		DeleteObject(skin->volumebar_thumb.bmp);

	if(skin->btn_play.bmp)
		DeleteObject(skin->btn_play.bmp);
	if(skin->btn_pause.bmp)
		DeleteObject(skin->btn_pause.bmp);
	if(skin->btn_prev.bmp)
		DeleteObject(skin->btn_prev.bmp);
	if(skin->btn_next.bmp)
		DeleteObject(skin->btn_next.bmp);
	if(skin->btn_back.bmp)
		DeleteObject(skin->btn_back.bmp);
	if(skin->btn_forward.bmp)
		DeleteObject(skin->btn_forward.bmp);
	if(skin->btn_stop.bmp)
		DeleteObject(skin->btn_stop.bmp);

	if(skin->btn_open.bmp)
		DeleteObject(skin->btn_open.bmp);
	if(skin->btn_mute.bmp)
		DeleteObject(skin->btn_mute.bmp);
	if(skin->btn_muted.bmp)
		DeleteObject(skin->btn_muted.bmp);
	if(skin->btn_equalizer.bmp)
		DeleteObject(skin->btn_equalizer.bmp);
	if(skin->btn_equalizer.bmp)
		DeleteObject(skin->btn_equalizer.bmp);
	if(skin->btn_screenshot.bmp)
		DeleteObject(skin->btn_screenshot.bmp);
	if(skin->btn_playlist.bmp)
		DeleteObject(skin->btn_playlist.bmp);
	if(skin->btn_fullscreen.bmp)
		DeleteObject(skin->btn_fullscreen.bmp);
}

/* load app skin files */
void freeappskin(skin_t *skin)
{
	if(!skin) return;

    skin->border_ok = 0;
    if(!skin->enable_appskin)
	    return;
	if(skin->title.img)
		free(skin->title.img);
	if(skin->title_left.img)
		free(skin->title_left.img);
	if(skin->title_right.img)
		free(skin->title_right.img);

	if(skin->border_left.img)
		free(skin->border_left.img);
	if(skin->border_right.img)
		free(skin->border_right.img);
	if(skin->border_bottom.img)
		free(skin->border_bottom.img);
	if(skin->bottom_left.img)
		free(skin->bottom_left.img);
	if(skin->bottom_right.img)
		free(skin->bottom_right.img);

	if(skin->btn_menu.img)
		free(skin->btn_menu.img);
	if(skin->btn_min.img)
		free(skin->btn_min.img);
	if(skin->btn_max.img)
		free(skin->btn_max.img);
	if(skin->btn_close.img)
		free(skin->btn_close.img);

	if(skin->title.bmp)
		DeleteObject(skin->title.bmp);
	if(skin->title_left.bmp)
		DeleteObject(skin->title_left.bmp);
	if(skin->title_right.bmp)
		DeleteObject(skin->title_right.bmp);

	if(skin->border_left.bmp)
		DeleteObject(skin->border_left.bmp);
	if(skin->border_right.bmp)
		DeleteObject(skin->border_right.bmp);
	if(skin->border_bottom.bmp)
		DeleteObject(skin->border_bottom.bmp);
	if(skin->bottom_left.bmp)
		DeleteObject(skin->bottom_left.bmp);
	if(skin->bottom_right.bmp)
		DeleteObject(skin->bottom_right.bmp);

	if(skin->btn_menu.bmp)
		DeleteObject(skin->btn_menu.bmp);
	if(skin->btn_min.bmp)
		DeleteObject(skin->btn_min.bmp);
	if(skin->btn_max.bmp)
		DeleteObject(skin->btn_max.bmp);
	if(skin->btn_close.bmp)
		DeleteObject(skin->btn_close.bmp);

}

void initskin(const char *dir, logo_t *logo, skin_t *skin)
{
    char *ininame = NULL, *skindir = NULL, *tmp, str[8];
    int r, g, b;

    desktopbpp = 32;//GetDesktopBitsPerPixel();

	if(dir && strlen(dir) > 0)
		strncpy(skin_name, dir, 255);
	else
		strncpy(skin_name, "default", 255);

	skindir = get_path("skin");
    ininame = calloc(1, strlen(skindir) + strlen(skin_name) + 15);
    sprintf(ininame, "%s\\%s\\skin.ini", skindir, skin_name);

	show_status2 = 0;
	logo->is_ok = 0;
	logo->bg_color = RGB(0,0,16);
	skin->is_ok = 0;
	skin->alpha = 0;
	skin->auto_hide = 0;
	skin->border_ok = 0;
	skin->dlg_frame = 0;
	skin->zoom_type = 0;
	skin->enable_appskin = 0;

	skin->background_width = 320;
	skin->background_height = 0;
	skin->background.state = 0;

	skin->status.color = RGB(0,0,0);
	skin->status.center = 0;
	skin->status.show = 0;
	skin->status2.color = RGB(0,0,0);
	skin->status2.center = 0;
	skin->status2.show = 0;
	skin->title_text.color = RGB(0,0,0);
	skin->title_text.alignment = 0;
	skin->title_text.center = 0;
	skin->title_text.show = 0;
	skin->seekbar.show = 0;
	skin->seekbar.width = 0;
	skin->seekbar.center = 0;

	init_bitmap(&logo->logo);

	init_bitmap(&skin->title);
	init_bitmap(&skin->title_left);
	init_bitmap(&skin->title_right);

	init_bitmap(&skin->border_left);
	init_bitmap(&skin->border_right);
	init_bitmap(&skin->border_bottom);
	init_bitmap(&skin->bottom_left);
	init_bitmap(&skin->bottom_right);

	init_bitmap(&skin->btn_menu);
	init_bitmap(&skin->btn_min);
	init_bitmap(&skin->btn_max);
	init_bitmap(&skin->btn_close);

	init_bitmap(&skin->background);
	init_bitmap(&skin->background_center);
	init_bitmap(&skin->background_left);
	init_bitmap(&skin->background_right);

	init_bitmap(&skin->seekbar);
	init_bitmap(&skin->seekbar_foregnd);
	init_bitmap(&skin->seekbar_thumb);

	init_bitmap(&skin->volumebar);
	init_bitmap(&skin->volumebar_foregnd);
	init_bitmap(&skin->volumebar_thumb);

	init_bitmap(&skin->btn_play);
	init_bitmap(&skin->btn_pause);
	init_bitmap(&skin->btn_prev);
	init_bitmap(&skin->btn_next);
	init_bitmap(&skin->btn_forward);
	init_bitmap(&skin->btn_back);
	init_bitmap(&skin->btn_stop);

	init_bitmap(&skin->btn_open);
	init_bitmap(&skin->btn_mute);
	init_bitmap(&skin->btn_muted);
	init_bitmap(&skin->btn_equalizer);
	init_bitmap(&skin->btn_screenshot);
	init_bitmap(&skin->btn_playlist);
	init_bitmap(&skin->btn_fullscreen);

    if (GetFileAttributes(ininame) != 0xFFFFFFFF) {

		logo->logo.img = pngRead(skindir, "logo");

	    if(!show_controlbar) {
			free(ininame);
	    	free(skindir);
	    	return;
	    }

		skin->background.img = pngRead(skindir, "background");
		skin->background_left.img = pngRead(skindir, "background_left");
		skin->background_right.img = pngRead(skindir, "background_right");
		skin->background_center.img = pngRead(skindir, "background_center");

		skin->seekbar.img = pngRead(skindir, "seekbar");
		skin->seekbar_foregnd.img = pngRead(skindir, "seekbar_foregnd");
		skin->seekbar_thumb.img = pngRead(skindir, "seekbar_thumb");

		skin->volumebar.img = pngRead(skindir, "volumebar");
		skin->volumebar_foregnd.img = pngRead(skindir, "volumebar_foregnd");
		skin->volumebar_thumb.img = pngRead(skindir, "volumebar_thumb");

		skin->btn_play.img = pngRead(skindir, "btn_play");
		skin->btn_pause.img = pngRead(skindir, "btn_pause");
		skin->btn_stop.img = pngRead(skindir, "btn_stop");
		skin->btn_prev.img = pngRead(skindir, "btn_prev");
		skin->btn_next.img = pngRead(skindir, "btn_next");
		skin->btn_back.img = pngRead(skindir, "btn_back");
		skin->btn_forward.img = pngRead(skindir, "btn_forward");

		skin->btn_open.img = pngRead(skindir, "btn_open");
		skin->btn_mute.img = pngRead(skindir, "btn_mute");
		skin->btn_muted.img = pngRead(skindir, "btn_muted");
		skin->btn_playlist.img = pngRead(skindir, "btn_playlist");
		skin->btn_equalizer.img = pngRead(skindir, "btn_equalizer");
		skin->btn_screenshot.img = pngRead(skindir, "btn_screenshot");
		skin->btn_fullscreen.img = pngRead(skindir, "btn_fullscreen");

		skin_ini = iniparser_load(ininame);
		if(skin_ini) {
			skin->enable_appskin = iniparser_getint(skin_ini , "appskin:enable", 0);
			skin->enable_alpha = iniparser_getint(skin_ini , "appskin:alpha", 1);
		    skin->background_width = iniparser_getint(skin_ini , "background:width", 320);
		    skin->background_center.show = iniparser_getint(skin_ini , "background:center", 1);
			skin->zoom_type = iniparser_getint(skin_ini , "background:zoom", 0);
			skin->alpha = iniparser_getint(skin_ini , "controlbar:alpha", 0);
			skin->auto_hide = iniparser_getint(skin_ini , "controlbar:hide", 0);
		    skin->status.top = iniparser_getint(skin_ini , "status:top", 0);
		    skin->status.left = iniparser_getint(skin_ini , "status:left", -1);
		    skin->status.right = iniparser_getint(skin_ini , "status:right", -1);
		    skin->status.width = iniparser_getint(skin_ini , "status:width", 0);
		    skin->status.height = iniparser_getint(skin_ini , "status:height", 0);
			skin->status.center = iniparser_getint(skin_ini , "status:center", 0);
			skin->status.show = iniparser_getint(skin_ini , "status:show", 0);

		    skin->status2.top = iniparser_getint(skin_ini , "status2:top", 0);
		    skin->status2.left = iniparser_getint(skin_ini , "status2:left", -1);
		    skin->status2.right = iniparser_getint(skin_ini , "status2:right", -1);
		    skin->status2.width = iniparser_getint(skin_ini , "status2:width", 0);
		    skin->status2.height = iniparser_getint(skin_ini , "status2:height", 0);
			skin->status2.center = iniparser_getint(skin_ini , "status2:center", 0);
			skin->status2.show = iniparser_getint(skin_ini , "status2:show", 0);
			show_status2 = skin->status2.show;

			skin->btn_play.show = iniparser_getint(skin_ini , "btn_play:show", 1);
			skin->btn_stop.show = iniparser_getint(skin_ini , "btn_stop:show", 1);
			skin->btn_prev.show = iniparser_getint(skin_ini , "btn_prev:show", 1);
			skin->btn_next.show = iniparser_getint(skin_ini , "btn_next:show", 1);
			skin->btn_back.show = iniparser_getint(skin_ini , "btn_back:show", 1);
			skin->btn_forward.show = iniparser_getint(skin_ini , "btn_forward:show", 1);

			skin->btn_open.show = iniparser_getint(skin_ini , "btn_open:show", 1);
			skin->btn_mute.show = iniparser_getint(skin_ini , "btn_mute:show", 1);
			skin->btn_playlist.show = iniparser_getint(skin_ini , "btn_playlist:show", 1);
			skin->btn_equalizer.show = iniparser_getint(skin_ini , "btn_equalizer:show", 1);
			skin->btn_screenshot.show = iniparser_getint(skin_ini , "btn_screenshot:show", 1);
			skin->btn_fullscreen.show = iniparser_getint(skin_ini , "btn_fullscreen:show", 1);

		    tmp = iniparser_getstring(skin_ini , "status:color", NULL);
		    if(tmp && strlen(tmp) > 5) {
				memset(str, 0, 8);
		    	strncpy(str, tmp, 2);
		    	r = strtoul(str, 0, 16);
				memset(str, 0, 8);
		    	strncpy(str, tmp+2, 2);
		    	g =  strtoul(str, 0, 16);
				memset(str, 0, 8);
		    	strncpy(str, tmp+4, 2);
		    	b =  strtoul(str, 0, 16);
		    	skin->status.color = RGB(r ,g ,b);
		    }

		    tmp = iniparser_getstring(skin_ini , "status2:color", NULL);
		    if(tmp && strlen(tmp) > 5) {
				memset(str, 0, 8);
		    	strncpy(str, tmp, 2);
		    	r = strtoul(str, 0, 16);
				memset(str, 0, 8);
		    	strncpy(str, tmp+2, 2);
		    	g =  strtoul(str, 0, 16);
				memset(str, 0, 8);
		    	strncpy(str, tmp+4, 2);
		    	b =  strtoul(str, 0, 16);
		    	skin->status2.color = RGB(r ,g ,b);
		    }

		    if(skin->enable_appskin) {
			    skin->title_text.top = iniparser_getint(skin_ini , "title_text:top", 0);
			    skin->title_text.left = iniparser_getint(skin_ini , "title_text:left", -1);
			    skin->title_text.right = iniparser_getint(skin_ini , "title_text:right", -1);
			    skin->title_text.size = iniparser_getint(skin_ini , "title_text:size", 10);
			    skin->title_text.height = iniparser_getint(skin_ini , "title_text:height", 0);
				skin->title_text.show = iniparser_getint(skin_ini , "title_text:show", 0);
				skin->title_text.alignment = iniparser_getint(skin_ini , "title_text:alignment", 0);
			    tmp = iniparser_getstring(skin_ini , "title_text:color", NULL);
			    if(tmp && strlen(tmp) > 5) {
					memset(str, 0, 8);
			    	strncpy(str, tmp, 2);
			    	r = strtoul(str, 0, 16);
					memset(str, 0, 8);
			    	strncpy(str, tmp+2, 2);
			    	g =  strtoul(str, 0, 16);
					memset(str, 0, 8);
			    	strncpy(str, tmp+4, 2);
			    	b =  strtoul(str, 0, 16);
			    	skin->title_text.color = RGB(r ,g ,b);
			    }
		    }

		    tmp = iniparser_getstring(skin_ini , "logo:bg_color", NULL);
		    if(tmp && strlen(tmp) > 5) {
				memset(str, 0, 8);
		    	strncpy(str, tmp, 2);
		    	r = strtoul(str, 0, 16);
				memset(str, 0, 8);
		    	strncpy(str, tmp+2, 2);
		    	g =  strtoul(str, 0, 16);
				memset(str, 0, 8);
		    	strncpy(str, tmp+4, 2);
		    	b =  strtoul(str, 0, 16);
		    	logo->bg_color = RGB(r ,g ,b);
		    }
	    }

		skin->background.state = skin->enable_appskin;
	    if(!skinned_player) skin->enable_appskin = 0;

		if(skin->enable_appskin) {

			skin->title.img = pngRead(skindir, "title");
			skin->title_left.img = pngRead(skindir, "title_left");
			skin->title_right.img = pngRead(skindir, "title_right");

			skin->border_left.img = pngRead(skindir, "border_left");
			skin->border_right.img = pngRead(skindir, "border_right");
			skin->border_bottom.img = pngRead(skindir, "border_bottom");
			skin->bottom_left.img = pngRead(skindir, "bottom_left");
			skin->bottom_right.img = pngRead(skindir, "bottom_right");

			skin->btn_menu.img = pngRead(skindir, "btn_menu");
			skin->btn_min.img = pngRead(skindir, "btn_min");
			skin->btn_max.img = pngRead(skindir, "btn_max");
			skin->btn_close.img = pngRead(skindir, "btn_close");
		}
    } else {
    	skin->status.show = 2;
    }
	free(ininame);
	free(skindir);
}

int get_button_left(skin_t *skin, bitmap_t *button, int width)
{
	int btn_left = 0;
	if(!button)
		return 0;

	if(button->right > 0) {
		if(button->center && skin && skin->background_center.width > 0)
			btn_left = skin->background_center.left + skin->background_center.width - button->right - button->width;
		else
			btn_left = width - button->right - button->width;
	} else {
		btn_left = button->left;
		if(button->center && skin && skin->background_center.width > 0)
			btn_left += skin->background_center.left;
	}
	return btn_left;
}

static int get_seekbar_rect(skin_t *skin, RECT win_rc, RECT *rc)
{
	rc->top = skin->seekbar.top;
	rc->bottom = skin->seekbar.top + skin->seekbar.height;
	if(skin->seekbar.center && skin->background_center.width > 0) {
		rc->left = skin->background_center.left + skin->seekbar.left;
		rc->right = skin->background_center.left + skin->background_center.width - skin->seekbar.right;
	} else {
		rc->left = skin->seekbar.left;
		rc->right = win_rc.right-skin->seekbar.right;
	}
}

BOOL TransparentBlt(HDC dcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC dcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent)
{
     HDC dc, maskDC, newMaskDC, newImageDC;
     HBITMAP bitmap, oldBitmap, maskBitmap, oldMask, newMask, oldNewMask, newImage, oldNewImage;
     
     if (nWidthDest < 1) return FALSE;
     if (nWidthSrc < 1) return FALSE;
     if (nHeightDest < 1) return FALSE;
     if (nHeightSrc < 1) return FALSE;

     dc = CreateCompatibleDC(NULL);
     bitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, GetDeviceCaps(dc, BITSPIXEL), NULL);

     if (bitmap == NULL)
     {
         DeleteDC(dc);
         return FALSE;
     }

     oldBitmap = (HBITMAP)SelectObject(dc, bitmap);

     if (!BitBlt(dc, 0, 0, nWidthSrc, nHeightSrc, dcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY))
     {
         SelectObject(dc, oldBitmap);
         DeleteObject(bitmap);
         DeleteDC(dc);
         return FALSE;
     }

     maskDC = CreateCompatibleDC(NULL);
     maskBitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, 1, NULL);

     if (maskBitmap == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteObject(bitmap);
         DeleteDC(dc);
         DeleteDC(maskDC);
         return FALSE;
     }

     oldMask =  (HBITMAP)SelectObject(maskDC, maskBitmap);

     SetBkColor(maskDC, RGB(0,0,0));
     SetTextColor(maskDC, RGB(255,255,255));
     if (!BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,NULL,0,0,BLACKNESS))
     {
         SelectObject(maskDC, oldMask);
         DeleteObject(maskBitmap);
         DeleteDC(maskDC);
         SelectObject(dc, oldBitmap);
         DeleteObject(bitmap);
         DeleteDC(dc);
         return FALSE;
     }

     SetBkColor(dc, crTransparent);
     BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,dc,0,0,SRCINVERT);

     SetBkColor(dc, RGB(0,0,0));
     SetTextColor(dc, RGB(255,255,255));
     BitBlt(dc, 0,0,nWidthSrc,nHeightSrc,maskDC,0,0,SRCAND);

     newMaskDC = CreateCompatibleDC(NULL);
     newMask = CreateBitmap(nWidthDest, nHeightDest, 1, GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newMask == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         SelectObject(maskDC, oldMask);
         DeleteDC(maskDC);
          DeleteDC(newMaskDC);
         DeleteObject(bitmap);
         DeleteObject(maskBitmap);
         return FALSE;
     }

     SetStretchBltMode(newMaskDC, COLORONCOLOR);
     oldNewMask = (HBITMAP) SelectObject(newMaskDC, newMask);
     StretchBlt(newMaskDC, 0, 0, nWidthDest, nHeightDest, maskDC, 0, 0, nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(maskDC, oldMask);
     DeleteDC(maskDC);
     DeleteObject(maskBitmap);

     newImageDC = CreateCompatibleDC(NULL);
     newImage = CreateBitmap(nWidthDest, nHeightDest, 1, GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newImage == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         DeleteDC(newMaskDC);
         DeleteObject(bitmap);
         return FALSE;
     }

     oldNewImage = (HBITMAP)SelectObject(newImageDC, newImage);
     StretchBlt(newImageDC, 0, 0, nWidthDest, nHeightDest, dc, 0, 0, nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(dc, oldBitmap);
     DeleteDC(dc);
     DeleteObject(bitmap);

     BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, newMaskDC, 0, 0, SRCAND);

     BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, newImageDC, 0, 0, SRCPAINT);

     SelectObject(newImageDC, oldNewImage);
     DeleteDC(newImageDC);
     SelectObject(newMaskDC, oldNewMask);
     DeleteDC(newMaskDC);
     DeleteObject(newImage);
     DeleteObject(newMask);

     return TRUE;
}

#define ALLOC_UNIT 100
HRGN BitmapToRegionEx(HBITMAP hbm32, VOID * pbits32, int width, int height, COLORREF cTransparentColor)
{
	BITMAP bm32;
	HANDLE hData;
	RGNDATA *pData;
	BYTE *p32;
	RECT *pr;
	BYTE lr, lg, lb, hr, hg, hb;
	HRGN h, hRgn = NULL;
	DWORD maxRects = ALLOC_UNIT;

	if (pbits32 && hbm32)
	{
		// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
		GetObject(hbm32, sizeof(bm32), &bm32);
		while (bm32.bmWidthBytes % 4)
			bm32.bmWidthBytes++;

		// For better performances, we will use the ExtCreateRegion() function to create the
		// region. This function take a RGNDATA structure on entry. We will add rectangles by
		// amount of ALLOC_UNIT number in this structure.
		hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
		pData = (RGNDATA *)GlobalLock(hData);
		pData->rdh.dwSize = sizeof(RGNDATAHEADER);
		pData->rdh.iType = RDH_RECTANGLES;
		pData->rdh.nCount = pData->rdh.nRgnSize = 0;
		SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

		// Keep on hand highest and lowest values for the "transparent" pixels
		lr = GetRValue(cTransparentColor);
		lg = GetGValue(cTransparentColor);
		lb = GetBValue(cTransparentColor);
		hr = min(0xff, lr);
		hg = min(0xff, lg);
		hb = min(0xff, lb);

		// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
		p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
		for (int y = 0; y < height; y++)
		{
			// Scan each bitmap pixel from left to right
			for (int x = 0; x < width; x++)
			{
				// Search for a continuous range of "non transparent pixels"
				int x0 = x;
				LONG *p = (LONG *)p32 + x;
				while (x < width)
				{
					BYTE b = GetRValue(*p);
					if (b >= lr && b <= hr)
					{
						b = GetGValue(*p);
						if (b >= lg && b <= hg)
						{
							b = GetBValue(*p);
							if (b >= lb && b <= hb)
								// This pixel is "transparent"
								break;
						}
					}
					p++;
					x++;
				}

				if (x > x0)
				{
					// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
					if (pData->rdh.nCount >= maxRects)
					{
						GlobalUnlock(hData);
						maxRects += ALLOC_UNIT;
						hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
						pData = (RGNDATA *)GlobalLock(hData);
					}
					pr = (RECT *)&pData->Buffer;
					SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
					if (x0 < pData->rdh.rcBound.left)
						pData->rdh.rcBound.left = x0;
					if (y < pData->rdh.rcBound.top)
						pData->rdh.rcBound.top = y;
					if (x > pData->rdh.rcBound.right)
						pData->rdh.rcBound.right = x;
					if (y+1 > pData->rdh.rcBound.bottom)
						pData->rdh.rcBound.bottom = y+1;
					pData->rdh.nCount++;

					// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
					// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
					if (pData->rdh.nCount == 2000)
					{
						HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
						if (hRgn) {
							CombineRgn(hRgn, hRgn, h, RGN_OR);
							DeleteObject(h);
						} else
							hRgn = h;
						pData->rdh.nCount = 0;
						SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
					}
				}
			}

			// Go to next row (remember, the bitmap is inverted vertically)
			p32 -= bm32.bmWidthBytes;
		}

		// Create or extend the region with the remaining rectangles
		h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
		if (hRgn)
		{
			CombineRgn(hRgn, hRgn, h, RGN_OR);
			DeleteObject(h);
		}
		else
			hRgn = h;

		// Clean up
		GlobalFree(hData);
	}
	return hRgn;
}

int set_window_rgn(HWND hWnd, skin_t *skin, int wtype, int force)
{
	RECT r, rt, rl, rr, rb;
	HBITMAP bmp;
	VOID *pbits32;
	HRGN rgn = NULL;
	HBRUSH brush;
	HDC dc, dc2;
	int bg_left, bg_width, bg_top, bg_height;

	if(!skin->enable_appskin || !skin->border_ok || (!skin->enable_alpha && !skin->dlg_frame)) {
		if(need_reset_rgn) {
			need_reset_rgn = 0;
			SetWindowRgn(hWnd, NULL, TRUE);
		}
		return -1;
	}

	GetWindowRect(hWnd, &r);
	r.right -= r.left;
	r.bottom -= r.top;
	r.left = r.top = 0;
	need_reset_rgn = 0;

	if(wtype) {
		if(!skin->enable_alpha && skin->dlg_frame) {
			rgn = CreateRectRgn(0, skin->dlg_frame, r.right, r.bottom-skin->dlg_frame);
			need_reset_rgn = 1;
		} else {
			if(!force && save_rect.right == r.right && save_rect.bottom == r.bottom)
				return 1;

			save_rect.right = r.right;
			save_rect.bottom = r.bottom;

			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {sizeof(BITMAPINFOHEADER),
				r.right, r.bottom, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };

			dc = CreateCompatibleDC(NULL);
			dc2 = CreateCompatibleDC(NULL);
			bmp = CreateDIBSection(dc2,(BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);

			brush = CreateSolidBrush(RGB(0,0,0));
			SelectObject(dc2, bmp);
			FillRect(dc2, &r, brush);
			DeleteObject(brush);

			if(skin->dlg_frame) {
				rt.left = 0;
				rt.right = r.right;
				rt.top = 0;
				rt.bottom = skin->dlg_frame;
				rb.left = 0;
				rb.right = r.right;
				rb.top = r.bottom - skin->dlg_frame;
				rb.bottom = r.bottom;

				brush = CreateSolidBrush(RGB(255,0,255));
				FillRect(dc2, &rt, brush);
				FillRect(dc2, &rb, brush);
				DeleteObject(brush);
			}

			rt.left = 0;
			rt.top = skin->dlg_frame;
			rt.bottom = skin->title.img->height + skin->dlg_frame;
			rt.right = r.right;

			rl.top = rt.bottom;
			rl.left = 0;
			rl.right = skin->border_left.img->width;
			rl.bottom = r.bottom - skin->border_bottom.img->height - skin->dlg_frame;

			rr.top = rt.bottom;
			rr.left = r.right - skin->border_right.img->width;
			rr.right = r.right;
			rr.bottom = r.bottom - skin->border_bottom.img->height - skin->dlg_frame;

			rb.left = 0;
			rb.right = r.right;
			rb.top = r.bottom - skin->border_bottom.img->height - skin->dlg_frame;
			rb.bottom = r.bottom - skin->dlg_frame;

			bg_left = skin->title_left.width;
			bg_width = rt.right-rt.left-skin->title_right.width;
			SelectObject(dc, skin->title.bmp);

			StretchBlt(dc2, bg_left, rt.top, bg_width, skin->title.img->height
				, dc, 0, 0, skin->title.img->width, skin->title.img->height, SRCCOPY);

			if(skin->title_left.width > 0) {
				SelectObject(dc, skin->title_left.bmp);
				BitBlt(dc2, 0, rt.bottom-skin->title.img->height
					, skin->title_left.width, skin->title.img->height, dc, 0, 0, SRCCOPY);
			}

			if(skin->title_right.width > 0) {
				SelectObject(dc, skin->title_right.bmp);
				BitBlt(dc2, rt.right-skin->title_right.width, rt.bottom-skin->title.img->height
					, skin->title_right.width, skin->title.img->height, dc, 0, 0, SRCCOPY);
			}

			bg_top = rl.top;
			bg_height = rl.bottom-skin->border_left.height;
			SelectObject(dc, skin->border_left.bmp);
			StretchBlt(dc2, rl.left, bg_top, skin->border_left.img->width, bg_height
				, dc, 0, 0, skin->border_left.img->width, skin->border_left.img->height, SRCCOPY);

			bg_top = rr.top;
			bg_height = rr.bottom-skin->border_right.height;
			SelectObject(dc, skin->border_right.bmp);
			StretchBlt(dc2, rr.left, bg_top, skin->border_right.img->width, bg_height
				, dc, 0, 0, skin->border_right.img->width, skin->border_right.img->height, SRCCOPY);

			bg_left = skin->bottom_left.width;
			bg_width = rb.right-rb.left-skin->bottom_right.width;
			SelectObject(dc, skin->border_bottom.bmp);
			StretchBlt(dc2, bg_left, rb.top, bg_width, skin->border_bottom.img->height
				, dc, 0, 0, skin->border_bottom.img->width, skin->border_bottom.img->height, SRCCOPY);

			if(skin->bottom_left.width > 0) {
				SelectObject(dc, skin->bottom_left.bmp);
				BitBlt(dc2, 0, rb.bottom-skin->bottom_left.height
					, skin->bottom_left.width, skin->bottom_left.img->height, dc, 0, 0, SRCCOPY);
			}

			if(skin->bottom_right.width > 0) {
				SelectObject(dc, skin->bottom_right.bmp);
				BitBlt(dc2, rb.right-skin->bottom_right.width, rb.bottom-skin->bottom_right.height
					, skin->bottom_right.width, skin->bottom_right.img->height, dc, 0, 0, SRCCOPY);
			}

			rgn = BitmapToRegionEx(bmp, pbits32, r.right, r.bottom, RGB(255,0,255));

			DeleteObject(bmp);
			DeleteDC(dc2);
			DeleteDC(dc);
		}
	} else {
		rgn = NULL;
	}

	SetWindowRgn(hWnd, rgn, TRUE);
	return 0;
}

int draw_border(HWND hWnd, skin_t *skin, char *title_text, BOOL fill)
{
	RECT r, rc, rt, rl, rr, rb;
	int bg_left, bg_width, bg_top, bg_height;
	HBITMAP bmp;
	LOGFONT fontRect;
	UINT Format = DT_END_ELLIPSIS;

	HDC hdc = GetWindowDC(hWnd);
	HDC dc = CreateCompatibleDC(hdc);
	HDC dc2 = CreateCompatibleDC(hdc);
	HBRUSH bkgndbrush = CreateSolidBrush(RGB(0,0,16));
	GetWindowRect(hWnd, &r);
	r.right -= r.left;
	r.bottom -= r.top;
	r.left = r.top = 0;

	bmp = CreateCompatibleBitmap(hdc, r.right, r.bottom);
	SelectObject(dc2, bmp);

	rt.left = 0;
	rt.top = skin->dlg_frame;
	rt.bottom = skin->title.img->height + skin->dlg_frame;
	rt.right = r.right;

	rl.top = rt.bottom;
	rl.left = 0;
	rl.right = skin->border_left.img->width;
	rl.bottom = r.bottom - skin->border_bottom.img->height - skin->dlg_frame;

	rr.top = rt.bottom;
	rr.left = r.right - skin->border_right.img->width;
	rr.right = r.right;
	rr.bottom = r.bottom - skin->border_bottom.img->height - skin->dlg_frame;

	rb.left = 0;
	rb.right = r.right;
	rb.top = r.bottom - skin->border_bottom.img->height - skin->dlg_frame;
	rb.bottom = r.bottom - skin->dlg_frame;

	if(skin->enable_appskin && skin->border_ok && fill) {
		rb.top -= skin->background_height;
		FillRect(dc2, &rb, bkgndbrush);
		rb.top += skin->background_height;
	}

	SetStretchBltMode(dc2, COLORONCOLOR);

	bg_left = skin->title_left.width;
	bg_width = rt.right-rt.left-skin->title_right.width;
	SelectObject(dc, skin->title.bmp);

	StretchBlt(dc2, bg_left, rt.top, bg_width, skin->title.img->height
		, dc, 0, 0, skin->title.img->width, skin->title.img->height, SRCCOPY);

	if(skin->title_left.width > 0) {
		SelectObject(dc, skin->title_left.bmp);
		BitBlt(dc2, 0, rt.bottom-skin->title.img->height
			, skin->title_left.width, skin->title.img->height, dc, 0, 0, SRCCOPY);
	}

	if(skin->title_right.width > 0) {
		SelectObject(dc, skin->title_right.bmp);
		BitBlt(dc2, rt.right-skin->title_right.width, rt.bottom-skin->title.img->height
			, skin->title_right.width, skin->title.img->height, dc, 0, 0, SRCCOPY);
	}

	if(skin->title_text.show && title_text) {
		rc.top = skin->title_text.top + skin->dlg_frame;
		rc.bottom = skin->title_text.top + skin->dlg_frame + skin->title_text.height;
		rc.left = rt.left;
		rc.right = rt.right;
		if(skin->title_text.left > 0)
			rc.left += skin->title_text.left;
		if(skin->title_text.right > 0)
			rc.right -= skin->title_text.right;
		SetBkMode(dc2, TRANSPARENT);
		memset(&fontRect,0,sizeof(LOGFONT));
		fontRect.lfHeight = -skin->title_text.size;
		fontRect.lfWeight = FW_BOLD;
		lstrcpy(fontRect.lfFaceName,"MS Shell Dlg");
		SelectObject(dc2, CreateFontIndirect(&fontRect));
		SetTextColor(dc2, skin->title_text.color);
		if(skin->title_text.alignment == 1)
			Format |= DT_LEFT;
		else if(skin->title_text.alignment == 2)
			Format |= DT_RIGHT;
		else
			Format |= DT_CENTER;
		DrawText(dc2, title_text, -1, &rc, Format | DT_NOPREFIX);
	}

	if(skin->btn_menu.exist) {
		SelectObject(dc, skin->btn_menu.bmp);
		if(skin->btn_menu.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_menu.right - skin->btn_menu.width
				, skin->btn_menu.top + skin->dlg_frame, skin->btn_menu.width, skin->btn_menu.height
				, dc, skin->btn_menu.width * skin->btn_menu.state, 0
				, skin->btn_menu.width, skin->btn_menu.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_menu.left, skin->btn_menu.top + skin->dlg_frame, skin->btn_menu.width
				, skin->btn_menu.height, dc, skin->btn_menu.width * skin->btn_menu.state, 0
				, skin->btn_menu.width, skin->btn_menu.height, RGB(255,0,255));
		}
	}

	if(skin->btn_min.exist) {
		SelectObject(dc, skin->btn_min.bmp);
		if(skin->btn_min.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_min.right - skin->btn_min.width
				, skin->btn_min.top + skin->dlg_frame, skin->btn_min.width, skin->btn_min.height
				, dc, skin->btn_min.width * skin->btn_min.state, 0
				, skin->btn_min.width, skin->btn_min.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_min.left, skin->btn_min.top + skin->dlg_frame, skin->btn_min.width
				, skin->btn_min.height, dc, skin->btn_min.width * skin->btn_min.state, 0
				, skin->btn_min.width, skin->btn_min.height, RGB(255,0,255));
		}
	}

	if(skin->btn_max.exist) {
		SelectObject(dc, skin->btn_max.bmp);
		if(skin->btn_max.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_max.right - skin->btn_max.width
				, skin->btn_max.top + skin->dlg_frame, skin->btn_max.width, skin->btn_max.height
				, dc, skin->btn_max.width * skin->btn_max.state, 0
				, skin->btn_max.width, skin->btn_max.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_max.left, skin->btn_max.top + skin->dlg_frame, skin->btn_max.width
				, skin->btn_max.height, dc, skin->btn_max.width * skin->btn_max.state, 0
				, skin->btn_max.width, skin->btn_max.height, RGB(255,0,255));
		}
	}

	if(skin->btn_close.exist) {
		SelectObject(dc, skin->btn_close.bmp);
		if(skin->btn_close.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_close.right - skin->btn_close.width
				, skin->btn_close.top + skin->dlg_frame, skin->btn_close.width, skin->btn_close.height
				, dc, skin->btn_close.width * skin->btn_close.state, 0
				, skin->btn_close.width, skin->btn_close.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_close.left, skin->btn_close.top + skin->dlg_frame, skin->btn_close.width
				, skin->btn_close.height, dc, skin->btn_close.width * skin->btn_close.state, 0
				, skin->btn_close.width, skin->btn_close.height, RGB(255,0,255));
		}
	}

	bg_top = rl.top;
	bg_height = rl.bottom-skin->border_left.height;
	SelectObject(dc, skin->border_left.bmp);
	StretchBlt(dc2, rl.left, bg_top, skin->border_left.img->width, bg_height
		, dc, 0, 0, skin->border_left.img->width, skin->border_left.img->height, SRCCOPY);

	bg_top = rr.top;
	bg_height = rr.bottom-skin->border_right.height;
	SelectObject(dc, skin->border_right.bmp);
	StretchBlt(dc2, rr.left, bg_top, skin->border_right.img->width, bg_height
		, dc, 0, 0, skin->border_right.img->width, skin->border_right.img->height, SRCCOPY);

	bg_left = skin->bottom_left.width;
	bg_width = rb.right-rb.left-skin->bottom_right.width;
	SelectObject(dc, skin->border_bottom.bmp);
	StretchBlt(dc2, bg_left, rb.top, bg_width, skin->border_bottom.img->height
		, dc, 0, 0, skin->border_bottom.img->width, skin->border_bottom.img->height, SRCCOPY);

	if(skin->bottom_left.width > 0) {
		SelectObject(dc, skin->bottom_left.bmp);
		BitBlt(dc2, 0, rb.bottom-skin->bottom_left.height
			, skin->bottom_left.width, skin->bottom_left.img->height, dc, 0, 0, SRCCOPY);
	}

	if(skin->bottom_right.width > 0) {
		SelectObject(dc, skin->bottom_right.bmp);
		BitBlt(dc2, rb.right-skin->bottom_right.width, rb.bottom-skin->bottom_right.height
			, skin->bottom_right.width, skin->bottom_right.img->height, dc, 0, 0, SRCCOPY);
	}

	BitBlt(hdc, rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top, dc2, rt.left, rt.top, SRCCOPY);
	BitBlt(hdc, rl.left, rl.top, rl.right-rl.left, rl.bottom-rl.top, dc2, rl.left, rl.top, SRCCOPY);
	BitBlt(hdc, rr.left, rr.top, rr.right-rr.left, rr.bottom-rr.top, dc2, rr.left, rr.top, SRCCOPY);
	BitBlt(hdc, rb.left, rb.top, rb.right-rb.left, rb.bottom-rb.top, dc2, rb.left, rb.top, SRCCOPY);
	DeleteObject(bkgndbrush);
	DeleteObject(bmp);
	DeleteDC(dc2);
	DeleteDC(dc);
	ReleaseDC(hWnd, hdc);

	return 0;
}

int draw_title_text(HWND hWnd, skin_t *skin, char *title_text)
{
	RECT r, rc, rt;
	HBITMAP bmp;
	LOGFONT fontRect;
	int bg_left, bg_width;
	HDC hdc, dc, dc2;
	UINT Format = DT_END_ELLIPSIS;

	if(!skin->title_text.show || !title_text)
		return -1;

	hdc = GetWindowDC(hWnd);
	dc = CreateCompatibleDC(hdc);
	dc2 = CreateCompatibleDC(hdc);
	GetWindowRect(hWnd, &r);
	r.right -= r.left;
	r.bottom -= r.top;
	r.left = r.top = 0;

	bmp = CreateCompatibleBitmap(hdc, r.right, r.bottom);
	SelectObject(dc2, bmp);

	rt.left = rt.top = 0;
	rt.bottom = skin->title.img->height;
	rt.right = r.right;
	SetStretchBltMode(dc2, COLORONCOLOR);

	bg_left = skin->title_left.width;
	bg_width = rt.right-rt.left-skin->title_right.width;
	SelectObject(dc, skin->title.bmp);

	StretchBlt(dc2, bg_left, rt.top, bg_width, skin->title.img->height
		, dc, 0, 0, skin->title.img->width, skin->title.img->height, SRCCOPY);

	if(skin->title_left.width > 0) {
		SelectObject(dc, skin->title_left.bmp);
		BitBlt(dc2, 0, rt.bottom-skin->title.img->height
			, skin->title_left.width, skin->title.img->height, dc, 0, 0, SRCCOPY);
	}

	if(skin->title_right.width > 0) {
		SelectObject(dc, skin->title_right.bmp);
		BitBlt(dc2, rt.right-skin->title_right.width, rt.bottom-skin->title.img->height
			, skin->title_right.width, skin->title.img->height, dc, 0, 0, SRCCOPY);
	}

	if(skin->title_text.show && title_text) {
		rc.top = skin->title_text.top;
		rc.bottom = skin->title_text.top + skin->title_text.height;
		rc.left = rt.left;
		rc.right = rt.right;
		if(skin->title_text.left > 0)
			rc.left += skin->title_text.left;
		if(skin->title_text.right > 0)
			rc.right -= skin->title_text.right;
		SetBkMode(dc2, TRANSPARENT);
		memset(&fontRect,0,sizeof(LOGFONT));
		fontRect.lfHeight = -skin->title_text.size;
		fontRect.lfWeight = FW_BOLD;
		lstrcpy(fontRect.lfFaceName,"MS Shell Dlg");
		SelectObject(dc2, CreateFontIndirect(&fontRect));
		SetTextColor(dc2, skin->title_text.color);
		if(skin->title_text.alignment == 1)
			Format |= DT_LEFT;
		else if(skin->title_text.alignment == 2)
			Format |= DT_RIGHT;
		else
			Format |= DT_CENTER;
		DrawText(dc2, title_text, -1, &rc, Format | DT_NOPREFIX);
	}

	if(skin->btn_menu.exist) {
		SelectObject(dc, skin->btn_menu.bmp);
		if(skin->btn_menu.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_menu.right - skin->btn_menu.width
				, skin->btn_menu.top, skin->btn_menu.width, skin->btn_menu.height
				, dc, skin->btn_menu.width * skin->btn_menu.state, 0
				, skin->btn_menu.width, skin->btn_menu.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_menu.left, skin->btn_menu.top, skin->btn_menu.width
				, skin->btn_menu.height, dc, skin->btn_menu.width * skin->btn_menu.state, 0
				, skin->btn_menu.width, skin->btn_menu.height, RGB(255,0,255));
		}
	}

	if(skin->btn_min.exist) {
		SelectObject(dc, skin->btn_min.bmp);
		if(skin->btn_min.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_min.right - skin->btn_min.width
				, skin->btn_min.top, skin->btn_min.width, skin->btn_min.height
				, dc, skin->btn_min.width * skin->btn_min.state, 0
				, skin->btn_min.width, skin->btn_min.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_min.left, skin->btn_min.top, skin->btn_min.width
				, skin->btn_min.height, dc, skin->btn_min.width * skin->btn_min.state, 0
				, skin->btn_min.width, skin->btn_min.height, RGB(255,0,255));
		}
	}

	if(skin->btn_max.exist) {
		SelectObject(dc, skin->btn_max.bmp);
		if(skin->btn_max.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_max.right - skin->btn_max.width
				, skin->btn_max.top, skin->btn_max.width, skin->btn_max.height
				, dc, skin->btn_max.width * skin->btn_max.state, 0
				, skin->btn_max.width, skin->btn_max.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_max.left, skin->btn_max.top, skin->btn_max.width
				, skin->btn_max.height, dc, skin->btn_max.width * skin->btn_max.state, 0
				, skin->btn_max.width, skin->btn_max.height, RGB(255,0,255));
		}
	}

	if(skin->btn_close.exist) {
		SelectObject(dc, skin->btn_close.bmp);
		if(skin->btn_close.right >= 0) {
			TransparentBlt(dc2, rt.right - skin->btn_close.right - skin->btn_close.width
				, skin->btn_close.top, skin->btn_close.width, skin->btn_close.height
				, dc, skin->btn_close.width * skin->btn_close.state, 0
				, skin->btn_close.width, skin->btn_close.height, RGB(255,0,255));
		} else {
			TransparentBlt(dc2, skin->btn_close.left, skin->btn_close.top, skin->btn_close.width
				, skin->btn_close.height, dc, skin->btn_close.width * skin->btn_close.state, 0
				, skin->btn_close.width, skin->btn_close.height, RGB(255,0,255));
		}
	}

	BitBlt(hdc, rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top, dc2, rt.left, rt.top, SRCCOPY);
	DeleteObject(bmp);
	DeleteDC(dc2);
	DeleteDC(dc);
	ReleaseDC(hWnd, hdc);
	return 0;
}

int on_paint(skin_t *skin, HDC hdc, HWND hDlg, HBRUSH bkgndbrush, RECT r, int paused)
{
	RECT seek_rc;
	int img_left, img_width, btn_left, volumeleft;
	HDC dc1 = CreateCompatibleDC(hdc);
	HDC dc2 = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, r.right, r.bottom);

	SelectObject(dc1, bmp);
	FillRect(dc1, &r, bkgndbrush);
	SetStretchBltMode(dc1, COLORONCOLOR);

	img_left = skin->background_left.width;
	img_width = r.right-r.left-skin->background_right.width;
	SelectObject(dc2, skin->background.bmp);
	if(skin->zoom_type == 1) {
		while(img_left < img_width) {
			BitBlt(dc1, img_left, r.bottom-skin->background_height
				, skin->background.img->width, skin->background_height, dc2, 0, 0, SRCCOPY);
			img_left += skin->background.img->width;
		}
	} else {
		StretchBlt(dc1, img_left, r.bottom-skin->background_height
			, img_width-skin->background_left.width, skin->background_height
			, dc2, 0, 0, skin->background.img->width, skin->background_height, SRCCOPY);
	}

	if(skin->background_left.width > 0) {
		SelectObject(dc2, skin->background_left.bmp);
		BitBlt(dc1, 0, r.bottom-skin->background_height
			, skin->background_left.width, skin->background_height, dc2, 0, 0, SRCCOPY);
	}

	if(skin->background_right.width > 0) {
		SelectObject(dc2, skin->background_right.bmp);
		BitBlt(dc1, r.right-skin->background_right.width, r.bottom-skin->background_height
			, skin->background_right.width, skin->background_height, dc2, 0, 0, SRCCOPY);
	}

	if(skin->background_center.width > 0) {
		skin->background_center.left = (r.right-skin->background_center.width)/2;
		SelectObject(dc2, skin->background_center.bmp);
		BitBlt(dc1, skin->background_center.left, r.bottom-skin->background_height
			, skin->background_center.width, skin->background_height, dc2, 0, 0, SRCCOPY);
	}

	if(skin->btn_open.exist) {
		SelectObject(dc2, skin->btn_open.bmp);
		btn_left = get_button_left(skin, &skin->btn_open, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_open.top, skin->btn_open.width, skin->btn_open.height
			, dc2, skin->btn_open.width * skin->btn_open.state, 0, SRCCOPY);
	}

	if(skin->btn_fullscreen.exist) {
		SelectObject(dc2, skin->btn_fullscreen.bmp);
		btn_left = get_button_left(skin, &skin->btn_fullscreen, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_fullscreen.top, skin->btn_fullscreen.width, skin->btn_fullscreen.height
			, dc2, skin->btn_fullscreen.width * skin->btn_fullscreen.state, 0, SRCCOPY);
	}

	if(skin->btn_playlist.exist) {
		SelectObject(dc2, skin->btn_playlist.bmp);
		btn_left = get_button_left(skin, &skin->btn_playlist, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_playlist.top, skin->btn_playlist.width, skin->btn_playlist.height
			, dc2, skin->btn_playlist.width * skin->btn_playlist.state, 0, SRCCOPY);
	}

	if(skin->btn_screenshot.exist) {
		SelectObject(dc2, skin->btn_screenshot.bmp);
		btn_left = get_button_left(skin, &skin->btn_screenshot, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_screenshot.top, skin->btn_screenshot.width, skin->btn_screenshot.height
			, dc2, skin->btn_screenshot.width * skin->btn_screenshot.state, 0, SRCCOPY);
	}

	if(skin->btn_equalizer.exist) {
		SelectObject(dc2, skin->btn_equalizer.bmp);
		btn_left = get_button_left(skin, &skin->btn_equalizer, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_equalizer.top, skin->btn_equalizer.width, skin->btn_equalizer.height
			, dc2, skin->btn_equalizer.width * skin->btn_equalizer.state, 0, SRCCOPY);
	}

	if(skin->btn_mute.exist) {
		if(is_audio_muted())
			SelectObject(dc2, skin->btn_muted.bmp);
		else
			SelectObject(dc2, skin->btn_mute.bmp);
		btn_left = get_button_left(skin, &skin->btn_mute, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_mute.top, skin->btn_mute.width, skin->btn_mute.height
			, dc2, skin->btn_mute.width * skin->btn_mute.state, 0, SRCCOPY);
	}

	if(skin->btn_stop.exist) {
		SelectObject(dc2, skin->btn_stop.bmp);
		btn_left = get_button_left(skin, &skin->btn_stop, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_stop.top, skin->btn_stop.width, skin->btn_stop.height
			, dc2, skin->btn_stop.width * skin->btn_stop.state, 0, SRCCOPY);
	}

	if(skin->btn_next.exist) {
		SelectObject(dc2, skin->btn_next.bmp);
		btn_left = get_button_left(skin, &skin->btn_next, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_next.top, skin->btn_next.width, skin->btn_next.height
			, dc2, skin->btn_next.width * skin->btn_next.state, 0, SRCCOPY);
	}

	if(skin->btn_prev.exist) {
		SelectObject(dc2, skin->btn_prev.bmp);
		btn_left = get_button_left(skin, &skin->btn_prev, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_prev.top, skin->btn_prev.width, skin->btn_prev.height
			, dc2, skin->btn_prev.width * skin->btn_prev.state, 0, SRCCOPY);
	}

	if(skin->btn_back.exist) {
		SelectObject(dc2, skin->btn_back.bmp);
		btn_left = get_button_left(skin, &skin->btn_back, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_back.top, skin->btn_back.width, skin->btn_back.height
			, dc2, skin->btn_back.width * skin->btn_back.state, 0, SRCCOPY);
	}

	if(skin->btn_forward.exist) {
		SelectObject(dc2, skin->btn_forward.bmp);
		btn_left = get_button_left(skin, &skin->btn_forward, r.right-r.left);
		BitBlt(dc1, btn_left, skin->btn_forward.top, skin->btn_forward.width, skin->btn_forward.height
			, dc2, skin->btn_forward.width * skin->btn_forward.state, 0, SRCCOPY);
	}

	if(skin->btn_play.exist) {
		if(paused)
			SelectObject(dc2, skin->btn_play.bmp);
		else
			SelectObject(dc2, skin->btn_pause.bmp);
		btn_left = get_button_left(skin, &skin->btn_play, r.right-r.left);
		TransparentBlt(dc1, btn_left, skin->btn_play.top
			, skin->btn_play.width, skin->btn_play.height
			, dc2, skin->btn_play.width * skin->btn_play.state, 0
			, skin->btn_play.width, skin->btn_play.height, RGB(255,0,255));
	}

	volumeleft = get_button_left(skin, &skin->volumebar, r.right-r.left);

	SelectObject(dc2, skin->volumebar.bmp);
	BitBlt(dc1, volumeleft, skin->volumebar.top
		, skin->volumebar.img->width, skin->volumebar.img->height, dc2, 0, 0, SRCCOPY);

	SelectObject(dc2, skin->volumebar_foregnd.bmp);
	BitBlt(dc1, volumeleft, skin->volumebar.top
		, skin->volumebar.width * volpercent / 100, skin->volumebar_foregnd.img->height, dc2, 0, 0, SRCCOPY);

	if(filename && have_audio && skin->volumebar_thumb.state) {
		SelectObject(dc2, skin->volumebar_thumb.bmp);
		TransparentBlt(dc1, volumeleft + (skin->volumebar.width - skin->volumebar_thumb.width)*volpercent/100
			, skin->volumebar_thumb.top, skin->volumebar_thumb.width, skin->volumebar_thumb.height
			, dc2, skin->volumebar_thumb.width * (skin->volumebar_thumb.state-1)
		, 0, skin->volumebar_thumb.width, skin->volumebar_thumb.height, RGB(255,0,255));
	}

	get_seekbar_rect(skin, r, &seek_rc);
	img_left = seek_rc.left;
	img_width = seek_rc.right - seek_rc.left;
	SelectObject(dc2, skin->seekbar.bmp);
	if(skin->seekbar.show == 1) {
		while(img_left < skin->seekbar.left+img_width-skin->seekbar.img->width) {
			BitBlt(dc1, img_left, skin->seekbar.top
				, skin->seekbar.img->width, skin->seekbar.img->height, dc2, 0, 0, SRCCOPY);
			img_left += skin->seekbar.img->width;
		}
		if(img_left < skin->seekbar.left+img_width)
			BitBlt(dc1, img_left, skin->seekbar.top, skin->seekbar.left+img_width-img_left
				, skin->seekbar.img->height, dc2, 0, 0, SRCCOPY);
	} else {
		StretchBlt(dc1, img_left, skin->seekbar.top, img_width, skin->seekbar.img->height
			, dc2, 0, 0, skin->seekbar.img->width, skin->seekbar.img->height, SRCCOPY);
	}

	img_left = seek_rc.left;
	img_width = img_width * seekpercent / percent_value;
	SelectObject(dc2, skin->seekbar_foregnd.bmp);
	if(skin->seekbar.show == 1) {
		while(img_left <= skin->seekbar.left+img_width-skin->seekbar_foregnd.img->width) {
			BitBlt(dc1, img_left, skin->seekbar.top , skin->seekbar_foregnd.img->width
				, skin->seekbar_foregnd.img->height, dc2, 0, 0, SRCCOPY);
			img_left += skin->seekbar_foregnd.img->width;
		}
		if(img_left < skin->seekbar.left+img_width)
			BitBlt(dc1, img_left, skin->seekbar.top, skin->seekbar.left+img_width-img_left
				, skin->seekbar_foregnd.img->height, dc2, 0, 0, SRCCOPY);
	} else {
		StretchBlt(dc1, img_left, skin->seekbar.top, img_width, skin->seekbar_foregnd.img->height
			, dc2, 0, 0, skin->seekbar_foregnd.img->width, skin->seekbar_foregnd.img->height, SRCCOPY);
	}

	if(skin->seekbar_thumb.show || skin->seekbar_thumb.state) {
		SelectObject(dc2, skin->seekbar_thumb.bmp);
		TransparentBlt(dc1, seek_rc.left + img_width - skin->seekbar_thumb.width/2
			, skin->seekbar_thumb.top, skin->seekbar_thumb.width, skin->seekbar_thumb.height, dc2
			, (skin->seekbar_thumb.show && !skin->seekbar_thumb.state)?0:skin->seekbar_thumb.width * (skin->seekbar_thumb.state-1)
			, 0, skin->seekbar_thumb.width, skin->seekbar_thumb.height, RGB(255,0,255));
	}

	if(show_status && strlen(status) > 1) {
		RECT rc;
		rc.top = skin->status.top;
		rc.bottom = skin->status.top + skin->status.height;
		UINT FORMAT = DT_RIGHT;

		if(skin->status.left >= 0) {
			if(skin->status.center && skin->background_center.width > 0)
				rc.left = skin->background_center.left + skin->status.left;
			else
				rc.left = skin->status.left;
			rc.right = rc.left + skin->status.width;
			FORMAT = DT_LEFT;
		} else {
			if(skin->status.center && skin->background_center.width > 0)
				rc.right = skin->background_center.left + skin->background_center.width - skin->status.right;
			else
				rc.right = r.right - skin->status.right;
			rc.left = rc.right - skin->status.width;
		}
		SetBkMode(dc1, TRANSPARENT);
		SelectObject(dc1, GetWindowFont(hDlg));
		SetTextColor(dc1, skin->status.color);
		DrawText(dc1, status, -1, &rc, FORMAT | DT_NOPREFIX);
	}

	if(show_status && show_status2 && strlen(status_text_timer2) > 1) {
		RECT rc;
		rc.top = skin->status2.top;
		rc.bottom = skin->status2.top + skin->status2.height;
		UINT FORMAT = DT_RIGHT;

		if(skin->status2.left >= 0) {
			if(skin->status2.center && skin->background_center.width > 0)
				rc.left = skin->background_center.left + skin->status2.left;
			else
				rc.left = skin->status2.left;
			rc.right = rc.left + skin->status2.width;
			FORMAT = DT_LEFT;
		} else {
			if(skin->status2.center && skin->background_center.width > 0)
				rc.right = skin->background_center.left + skin->background_center.width - skin->status2.right;
			else
				rc.right = r.right - skin->status2.right;
			rc.left = rc.right - skin->status2.width;
		}
		SetBkMode(dc1, TRANSPARENT);
		SelectObject(dc1, GetWindowFont(hDlg));
		SetTextColor(dc1, skin->status2.color);
		DrawText(dc1, status_text_timer2, -1, &rc, FORMAT | DT_NOPREFIX);
	}

	BitBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top
		, dc1, r.left, r.top, SRCCOPY);
	DeleteObject(bmp);
	DeleteDC(dc1);
	DeleteDC(dc2);
}

int on_mouse_move(skin_t *skin, HWND hDlg, RECT r, POINT p)
{
	float percent;
	char tmps[20];
	RECT seek_rc;
	int btn_left, pos, volumeleft, refresh = 0;
	if(seek_dropping) {
		get_seekbar_rect(skin, r, &seek_rc);
		if(p.x > seek_rc.left && p.x < seek_rc.right) {
			percent = (p.x-seek_rc.left)*percent_value/(seek_rc.right - seek_rc.left);
		} else if (p.x <= seek_rc.left) {
			percent = 0;
		} else if (p.x >= seek_rc.right) {
			percent = percent_value;
		}
		if(seek_realtime) {
			if(GetTickCount() > last_seek + 80) {
				sprintf(tmps, "seek %f 1", percent*100.0/percent_value);
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				last_seek = GetTickCount();
			}
		} else if(show_status) {
			pos = get_seek_pos(percent*100.0/percent_value);
			snprintf(tmps, 16, " %02d:%02d:%02d", pos/3600, (pos/60)%60, pos%60);
			strncpy(status, tmps, 9);
		}
		seekpercent = (int)percent;
		refresh = 1;
	} else if(volume_dropping) {
		volumeleft = get_button_left(skin, &skin->volumebar, r.right-r.left);
		if (p.x > volumeleft && p.x < volumeleft + skin->volumebar.width) {
			volpercent = (p.x - volumeleft)*100/(skin->volumebar.width);
		} else if(p.x <= volumeleft) {
			volpercent = 0;
		}  else if (p.x >= volumeleft + skin->volumebar.width) {
			volpercent = 100;
		}
		sprintf(tmps, "volume %d 1", volpercent);
		mp_input_queue_cmd(mp_input_parse_cmd(tmps));
		refresh = 1;
	} else {
		if(skin->btn_play.exist) {
			btn_left = get_button_left(skin, &skin->btn_play, r.right-r.left);
			if(p.y >= skin->btn_play.top && p.y <= skin->btn_play.top + skin->btn_play.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_play.width) {
				skin->btn_play.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_play.state) {
				skin->btn_play.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}

		if(skin->btn_back.exist) {
			btn_left = get_button_left(skin, &skin->btn_back, r.right-r.left);
			if(p.y >= skin->btn_back.top && p.y <= skin->btn_back.top + skin->btn_back.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_back.width) {
				skin->btn_back.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_back.state) {
				skin->btn_back.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_forward.exist) {
			btn_left = get_button_left(skin, &skin->btn_forward, r.right-r.left);
			if(p.y >= skin->btn_forward.top && p.y <= skin->btn_forward.top + skin->btn_forward.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_forward.width) {
				skin->btn_forward.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_forward.state) {
				skin->btn_forward.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_prev.exist) {
			btn_left = get_button_left(skin, &skin->btn_prev, r.right-r.left);
			if(p.y >= skin->btn_prev.top && p.y <= skin->btn_prev.top + skin->btn_prev.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_prev.width) {
				skin->btn_prev.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_prev.state) {
				skin->btn_prev.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_next.exist) {
			btn_left = get_button_left(skin, &skin->btn_next, r.right-r.left);
			if(p.y >= skin->btn_next.top && p.y <= skin->btn_next.top + skin->btn_next.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_next.width) {
				skin->btn_next.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_next.state) {
				skin->btn_next.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_stop.exist) {
			btn_left = get_button_left(skin, &skin->btn_stop, r.right-r.left);
			if(p.y >= skin->btn_stop.top && p.y <= skin->btn_stop.top + skin->btn_stop.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_stop.width) {
				skin->btn_stop.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_stop.state) {
				skin->btn_stop.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}

		if(skin->btn_fullscreen.exist) {
			btn_left = get_button_left(skin, &skin->btn_fullscreen, r.right-r.left);
			if(p.y >= skin->btn_fullscreen.top && p.y <= skin->btn_fullscreen.top + skin->btn_fullscreen.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_fullscreen.width) {
				skin->btn_fullscreen.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_fullscreen.state) {
				skin->btn_fullscreen.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_playlist.exist) {
			btn_left = get_button_left(skin, &skin->btn_playlist, r.right-r.left);
			if(p.y >= skin->btn_playlist.top && p.y <= skin->btn_playlist.top + skin->btn_playlist.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_playlist.width) {
				skin->btn_playlist.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_playlist.state) {
				skin->btn_playlist.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_open.exist) {
			btn_left = get_button_left(skin, &skin->btn_open, r.right-r.left);
			if(p.y >= skin->btn_open.top && p.y <= skin->btn_open.top + skin->btn_open.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_open.width) {
				skin->btn_open.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_open.state) {
				skin->btn_open.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_mute.exist) {
			btn_left = get_button_left(skin, &skin->btn_mute, r.right-r.left);
			if(p.y >= skin->btn_mute.top && p.y <= skin->btn_mute.top + skin->btn_mute.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_mute.width) {
				skin->btn_mute.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_mute.state) {
				skin->btn_mute.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_screenshot.exist) {
			btn_left = get_button_left(skin, &skin->btn_screenshot, r.right-r.left);
			if(p.y >= skin->btn_screenshot.top && p.y <= skin->btn_screenshot.top + skin->btn_screenshot.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_screenshot.width) {
				skin->btn_screenshot.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_screenshot.state) {
				skin->btn_screenshot.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}
		if(skin->btn_equalizer.exist) {
			btn_left = get_button_left(skin, &skin->btn_equalizer, r.right-r.left);
			if(p.y >= skin->btn_equalizer.top && p.y <= skin->btn_equalizer.top + skin->btn_equalizer.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_equalizer.width) {
				skin->btn_equalizer.state = 1;
				refresh = 1;
				SetCapture(hDlg);
			} else if(skin->btn_equalizer.state) {
				skin->btn_equalizer.state = 0;
				refresh = 1;
				ReleaseCapture();
			}
		}

		get_seekbar_rect(skin, r, &seek_rc);
		if(filename && p.y >= seek_rc.top-2 && p.y <= seek_rc.bottom
			&& p.x > seek_rc.left && p.x <= seek_rc.right) {
			skin->seekbar_thumb.state = 2;
			refresh = 1;
			SetCapture(hDlg);
		} else if(skin->seekbar_thumb.state) {
			skin->seekbar_thumb.state = 0;
			refresh = 1;
			ReleaseCapture();
		}

		volumeleft = get_button_left(skin, &skin->volumebar, r.right-r.left);
		if (filename && have_audio && p.y >= skin->volumebar.top-2 && p.y <= skin->volumebar.top + skin->volumebar.height
			&& p.x > volumeleft && p.x <= volumeleft + skin->volumebar.width) {
			skin->volumebar_thumb.state = 2;
			refresh = 1;
			SetCapture(hDlg);
		} else if(have_audio && skin->volumebar_thumb.state > 1) {
			skin->volumebar_thumb.state = 1;
			refresh = 1;
			ReleaseCapture();
		}
	}
	return refresh;
}

int on_lbutton_down(skin_t *skin, HWND hDlg, RECT r, POINT p, HWND hCmd)
{
	int btn_left, volumeleft, refresh = 0;
	RECT seek_rc;

	get_seekbar_rect(skin, r, &seek_rc);

	if(skin->btn_play.exist) {
		btn_left = get_button_left(skin, &skin->btn_play, r.right-r.left);
		if(p.y >= skin->btn_play.top && p.y <= skin->btn_play.top + skin->btn_play.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_play.width) {
			skin->btn_play.state = 2;
			refresh = 1;
			PostMessage(hCmd, WM_COMMAND, IDM_PLAY_PAUSE, NULL);
		} else if(skin->btn_play.state) {
			skin->btn_play.state = 0;
			refresh = 1;
		}
	}

	if(skin->btn_back.exist) {
		btn_left = get_button_left(skin, &skin->btn_back, r.right-r.left);
		if(p.y >= skin->btn_back.top && p.y <= skin->btn_back.top + skin->btn_back.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_back.width) {
			skin->btn_back.state = 2;
			refresh = 1;
			mp_input_queue_cmd(mp_input_parse_cmd("seek -30"));
		} else if(skin->btn_back.state) {
			skin->btn_back.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_forward.exist) {
		btn_left = get_button_left(skin, &skin->btn_forward, r.right-r.left);
		if(p.y >= skin->btn_forward.top && p.y <= skin->btn_forward.top + skin->btn_forward.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_forward.width) {
			skin->btn_forward.state = 2;
			refresh = 1;
			mp_input_queue_cmd(mp_input_parse_cmd("seek 30"));
		} else if(skin->btn_forward.state) {
			skin->btn_forward.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_prev.exist) {
		btn_left = get_button_left(skin, &skin->btn_prev, r.right-r.left);
		if(p.y >= skin->btn_prev.top && p.y <= skin->btn_prev.top + skin->btn_prev.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_prev.width) {
			skin->btn_prev.state = 2;
			refresh = 1;
			mp_input_queue_cmd(mp_input_parse_cmd("pt_step -1"));
		} else if(skin->btn_prev.state) {
			skin->btn_prev.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_next.exist) {
		btn_left = get_button_left(skin, &skin->btn_next, r.right-r.left);
		if(p.y >= skin->btn_next.top && p.y <= skin->btn_next.top + skin->btn_next.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_next.width) {
			skin->btn_next.state = 2;
			refresh = 1;
			mp_input_queue_cmd(mp_input_parse_cmd("pt_step 1"));
		} else if(skin->btn_next.state) {
			skin->btn_next.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_stop.exist) {
		btn_left = get_button_left(skin, &skin->btn_stop, r.right-r.left);
		if(p.y >= skin->btn_stop.top && p.y <= skin->btn_stop.top + skin->btn_stop.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_stop.width) {
			skin->btn_stop.state = 2;
			refresh = 1;
			mp_input_queue_cmd(mp_input_parse_cmd("stop"));
		} else if(skin->btn_stop.state) {
			skin->btn_stop.state = 0;
			refresh = 1;
		}
	}

	if(skin->btn_fullscreen.exist) {
		btn_left = get_button_left(skin, &skin->btn_fullscreen, r.right-r.left);
		if(p.y >= skin->btn_fullscreen.top && p.y <= skin->btn_fullscreen.top + skin->btn_fullscreen.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_fullscreen.width) {
			skin->btn_fullscreen.state = 2;
			refresh = 1;
		} else if(skin->btn_fullscreen.state) {
			skin->btn_fullscreen.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_playlist.exist) {
		btn_left = get_button_left(skin, &skin->btn_playlist, r.right-r.left);
		if(p.y >= skin->btn_playlist.top && p.y <= skin->btn_playlist.top + skin->btn_playlist.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_playlist.width) {
			skin->btn_playlist.state = 2;
			refresh = 1;
		} else if(skin->btn_playlist.state) {
			skin->btn_playlist.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_open.exist) {
		btn_left = get_button_left(skin, &skin->btn_open, r.right-r.left);
		if(p.y >= skin->btn_open.top && p.y <= skin->btn_open.top + skin->btn_open.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_open.width) {
			skin->btn_open.state = 2;
			refresh = 1;
		} else if(skin->btn_open.state) {
			skin->btn_open.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_mute.exist) {
		btn_left = get_button_left(skin, &skin->btn_mute, r.right-r.left);
		if(p.y >= skin->btn_mute.top && p.y <= skin->btn_mute.top + skin->btn_mute.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_mute.width) {
			skin->btn_mute.state = 2;
			refresh = 1;
		} else if(skin->btn_mute.state) {
			skin->btn_mute.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_screenshot.exist) {
		btn_left = get_button_left(skin, &skin->btn_screenshot, r.right-r.left);
		if(p.y >= skin->btn_screenshot.top && p.y <= skin->btn_screenshot.top + skin->btn_screenshot.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_screenshot.width) {
			skin->btn_screenshot.state = 2;
			refresh = 1;
		} else if(skin->btn_screenshot.state) {
			skin->btn_screenshot.state = 0;
			refresh = 1;
		}
	}
	if(skin->btn_equalizer.exist) {
		btn_left = get_button_left(skin, &skin->btn_equalizer, r.right-r.left);
		if(p.y >= skin->btn_equalizer.top && p.y <= skin->btn_equalizer.top + skin->btn_equalizer.height
			&& p.x >= btn_left && p.x <= btn_left + skin->btn_equalizer.width) {
			skin->btn_equalizer.state = 2;
			refresh = 1;
		} else if(skin->btn_equalizer.state) {
			skin->btn_equalizer.state = 0;
			refresh = 1;
		}
	}

	if(filename && p.y >= seek_rc.top-2 && p.y <= seek_rc.bottom
		&& p.x > seek_rc.left && p.x <= seek_rc.right) {
		skin->seekbar_thumb.state = 3;
		refresh = 1;
		seek_dropping = 1;
		SetCapture(hDlg);
	} else if(skin->seekbar_thumb.state) {
		skin->seekbar_thumb.state = 0;
		refresh = 1;
	}

	volumeleft = get_button_left(skin, &skin->volumebar, r.right-r.left);
	if (filename && have_audio && p.y >= skin->volumebar.top-2 && p.y <= skin->volumebar.top + skin->volumebar.height
		&& p.x > volumeleft && p.x <= volumeleft + skin->volumebar.width) {
		skin->volumebar_thumb.state = 3;
		refresh = 1;
		volume_dropping = 1;
		SetCapture(hDlg);
	} else if(have_audio && skin->volumebar_thumb.state > 1) {
		skin->volumebar_thumb.state = 1;
		refresh = 1;
	}

	return refresh;
}

int on_lbutton_up(skin_t *skin, HWND hDlg, RECT r, POINT p, HWND hCmd)
{
	float x;
	char tmps[20];
	int btn_left, volumeleft, refresh = 0;
	RECT seek_rc;

	ReleaseCapture();

	if(seek_dropping) {
		seek_dropping = 0;
		volume_dropping = 0;
		get_seekbar_rect(skin, r, &seek_rc);
		if(p.x > seek_rc.left && p.x < seek_rc.right) {
			x = (float)(p.x-seek_rc.left)*100.0/(float)(seek_rc.right - seek_rc.left);
		} else if (p.x <= seek_rc.left) {
			x = 0;
		} else if (p.x >= seek_rc.right) {
			x = 100;
		}
		if(!seek_realtime || !last_seek) {
			if(is_asf_format)
				sprintf(tmps, "seek %f 1", x);
			else
				sprintf(tmps, "seek %d", get_seek_offset(x));
			mp_input_queue_cmd(mp_input_parse_cmd(tmps));
		}
		last_seek = 0;
		if(p.y >= seek_rc.top-2 && p.y <= seek_rc.bottom
			&& p.x > seek_rc.left && p.x <= seek_rc.right)
			skin->seekbar_thumb.state = 1;
		else
			skin->seekbar_thumb.state = 0;
		refresh = 1;
	} else if (volume_dropping) {
		seek_dropping = 0;
		volume_dropping = 0;

		volumeleft = get_button_left(skin, &skin->volumebar, r.right-r.left);
		if (p.x > volumeleft && p.x < volumeleft + skin->volumebar.width) {
			x = (p.x - volumeleft)*100/(skin->volumebar.width);
		} else if(p.x <= volumeleft) {
			x = 0;
		}  else if (p.x >= volumeleft + skin->volumebar.width) {
			x = 100;
		}
		sprintf(tmps, "volume %d 1", (int)x);
		mp_input_queue_cmd(mp_input_parse_cmd(tmps));
		if (have_audio && p.y >= skin->volumebar.top-2 && p.y <= skin->volumebar.top + skin->volumebar.height
			&& p.x > volumeleft && p.x <= volumeleft + skin->volumebar.width)
			skin->volumebar_thumb.state = 2;
		else if(have_audio)
			skin->volumebar_thumb.state = 1;
		refresh = 1;
	} else {
		if(skin->btn_play.exist) {
			btn_left = get_button_left(skin, &skin->btn_play, r.right-r.left);
			if(p.y >= skin->btn_play.top && p.y <= skin->btn_play.top + skin->btn_play.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_play.width) {
				skin->btn_play.state = 1;
				refresh = 1;
			} else if(skin->btn_play.state) {
				skin->btn_play.state = 0;
				refresh = 1;
			}
		}

		if(skin->btn_back.exist) {
			btn_left = get_button_left(skin, &skin->btn_back, r.right-r.left);
			if(p.y >= skin->btn_back.top && p.y <= skin->btn_back.top + skin->btn_back.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_back.width) {
				skin->btn_back.state = 1;
				refresh = 1;
			} else if(skin->btn_back.state) {
				skin->btn_back.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_forward.exist) {
			btn_left = get_button_left(skin, &skin->btn_forward, r.right-r.left);
			if(p.y >= skin->btn_forward.top && p.y <= skin->btn_forward.top + skin->btn_forward.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_forward.width) {
				skin->btn_forward.state = 1;
				refresh = 1;
			} else if(skin->btn_forward.state) {
				skin->btn_forward.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_prev.exist) {
			btn_left = get_button_left(skin, &skin->btn_prev, r.right-r.left);
			if(p.y >= skin->btn_prev.top && p.y <= skin->btn_prev.top + skin->btn_prev.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_prev.width) {
				skin->btn_prev.state = 1;
				refresh = 1;
			} else if(skin->btn_prev.state) {
				skin->btn_prev.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_next.exist) {
			btn_left = get_button_left(skin, &skin->btn_next, r.right-r.left);
			if(p.y >= skin->btn_next.top && p.y <= skin->btn_next.top + skin->btn_next.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_next.width) {
				skin->btn_next.state = 1;
				refresh = 1;
			} else if(skin->btn_next.state) {
				skin->btn_next.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_stop.exist) {
			btn_left = get_button_left(skin, &skin->btn_stop, r.right-r.left);
			if(p.y >= skin->btn_stop.top && p.y <= skin->btn_stop.top + skin->btn_stop.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_stop.width) {
				skin->btn_stop.state = 1;
				refresh = 1;
			} else if(skin->btn_stop.state) {
				skin->btn_stop.state = 0;
				refresh = 1;
			}
		}

		if(skin->btn_fullscreen.exist) {
			btn_left = get_button_left(skin, &skin->btn_fullscreen, r.right-r.left);
			if(p.y >= skin->btn_fullscreen.top && p.y <= skin->btn_fullscreen.top + skin->btn_fullscreen.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_fullscreen.width) {
				skin->btn_fullscreen.state = 1;
				refresh = 1;
				if(filename)
					mp_input_queue_cmd(mp_input_parse_cmd("vo_fullscreen"));
			} else if(skin->btn_fullscreen.state) {
				skin->btn_fullscreen.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_playlist.exist) {
			btn_left = get_button_left(skin, &skin->btn_playlist, r.right-r.left);
			if(p.y >= skin->btn_playlist.top && p.y <= skin->btn_playlist.top + skin->btn_playlist.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_playlist.width) {
				skin->btn_playlist.state = 1;
				refresh = 1;
				PostMessage(hCmd, WM_COMMAND, IDM_PLAYLIST, NULL);
			} else if(skin->btn_playlist.state) {
				skin->btn_playlist.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_open.exist) {
			btn_left = get_button_left(skin, &skin->btn_open, r.right-r.left);
			if(p.y >= skin->btn_open.top && p.y <= skin->btn_open.top + skin->btn_open.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_open.width) {
				skin->btn_open.state = 1;
				refresh = 1;
				PostMessage(hCmd, WM_COMMAND, IDM_OPEN, NULL);
			} else if(skin->btn_open.state) {
				skin->btn_open.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_mute.exist) {
			btn_left = get_button_left(skin, &skin->btn_mute, r.right-r.left);
			if(p.y >= skin->btn_mute.top && p.y <= skin->btn_mute.top + skin->btn_mute.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_mute.width) {
				skin->btn_mute.state = 1;
				refresh = 1;
				if(filename)
					mp_input_queue_cmd(mp_input_parse_cmd("mute"));
			} else if(skin->btn_mute.state) {
				skin->btn_mute.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_screenshot.exist) {
			btn_left = get_button_left(skin, &skin->btn_screenshot, r.right-r.left);
			if(p.y >= skin->btn_screenshot.top && p.y <= skin->btn_screenshot.top + skin->btn_screenshot.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_screenshot.width) {
				skin->btn_screenshot.state = 1;
				refresh = 1;
				if(filename)
					mp_input_queue_cmd(mp_input_parse_cmd("screenshot"));
			} else if(skin->btn_screenshot.state) {
				skin->btn_screenshot.state = 0;
				refresh = 1;
			}
		}
		if(skin->btn_equalizer.exist) {
			btn_left = get_button_left(skin, &skin->btn_equalizer, r.right-r.left);
			if(p.y >= skin->btn_equalizer.top && p.y <= skin->btn_equalizer.top + skin->btn_equalizer.height
				&& p.x >= btn_left && p.x <= btn_left + skin->btn_equalizer.width) {
				skin->btn_equalizer.state = 1;
				refresh = 1;
				PostMessage(hCmd, WM_COMMAND, IDM_OPENEQ, NULL);
			} else if(skin->btn_equalizer.state) {
				skin->btn_equalizer.state = 0;
				refresh = 1;
			}
		}

		if(!bSeeking && filename) {
			volumeleft = get_button_left(skin, &skin->volumebar, r.right-r.left);
			if(p.y >= seek_rc.top-2 && p.y <= seek_rc.bottom
				&& p.x > seek_rc.left && p.x <= seek_rc.right) {
				x =  (float)(p.x-seek_rc.left)*100.0/ (float)(seek_rc.right - seek_rc.left);
				if(is_asf_format)
					sprintf(tmps, "seek %f 1", x);
				else
					sprintf(tmps, "seek %d", get_seek_offset(x));
				bSeeking = 1;
			} else if (p.y >= skin->volumebar.top-2 && p.y <= skin->volumebar.top + skin->volumebar.height
				&& p.x > volumeleft && p.x <= volumeleft + skin->volumebar.width) {
				x = (p.x-volumeleft)*100/(skin->volumebar.width);
				sprintf(tmps, "volume %d 1", (int)x);
			}
			mp_input_queue_cmd(mp_input_parse_cmd(tmps));
			refresh = 1;
		}
	}
	
	return refresh;
}

void on_mouse_leave(skin_t *skin)
{
	seek_dropping = 0;
	volume_dropping = 0;
	skin->seekbar_thumb.state = 0;
	skin->volumebar_thumb.state = 1;
	skin->btn_play.state = 0;
	skin->btn_back.state = 0;
	skin->btn_forward.state = 0;
	skin->btn_prev.state = 0;
	skin->btn_next.state = 0;
	skin->btn_stop.state = 0;
	skin->btn_fullscreen.state = 0;
	skin->btn_playlist.state = 0;
	skin->btn_open.state = 0;
	skin->btn_mute.state = 0;
	skin->btn_screenshot.state = 0;
	skin->btn_equalizer.state = 0;
}

int on_ncmouse_move(skin_t *skin, HWND hWnd, char *title_text)
{
	RECT r, rcb;
	POINT p;

	if(!skin || !hWnd || !skin->enable_appskin || !skin->border_ok)
		return 0;

	GetCursorPos(&p);
	GetWindowRect(hWnd, &r);
	p.x -= r.left;
	p.y -= r.top;
	r.right -= r.left;

	if(skin->btn_menu.exist) {
		rcb.top = skin->btn_menu.top;
		rcb.bottom = rcb.top + skin->btn_menu.height;
		if(skin->btn_menu.right >= 0)
			rcb.left = r.right - skin->btn_menu.right - skin->btn_menu.width;
		else
			rcb.left = skin->btn_menu.left;
		rcb.right = rcb.left+skin->btn_menu.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_menu.state = 1;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_menu.state) {
			skin->btn_menu.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_min.exist) {
		rcb.top = skin->btn_min.top;
		rcb.bottom = rcb.top + skin->btn_min.height;
		if(skin->btn_min.right >= 0)
			rcb.left = r.right - skin->btn_min.right - skin->btn_min.width;
		else
			rcb.left = skin->btn_min.left;
		rcb.right = rcb.left+skin->btn_min.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_min.state = 1;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_min.state) {
			skin->btn_min.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_max.exist) {
		rcb.top = skin->btn_max.top;
		rcb.bottom = rcb.top + skin->btn_max.height;
		if(skin->btn_max.right >= 0)
			rcb.left = r.right - skin->btn_max.right - skin->btn_max.width;
		else
			rcb.left = skin->btn_max.left;
		rcb.right = rcb.left+skin->btn_max.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_max.state = 1;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_max.state) {
			skin->btn_max.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_close.exist) {
		rcb.top = skin->btn_close.top;
		rcb.bottom = rcb.top + skin->btn_close.height;
		if(skin->btn_close.right >= 0)
			rcb.left = r.right - skin->btn_close.right - skin->btn_close.width;
		else
			rcb.left = skin->btn_close.left;
		rcb.right = rcb.left+skin->btn_close.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_close.state = 1;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_close.state) {
			skin->btn_close.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}
}

int on_nclbutton_down(skin_t *skin, HWND hWnd, char *title_text)
{
	RECT r, rcb;
	POINT p;

	if(!skin || !hWnd || !skin->enable_appskin || !skin->border_ok)
		return 0;

	GetCursorPos(&p);
	GetWindowRect(hWnd, &r);
	p.x -= r.left;
	p.y -= r.top;
	r.right -= r.left;

	if(skin->btn_menu.exist) {
		rcb.top = skin->btn_menu.top;
		rcb.bottom = rcb.top + skin->btn_menu.height;
		if(skin->btn_menu.right >= 0)
			rcb.left = r.right - skin->btn_menu.right - skin->btn_menu.width;
		else
			rcb.left = skin->btn_menu.left;
		rcb.right = rcb.left+skin->btn_menu.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_menu.state = 2;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_menu.state) {
			skin->btn_menu.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_min.exist) {
		rcb.top = skin->btn_min.top;
		rcb.bottom = rcb.top + skin->btn_min.height;
		if(skin->btn_min.right >= 0)
			rcb.left = r.right - skin->btn_min.right - skin->btn_min.width;
		else
			rcb.left = skin->btn_min.left;
		rcb.right = rcb.left+skin->btn_min.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_min.state = 2;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_min.state) {
			skin->btn_min.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_max.exist) {
		rcb.top = skin->btn_max.top;
		rcb.bottom = rcb.top + skin->btn_max.height;
		if(skin->btn_max.right >= 0)
			rcb.left = r.right - skin->btn_max.right - skin->btn_max.width;
		else
			rcb.left = skin->btn_max.left;
		rcb.right = rcb.left+skin->btn_max.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_max.state = 2;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_max.state) {
			skin->btn_max.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_close.exist) {
		rcb.top = skin->btn_close.top;
		rcb.bottom = rcb.top + skin->btn_close.height;
		if(skin->btn_close.right >= 0)
			rcb.left = r.right - skin->btn_close.right - skin->btn_close.width;
		else
			rcb.left = skin->btn_close.left;
		rcb.right = rcb.left+skin->btn_close.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_close.state = 2;
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_close.state) {
			skin->btn_close.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}
}

int on_nclbutton_up(skin_t *skin, HWND hWnd, HMENU hPopupMenu, char *title_text)
{
	RECT r, rcb;
	POINT p;

	if(!skin || !hWnd || !skin->enable_appskin || !skin->border_ok)
		return 0;

	GetCursorPos(&p);
	GetWindowRect(hWnd, &r);

	p.x -= r.left;
	p.y -= r.top;
	r.right -= r.left;

	if(skin->btn_menu.exist) {
		rcb.top = skin->btn_menu.top;
		rcb.bottom = rcb.top + skin->btn_menu.height;
		if(skin->btn_menu.right >= 0)
			rcb.left = r.right - skin->btn_menu.right - skin->btn_menu.width;
		else
			rcb.left = skin->btn_menu.left;
		rcb.right = rcb.left+skin->btn_menu.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_menu.state = 0;
			GetCursorPos(&p);
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
			draw_border(hWnd, skin, title_text, FALSE);
		} else if(skin->btn_menu.state) {
			skin->btn_menu.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_min.exist) {
		rcb.top = skin->btn_min.top;
		rcb.bottom = rcb.top + skin->btn_min.height;
		if(skin->btn_min.right >= 0)
			rcb.left = r.right - skin->btn_min.right - skin->btn_min.width;
		else
			rcb.left = skin->btn_min.left;
		rcb.right = rcb.left+skin->btn_min.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_min.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
			PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
		} else if(skin->btn_min.state) {
			skin->btn_min.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_max.exist) {
		rcb.top = skin->btn_max.top;
		rcb.bottom = rcb.top + skin->btn_max.height;
		if(skin->btn_max.right >= 0)
			rcb.left = r.right - skin->btn_max.right - skin->btn_max.width;
		else
			rcb.left = skin->btn_max.left;
		rcb.right = rcb.left+skin->btn_max.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			skin->btn_max.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
			if (IsZoomed(hWnd))
				PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
			else
				PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		} else if(skin->btn_max.state) {
			skin->btn_max.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}

	if(skin->btn_close.exist) {
		rcb.top = skin->btn_close.top;
		rcb.bottom = rcb.top + skin->btn_close.height;
		if(skin->btn_close.right >= 0)
			rcb.left = r.right - skin->btn_close.right - skin->btn_close.width;
		else
			rcb.left = skin->btn_close.left;
		rcb.right = rcb.left+skin->btn_close.width;

		if(p.y >= rcb.top && p.y <= rcb.bottom && p.x >= rcb.left && p.x <= rcb.right) {
			mp_input_queue_cmd(mp_input_parse_cmd("quit"));
			ExitGuiThread = 1;
		} else if(skin->btn_close.state) {
			skin->btn_close.state = 0;
			draw_border(hWnd, skin, title_text, FALSE);
		}
	}
}

