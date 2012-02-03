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

#ifndef MPLAYER_WW_SKIN_H
#define MPLAYER_WW_SKIN_H

typedef struct
{
    char *name;                  /* image name */
    int size;                    /* image data size in bytes */
    int width;                   /* image width */
    int height;                  /* image height */
    char *data;                  /* pointer to image data */
} image_t;

typedef struct
{
    HBITMAP bmp;
    void    *data;                   /* pointer to bitmap data */
    image_t *img;                    /* pointer to image */
    int     top;                     /* image top side */
    int     left;                    /* image left side */
    int     right;                   /* image right side */
    int     width;                   /* image width */
    int     height;                  /* image height */
    int     state;                   /* bitmap state */
    int     exist;                   /* bitmap exist */
    int     show;                    /* always display */
    int     center;                  /* be center */
} bitmap_t;

typedef struct
{
    int  top;                     /* text top side */
    int  left;                    /* text left side */
    int  right;                   /* text right side */
    int  width;                   /* text width */
    int  height;                  /* text height */
    int  show;                    /* display text */
    int  size;                    /* font size */
    int  center;                  /* be center */
    UINT color;                   /* text color */
    UINT alignment;               /* text alignment */
} text_t;

typedef struct
{
	bitmap_t title;
	bitmap_t title_left;
	bitmap_t title_right;

	bitmap_t border_left;
	bitmap_t border_right;
	bitmap_t border_bottom;
	bitmap_t bottom_left;
	bitmap_t bottom_right;

	bitmap_t btn_menu;
	bitmap_t btn_min;
	bitmap_t btn_max;
	bitmap_t btn_close;

	bitmap_t background;
	bitmap_t background_center;
	bitmap_t background_left;
	bitmap_t background_right;
	int      background_width;
	int      background_height;

	bitmap_t seekbar;
	bitmap_t seekbar_foregnd;
	bitmap_t seekbar_thumb;

	bitmap_t volumebar;
	bitmap_t volumebar_foregnd;
	bitmap_t volumebar_thumb;

	bitmap_t btn_play;
	bitmap_t btn_pause;
	bitmap_t btn_prev;
	bitmap_t btn_next;
	bitmap_t btn_forward;
	bitmap_t btn_back;
	bitmap_t btn_stop;

	bitmap_t btn_open;
	bitmap_t btn_mute;
	bitmap_t btn_muted;
	bitmap_t btn_equalizer;
	bitmap_t btn_screenshot;
	bitmap_t btn_playlist;
	bitmap_t btn_fullscreen;

	text_t   title_text;
	text_t   status;
	text_t   status2;

	UINT     is_ok;
	UINT     border_ok;
	UINT     enable_appskin;
	UINT     enable_alpha;
	UINT     zoom_type;
	UINT     dlg_frame;
	UINT     auto_hide;
	UINT     alpha;
} skin_t;

typedef struct
{
	bitmap_t logo;
	UINT     is_ok;
	UINT     bg_color;
} logo_t;

void initskin(const char *dir, logo_t *logo, skin_t *skin);

int loadlogo(logo_t *logo, HWND hWnd);
void freelogo(logo_t *logo);

int loadskin(skin_t *skin, HWND hWnd);
void freeskin(skin_t *skin);

int loadappskin(skin_t *skin, HWND hWnd);
void freeappskin(skin_t *skin);

int get_button_left(skin_t *skin, bitmap_t *button, int width);

int set_window_rgn(HWND hWnd, skin_t *skin, int wtype, int force);
int draw_border(HWND hWnd, skin_t *skin, char *title_text, BOOL fill);
int draw_title_text(HWND hWnd, skin_t *skin, char *title_text);

HRGN BitmapToRegionEx(HBITMAP hbm32, VOID * pbits32, int width, int height, COLORREF cTransparentColor);
BOOL TransparentBlt(HDC dcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC dcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent);

int on_lbutton_down(skin_t *skin, HWND hDlg, RECT r, POINT p, HWND hCmd);
int on_lbutton_up(skin_t *skin, HWND hDlg, RECT r, POINT p, HWND hCmd);
void on_mouse_leave(skin_t *skin);

int on_paint(skin_t *skin, HDC hdc, HWND hDlg, HBRUSH bkgndbrush, RECT r, int paused);
int on_mouse_move(skin_t *skin, HWND hDlg, RECT r, POINT p);
int on_ncmouse_move(skin_t *skin, HWND hWnd, char *title_text);
int on_nclbutton_down(skin_t *skin, HWND hWnd, char *title_text);
int on_nclbutton_up(skin_t *skin, HWND hWnd, HMENU hPopupMenu, char *title_text);

#endif /* MPLAYER_WW_SKIN_H */

