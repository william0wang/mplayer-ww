/*
 * Directx v2 or later DirectDraw interface
 *
 * Copyright (c) 2002 - 2005 Sascha Sommer <saschasommer@freenet.de>
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

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <shellapi.h>
#include <commctrl.h>
#include <winuser.h>
#include <mbstring.h>
#include "winstuff.h"
#include "resource.h"
#include "version.h"
#include "config.h"
#include "video_out.h"
#include "video_out_internal.h"
#include "libmpcodecs/vf.h"
#include "fastmemcpy.h"
#include "input/input.h"
#include "libmpcodecs/vd.h"
#include "osdep/keycodes.h"
#include "input/mouse.h"
#include "skin/skin.h"
#include "mp_msg.h"
#include "aspect.h"
#include "geometry.h"
#include "win7shellapi.h"
#include "mp_fifo.h"
#include "sub/sub.h"

#ifdef CONFIG_GUI
#include "gui/interface.h"
#endif

#ifndef WM_XBUTTONDOWN
# define WM_XBUTTONDOWN    0x020B
# define WM_XBUTTONUP      0x020C
# define WM_XBUTTONDBLCLK  0x020D
#endif

#define IDTB_PLAY_PAUSE WM_USER + 956
#define IDTB_PREVIOUS   WM_USER + 695
#define IDTB_NEXT       WM_USER + 895

#define WNDCLASSNAME_WINDOWED   "MPlayer WW for Windows"
#define WNDCLASSNAME_FULLSCREEN "MPlayer - Fullscreen"
#define WNDSTYLE WS_OVERLAPPEDWINDOW|WS_SIZEBOX

static LPDIRECTDRAWCOLORCONTROL	g_cc = NULL;		//color control interface
static LPDIRECTDRAW7        g_lpdd = NULL;          //DirectDraw Object
static LPDIRECTDRAWSURFACE7  g_lpddsPrimary = NULL;  //Primary Surface: viewport through the Desktop
static LPDIRECTDRAWSURFACE7  g_lpddsOverlay = NULL;  //Overlay Surface
static LPDIRECTDRAWSURFACE7  g_lpddsBack = NULL;     //Back surface
static LPDIRECTDRAWCLIPPER  g_lpddclipper;          //clipper object, can only be used without overlay
static DDSURFACEDESC2		ddsdsf;                 //surface descripiton needed for locking
static HINSTANCE            hddraw_dll;             //handle to ddraw.dll
static HINSTANCE			hInstance;
static RECT                 rd;                     //rect of our stretched image
static RECT                 rs;                     //rect of our source image
static HWND                 hWnd=NULL;              //handle to the window
static HWND                 hWndFS=NULL;           //fullscreen window
static HBRUSH               colorbrush = NULL;      // Handle to colorkey brush
static HBRUSH               bkgndbrush = NULL;
static HICON                mplayericon = NULL;     // Handle to mplayer icon
static HCURSOR              mplayercursor = NULL;   // Handle to mplayer cursor
static uint32_t image_width, image_height;          //image width and height
static uint32_t d_image_width, d_image_height;      //image width and height zoomed
static uint8_t  *image=NULL;                        //image data
static void* tmp_image = NULL;
static uint32_t image_format=0;                       //image format
static uint32_t primary_image_format;
static uint32_t vm_height=0;
static uint32_t vm_width=0;
static uint32_t vm_bpp=0;
static uint32_t dstride;                            //surface stride
static uint32_t nooverlay = 0;                      //NonOverlay mode
static DWORD    destcolorkey;                       //colorkey for our surface
static COLORREF windowcolor = RGB(0,0,16);          //windowcolor == colorkey
static int adapter_count=0;
static GUID selected_guid;
static GUID *selected_guid_ptr = NULL;
static RECT monitor_rect;	                        //monitor coordinates
static float window_aspect = 1.2;
static char *subdownloader = NULL;
static BOOL (WINAPI* myGetMonitorInfo)(HMONITOR, LPMONITORINFO) = NULL;
static BOOL (WINAPI* ChangeWindowMessageFilter)(UINT message, DWORD dwFlag) = NULL;
static RECT last_rect = {0xDEADC0DE, 0xDEADC0DE, 0xDEADC0DE, 0xDEADC0DE};

#include "M_option.h"

typedef struct {
  void *queue_head;
  void *queue_tail;
  unsigned int global_palette[16];
  unsigned int orig_frame_width, orig_frame_height;
  unsigned int scale_x, scale_y;
} spudec_tmp_t;

#include "stream/stream.h"

extern int get_sub_size(void);
extern char *get_help_text(void);
extern char *get_path(const char *filename);
extern void mp_parse_commandline(char *cmdline);
extern m_obj_settings_t* vf_settings;
//extern float force_monitor_aspect;
#define DMAX_PATH MAX_PATH*2
#ifdef CONFIG_ICONV
extern char *sub_cp;
static char AlternativeFont[DMAX_PATH];
#endif
extern int force_load_font;
extern int save_volume;
extern char *sub_font_name;
extern float text_font_scale_factor;
extern int sub_pos;
extern char *filename;
extern char *dvd_device;
extern float expand;
extern int show_menubar;
static int show_menubar_w32;
extern int show_controlbar;
extern int skinned_player;
extern int show_status;
extern int show_status2;
extern int use_filename_title;
extern int use_rightmenu;
extern int have_audio;
extern int using_theme;
extern int disable_screensaver;
extern BOOL bScrActiveSave;

#define CONTROLBAR_H 20
static int min_video_width=360;
static int controlbar_offset = 0;
static int auto_hide_control_save = 0;
static int controlbar_height_fixed = 0;
extern int controlbar_height;
extern int auto_hide_control;
extern skin_t gui_skin;
extern logo_t gui_logo;

#define WM_NOTIFYICON	 (WM_USER+1973)
#define WM_SHOWCURSOR	 (WM_USER+1974)

static long WM_TASKBARCREATED;
static NOTIFYICONDATA nid;
static HWND hWndP=NULL;
static HWND hWndControlbar = NULL;
static HWND hWndSeekbar = NULL;
static HWND hWndVolumebar = NULL;
static HBITMAP hLogo;
static int logoW, logoH;
static int seekbar_width = 0;
static const int volumebar_width = 60;
static const int volumebar_height = 20;
static int ControlbarShow = TRUE;
static int PopMenuShow = TRUE;
static int xborder,yborder;
static int xborder0,yborder0;
static int xborder1,yborder1;
static int disp_height,disp_top;
static int paused = 0;
static int WndState = SW_NORMAL;
static int bIdle;
extern char status[128];
extern int volpercent;
extern float percent_value;
extern int seekpercent;
extern int bSeeking;
extern int seek_realtime;
extern int skin_controlbar;

HANDLE hAccelTable;
int systray = 0;
int switch_view = 0;
int auto_hide = 1500;
int fixedsize = 0;

#define TIMER_SWITCH_VIEW 100
#define TIMER_HIDE_CURSOR 101
#define TIMER_PLAY_PAUSE 107
#define TIMER_DVDNAV 119

static int nothide_times = 0;
static int paused_save = 1;
static HMENU hMenu, hSubtitleMenu, hFavoriteMenu, hSkinMenu, hPopupMenu;
static MENUITEMINFO mi,minf2, minf3;
static int menuHeight = 0;
static BOOL bFullView = TRUE;
static BOOL bRBDown = FALSE;
static BOOL bWheel = FALSE;
extern char *new_audio_stream;
extern char status_text_timer2[64];
extern char *skins_name[100];
extern char *skin_name;

#define MOVING_NONE	 0
#define MOVING_SUB	  1
#define MOVING_WINDOW   2

static int FullHide;
static int bHide=FALSE;
static DWORD start_lbutton_down;
static DWORD start_lbutton_dbcheck;
static int old_x, old_y, bMoving = MOVING_NONE;
static float factors[3] = {0, -0.1875, -0.2975};
static int factor_index = 0;
static int save_vo_fs = -1;
static int video_inited = 0;

static int play_finished = 0;
extern char *last_filename;
extern int save_width;
extern int save_height;
extern int save_x;
extern int save_y;
static float factor_x=0,factor_y=0;
static int delta_x=0,delta_y=0;
static int disable_manage = 0;
static int is_dvdnav = 0;
static int dvdnav_checktime = 0;
extern int sys_Language;
extern int gui_thread;
extern int ExitGuiThread;
extern int no_dvdnav;
extern int reload_when_open;
extern int auto_resume;
static char subMenu[16]={0},subMenuItem[16]={0};
static char favMenu[32]={0},favMenuItem[32]={0},favMenuManage[32]={0};
extern int seek_dropping;
extern int volume_dropping;
static HHOOK hMHook = NULL;
static BOOL bNcLButtonDown = FALSE;
static char title_text[512] = {0};

extern int mouse_dvdnav;
extern int auto_shutdown;
extern int open_with_dshow_demux;
extern int get_video_length();
extern void shutdown_when_necessary(HWND hWnd);
extern int is_win7;
static unsigned int s_uTBBC = WM_NULL;
static ITaskbarList3 *g_pTaskbarList = NULL;
static int need_recreate = 0;

extern void update_subtitle_menu();

extern void ShowEqualizer(HWND hWnd);
extern int GetDVDDriver(HWND hWnd);
extern int GeBlurayDriver(HWND hWnd);
extern BOOL GetURL(HINSTANCE hInstance, HWND hWndParent, char **url);
extern BOOL GetOpenFile2(HINSTANCE hInstance, HWND hWndParent, char **file, char **audio);
extern BOOL GetPlayCD(HINSTANCE hInstance, HWND hWndParent);

/*****************************************************************************
 * DirectDraw GUIDs.
 * Defining them here allows us to get rid of the dxguid library during
 * the linking stage.
 *****************************************************************************/
#define IID_IDirectDraw7 MP_IID_IDirectDraw7
static const GUID MP_IID_IDirectDraw7 =
{
	0x15e65ec0,0x3b9c,0x11d2,{0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b}
};

#define IID_IDirectDrawColorControl MP_IID_IDirectDrawColorControl
static const GUID MP_IID_IDirectDrawColorControl =
{
	0x4b9f0ee0,0x0d7e,0x11d0,{0x9b,0x06,0x00,0xa0,0xc9,0x03,0xa3,0xb8}
};


typedef struct directx_fourcc_caps
{
   char*  img_format_name;      //human readable name
   uint32_t img_format;         //as MPlayer image format
   uint32_t drv_caps;           //what hw supports with this format
   DDPIXELFORMAT g_ddpfOverlay; //as Directx Sourface description
} directx_fourcc_caps;


static directx_fourcc_caps g_ddpf[] =
{
	{"YV12 ",IMGFMT_YV12 ,0,{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','V','1','2'),0,0,0,0,0}},
	{"I420 ",IMGFMT_I420 ,0,{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('I','4','2','0'),0,0,0,0,0}},   //yv12 with swapped uv
	{"IYUV ",IMGFMT_IYUV ,0,{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('I','Y','U','V'),0,0,0,0,0}},   //same as i420
	{"YVU9 ",IMGFMT_YVU9 ,0,{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','V','U','9'),0,0,0,0,0}},
	{"YUY2 ",IMGFMT_YUY2 ,0,{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0}},
	{"UYVY ",IMGFMT_UYVY ,0,{sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0}},
 	{"BGR8 ",IMGFMT_BGR8 ,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 8,  0x00000000, 0x00000000, 0x00000000, 0}},
	{"RGB15",IMGFMT_RGB15,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x0000001F, 0x000003E0, 0x00007C00, 0}},   //RGB 5:5:5
	{"BGR15",IMGFMT_BGR15,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x00007C00, 0x000003E0, 0x0000001F, 0}},
	{"RGB16",IMGFMT_RGB16,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x0000001F, 0x000007E0, 0x0000F800, 0}},   //RGB 5:6:5
    {"BGR16",IMGFMT_BGR16,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x0000F800, 0x000007E0, 0x0000001F, 0}},
	{"RGB24",IMGFMT_RGB24,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 24,  0x000000FF, 0x0000FF00, 0x00FF0000, 0}},
    {"BGR24",IMGFMT_BGR24,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 24,  0x00FF0000, 0x0000FF00, 0x000000FF, 0}},
    {"RGB32",IMGFMT_RGB32,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32,  0x000000FF, 0x0000FF00, 0x00FF0000, 0}},
    {"BGR32",IMGFMT_BGR32,0,{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32,  0x00FF0000, 0x0000FF00, 0x000000FF, 0}}
};
#define NUM_FORMATS (sizeof(g_ddpf) / sizeof(g_ddpf[0]))

static const vo_info_t info =
{
	"Directx DDraw YUV/RGB/BGR renderer",
	"directx",
	"Sascha Sommer <saschasommer@freenet.de>",
	""
};

const LIBVO_EXTERN(directx)

static void draw_alpha(int x0, int y0, int w, int h, unsigned char *src,
		unsigned char *srca, int stride)
{
    switch(image_format) {
    case IMGFMT_YV12 :
    case IMGFMT_I420 :
	case IMGFMT_IYUV :
	case IMGFMT_YVU9 :
    	vo_draw_alpha_yv12(w,h,src,srca,stride,((uint8_t *) image) + dstride*y0 + x0,dstride);
	break;
	case IMGFMT_YUY2 :
	    vo_draw_alpha_yuy2(w,h,src,srca,stride,((uint8_t *) image)+ dstride*y0 + 2*x0 ,dstride);
    break;
    case IMGFMT_UYVY :
        vo_draw_alpha_yuy2(w,h,src,srca,stride,((uint8_t *) image) + dstride*y0 + 2*x0 + 1,dstride);
    break;
	case IMGFMT_RGB15:
    case IMGFMT_BGR15:
		vo_draw_alpha_rgb15(w,h,src,srca,stride,((uint8_t *) image)+dstride*y0+2*x0,dstride);
    break;
    case IMGFMT_RGB16:
	case IMGFMT_BGR16:
        vo_draw_alpha_rgb16(w,h,src,srca,stride,((uint8_t *) image)+dstride*y0+2*x0,dstride);
    break;
    case IMGFMT_RGB24:
	case IMGFMT_BGR24:
        vo_draw_alpha_rgb24(w,h,src,srca,stride,((uint8_t *) image)+dstride*y0+4*x0,dstride);
    break;
    case IMGFMT_RGB32:
	case IMGFMT_BGR32:
        vo_draw_alpha_rgb32(w,h,src,srca,stride,((uint8_t *) image)+dstride*y0+4*x0,dstride);
    break;
    }
}

static void draw_osd(void)
{
    vo_draw_text(image_width,image_height,draw_alpha);
}

static int
query_format(uint32_t format)
{
    uint32_t i=0;
    while ( i < NUM_FORMATS )
    {
		if (g_ddpf[i].img_format == format)
		    return g_ddpf[i].drv_caps;
	    i++;
    }
    return 0;
}

static uint32_t Directx_CreatePrimarySurface(void)
{
    DDSURFACEDESC2   ddsd;
    //cleanup
	if(g_lpddsPrimary)g_lpddsPrimary->lpVtbl->Release(g_lpddsPrimary);
	g_lpddsPrimary=NULL;

    if(vidmode)g_lpdd->lpVtbl->SetDisplayMode(g_lpdd,vm_width,vm_height,vm_bpp,vo_refresh_rate,0);
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    //set flags and create a primary surface.
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    if(g_lpdd->lpVtbl->CreateSurface(g_lpdd,&ddsd, &g_lpddsPrimary, NULL )== DD_OK)
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>primary surface created\n");
	else
	{
		mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>could not create primary surface\n");
		return 1;
	}
	return 0;
}

static uint32_t Directx_CreateOverlay(uint32_t imgfmt)
{
    HRESULT ddrval;
    DDSURFACEDESC2   ddsdOverlay;
    uint32_t        i=0;
	while ( i < NUM_FORMATS && imgfmt != g_ddpf[i].img_format)
	{
		i++;
	}
	if (!g_lpdd || !g_lpddsPrimary || i == NUM_FORMATS)
        return 1;
    //cleanup
	if (g_lpddsOverlay)g_lpddsOverlay->lpVtbl->Release(g_lpddsOverlay);
	if (g_lpddsBack)g_lpddsBack->lpVtbl->Release(g_lpddsBack);
	g_lpddsOverlay= NULL;
	g_lpddsBack = NULL;
	//create our overlay
    ZeroMemory(&ddsdOverlay, sizeof(ddsdOverlay));
    ddsdOverlay.dwSize = sizeof(ddsdOverlay);
    ddsdOverlay.ddsCaps.dwCaps=DDSCAPS_OVERLAY | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
    ddsdOverlay.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_BACKBUFFERCOUNT| DDSD_PIXELFORMAT;
    ddsdOverlay.dwWidth=image_width;
    ddsdOverlay.dwHeight=image_height;
    ddsdOverlay.dwBackBufferCount=2;
	ddsdOverlay.ddpfPixelFormat=g_ddpf[i].g_ddpfOverlay;
	if(vo_doublebuffering)   //tribblebuffering
	{
		if (g_lpdd->lpVtbl->CreateSurface(g_lpdd,&ddsdOverlay, &g_lpddsOverlay, NULL)== DD_OK)
		{
			mp_msg(MSGT_VO, MSGL_V,"<vo_directx><INFO>overlay with format %s created\n",g_ddpf[i].img_format_name);
            //get the surface directly attached to the primary (the back buffer)
            ddsdOverlay.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
            if(g_lpddsOverlay->lpVtbl->GetAttachedSurface(g_lpddsOverlay,&ddsdOverlay.ddsCaps, &g_lpddsBack) != DD_OK)
			{
				mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't get attached surface\n");
				return 1;
			}
			return 0;
		}
		vo_doublebuffering=0; //disable tribblebuffering
		mp_msg(MSGT_VO, MSGL_V,"<vo_directx><WARN>cannot create tribblebuffer overlay with format %s\n",g_ddpf[i].img_format_name);
	}
	//single buffer
	mp_msg(MSGT_VO, MSGL_V,"<vo_directx><INFO>using singlebuffer overlay\n");
    ddsdOverlay.dwBackBufferCount=0;
    ddsdOverlay.ddsCaps.dwCaps=DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
    ddsdOverlay.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
    ddsdOverlay.ddpfPixelFormat=g_ddpf[i].g_ddpfOverlay;
	// try to create the overlay surface
	ddrval = g_lpdd->lpVtbl->CreateSurface(g_lpdd,&ddsdOverlay, &g_lpddsOverlay, NULL);
	if(ddrval != DD_OK)
	{
	   if(ddrval == DDERR_INVALIDPIXELFORMAT)mp_msg(MSGT_VO,MSGL_V,"<vo_directx><ERROR> invalid pixelformat: %s\n",g_ddpf[i].img_format_name);
       else mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>");
       switch(ddrval)
	   {
	      case DDERR_INCOMPATIBLEPRIMARY:
			 {mp_msg(MSGT_VO, MSGL_ERR,"incompatible primary surface\n");break;}
		  case DDERR_INVALIDCAPS:
			 {mp_msg(MSGT_VO, MSGL_ERR,"invalid caps\n");break;}
	      case DDERR_INVALIDOBJECT:
		     {mp_msg(MSGT_VO, MSGL_ERR,"invalid object\n");break;}
	      case DDERR_INVALIDPARAMS:
		     {mp_msg(MSGT_VO, MSGL_ERR,"invalid parameters\n");break;}
	      case DDERR_NODIRECTDRAWHW:
		     {mp_msg(MSGT_VO, MSGL_ERR,"no directdraw hardware\n");break;}
	      case DDERR_NOEMULATION:
		     {mp_msg(MSGT_VO, MSGL_ERR,"can't emulate\n");break;}
	      case DDERR_NOFLIPHW:
		     {mp_msg(MSGT_VO, MSGL_ERR,"hardware can't do flip\n");break;}
	      case DDERR_NOOVERLAYHW:
		     {mp_msg(MSGT_VO, MSGL_ERR,"hardware can't do overlay\n");break;}
	      case DDERR_OUTOFMEMORY:
		     {mp_msg(MSGT_VO, MSGL_ERR,"not enough system memory\n");break;}
	      case DDERR_UNSUPPORTEDMODE:
			 {mp_msg(MSGT_VO, MSGL_ERR,"unsupported mode\n");break;}
		  case DDERR_OUTOFVIDEOMEMORY:
			 {mp_msg(MSGT_VO, MSGL_ERR,"not enough video memory\n");break;}
          default:
             mp_msg(MSGT_VO, MSGL_ERR,"create surface failed with 0x%x\n",ddrval);
	   }
	   return 1;
	}
    g_lpddsBack = g_lpddsOverlay;
	return 0;
}

static uint32_t Directx_CreateBackpuffer(void)
{
    DDSURFACEDESC2   ddsd;
	//cleanup
	if (g_lpddsBack)g_lpddsBack->lpVtbl->Release(g_lpddsBack);
	g_lpddsBack=NULL;
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.ddsCaps.dwCaps= DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwFlags= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.dwWidth=image_width;
    ddsd.dwHeight=image_height;
    if(g_lpdd->lpVtbl->CreateSurface( g_lpdd, &ddsd, &g_lpddsBack, 0 ) != DD_OK )
	{
		mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't create backpuffer\n");
		return 1;
	}
    mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>backbuffer created\n");
	return 0;
}

static void uninit(void)
{
	if (g_cc != NULL)
	{
		g_cc->lpVtbl->Release(g_cc);
	}
	g_cc=NULL;
	if (g_lpddclipper != NULL) g_lpddclipper->lpVtbl->Release(g_lpddclipper);
	g_lpddclipper = NULL;
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>clipper released\n");
	if (g_lpddsBack != NULL) g_lpddsBack->lpVtbl->Release(g_lpddsBack);
	g_lpddsBack = NULL;
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>back surface released\n");
	if(vo_doublebuffering && !nooverlay)
	{
		if (g_lpddsOverlay != NULL)g_lpddsOverlay->lpVtbl->Release(g_lpddsOverlay);
        g_lpddsOverlay = NULL;
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>overlay surface released\n");
	}
	if (g_lpddsPrimary != NULL) g_lpddsPrimary->lpVtbl->Release(g_lpddsPrimary);
    g_lpddsPrimary = NULL;
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>primary released\n");
	if (hMHook) {
		UnhookWindowsHookEx(hMHook);
		hMHook = NULL;
	}
	if (hPopupMenu) {
		DestroyMenu(hPopupMenu);
		hPopupMenu = NULL;
	}
	if (hWndFS) {
		DestroyWindow(hWndFS);
		hWndFS = NULL;
	}
	if ((WinID == -1) && hWnd) {
		DestroyWindow(hWnd);
		hWnd = NULL;
	}
	if(hWndP) {
		DestroyWindow(hWndP);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		hWndP = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>window destroyed\n");
	UnregisterClass(WNDCLASSNAME_WINDOWED, GetModuleHandle(NULL));
	UnregisterClass(WNDCLASSNAME_FULLSCREEN, GetModuleHandle(NULL));
	if (mplayericon) {
		DestroyIcon(mplayericon);
		mplayericon = NULL;
	}
	if (mplayercursor) {
		DestroyCursor(mplayercursor);
		mplayercursor = NULL;
	}
	if (colorbrush) {
		DeleteObject(colorbrush);
		colorbrush = NULL;
	}
	if (bkgndbrush) {
		DeleteObject(bkgndbrush);
		bkgndbrush = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>GDI resources deleted\n");
	if (g_lpdd) {
		if(vidmode)g_lpdd->lpVtbl->RestoreDisplayMode(g_lpdd);
		g_lpdd->lpVtbl->Release(g_lpdd);
		g_lpdd = NULL;
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>directdrawobject released\n");
	}
	if (hddraw_dll) {
		FreeLibrary( hddraw_dll);
		hddraw_dll= NULL;
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>ddraw.dll freed\n");
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>uninitialized\n");
}


static void uninitwin(void){
	if (g_cc)
	{
		g_cc->lpVtbl->Release(g_cc);
		g_cc=NULL;
	}
	if (g_lpddclipper) {
		g_lpddclipper->lpVtbl->Release(g_lpddclipper);
		g_lpddclipper = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>clipper released\n");
	if (g_lpddsBack) {
		g_lpddsBack->lpVtbl->Release(g_lpddsBack);
		g_lpddsBack = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>back surface released\n");
	if(vo_doublebuffering && !nooverlay)
	{
		if (g_lpddsOverlay) {
			g_lpddsOverlay->lpVtbl->Release(g_lpddsOverlay);
			g_lpddsOverlay = NULL;
		}
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>overlay surface released\n");
	}
	if (g_lpddsPrimary) {
		g_lpddsPrimary->lpVtbl->Release(g_lpddsPrimary);
	g_lpddsPrimary = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>primary released\n");
	if (hPopupMenu) {
		DestroyMenu(hPopupMenu);
		hPopupMenu = NULL;
	}
	if (hMHook) {
		UnhookWindowsHookEx(hMHook);
		hMHook = NULL;
	}
	if (g_pTaskbarList) {
		ITaskbarList3_Release(g_pTaskbarList);
		g_pTaskbarList = NULL;
	}
	if (hWndFS) {
		DestroyWindow(hWndFS);
		hWndFS = NULL;
	}
	//  if ((WinID == -1) && hWnd) {
	//	  DestroyWindow(hWnd);
	//	  hWnd = NULL;
	//  }
	if(hWndP) {
		DestroyWindow(hWndP);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		hWndP = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>window destroyed\n");
	UnregisterClass(WNDCLASSNAME_WINDOWED, GetModuleHandle(NULL));
	UnregisterClass(WNDCLASSNAME_FULLSCREEN, GetModuleHandle(NULL));
	if (mplayericon) {
		DestroyIcon(mplayericon);
		mplayericon = NULL;
	}
	if (mplayercursor) {
		DestroyCursor(mplayercursor);
		mplayercursor = NULL;
	}
	if (colorbrush) {
		DeleteObject(colorbrush);
		colorbrush = NULL;
	}
	if (bkgndbrush) {
		DeleteObject(bkgndbrush);
		bkgndbrush = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>GDI resources deleted\n");
	if (g_lpdd) {
		if(vidmode)g_lpdd->lpVtbl->RestoreDisplayMode(g_lpdd);
		g_lpdd->lpVtbl->Release(g_lpdd);
		g_lpdd = NULL;
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>directdrawobject released\n");
	}
	if (hddraw_dll) {
		FreeLibrary( hddraw_dll);
		hddraw_dll= NULL;
		mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>ddraw.dll freed\n");
	}
	if (subdownloader) {
		free(subdownloader);
		subdownloader = NULL;
	}
	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>uninitialized\n");
}

static BOOL WINAPI EnumCallbackEx(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR  hm)
{
    mp_msg(MSGT_VO, MSGL_INFO ,"<vo_directx> adapter %d: ", adapter_count);

    if (!lpGUID)
    {
        mp_msg(MSGT_VO, MSGL_INFO ,"%s", "Primary Display Adapter");
    }
    else
    {
        mp_msg(MSGT_VO, MSGL_INFO ,"%s", lpDriverDescription);
    }

    if(adapter_count == vo_adapter_num){
        MONITORINFO mi;
        if (!lpGUID)
            selected_guid_ptr = NULL;
        else
        {
            selected_guid = *lpGUID;
            selected_guid_ptr = &selected_guid;
        }
        mi.cbSize = sizeof(mi);

        if (myGetMonitorInfo(hm, &mi)) {
			monitor_rect = mi.rcMonitor;
        }
        mp_msg(MSGT_VO, MSGL_INFO ,"\t\t<--");
    }
    mp_msg(MSGT_VO, MSGL_INFO ,"\n");

    adapter_count++;

    return 1; // list all adapters
}

static uint32_t Directx_InitDirectDraw(void)
{
	HRESULT    (WINAPI *OurDirectDrawCreateEx)(GUID *,LPVOID *, REFIID,IUnknown FAR *);
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWENUMERATEEX OurDirectDrawEnumerateEx;
	HINSTANCE user32dll=LoadLibrary("user32.dll");

	adapter_count = 0;
	if(user32dll){
		myGetMonitorInfo=GetProcAddress(user32dll,"GetMonitorInfoA");
		if(!myGetMonitorInfo && vo_adapter_num){
			mp_msg(MSGT_VO, MSGL_ERR, "<vo_directx> -adapter is not supported on Win95\n");
			vo_adapter_num = 0;
		}
	}

	mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>Initing DirectDraw\n" );

	//load direct draw DLL: based on videolans code
	hddraw_dll = LoadLibrary("DDRAW.DLL");
	if( hddraw_dll == NULL )
    {
        mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>failed loading ddraw.dll\n" );
		return 1;
    }

    last_rect.left = 0xDEADC0DE;   // reset window position cache

	if(vo_adapter_num){ //display other than default
        OurDirectDrawEnumerateEx = (LPDIRECTDRAWENUMERATEEX) GetProcAddress(hddraw_dll,"DirectDrawEnumerateExA");
        if (!OurDirectDrawEnumerateEx){
            FreeLibrary( hddraw_dll );
            hddraw_dll = NULL;
            mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>failed geting proc address: DirectDrawEnumerateEx\n");
            mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>no directx 7 or higher installed\n");
            return 1;
        }

        // enumerate all display devices attached to the desktop
        OurDirectDrawEnumerateEx(EnumCallbackEx, NULL, DDENUM_ATTACHEDSECONDARYDEVICES );

        if(vo_adapter_num >= adapter_count)
            mp_msg(MSGT_VO, MSGL_ERR,"Selected adapter (%d) doesn't exist: Default Display Adapter selected\n",vo_adapter_num);
    }
    FreeLibrary(user32dll);

	OurDirectDrawCreateEx = (void *)GetProcAddress(hddraw_dll, "DirectDrawCreateEx");
    if ( OurDirectDrawCreateEx == NULL )
     {
         FreeLibrary( hddraw_dll );
         hddraw_dll = NULL;
        mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>failed geting proc address: DirectDrawCreateEx\n");
 		return 1;
     }

	// initialize DirectDraw and create directx v7 object
    if (OurDirectDrawCreateEx(selected_guid_ptr, (VOID**)&g_lpdd, &IID_IDirectDraw7, NULL ) != DD_OK )
    {
        FreeLibrary( hddraw_dll );
        hddraw_dll = NULL;
        mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't initialize ddraw\n");
		return 1;
    }

	//get current screen siz for selected monitor ...
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
	g_lpdd->lpVtbl->GetDisplayMode(g_lpdd, &ddsd);
	if(vo_screenwidth && vo_screenheight)
	{
	    vm_height=vo_screenheight;
	    vm_width=vo_screenwidth;
	}
    else
    {
	    vm_height=ddsd.dwHeight;
	    vm_width=ddsd.dwWidth;
	}


	if(vo_dbpp)vm_bpp=vo_dbpp;
	else vm_bpp=ddsd.ddpfPixelFormat.dwRGBBitCount;

	if(vidmode){
		if (g_lpdd->lpVtbl->SetCooperativeLevel(g_lpdd, hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN) != DD_OK)
		{
	        mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't set cooperativelevel for exclusive mode\n");
            return 1;
		}
		/*SetDisplayMode(ddobject,width,height,bpp,refreshrate,aditionalflags)*/
		if(g_lpdd->lpVtbl->SetDisplayMode(g_lpdd,vm_width, vm_height, vm_bpp,0,0) != DD_OK)
		{
	        mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't set displaymode\n");
	        return 1;
		}
	    mp_msg(MSGT_VO, MSGL_V,"<vo_directx><INFO>Initialized adapter %i for %i x %i @ %i \n",vo_adapter_num,vm_width,vm_height,vm_bpp);
	    return 0;
	}
	if (g_lpdd->lpVtbl->SetCooperativeLevel(g_lpdd, hWnd, DDSCL_NORMAL) != DD_OK) // or DDSCL_SETFOCUSWINDOW
     {
        mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>could not set cooperativelevel for hardwarecheck\n");
		return 1;
    }
    mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>DirectDraw Initialized\n");
	return 0;
}

static void check_events(void)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
    {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    }
}

static uint32_t Directx_ManageDisplay(void)
{
    HRESULT         ddrval;
    DDCAPS          capsDrv;
    DDOVERLAYFX     ovfx;
    DWORD           dwUpdateFlags=0;
    int width,height,height_fixed,top=0;

	if (disable_manage) return 0;

	if(!vidmode && !vo_fs && WinID!=-1) {
	  RECT current_rect = {0, 0, 0, 0};
	  GetWindowRect(hWnd, &current_rect);
	  if ((current_rect.left   == last_rect.left)
	  &&  (current_rect.top	== last_rect.top)
	  &&  (current_rect.right  == last_rect.right)
	  &&  (current_rect.bottom == last_rect.bottom))
		return 0;
	  last_rect = current_rect;
	}

	if(vo_fs||vidmode){
		aspect(&width,&height,A_ZOOM);
		if (!vo_keepaspect)
		{
		  if ( (factor_x == 0) && (factor_y == 0) )
		  {
			width = vo_screenwidth;
			height = vo_screenheight;
		  }
		  else
		  {
			width = (int)(width*(1+factor_x))>>1<<1;
			height = (int)(height*(1+factor_y))>>1<<1;
		  }
		}
		rd.left=(vo_screenwidth-width)/2 + delta_x;
		if (expand >= 0)
		  rd.top=(vo_screenheight-height)/2 + delta_y;
		else
		  rd.top=(vo_screenheight+height*expand+1)/2 + delta_y;
		if(show_controlbar && hWndControlbar) {
			SetWindowPos(hWndControlbar,NULL,0 ,vo_screenheight-controlbar_height_fixed+controlbar_offset,vo_screenwidth ,controlbar_height_fixed,SWP_NOZORDER|SWP_NOOWNERZORDER);
			if(!skin_controlbar && hWndSeekbar && hWndVolumebar) {
				seekbar_width = vo_screenwidth - volumebar_width;
				MoveWindow(hWndSeekbar, 0, 0, seekbar_width, volumebar_height, TRUE);
				MoveWindow(hWndVolumebar, seekbar_width, 0, volumebar_width, volumebar_height,TRUE);
			}
			InvalidateRect(hWndControlbar, NULL, FALSE);
		}
		SetFocus(vo_fs?hWndFS:hWnd);
		FullHide = !auto_hide;
	}
	else if (WinID != -1 && vo_geometry) {
		POINT pt;
		pt.x = vo_dx;
		pt.y = vo_dy;
		ClientToScreen(hWnd,&pt);
		width=d_image_width;
		height=d_image_height;
		rd.left = pt.x;
		rd.top = pt.y;
		while(ShowCursor(TRUE)<=0);
	}
	else {
		POINT pt;
		if (bFullView) {
			GetClientRect(hWnd, &rd);
			width = rd.right;
			height = rd.bottom;
			height_fixed = height - controlbar_height_fixed;
			height -= controlbar_height;
		} else {
			GetWindowRect(hWnd, &rd);
			width = rd.right - rd.left;
			height = rd.bottom - rd.top;
			height_fixed = height;
		}
		if(show_controlbar) {
			SetWindowPos(hWndControlbar,NULL,0 ,height_fixed,width,controlbar_height_fixed,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
			if(!skin_controlbar && hWndSeekbar && hWndVolumebar) {
				seekbar_width = width - volumebar_width;
				MoveWindow(hWndSeekbar, 0, 0, seekbar_width, volumebar_height, TRUE);
				MoveWindow(hWndVolumebar, seekbar_width, 0, volumebar_width, volumebar_height,TRUE);
			}
			InvalidateRect(hWndControlbar, NULL, FALSE);
			if(auto_hide_control) {
				ControlbarShow = FALSE;
				ShowWindow(hWndControlbar, SW_HIDE);
			}
		}
		FullHide = FALSE;

		if(!nooverlay && (!width || !height) && g_lpddsOverlay){
			/*window is minimized*/
			ddrval = g_lpddsOverlay->lpVtbl->UpdateOverlay(g_lpddsOverlay,NULL, g_lpddsPrimary, NULL, DDOVER_HIDE, NULL);
			return 0;
		}

		if (bFullView) {
			pt.x = pt.y = 0;
		} else {
			pt.x = -xborder1;
			pt.y = -yborder1;
		}
		ClientToScreen(hWnd, &pt);
		top = pt.y;
		if (vo_keepaspect){
			int tmpheight=((float)width/window_aspect);
			tmpheight+=tmpheight&1;
			if(tmpheight > height){
				pt.x += (width - height * window_aspect) / 2;
				width = height * window_aspect;
				width += width & 1;
			}
			else
			{
				pt.y += (height - tmpheight) >> 1;
				height = tmpheight;
			}
		}
		rd.left = pt.x - monitor_rect.left;
		rd.top = pt.y - monitor_rect.top;
	}
	rd.right=rd.left+width;
	rd.bottom=rd.top+height;

	if(!g_lpddsBack) return 0;
	/*ok, let's workaround some overlay limitations*/
	if(!nooverlay)
	{
		uint32_t        uStretchFactor1000;  //minimum stretch
        uint32_t        xstretch1000,ystretch1000;
		/*get driver capabilities*/
        ZeroMemory(&capsDrv, sizeof(capsDrv));
        capsDrv.dwSize = sizeof(capsDrv);
        if(g_lpdd->lpVtbl->GetCaps(g_lpdd,&capsDrv, NULL) != DD_OK)return 1;
		/*get minimum stretch, depends on display adaptor and mode (refresh rate!) */
        uStretchFactor1000 = capsDrv.dwMinOverlayStretch>1000 ? capsDrv.dwMinOverlayStretch : 1000;
        rd.right = ((width+rd.left)*uStretchFactor1000+999)/1000;
        rd.bottom = (height+rd.top)*uStretchFactor1000/1000;
        /*calculate xstretch1000 and ystretch1000*/
        xstretch1000 = ((rd.right - rd.left)* 1000)/image_width ;
        ystretch1000 = ((rd.bottom - rd.top)* 1000)/image_height;
		rs.left=0;
		rs.right=image_width;
		rs.top=0;
		rs.bottom=image_height;
        if(rd.left < 0)rs.left=(-rd.left*1000)/xstretch1000;
        if(rd.top < 0)rs.top=(-rd.top*1000)/ystretch1000;
        if(rd.right > vo_screenwidth)rs.right=((vo_screenwidth-rd.left)*1000)/xstretch1000;
        if(rd.bottom > vo_screenheight)rs.bottom=((vo_screenheight-rd.top)*1000)/ystretch1000;
		/*do not allow to zoom or shrink if hardware isn't able to do so*/
		if((width < image_width)&& !(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSHRINKX))
		{
			if(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSHRINKXN)mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can only shrinkN\n");
	        else mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can't shrink x\n");
	        rd.right=rd.left+image_width;
		}
		else if((width > image_width)&& !(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSTRETCHX))
		{
			if(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSTRETCHXN)mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can only stretchN\n");
	        else mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can't stretch x\n");
	        rd.right = rd.left+image_width;
		}
		if((height < image_height) && !(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSHRINKY))
		{
			if(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSHRINKYN)mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can only shrinkN\n");
	        else mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can't shrink y\n");
	        rd.bottom = rd.top + image_height;
		}
		else if((height > image_height ) && !(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSTRETCHY))
		{
			if(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYSTRETCHYN)mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can only stretchN\n");
	        else mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can't stretch y\n");
	        rd.bottom = rd.top + image_height;
		}
		/*the last thing to check are alignment restrictions
          these expressions (x & -y) just do alignment by dropping low order bits...
          so to round up, we add first, then truncate*/
		if((capsDrv.dwCaps & DDCAPS_ALIGNBOUNDARYSRC) && capsDrv.dwAlignBoundarySrc)
		  rs.left = (rs.left + capsDrv.dwAlignBoundarySrc / 2) & -(signed)(capsDrv.dwAlignBoundarySrc);
        if((capsDrv.dwCaps & DDCAPS_ALIGNSIZESRC) && capsDrv.dwAlignSizeSrc)
		  rs.right = rs.left + ((rs.right - rs.left + capsDrv.dwAlignSizeSrc / 2) & -(signed) (capsDrv.dwAlignSizeSrc));
        if((capsDrv.dwCaps & DDCAPS_ALIGNBOUNDARYDEST) && capsDrv.dwAlignBoundaryDest)
		  rd.left = (rd.left + capsDrv.dwAlignBoundaryDest / 2) & -(signed)(capsDrv.dwAlignBoundaryDest);
        if((capsDrv.dwCaps & DDCAPS_ALIGNSIZEDEST) && capsDrv.dwAlignSizeDest)
		  rd.right = rd.left + ((rd.right - rd.left) & -(signed) (capsDrv.dwAlignSizeDest));
		/*create an overlay FX structure to specify a destination color key*/
		ZeroMemory(&ovfx, sizeof(ovfx));
        ovfx.dwSize = sizeof(ovfx);
        ovfx.dckDestColorkey.dwColorSpaceLowValue = destcolorkey;
        ovfx.dckDestColorkey.dwColorSpaceHighValue = destcolorkey;

        // set the flags we'll send to UpdateOverlay      //DDOVER_AUTOFLIP|DDOVERFX_MIRRORLEFTRIGHT|DDOVERFX_MIRRORUPDOWN could be useful?;
        dwUpdateFlags = DDOVER_SHOW | DDOVER_DDFX;
        /*if hardware can't do colorkeying set the window on top*/
		if(capsDrv.dwCKeyCaps & DDCKEYCAPS_DESTOVERLAY) dwUpdateFlags |= DDOVER_KEYDESTOVERRIDE;
        else  {
			if (!tmp_image)
				vo_ontop = 1;
			SetWindowPos(vo_fs?hWndFS:hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOCOPYBITS);
		}
	}
    else
    {
        g_lpddclipper->lpVtbl->SetHWnd(g_lpddclipper, 0,(vo_fs && !vidmode)?hWndFS: hWnd);
    }

	disp_top = rd.top - top;
	disp_height = rd.bottom - top - disp_top;

    /*make sure the overlay is inside the screen*/
    if(rd.left<0)rd.left=0;
    if(rd.right>vo_screenwidth)rd.right=vo_screenwidth;
    if(rd.top<0)rd.top=0;
    if(rd.bottom>vo_screenheight)rd.bottom=vo_screenheight;

  	/*for nonoverlay mode we are finished, for overlay mode we have to display the overlay first*/
	if(nooverlay)return 0;

//    printf("overlay: %i %i %ix%i\n",rd.left,rd.top,rd.right - rd.left,rd.bottom - rd.top);
	ddrval = g_lpddsOverlay->lpVtbl->UpdateOverlay(g_lpddsOverlay,&rs, g_lpddsPrimary, &rd, dwUpdateFlags, &ovfx);
    if(FAILED(ddrval))
    {
        // one cause might be the driver lied about minimum stretch
        // we should try upping the destination size a bit, or
        // perhaps shrinking the source size
	   	mp_msg(MSGT_VO, MSGL_ERR ,"<vo_directx><ERROR>UpdateOverlay failed\n" );
	  	mp_msg(MSGT_VO, MSGL_ERR ,"<vo_directx><ERROR>Overlay:x1:%i,y1:%i,x2:%i,y2:%i,w:%i,h:%i\n",rd.left,rd.top,rd.right,rd.bottom,rd.right - rd.left,rd.bottom - rd.top );
	  	mp_msg(MSGT_VO, MSGL_ERR ,"<vo_directx><ERROR>");
		switch (ddrval)
		{
			case DDERR_NOSTRETCHHW:
				{mp_msg(MSGT_VO, MSGL_ERR ,"hardware can't stretch: try to size the window back\n");break;}
            case DDERR_INVALIDRECT:
				{mp_msg(MSGT_VO, MSGL_ERR ,"invalid rectangle\n");break;}
			case DDERR_INVALIDPARAMS:
				{mp_msg(MSGT_VO, MSGL_ERR ,"invalid parameters\n");break;}
			case DDERR_HEIGHTALIGN:
				{mp_msg(MSGT_VO, MSGL_ERR ,"height align\n");break;}
			case DDERR_XALIGN:
				{mp_msg(MSGT_VO, MSGL_ERR ,"x align\n");break;}
			case DDERR_UNSUPPORTED:
				{mp_msg(MSGT_VO, MSGL_ERR ,"unsupported\n");break;}
			case DDERR_INVALIDSURFACETYPE:
				{mp_msg(MSGT_VO, MSGL_ERR ,"invalid surfacetype\n");break;}
			case DDERR_INVALIDOBJECT:
				{mp_msg(MSGT_VO, MSGL_ERR ,"invalid object\n");break;}
			case DDERR_SURFACELOST:
				{
					mp_msg(MSGT_VO, MSGL_ERR ,"surfaces lost\n");
					g_lpddsOverlay->lpVtbl->Restore( g_lpddsOverlay ); //restore and try again
			        g_lpddsPrimary->lpVtbl->Restore( g_lpddsPrimary );
			        ddrval = g_lpddsOverlay->lpVtbl->UpdateOverlay(g_lpddsOverlay,&rs, g_lpddsPrimary, &rd, dwUpdateFlags, &ovfx);
					if(ddrval !=DD_OK)mp_msg(MSGT_VO, MSGL_FATAL ,"<vo_directx><FATAL ERROR>UpdateOverlay failed again\n" );
					break;
				}
            default:
                mp_msg(MSGT_VO, MSGL_ERR ," 0x%x\n",ddrval);
		}
	    /*ok we can't do anything about it -> hide overlay*/
		if(ddrval != DD_OK)
		{
			ddrval = g_lpddsOverlay->lpVtbl->UpdateOverlay(g_lpddsOverlay,NULL, g_lpddsPrimary, NULL, DDOVER_HIDE, NULL);
            return 1;
		}
	}
    return 0;
}

//find out supported overlay pixelformats
static uint32_t Directx_CheckOverlayPixelformats(void)
{
    DDCAPS          capsDrv;
    HRESULT         ddrval;
    DDSURFACEDESC2   ddsdOverlay;
	uint32_t        i;
    uint32_t        formatcount = 0;
	//get driver caps to determine overlay support
    ZeroMemory(&capsDrv, sizeof(capsDrv));
    capsDrv.dwSize = sizeof(capsDrv);
	ddrval = g_lpdd->lpVtbl->GetCaps(g_lpdd,&capsDrv, NULL);
    if (FAILED(ddrval))
	{
        mp_msg(MSGT_VO, MSGL_ERR ,"<vo_directx><ERROR>failed getting ddrawcaps\n");
		return 1;
	}
    if (!(capsDrv.dwCaps & DDCAPS_OVERLAY))
    {
		mp_msg(MSGT_VO, MSGL_ERR ,"<vo_directx><ERROR>Your card doesn't support overlay\n");
		return 1;
	}
    mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>testing supported overlay pixelformats\n");
    //it is not possible to query for pixel formats supported by the
    //overlay hardware: try out various formats till one works
    ZeroMemory(&ddsdOverlay, sizeof(ddsdOverlay));
    ddsdOverlay.dwSize = sizeof(ddsdOverlay);
    ddsdOverlay.ddsCaps.dwCaps=DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
    ddsdOverlay.dwFlags= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH| DDSD_PIXELFORMAT;
    ddsdOverlay.dwWidth=300;
    ddsdOverlay.dwHeight=280;
    ddsdOverlay.dwBackBufferCount=0;
    //try to create an overlay surface using one of the pixel formats in our global list
	i=0;
    do
    {
   		ddsdOverlay.ddpfPixelFormat=g_ddpf[i].g_ddpfOverlay;
        ddrval = g_lpdd->lpVtbl->CreateSurface(g_lpdd,&ddsdOverlay, &g_lpddsOverlay, NULL);
        if (ddrval == DD_OK)
		{
			 mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><FORMAT OVERLAY>%i %s supported\n",i,g_ddpf[i].img_format_name);
			 g_ddpf[i].drv_caps = VFCAP_CSP_SUPPORTED |VFCAP_OSD |VFCAP_CSP_SUPPORTED_BY_HW | VFCAP_HWSCALE_UP;
			 formatcount++;}
	    else if (vf_settings) {
			// try to remove unsupported overlay format in vf
			int j;
			char *name=g_ddpf[i].img_format_name;
			for(j = 0 ; vf_settings[j].name ; j++) {
				if (!strnicmp(vf_settings[j].name, name, 4)){
					*vf_settings[j].name = '_';
					break;
				}
			}
			mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><FORMAT OVERLAY>%i %s not supported\n",i,name);
		}
		if (g_lpddsOverlay != NULL) {g_lpddsOverlay->lpVtbl->Release(g_lpddsOverlay);g_lpddsOverlay = NULL;}
	} while( ++i < NUM_FORMATS );
    mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>Your card supports %i of %i overlayformats\n",formatcount, NUM_FORMATS);
	if (formatcount == 0)
	{
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><WARN>Your card supports overlay, but we couldn't create one\n");
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>This can have the following reasons:\n");
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>- you are already using an overlay with another app\n");
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>- you don't have enough videomemory\n");
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>- vo_directx doesn't support the cards overlay pixelformat\n");
		return 1;
	}
    if(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYMIRRORLEFTRIGHT)mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>can mirror left right\n"); //I don't have hardware which
    if(capsDrv.dwFXCaps & DDFXCAPS_OVERLAYMIRRORUPDOWN )mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>can mirror up down\n");      //supports those send me one and I'll implement ;)
	return 0;
}

//find out the Pixelformat of the Primary Surface
static uint32_t Directx_CheckPrimaryPixelformat(void)
{
	uint32_t i=0;
    uint32_t formatcount = 0;
	DDPIXELFORMAT	ddpf;
	DDSURFACEDESC2   ddsd;
    HDC             hdc;
    HRESULT         hres;
	COLORREF        rgbT=RGB(0,0,0);
	mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>checking primary surface\n");
	memset( &ddpf, 0, sizeof( DDPIXELFORMAT ));
    ddpf.dwSize = sizeof( DDPIXELFORMAT );
    //we have to create a primary surface first
	if(Directx_CreatePrimarySurface()!=0)return 1;
	if(g_lpddsPrimary->lpVtbl->GetPixelFormat( g_lpddsPrimary, &ddpf ) != DD_OK )
	{
		mp_msg(MSGT_VO, MSGL_FATAL ,"<vo_directx><FATAL ERROR>can't get pixelformat\n");
		return 1;
	}
    while ( i < NUM_FORMATS )
    {
	   if (g_ddpf[i].g_ddpfOverlay.dwRGBBitCount == ddpf.dwRGBBitCount)
	   {
           if (g_ddpf[i].g_ddpfOverlay.dwRBitMask == ddpf.dwRBitMask)
		   {
			   mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><FORMAT PRIMARY>%i %s supported\n",i,g_ddpf[i].img_format_name);
			   g_ddpf[i].drv_caps = VFCAP_CSP_SUPPORTED |VFCAP_OSD;
			   formatcount++;
               primary_image_format=g_ddpf[i].img_format;
		   }
	   }
	   i++;
    }
    //get the colorkey for overlay mode
	destcolorkey = CLR_INVALID;
    if (windowcolor != CLR_INVALID && g_lpddsPrimary->lpVtbl->GetDC(g_lpddsPrimary,&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);
        SetPixel(hdc, 0, 0, windowcolor);
        g_lpddsPrimary->lpVtbl->ReleaseDC(g_lpddsPrimary,hdc);
    }
    // read back the converted color
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = g_lpddsPrimary->lpVtbl->Lock(g_lpddsPrimary,NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;
    if (hres == DD_OK)
    {
        destcolorkey = *(DWORD *) ddsd.lpSurface;
        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            destcolorkey &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;
        g_lpddsPrimary->lpVtbl->Unlock(g_lpddsPrimary,NULL);
    }
    if (windowcolor != CLR_INVALID && g_lpddsPrimary->lpVtbl->GetDC(g_lpddsPrimary,&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        g_lpddsPrimary->lpVtbl->ReleaseDC(g_lpddsPrimary,hdc);
    }
	//release primary
	g_lpddsPrimary->lpVtbl->Release(g_lpddsPrimary);
	g_lpddsPrimary = NULL;
	if(formatcount==0)
	{
		mp_msg(MSGT_VO, MSGL_FATAL ,"<vo_directx><FATAL ERROR>Unknown Pixelformat\n");
		return 1;
	}
	return 0;
}

static void SetView(void){
	RECT rc;
	static BOOL bOldView = TRUE;

	if (bOldView == bFullView) return;

	disable_manage = 1;

	if (bFullView) {
		xborder = xborder0;
		yborder = yborder0;
		GetWindowRect(hWnd, &rc);
		if(gui_skin.enable_appskin && gui_skin.border_ok) {
			rc.top -= gui_skin.title.img->height;
			rc.left -= gui_skin.border_left.img->width;
			rc.right += gui_skin.border_right.img->width;
			rc.bottom += gui_skin.border_bottom.img->height;
		} else {
			AdjustWindowRect(&rc, WNDSTYLE, TRUE);
			if(!show_menubar_w32)
				rc.top += menuHeight;
		}
		SetWindowLong(hWnd, GWL_STYLE, WNDSTYLE|WS_VISIBLE);
		SetWindowPos(hWnd, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top+controlbar_height,
			SWP_NOZORDER|SWP_NOOWNERZORDER);
		if(show_menubar_w32)
			SetMenu(hWnd, hMenu);
		if(show_controlbar)
			ShowWindow(hWndControlbar, SW_SHOW);
	}
	else {
		POINT p;
		p.x = p.y = 0;
		ClientToScreen(hWnd, &p);
		xborder = yborder = 0;
		GetClientRect(hWnd,&rc);
		SetMenu(hWnd, NULL);
		if(show_controlbar)
			ShowWindow(hWndControlbar, SW_HIDE);
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP|WS_THICKFRAME|WS_VISIBLE);
		SetWindowPos(hWnd, NULL, p.x, p.y, rc.right, rc.bottom-controlbar_height,
			SWP_NOZORDER|SWP_NOOWNERZORDER);
	}
	bOldView = bFullView;
	disable_manage = 0;

	Directx_ManageDisplay();
	
}

static BOOL CALLBACK ControlbarProcNormal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
   		case WM_NOTIFY:
			SetFocus(vo_fs?hWndFS:hWnd);
		break;
		case WM_HSCROLL:
		{
			char tmps[20];
			POINT p;
			float x;
			SetFocus(vo_fs?hWndFS:hWnd);
			GetCursorPos(&p);
			ScreenToClient((HWND)lParam, &p);
			if ((HWND)lParam == hWndSeekbar)
				x = (p.x-10)*100.0/(seekbar_width-20);
			else
				x = (p.x-10)*100/(volumebar_width-20);
			SendMessage((HWND)lParam, TBM_SETPOS, TRUE, (LPARAM)x);
			if (!bSeeking && (LOWORD(wParam)==TB_ENDTRACK || LOWORD(wParam)==TB_THUMBTRACK))
			{
				if ((HWND)lParam == hWndSeekbar) {
					bSeeking = 1;
					sprintf(tmps, "seek %f 1", x);
				}
				else if ((HWND)lParam == hWndVolumebar)
				{
					sprintf(tmps, "volume %d 1", (int)x);
				}
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
			}
			
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL CALLBACK ControlbarProcSkin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
		{
			RECT r;
			PAINTSTRUCT ps;
			HDC hdc;
			HBRUSH brush,oldbrush;
			GetClientRect(hDlg, &r);
			hdc = BeginPaint(hDlg, &ps);
			if(gui_skin.is_ok && (!vo_fs || ControlbarShow)) {
				on_paint(&gui_skin, hdc, hDlg, bkgndbrush, r, paused || play_finished);
			} else {
				brush=CreateSolidBrush(RGB(0,0,0));
				oldbrush=(HBRUSH)SelectObject(hdc,brush);
				Rectangle(hdc,r.left, r.top, r.right, r.bottom);
				SelectObject(hdc,oldbrush);
			}
			EndPaint(hDlg, &ps);
			return FALSE;
		}
	case WM_MOUSEMOVE:
		{
			RECT r;
			POINT p;
			p.x = LOWORD(lParam);
			p.y = HIWORD(lParam);
			GetClientRect(hDlg, &r);

			if(auto_hide_control && !(p.x > 0 && p.x < r.right && p.y > 0 && p.y < r.bottom)) {
				ReleaseCapture();
				ControlbarShow = FALSE;
				on_mouse_leave(&gui_skin);
				ShowWindow(hDlg, SW_HIDE);
				break;
			}

			if (vo_fs && auto_hide) {
				KillTimer(hWndFS, TIMER_HIDE_CURSOR);
				ShowCursor(TRUE);
			}

			if(on_mouse_move(&gui_skin, hDlg, r, p))
				InvalidateRect(hDlg, NULL, FALSE);

			if(auto_hide_control) SetCapture(hDlg);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			RECT r;
			POINT p;
			p.x = LOWORD(lParam);
			p.y = HIWORD(lParam);
			GetClientRect(hDlg, &r);

			if(on_lbutton_down(&gui_skin, hDlg, r, p, hWnd))
				InvalidateRect(hDlg, NULL, FALSE);

			if(auto_hide_control) SetCapture(hDlg);			
			return FALSE;
		}
	case WM_LBUTTONUP:
		{
			RECT r;
			POINT p;
			p.x = LOWORD(lParam);
			p.y = HIWORD(lParam);
			GetClientRect(hDlg, &r);
			ReleaseCapture();

			if(on_lbutton_up(&gui_skin, hDlg, r, p, hWnd))
				InvalidateRect(hDlg, NULL, FALSE);

			if(auto_hide_control) SetCapture(hDlg);
			return FALSE;
		}
	}

    return FALSE;
}

extern void display_playlistwindow(int x,int y,int style);
extern void close_playlistwindow(void);
extern int is_dvdnav_menu();

static void AddFavorite(const char *item){
	minf2.wID++;
	minf2.dwTypeData = item;
	minf2.cch = strlen(item);
	InsertMenuItem(hFavoriteMenu, 0xFFFFFFFF, TRUE, &minf2);
}

static void UpdateFavorite(int id,const char *item){
	int nID = IDM_FAVORITES + id;
	if(nID > IDM_FAVORITES && nID <= minf2.wID)
		ModifyMenu(hFavoriteMenu, nID , MF_BYCOMMAND | MF_STRING , nID, item);
}

static void ReflashFavorite(){
	int i = minf2.wID - IDM_FAVORITES;
	int j = 2;
	if(auto_resume > 1)
		j++;
	i += j;

	while(i >= j)
	{
		DeleteMenu(hFavoriteMenu, i, MF_BYPOSITION);
		i--;
	}
	minf2.wID = IDM_FAVORITES;
	if(initStatus())
	{
		int num = getStatusNum();
		int i;
		for(i = 0; i < num; i++)
		{
			char s[DMAX_PATH],t[16];
			char *str = getNameById(i);
			if(str) {
				int time = getStatusById(i);
				if(time >= 3600)
					snprintf(t, 16, " [%02d:%02d:%02d]", time/3600, (time/60)%60, time%60);
				else if(time < 3600 && time > 0)
					snprintf(t, 16, " [%02d:%02d]", time/60, time%60);
				else
					snprintf(t, 16, " [00:00]");
				snprintf(s, DMAX_PATH, "%s%s", _mbsrchr(str,'\\') + 1, t);
				free(str);
				AddFavorite(s);
			} else {
				snprintf(s, DMAX_PATH, "Unknown file");
				AddFavorite(s);
			}
		}
		freeStatus();
	}
	DrawMenuBar(hWnd);
}

static void show_cursor(int times)
{
	if(vo_fs && auto_hide) {
		nothide_times = times;
		while (ShowCursor(TRUE)<0);
		SetTimer(hWndFS, TIMER_HIDE_CURSOR, auto_hide, NULL);
	}
}

typedef HRESULT (__stdcall * SetWindowThemeFunct)(HWND, LPCWSTR, LPCWSTR);
typedef HRESULT (__stdcall * DwmAttributeFunct)(HWND,DWORD,LPCVOID,DWORD);

static LRESULT CALLBACK MouseHookProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode == HC_ACTION)
	{
		// get a pointer to the mouse hook struct.
		PMOUSEHOOKSTRUCT mhs = (PMOUSEHOOKSTRUCT) lParam;

		// intercept messages for left button down and up
		switch(wParam)
		{
		case WM_NCLBUTTONDOWN:
			{
				if (!bNcLButtonDown && HTCAPTION == mhs->wHitTestCode && mhs->hwnd == hWnd)
					bNcLButtonDown = TRUE;
			}
			break;
		case WM_LBUTTONUP:
			{
				if(bNcLButtonDown && mhs->hwnd == hWnd) {
					PostMessage(hWnd, WM_NCLBUTTONUP, HTCAPTION, MAKELONG(mhs->pt.x,mhs->pt.y));
					bNcLButtonDown = FALSE;
				}
			}
			break;

		default:
			break;
		}
	}
	return CallNextHookEx(hMHook, nCode, wParam, lParam);
}

static void UpdateThumbnailToolbar(HWND hWnd, BOOL ReAdd)
{
	if(need_recreate) {
		HRESULT hr = CoCreateInstance(&CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, &IID_ITaskbarList3Vtbl, (void **)&g_pTaskbarList);
		if (SUCCEEDED(hr)) {
			hr = ITaskbarList3_HrInit(g_pTaskbarList);
			if (FAILED(hr)) {
				ITaskbarList3_Release(g_pTaskbarList);
				g_pTaskbarList = NULL;
			}
		}
		need_recreate = 0;
	}
	if (g_pTaskbarList && !vo_fs) {
		if(play_finished || !filename)
			ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_NOPROGRESS);
		else if(paused)
			ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_PAUSED);
		else
			ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_NORMAL);

		THUMBBUTTON buttons[3] = {};

		buttons[0].dwMask = THB_BITMAP | THB_FLAGS;
		buttons[0].dwFlags = THBF_ENABLED;
		buttons[0].iId = IDTB_PREVIOUS;
		buttons[0].iBitmap = 2;

		buttons[1].dwMask = THB_BITMAP | THB_FLAGS;
		buttons[1].dwFlags = THBF_ENABLED;
		buttons[1].iId = IDTB_PLAY_PAUSE;
		if(paused || play_finished || !filename)
			buttons[1].iBitmap = 0;
		else
			buttons[1].iBitmap = 1;
		buttons[2].dwMask = THB_BITMAP | THB_FLAGS;
		buttons[2].dwFlags = THBF_ENABLED;
		buttons[2].iId = IDTB_NEXT;
		buttons[2].iBitmap = 3;

		if(ReAdd) {
			HIMAGELIST hImglist = ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TASK_THUMBNAIL),
				16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
			if(hImglist) {
				ITaskbarList3_ThumbBarSetImageList(g_pTaskbarList, hWnd, hImglist);
				ITaskbarList3_ThumbBarAddButtons(g_pTaskbarList, hWnd, 3, buttons);
				ImageList_Destroy(hImglist);
			}
		} else
			ITaskbarList3_ThumbBarUpdateButtons(g_pTaskbarList, hWnd, 3, buttons);

	}
}

static int reloadskin(HWND hWnd, HWND hWndctrl, const char *skname, int appskin)
{
	int width, height;
	HINSTANCE h;
	char *kk_ini, border[8];
	BOOL SkinedApp = gui_skin.enable_appskin & gui_skin.border_ok;

	if(!skname || !hWnd || vo_fs)
		return 0;

	SetWindowRgn(hWnd, NULL, TRUE);
	ShowWindow(hWnd,SW_HIDE);

	freeappskin(&gui_skin);
	freeskin(&gui_skin);
	freelogo(&gui_logo);

	initskin(skname, &gui_logo, &gui_skin);

	if (bkgndbrush) {
		DeleteObject(bkgndbrush);
		bkgndbrush = CreateSolidBrush(gui_logo.bg_color);
	}

	if(show_controlbar && gui_skin.status.show)
		show_status = gui_skin.status.show;

	auto_hide_control = auto_hide_control_save;

	loadlogo(&gui_logo, hWnd);
	if(loadappskin(&gui_skin, hWnd)) {
		show_menubar_w32 = 0;
		if(!SkinedApp) {
			SetMenu(hWnd, NULL);
			if(!hMHook) hMHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC) MouseHookProc, hInstance, GetCurrentThreadId());
			if(h = LoadLibrary("dwmapi.dll")) {
				DWORD dwAttr = 1;
				DwmAttributeFunct DwmSetWindowAttribute = (DwmAttributeFunct)GetProcAddress(h, "DwmSetWindowAttribute");
				if(DwmSetWindowAttribute) DwmSetWindowAttribute(hWnd, 2, &dwAttr, 4);
				FreeLibrary(h);
			}
			if(h = LoadLibrary("uxtheme.dll")) {
			   SetWindowThemeFunct SetWindowTheme = (SetWindowThemeFunct)GetProcAddress(h, "SetWindowTheme");
			   if(SetWindowTheme) SetWindowTheme(hWnd, L" ",L" ");
			   FreeLibrary(h);
			}
		}
	} else if(SkinedApp) {
		UnhookWindowsHookEx(hMHook);
		hMHook = NULL;
		if(h = LoadLibrary("dwmapi.dll")) {
			DWORD dwAttr = 0;
			DwmAttributeFunct DwmSetWindowAttribute = (DwmAttributeFunct)GetProcAddress(h, "DwmSetWindowAttribute");
			if(DwmSetWindowAttribute) DwmSetWindowAttribute(hWnd, 2, &dwAttr, 4);
			FreeLibrary(h);
		}
		if(h = LoadLibrary("uxtheme.dll")) {
		   SetWindowThemeFunct SetWindowTheme = (SetWindowThemeFunct)GetProcAddress(h, "SetWindowTheme");
		   if(SetWindowTheme) SetWindowTheme(hWnd, NULL, NULL);
		   FreeLibrary(h);
		}
		if(show_menubar) {
			show_menubar_w32 = show_menubar;
			SetMenu(hWnd, hMenu);
		}
	}

	if(show_controlbar && hWndctrl) {
		controlbar_offset = skin_controlbar ? 0 : 4;
		if(!skin_controlbar) {
			controlbar_height_fixed = volumebar_height;
			if(!hWndSeekbar) {
				hWndSeekbar = GetDlgItem(hWndctrl, IDC_CTL_SEEK);
				PostMessage(hWndSeekbar, TBM_SETRANGEMAX, FALSE, 100);
			}
			if(!hWndVolumebar) {
				hWndVolumebar = GetDlgItem(hWndctrl, IDC_CTL_VOLUME);
				PostMessage(hWndVolumebar, TBM_SETRANGEMAX, FALSE, 100);
			}
			PostMessage(hWndVolumebar, TBM_SETPOS, TRUE, volpercent);
			SetWindowLong(hWndctrl, DWL_DLGPROC, (LONG)ControlbarProcNormal);
			ShowWindow(hWndSeekbar, SW_SHOW);
			ShowWindow(hWndVolumebar, SW_SHOW);
		} else {
			if(hWndSeekbar) ShowWindow(hWndSeekbar, SW_HIDE);
			if(hWndVolumebar)  ShowWindow(hWndVolumebar, SW_HIDE);
			SetWindowLong(hWndctrl, DWL_DLGPROC, (LONG)ControlbarProcSkin);
			loadskin(&gui_skin, hWndctrl);
			if(gui_skin.auto_hide) auto_hide_control = 1;
			controlbar_height_fixed = gui_skin.background_height;
		}
		if(auto_hide_control) {
			controlbar_height = 0;
			ControlbarShow = FALSE;
			ShowWindow(hWndctrl, SW_HIDE);
		} else {
			controlbar_height = controlbar_height_fixed;
			ShowWindow(hWndctrl, SW_SHOW);
		}
		InvalidateRect(hWndctrl, NULL, FALSE);
	}

	if(gui_skin.background_width > 360)
		min_video_width = gui_skin.background_width;
	else
		min_video_width = 360;

	if(gui_skin.enable_appskin && gui_skin.border_ok) {
		xborder0 = xborder = gui_skin.border_left.img->width + gui_skin.border_right.img->width;
		yborder0 = yborder = gui_skin.title.img->height + gui_skin.border_bottom.img->height + controlbar_height;
	} else {
		RECT rd;
		memset(&rd, 0, sizeof(rd));
		AdjustWindowRect(&rd,WNDSTYLE,TRUE);
		xborder = rd.right - rd.left;
		yborder = rd.bottom - rd.top;
		if(!show_menubar_w32)
			yborder -= GetSystemMetrics(SM_CYMENU);
		yborder += controlbar_height;
		xborder0 = xborder;
		yborder0 = yborder;
	}

	if(!filename) {
		width = min_video_width;
		height = (int)((float)min_video_width/1.2);
		window_aspect = (float)(width-xborder)/(float)(height-yborder);
		SetWindowPos(hWnd,NULL,0,0,1,1,SWP_NOMOVE);
	} else {
		vo_dheight  = (int)((float)vo_dwidth / window_aspect);
		width = vo_dwidth+xborder;
		height = vo_dheight+yborder;
	}

	kk_ini = get_path("kk.ini");
	WritePrivateProfileString("gui", "skin", skname, kk_ini);
	if(appskin) {
		sprintf(border, "%d",skinned_player);
		WritePrivateProfileString("gui", "skinned_player", border, kk_ini);
	} else {
		EnableMenuItem(hSkinMenu, IDM_SKINS+101, gui_skin.background.state?MF_ENABLED:MF_GRAYED);
	}
	sprintf(border, "%d",skin_controlbar);
	WritePrivateProfileString("gui", "skin_controlbar", border, kk_ini);
	free(kk_ini);

	if(skin_name && skin_name != skname) {
		free(skin_name);
		skin_name = strdup(skname);
	}

	SetWindowPos(hWnd,NULL,0,0,width,height,SWP_NOMOVE|SWP_SHOWWINDOW);
	set_window_rgn(hWnd, &gui_skin, !vo_fs & bFullView, TRUE);

	UpdateThumbnailToolbar(hWnd, TRUE);
	return 1;
}

extern int ShowFavDlg(HINSTANCE hInstance,HWND hWndParent);

//function handles input
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	if ( message == WM_TASKBARCREATED )
	{
		Shell_NotifyIcon(NIM_ADD, &nid);
		return 0;
	}

	if(is_win7 && message == s_uTBBC) {
		if (!g_pTaskbarList) {
			HRESULT hr = CoCreateInstance(&CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, &IID_ITaskbarList3Vtbl, (void **)&g_pTaskbarList);
			if (SUCCEEDED(hr)) {
				hr = ITaskbarList3_HrInit(g_pTaskbarList);
				if (FAILED(hr)) {
					ITaskbarList3_Release(g_pTaskbarList);
					g_pTaskbarList = NULL;
				} else {
					if(!play_finished && filename) {
						if(paused)
							ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_PAUSED);
						else 
							ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_NORMAL);
					}
					HIMAGELIST hImglist = ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TASK_THUMBNAIL),
						16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
					if (hImglist) {
						hr = ITaskbarList3_ThumbBarSetImageList(g_pTaskbarList, hWnd, hImglist);
						if (SUCCEEDED(hr)) {
							THUMBBUTTON buttons[3] = {};

							buttons[0].dwMask = THB_BITMAP | THB_FLAGS;
							buttons[0].dwFlags = THBF_ENABLED;
							buttons[0].iId = IDTB_PREVIOUS;
							buttons[0].iBitmap = 2;

							buttons[1].dwMask = THB_BITMAP | THB_FLAGS;
							buttons[1].dwFlags = THBF_ENABLED;
							buttons[1].iId = IDTB_PLAY_PAUSE;
							if(paused || play_finished || !filename)
								buttons[1].iBitmap = 0;
							else
								buttons[1].iBitmap = 1;

							buttons[2].dwMask = THB_BITMAP | THB_FLAGS;
							buttons[2].dwFlags = THBF_ENABLED;
							buttons[2].iId = IDTB_NEXT;
							buttons[2].iBitmap = 3;

							ITaskbarList3_ThumbBarAddButtons(g_pTaskbarList, hWnd, 3, buttons);
							ImageList_Destroy(hImglist);
						}
						hImglist = NULL;
					}
				}
			}
		}
	} else switch (message) {
		case WM_NCCALCSIZE:
		{
			gui_skin.dlg_frame = 0;
			if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView && hWndFS) {
				RECT *rc;
				if(wParam)
					rc = &((LPNCCALCSIZE_PARAMS)lParam)->rgrc[0];
				else
					rc = (LPRECT)lParam;
				rc->top += gui_skin.title.img->height;
				rc->left += gui_skin.border_left.img->width;
				rc->right -= gui_skin.border_right.img->width;
				rc->bottom -= gui_skin.border_bottom.img->height;
				if(IsZoomed(hWnd)) {
					gui_skin.dlg_frame = GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYDLGFRAME);
					rc->top += gui_skin.dlg_frame;
					rc->bottom -= gui_skin.dlg_frame;
				}
				return 0;
			}
			break;
		}
		case WM_NCHITTEST:
		{
			if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView) {
				LRESULT lRet = DefWindowProc(hWnd, message, wParam, lParam);
				if (lRet == HTZOOM || lRet == HTHELP || lRet == HTREDUCE || lRet == HTSYSMENU
					|| lRet == HTMAXBUTTON || lRet == HTMINBUTTON || lRet == HTCLOSE)
					return HTCAPTION;
				else
					return lRet;
			}
			break;
	    }
		case WM_NCMOUSEMOVE:
		{
			if(vo_fs || !bFullView || !gui_skin.enable_appskin || !gui_skin.border_ok)
				break;
			on_ncmouse_move(&gui_skin, hWnd, title_text);
			break;
		}
		case WM_NCLBUTTONDOWN:
		{
			if(vo_fs || !bFullView)
				break;
			if(gui_skin.enable_appskin && gui_skin.border_ok)
				on_nclbutton_down(&gui_skin, hWnd, title_text);
			else if(wParam == HTSYSMENU && !show_menubar && hPopupMenu) {
				POINT p;
				p.x = LOWORD(lParam);
				p.y = HIWORD(lParam);
				TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
				return TRUE;
			}
			break;
		}
		case WM_NCLBUTTONUP:
		{
			if(vo_fs || !bFullView || !gui_skin.enable_appskin || !gui_skin.border_ok)
				break;
			on_nclbutton_up(&gui_skin, hWnd, hPopupMenu, title_text);
			break;
		}
		case WM_NCRBUTTONDOWN:
		{
			if(vo_fs || !bFullView || show_menubar_w32 || !hPopupMenu ||
				wParam == HTMAXBUTTON || wParam == HTMINBUTTON || wParam == HTCLOSE)
				break;
			POINT p;
			p.x = LOWORD(lParam);
			p.y = HIWORD(lParam);
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
			return TRUE;
		}
		case WM_NCACTIVATE:
			if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView) {
				draw_border(hWnd, &gui_skin, title_text, FALSE);
				return TRUE;
			} else if(vidmode && adapter_count > 2) //only disable if more than one adapter.
				return 0;
			break;
		case 0x00AE/*WM_NCUAHDRAWCAPTION*/:
		case 0x00AF/*WM_NCUAHDRAWFRAME*/:
			if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView)
				return 0;
			break;
		case WM_ACTIVATEAPP:
			if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView)
				draw_border(hWnd, &gui_skin, title_text, FALSE);
			break;
		case WM_NCPAINT:
		{
			if (!vo_fs) {
				if(gui_skin.enable_appskin && gui_skin.border_ok && bFullView) {
					draw_border(hWnd, &gui_skin, title_text, FALSE);
					return 0;
				} else if(!vidmode && !bFullView) {
					RECT rc;
					HDC hdc;
					GetWindowRect(hWnd, &rc);
					rc.right -= rc.left;
					rc.bottom -= rc.top;
					rc.left = 0;
					rc.top = 0;
					hdc = GetWindowDC(hWnd);
					FillRect(hdc, &rc, bIdle?bkgndbrush:colorbrush);
					ReleaseDC(hWnd, hdc);
					return 0;
				}
			}
			break;
		}
		case WM_ERASEBKGND:
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			if (!vo_fs && !vidmode) {
				if (bFullView) rc.bottom -= controlbar_height;
				if (bIdle) {
					int width,height;
					HDC dc1 = CreateCompatibleDC((HDC)wParam);
					HDC dc2 = CreateCompatibleDC((HDC)wParam);
					HBITMAP bmp = CreateCompatibleBitmap((HDC)wParam, rc.right, rc.bottom);
					SelectObject(dc1, bmp);
					if(gui_logo.is_ok) {
						SelectObject(dc2, gui_logo.logo.bmp);
						width = gui_logo.logo.img->width;
						height = gui_logo.logo.img->height;
					} else {
						SelectObject(dc2, hLogo);
						width = logoW;
						height = logoH;
					}
					FillRect(dc1, &rc, bkgndbrush);
					BitBlt(dc1, (rc.right+rc.left-width)>>1, (rc.bottom+rc.top-height)>>1,
						width, height, dc2, 0, 0, SRCCOPY);
					BitBlt((HDC)wParam, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
						dc1, rc.left, rc.top, SRCCOPY);
					DeleteObject(bmp);
					DeleteDC(dc1);
					DeleteDC(dc2);
				} else {
					FillRect((HDC)wParam, &rc, colorbrush);
				}
			} else {
				FillRect((HDC)wParam, &rc, bIdle?bkgndbrush:colorbrush);
			}
			return FALSE;
		}
		case WM_COMMAND:
			{
			char tmps[DMAX_PATH + 21];
			if(LOWORD(wParam) >= IDM_SKINS) {
				int id = LOWORD(wParam) - IDM_SKINS;
				if(id < 100) {
					if(reloadskin(hWnd, hWndControlbar, skins_name[id], 0))
						CheckMenuRadioItem(hSkinMenu, IDM_SKINS, IDM_SKINS+100, IDM_SKINS+id, MF_BYCOMMAND);
				} else if(id == 101) {
					skinned_player = !skinned_player;
					reloadskin(hWnd, hWndControlbar, skin_name, 1);
					CheckMenuItem(hSkinMenu, IDM_SKINS+101, skinned_player?MF_CHECKED:MF_UNCHECKED);
				} else if(id == 102) {
					skin_controlbar = !skin_controlbar;
					reloadskin(hWnd, hWndControlbar, skin_name, 1);
					CheckMenuItem(hSkinMenu, IDM_SKINS+102, skin_controlbar?MF_CHECKED:MF_UNCHECKED);
				}
				return 0;
			} else if (LOWORD(wParam) >= IDM_FAVORITES) {
				int id = LOWORD(wParam) - IDM_FAVORITES - 1;
				if(id == -1 && filename && !strstr(filename, "://"))
				{
					initStatus();
					int nid = addStatus(filename);
					if(nid >= 0)
					{
						char s[DMAX_PATH],t[16];
						int i;
						if(nid == 0)
							i = getStatusNum();
						else
							i = nid;
						int time = getStatusById(i - 1);
						if(time >= 3600)
							snprintf(t, 16, " [%02d:%02d:%02d]", time/3600, (time/60)%60, time%60);
						else if(time < 3600 && time > 0)
							snprintf(t, 16, " [%02d:%02d]", time/60, time%60);
						else
							snprintf(t, 16, " [00:00]");
						snprintf(s, DMAX_PATH, "%s%s", _mbsrchr(filename,'\\') + 1, t);
						if(nid == 0)
							AddFavorite(s);
						else
							UpdateFavorite(i,s);
					}
					freeStatus();
				}
				else if(id >= 0)
				{
					initStatus();
					int time = getStatusById(id);
					char *file = getNameById(id);
					freeStatus();
					if(file) {
						snprintf(tmps,DMAX_PATH, "loadfile \"%s\"", file);
						free(file);
						mp_input_queue_cmd(mp_input_parse_cmd(tmps));
						seek2time(time);
					}
				}
				return 0;
			}
			if (LOWORD(wParam) >= IDM_SUBTITLE) {
				if(show_menubar)
					CheckMenuRadioItem(hSubtitleMenu, IDM_SUBTITLE, IDM_SUBTITLE + 99,
						LOWORD(wParam), MF_BYCOMMAND);
				sprintf(tmps, "sub_select %d", LOWORD(wParam) - IDM_SUBTITLE - 1);
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				return 0;
			}
			char *nfilename;

			switch (LOWORD(wParam)) {
				case IDM_OPEN:
				case IDM_ADD: {
					show_cursor(6);
					char *cmdline = NULL;
					BOOL bOpen = LOWORD(wParam)==IDM_OPEN;
					char play_str[64], add_str[64];
					if(sys_Language == 1) {
						LoadString(hInstance, IDS_LISTPLAY_SC, play_str, 60);
						LoadString(hInstance, IDS_LISTADD_SC, add_str, 60);
					} else if(sys_Language == 3 || sys_Language == 4){ 
						LoadString(hInstance, IDS_LISTPLAY_TC, play_str, 60);
						LoadString(hInstance, IDS_LISTADD_TC, add_str, 60);
					} else{ 
						strcpy(play_str,"Open File(s) to Play");
						strcpy(add_str,"Add File(s) to Playlist");
					}
					int i = GetOpenFileW(bOpen? play_str : add_str,  &nfilename, bOpen?&cmdline:NULL);
					if (i==1)
						snprintf(tmps, DMAX_PATH , "loadlist \"%s\" %d", nfilename, !bOpen);
					else if (i==2)
						snprintf(tmps, DMAX_PATH , "loadfile \"%s\" %d", nfilename, !bOpen);
					else
						break;
					close_playlistwindow();
					if(bOpen)
						SaveOldStatus();
					mp_parse_commandline(cmdline);
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					if(reload_when_open && bOpen)
						mp_input_queue_cmd(mp_input_parse_cmd("reload"));
					break;
				}
				case IDM_OPENAVS: {
					show_cursor(6);
					if(!vo_fs && !bFullView) {
						bFullView = TRUE;
						SetView();
					}
					char *cmdline = NULL;
					char play_str[64];
					if(sys_Language == 1) {
						LoadString(hInstance, IDS_LISTPLAY_SC, play_str, 60);
					} else if(sys_Language == 3 || sys_Language == 4) {
						LoadString(hInstance, IDS_LISTPLAY_TC, play_str, 60);
					} else {
						strcpy(play_str,"Open File(s) to Play");
					}
					int i = GetOpenFileW(play_str ,&nfilename, &cmdline);
					if (i==1) {
						snprintf(tmps, DMAX_PATH , "loadlist \"%s\"", nfilename);
					} else if (i==2) {
						open_with_dshow_demux = 1;
						snprintf(tmps, DMAX_PATH , "loadfile \"%s\"", nfilename);
					} else
						break;
					close_playlistwindow();
					SaveOldStatus();
					mp_parse_commandline(cmdline);
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					if(reload_when_open)
						mp_input_queue_cmd(mp_input_parse_cmd("reload"));
					break;
				}
				case IDM_FAVORITES_MANAGE:{
					show_cursor(6);
					if(ShowFavDlg(hInstance,hWnd))
						ReflashFavorite();
					break;
				}
				case IDM_PLAYLIST:{
					RECT r;
					POINT pt;
					show_cursor(6);
					if(!vo_fs) {
						GetWindowRect(hWnd, &r);
						pt.x = r.left;
						pt.y = r.bottom;
						if( pt.x >  vo_screenwidth-600)
							pt.x = vo_screenwidth-600;
						if( pt.y > vo_screenheight-300)
							pt.y = vo_screenheight-300;
					} else {
						pt.x = vo_screenwidth/2-300;
						pt.y = vo_screenheight-300;
					}
					HWND layer = HWND_NOTOPMOST;
					if (vo_fs || vo_ontop)
						layer = HWND_TOPMOST;
					display_playlistwindow(pt.x,pt.y,layer);
					break;
				}
				case IDM_OPENDVD: {
					if(!GetDVDDriver(hWnd))
						break;
					close_playlistwindow();
#ifdef CONFIG_DVDNAV
					if(!no_dvdnav) {
						mp_parse_commandline("-nocache");
						mp_input_queue_cmd(mp_input_parse_cmd("loadfile \"dvdnav://\""));
					}else
#endif
					mp_input_queue_cmd(mp_input_parse_cmd("loadfile \"dvd://\""));
					if(reload_when_open)
						mp_input_queue_cmd(mp_input_parse_cmd("reload"));
					break;
				}
				case IDM_OPENCD:
				{
					close_playlistwindow();
					if(GetPlayCD(hInstance, hWnd))
						mp_input_queue_cmd(mp_input_parse_cmd("loadfile \"cdda://\""));
					break;
				}
				case IDM_OPENFILE:
				{
					close_playlistwindow();
					char *file = NULL, *audio = NULL;
					if(GetOpenFile2(hInstance, hWnd, &file, &audio)) {
						if(file && audio) {
							if(strlen(audio) > 0)
								new_audio_stream = strdup(audio);
							free(audio);
						}
						if(file) {
							if(strlen(file) > 0)
								mp_input_queue_cmd(mp_input_parse_cmd(file));
							free(file);
						}
					}
					break;
				}
				case IDM_OPENURL:
				{
					close_playlistwindow();
					char *url = NULL;
					if(GetURL(hInstance, hWnd, &url)) {
						if(url) {
							if(strlen(url) > 0)
								mp_input_queue_cmd(mp_input_parse_cmd(url));
							free(url);
						}
					}
					break;
				}
				case IDTB_PLAY_PAUSE:
				case IDM_PLAY_PAUSE:
					{
						if(play_finished && last_filename)
						{
							DWORD lfa = GetFileAttributes(last_filename);
							if(lfa == 0xFFFFFFFF || (lfa & FILE_ATTRIBUTE_DIRECTORY))
								break;
							snprintf(tmps, DMAX_PATH + 20 , "loadfile \"%s\"", last_filename);
							mp_input_queue_cmd(mp_input_parse_cmd(tmps));
							paused_save = !paused_save;
							if(show_controlbar || g_pTaskbarList)
								SetTimer(hWnd, TIMER_PLAY_PAUSE, 200, NULL);
						}
						else
							mp_input_queue_cmd(mp_input_parse_cmd("pause"));
						break;
					}
				case IDM_STOP:
					{mp_input_queue_cmd(mp_input_parse_cmd("stop")); break;}
				case IDTB_PREVIOUS:
				case IDM_PRE:
					{mp_input_queue_cmd(mp_input_parse_cmd("pt_step -1")); break;}
				case IDTB_NEXT:
				case IDM_NEXT:
					{mp_input_queue_cmd(mp_input_parse_cmd("pt_step 1")); break;}
				case IDM_SPEED_UP:
					{mp_input_queue_cmd(mp_input_parse_cmd("speed_incr 0.1")); break;}
				case IDM_SPEED_DOWN:
					{mp_input_queue_cmd(mp_input_parse_cmd("speed_incr -0.1")); break;}
				case IDM_SPEED_NORMAL:
					{mp_input_queue_cmd(mp_input_parse_cmd("speed_set 1.0")); break;}
				case IDM_VOLUME_UP:
					{mp_input_queue_cmd(mp_input_parse_cmd("volume 1")); break;}
				case IDM_VOLUME_DOWN:
					{mp_input_queue_cmd(mp_input_parse_cmd("volume -1")); break;}
				case IDM_VOLUME_MUTE:
					{mp_input_queue_cmd(mp_input_parse_cmd("mute")); break;}
				case IDM_DVDC_PRE:
					{mp_input_queue_cmd(mp_input_parse_cmd("seek_chapter -1")); break;}
				case IDM_DVDC_NEXT:
					{mp_input_queue_cmd(mp_input_parse_cmd("seek_chapter 1")); break;}
				case IDM_DVD_ANGLE:
					{
						mp_input_queue_cmd(mp_input_parse_cmd("switch_angle"));
						KillTimer(hWnd, TIMER_DVDNAV);
						SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
						dvdnav_checktime=0;
						break;
					}
				case IDM_DVDT_PRE:
					{mp_input_queue_cmd(mp_input_parse_cmd("switch_title -1")); break;}
				case IDM_DVDT_NEXT:
					{mp_input_queue_cmd(mp_input_parse_cmd("switch_title 1")); break;}
				case IDM_DVD_TITLE:
					{
						mp_input_queue_cmd(mp_input_parse_cmd("dvdnav prev"));
						KillTimer(hWnd, TIMER_DVDNAV);
						SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
						dvdnav_checktime=0;
						break;
					}
				case IDM_DVD_ROOT:
					{
						mp_input_queue_cmd(mp_input_parse_cmd("dvdnav menu"));
						KillTimer(hWnd, TIMER_DVDNAV);
						SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
						dvdnav_checktime=0;
						break;
					}
				case IDM_DVD_UP:
					{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav up")); break;}
				case IDM_DVD_DOWN:
					{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav down")); break;}
				case IDM_DVD_LEFT:
					{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav left")); break;}
				case IDM_DVD_RIGHT:
					{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav right")); break;}
				case IDM_DVD_SEL:
					{
						mp_input_queue_cmd(mp_input_parse_cmd("dvdnav select"));
						KillTimer(hWnd, TIMER_DVDNAV);
						SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
						dvdnav_checktime=0;
						break;
					}
				case IDM_LOAD_SUB:
					show_cursor(3);
					if (GetSubFile(&nfilename)) {
						sprintf(tmps, "sub_load \"%s\"", nfilename);
						mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					}
					break;
				case IDM_OPENEQ:
					ShowEqualizer(hWnd);
					show_cursor(6);
					break;
				case IDM_SWITCH_FONT:
					mp_input_queue_cmd(mp_input_parse_cmd("switch_font"));
					break;
				case IDM_SUB_FONT:
					show_cursor(6);
					if (GetFontFile(AlternativeFont)) {
						sub_font_name = AlternativeFont;
						force_load_font = 1;
						vo_osd_changed(OSDTYPE_SUBTITLE);
					}
					break;
				case IDM_EXIT:
				case IDM_CLOSE:
					mp_input_queue_cmd(mp_input_parse_cmd("quit"));
					ExitGuiThread = 1;
					break;
				case IDM_ASPECT_FILE:
				case IDM_ASPECT_43:
				case IDM_ASPECT_54:
				case IDM_ASPECT_169:
				case IDM_ASPECT_235:
					if(!filename) break;
					switch(LOWORD(wParam))
					{
					case IDM_ASPECT_43:
						snprintf(tmps, DMAX_PATH , "switch_ratio 1.3333333");
						break;
					case IDM_ASPECT_54:
						snprintf(tmps, DMAX_PATH , "switch_ratio 1.25");
						break;
					case IDM_ASPECT_169:
						snprintf(tmps, DMAX_PATH , "switch_ratio 1.77777778");
						break;
					case IDM_ASPECT_235:
						snprintf(tmps, DMAX_PATH , "switch_ratio 2.35");
						break;
					default:
						snprintf(tmps, DMAX_PATH , "switch_ratio");
						break;
					}
					CheckMenuRadioItem(hMenu, IDM_ASPECT_FILE, IDM_ASPECT_169, LOWORD(wParam), MF_BYCOMMAND);
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					break;
				case IDM_AUDIO_STEREO:
				case IDM_AUDIO_LEFT:
				case IDM_AUDIO_RIGHT:
					if(!filename) break;
					switch(LOWORD(wParam))
					{
					case IDM_AUDIO_LEFT:
						snprintf(tmps, DMAX_PATH , "balance -1 1");
						break;
					case IDM_AUDIO_RIGHT:
						snprintf(tmps, DMAX_PATH , "balance 1 1");
						break;
					default:
						snprintf(tmps, DMAX_PATH , "balance 0 1");
						break;
					}
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					break;
				case IDM_AUIDO_DSP:
					if(!filename) break;
					snprintf(tmps, DMAX_PATH , "audio_dsp cfg");
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					break;
				case IDM_SWITCH_AUDIO:
					if(!filename) break;
					snprintf(tmps, DMAX_PATH , "switch_audio");
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					break;
				case IDM_KEEP_ASPECT:
					vo_keepaspect = !vo_keepaspect;
					CheckMenuItem(hMenu, IDM_KEEP_ASPECT, vo_keepaspect?MF_CHECKED:MF_UNCHECKED);
					Directx_ManageDisplay();
					break;
				case IDM_MOUSE_DVDNAV:
					mouse_dvdnav = !mouse_dvdnav;
					CheckMenuItem(hMenu, IDM_MOUSE_DVDNAV, mouse_dvdnav?MF_CHECKED:MF_UNCHECKED);
					break;
				case IDM_SHUTDOWN:
					auto_shutdown = !auto_shutdown;
					CheckMenuItem(hMenu, IDM_SHUTDOWN, auto_shutdown?MF_CHECKED:MF_UNCHECKED);
					break;
				case IDM_FIXED_SIZE:
					fixedsize = !fixedsize;
					CheckMenuItem(hMenu, IDM_FIXED_SIZE, fixedsize?MF_CHECKED:MF_UNCHECKED);
					break;
				case IDM_SCREENSAVER:
					disable_screensaver = !disable_screensaver;
					disable_screen_saver(disable_screensaver);
					CheckMenuItem(hMenu, IDM_SCREENSAVER, disable_screensaver?MF_CHECKED:MF_UNCHECKED);
					break;
				case IDM_MEDIA_INFO:
					show_cursor(6);
					show_media_info();
					break;
				case IDM_HOMEPAGE:
					show_cursor(3);
					ShellExecute(0, NULL, "http://mplayer-ww.com", NULL, NULL, SW_NORMAL);
					break;
				case IDM_MANPAGE:
						show_cursor(3);
						nfilename = get_path("help/man_page.html");
						if (GetFileAttributes(nfilename) == 0xFFFFFFFF)
							nfilename = get_path("man_page_ww.html");
						if (GetFileAttributes(nfilename) == 0xFFFFFFFF)
							nfilename = get_path("man_page.html");
						if (GetFileAttributes(nfilename) != 0xFFFFFFFF)
							ShellExecute(0, NULL, nfilename, NULL, NULL, SW_NORMAL);
						else
							ShellExecute(0, NULL, "http://www.mplayerhq.hu/DOCS/man/en/mplayer.1.html", NULL, NULL, SW_NORMAL);
						free(nfilename);
						break;
				case IDM_CMDLINE:
					show_cursor(3);
					MessageBox(hWnd, get_help_text(), "MPlayer", MB_OK | MB_ICONEXCLAMATION | (vo_ontop?MB_TOPMOST:0));
					break;
				case IDM_SUB_DOWNLOAD:
					if(subdownloader && filename) {
						snprintf(tmps, DMAX_PATH , "--send-to-mplayer --filename \"%s\"", filename);
						ShellExecute(0, NULL, subdownloader, tmps, NULL, SW_SHOW);
					}
					break;
				case IDM_PREVIEW:
					if(!filename)
						break;
					show_cursor(6);
					nfilename = get_path("meditor.exe");
					if (GetFileAttributes(nfilename) != 0xFFFFFFFF) {
					    snprintf(tmps, DMAX_PATH , "--generate-preview --filename \"%s\" --duration %d",
					    	filename, get_video_length());
						ShellExecute(0,NULL , nfilename, tmps, NULL, SW_SHOW);
						free(nfilename);
						break;
					} else
						free(nfilename);
					break;
				case IDM_CONFIG_ASSOC:
					show_cursor(6);
					nfilename = get_path("meditor.exe");
					if (GetFileAttributes(nfilename) != 0xFFFFFFFF)
						ShellExecute(0,NULL , nfilename, "--open-assoc", NULL, SW_SHOW);
					free(nfilename);
					break;
				case IDM_CONFIG:
					show_cursor(6);
					nfilename = get_path("meditor2.exe");
					if(GetFileAttributes(nfilename) == 0xFFFFFFFF) {
						free(nfilename);
						nfilename = get_path("meditor.exe");
					}
					if (GetFileAttributes(nfilename) != 0xFFFFFFFF)	{
						ShellExecute(0,NULL , nfilename, "--open-ontop", NULL, SW_SHOW);
						free(nfilename);
						break;
					}
					else
						free(nfilename);
				case IDM_INPUT_CONF:
					show_cursor(3);
					nfilename = get_path("meditor.exe");
					if (GetFileAttributes(nfilename) != 0xFFFFFFFF) {
						ShellExecute(0,NULL , nfilename, "--open-hotkey", NULL, SW_SHOW);
						free(nfilename);
						break;
					}
					else
						free(nfilename);
				case IDM_LOG:
				{
					show_cursor(3);
					static char *fnames[] = {
						"mplayer.ini", "input.ini", "log.txt"
					};
					nfilename = get_path(fnames[LOWORD(wParam) - IDM_CONFIG]);
					ShellExecute(0, NULL, nfilename, NULL, NULL, SW_NORMAL);
					free(nfilename);
					break;
				}
				case IDM_CHECKUPDATE: {
					show_cursor(3);
					check_update();
					break;
					}
				case IDM_ABOUT: {
					show_cursor(3);
					MSGBOXPARAMS mp;
					memset(&mp, 0, sizeof(mp));
					mp.cbSize = sizeof(mp);
					mp.hwndOwner = hWnd;
					mp.hInstance = GetModuleHandle(NULL);
					mp.lpszText = "MPlayer WW "VERSION"\n\nCopyright (C) 2000-2011 MPlayer Team\n\n\nMPlayer is distributed under the terms of the GNU\n General Public License Version 2. \nSourcecode is available at http://www.mplayerhq.hu";
					mp.lpszCaption = "About";
					mp.dwStyle = MB_USERICON | MB_OK | (vo_ontop?MB_TOPMOST:0);
					mp.lpszIcon = (LPSTR)IDI_APPICON;
					MessageBoxIndirect(&mp);
					break;
				}
			}
			return 0;
		}
		case WM_DROPFILES:
		{
			DWORD t;
			char tmps[DMAX_PATH],fname[DMAX_PATH];
			wchar_t tmpw[DMAX_PATH], tmpshot[DMAX_PATH];
			int i=0, j=DragQueryFileA((HDROP)wParam, -1,  NULL, 0);
			if (j == 1) {
				char * sub_exts[] = {  "utf", "utf8", "utf-8", "idx", "sub", "srt", "smi", "rt", "txt", "ssa", "aqt", "jss", "js", "ass", "lrc", NULL};
				char * pls_exts[] = {  "pls", "m3u", "m3u8", "fls", NULL };
				char *tmp_fname_ext;

				DragQueryFileA((HDROP)wParam, 0, tmps, DMAX_PATH-1);
				t = GetFileAttributes(tmps);
				if(t == 0xFFFFFFFF)
				{
					DragQueryFileW((HDROP)wParam, 0, tmpw, DMAX_PATH-1);
					GetShortPathNameW(tmpw,tmpshot,DMAX_PATH);
					wcstombs(tmps,tmpshot,DMAX_PATH);
				}
				else if (t & 0x10) // is a Directory?
				{
					directory_to_playlist(tmps);
					if (GetKeyState(VK_CONTROL) < 0)
						sprintf(tmps, "addlist \"%s\"", mplayer_pls);
					else
						sprintf(tmps, "loadlist \"%s\"", mplayer_pls);
					if(!vo_fs && !bFullView)
					{
						bFullView = TRUE;
						SetView();
					}
					close_playlistwindow();
					if(GetKeyState(VK_CONTROL) >= 0)
						SaveOldStatus();
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					if(reload_when_open && GetKeyState(VK_CONTROL) >= 0)
						mp_input_queue_cmd(mp_input_parse_cmd("reload"));
					goto EndDrag;
				}
				if (tmp_fname_ext = strrchr(tmps,'.'))
				{
					tmp_fname_ext++;
#ifdef CONFIG_ICONV
					for (i = (sub_cp ? 3 : 0); sub_exts[i]; i++)
#else
					for (i = 0; sub_exts[i]; i++)
#endif
					{
						if (!stricmp(sub_exts[i], tmp_fname_ext)) {
							char *nfilename = strdup(tmps);
							sprintf(tmps, "sub_load \"%s\"", nfilename);
							free(nfilename);
							mp_input_queue_cmd(mp_input_parse_cmd(tmps));
							goto EndDrag;
						}
					}
					for (i = 0; pls_exts[i]; i++)
					{
						if (!stricmp(pls_exts[i], tmp_fname_ext)) {
							HANDLE hFile;
							DWORD filesize = 0,filesizehigh = 0;
							hFile = CreateFile(tmps, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							if(hFile) {
								filesize = GetFileSize(hFile, &filesizehigh);
								CloseHandle(hFile);
							}
							if(!filesizehigh && filesize < 20480) {
								char *nfilename = strdup(tmps);
								sprintf(tmps, "loadlist \"%s\"", nfilename);
								free(nfilename);
								mp_input_queue_cmd(mp_input_parse_cmd(tmps));
								goto EndDrag;
							}
							break;
						}
					}
				}
				strcpy(fname,tmps);
				if (GetKeyState(VK_CONTROL) < 0)
					snprintf(tmps, DMAX_PATH, "loadfile \"%s\" 1", fname);
				else
					snprintf(tmps, DMAX_PATH, "loadfile \"%s\"", fname);
			} else {
				FILE * fp = fopen(mplayer_pls, "w");
				while (i<j) {
					DragQueryFileA((HDROP)wParam, i++, tmps, DMAX_PATH);
					if(GetFileAttributes(tmps) == 0xFFFFFFFF)
					{
						DragQueryFileW((HDROP)wParam, i - 1, tmpw, DMAX_PATH);
						GetShortPathNameW(tmpw,tmpshot,DMAX_PATH);
						wcstombs(tmps,tmpshot,DMAX_PATH);
					}
					fprintf(fp, "%s\n", tmps);
				}
				fclose(fp);
				if (GetKeyState(VK_CONTROL) < 0)
					sprintf(tmps, "addlist \"%s\"", mplayer_pls);
				else
					sprintf(tmps, "loadlist \"%s\"", mplayer_pls);
			}
			close_playlistwindow();
			if(GetKeyState(VK_CONTROL) >= 0)
				SaveOldStatus();
			mp_input_queue_cmd(mp_input_parse_cmd(tmps));
			if(reload_when_open && GetKeyState(VK_CONTROL) >= 0)
				mp_input_queue_cmd(mp_input_parse_cmd("reload"));
EndDrag:
			DragFinish((HDROP)wParam);
			SetForegroundWindow(hWnd);
			return 0;
		}
	case WM_TIMER:
		switch (wParam) {
		case TIMER_HIDE_CURSOR:
			if(nothide_times)
			{
				--nothide_times;
				break;
			}
			if (!GetMenu(hWnd))
				while(ShowCursor(FALSE)>=0);
			KillTimer(hWndFS, TIMER_HIDE_CURSOR);
			break;

			case TIMER_DVDNAV:
#ifdef CONFIG_DVDNAV
				if (!no_dvdnav && filename && is_dvdnav) {
					int is_menu = is_dvdnav_menu();
					if(mouse_dvdnav && !is_menu) {
						KillTimer(hWnd, TIMER_DVDNAV);
						mouse_dvdnav = 0;
						dvdnav_checktime = 0;
						CheckMenuItem(hMenu, IDM_MOUSE_DVDNAV, MF_UNCHECKED);
						update_subtitle_menu();
					} else if(is_menu && !mouse_dvdnav) {
						KillTimer(hWnd, TIMER_DVDNAV);
						mouse_dvdnav = 1;
						dvdnav_checktime = 0;
						CheckMenuItem(hMenu, IDM_MOUSE_DVDNAV, MF_CHECKED);
						update_subtitle_menu();
					}
					if(dvdnav_checktime > 3) {
						KillTimer(hWnd, TIMER_DVDNAV);
						dvdnav_checktime = 0;
					}
					dvdnav_checktime++;
				}
#else
				KillTimer(hWnd, TIMER_DVDNAV);
#endif
				break;
		case TIMER_PLAY_PAUSE:
			if(paused_save == paused)
				break;
			paused_save = paused;
			if (show_controlbar && hWndControlbar)
				InvalidateRect(hWndControlbar, NULL, FALSE);
			UpdateThumbnailToolbar(hWnd, FALSE);
			break;
		case TIMER_SWITCH_VIEW:
			if (!vo_fs && WndState == SW_NORMAL) {
				BOOL bNewView;
				POINT p;
				RECT r;
				GetCursorPos(&p);
				GetWindowRect(hWnd, &r);
				bNewView = p.x>r.left+5 && p.x<r.right-5 && p.y>r.top+5 && p.y<r.bottom-5;
				if (bNewView != bFullView) {
					bFullView = bNewView;
					SetView();
				}
			}
			break;
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			if(!no_dvdnav && mouse_dvdnav && filename && is_dvdnav)
			{
				char tmps[32];
				snprintf(tmps, 32, "set_mouse_pos %d %d", LOWORD(lParam), HIWORD(lParam));
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				break;
			}
			RECT r;
			static DWORD lastp=0;
			if (lastp != lParam)
			{
				lastp = lParam;
				if (vo_fs||vidmode) {
					if (auto_hide) {
						while (ShowCursor(TRUE)<0);
						SetTimer(hWndFS, TIMER_HIDE_CURSOR, auto_hide, NULL);
					} else
					if (!bHide) {
						bHide = TRUE;
						while(ShowCursor(FALSE)>=0);
					}
				}

				if (bMoving == MOVING_SUB) {
					int diff_x = LOWORD(lastp) - old_x;
					if (diff_x > 10 || diff_x < -10) {
						if (vo_spudec) {
							((spudec_tmp_t *)vo_spudec)->scale_x *= (diff_x > 0)?1.05:0.95;
							if (GetKeyState(VK_CONTROL) >= 0) ((spudec_tmp_t *)vo_spudec)->scale_y *= (diff_x > 0)?1.05:0.95;
						}
						text_font_scale_factor += (diff_x > 0)?0.1:-0.1;
						force_load_font = 1;
						vo_osd_changed(OSDTYPE_SUBTITLE);
						old_x = LOWORD(lastp);
					}
					else
					{
						char tmps[20];
						int h = HIWORD(lastp) - disp_top;
						if (h < 0) h = 0; else
						if (h > disp_height) h = disp_height;
						if (!FullHide) sub_pos = h*100/disp_height;
						sprintf(tmps, "sub_pos %d", FullHide?(h-old_y)>>2:0);
						old_y = h;
						mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					}
				} else
				if (vo_fs||vidmode) {
					if (vo_fs ) {
						if(HIWORD(lParam) < menuHeight)
							PopMenuShow = TRUE;
						else if(HIWORD(lParam) >= menuHeight)
							PopMenuShow = FALSE;
					}
					if (ControlbarShow && HIWORD(lParam) < vo_screenheight-controlbar_height_fixed+controlbar_offset) {
						ControlbarShow = FALSE;
						ShowWindow(hWndControlbar, SW_HIDE);
					} else
					if (!ControlbarShow && HIWORD(lParam) >= vo_screenheight-controlbar_height_fixed+controlbar_offset) {
						if (bHide) {
							bHide = FALSE;
							ShowCursor(TRUE);
						} else
							KillTimer(hWndFS, TIMER_HIDE_CURSOR);
						ControlbarShow = TRUE;
						ShowWindow(hWndControlbar, SW_SHOW);
					}
				} else if (bMoving == MOVING_WINDOW) {
					GetWindowRect(hWnd, &r);
					int new_x = r.left+LOWORD(lastp)-old_x;
					int new_y = r.top+HIWORD(lastp)-old_y;
					if(new_x < vo_screenwidth && new_y < vo_screenheight )
						SetWindowPos(hWnd, NULL,	new_x , new_y , 0,0,
							SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
				} else if(!vo_fs && bFullView && auto_hide_control && show_controlbar) {
					int inctrl;
					POINT pt;
					pt.x = LOWORD(lParam);
					pt.y = HIWORD(lParam);
					GetClientRect(hWnd, &r);
					inctrl = (pt.x > r.left && pt.x < r.right &&
						pt.y > r.bottom - controlbar_height_fixed && pt.y < r.bottom);
					if(!ControlbarShow && inctrl) {
						ControlbarShow = TRUE;
						ShowWindow(hWndControlbar,SW_SHOWNOACTIVATE);
						if(skin_controlbar) SetCapture(hWndControlbar);
					} else if (!skin_controlbar && ControlbarShow && !inctrl) {
						ControlbarShow = FALSE;
						ShowWindow(hWndControlbar,SW_HIDE);
					}
				}
			}
			return 0;
		}
	case WM_SIZING:
		{
			if (!vidmode && !vo_fs)
			{
				LPRECT rc = (LPRECT)lParam;
				if(bFullView && rc->right - rc->left < min_video_width)
					rc->right = rc->left + min_video_width;
				if(vo_keepaspect) {
					switch (wParam)
					{
						case WMSZ_LEFT:
						case WMSZ_RIGHT:
							rc->bottom = rc->top+(rc->right-rc->left-xborder)/window_aspect+yborder+0.5;
							break;
						case WMSZ_TOP:
						case WMSZ_BOTTOM:
						case WMSZ_TOPRIGHT:
						case WMSZ_BOTTOMRIGHT:
							rc->right = rc->left+(rc->bottom-rc->top-yborder)*window_aspect+xborder+0.5;
							if(rc->right - rc->left < min_video_width){
								rc->right = rc->left + min_video_width;
							rc->bottom = rc->top+(rc->right-rc->left-xborder)/window_aspect+yborder+0.5;
							}
							break;
						case WMSZ_TOPLEFT:
						case WMSZ_BOTTOMLEFT:
							rc->left = rc->right-(rc->bottom-rc->top-yborder)*window_aspect-xborder+0.5;
							break;
					}
					set_window_rgn(hWnd, &gui_skin, !vo_fs & bFullView, TRUE);
					return 0;
				} else if(rc->bottom - rc->top < yborder)
					rc->bottom = rc->top + yborder;
			}
			set_window_rgn(hWnd, &gui_skin, !vo_fs & bFullView, TRUE);
			break;
		}
		case WM_SIZE:
			set_window_rgn(hWnd, &gui_skin, !vo_fs & bFullView, FALSE);
			break;
		case WM_SHOWCURSOR:
			ShowCursor(TRUE);
			return 0;
		case WM_NOTIFYICON:
		{
			switch (lParam) {
				case WM_LBUTTONDOWN:
					if (WndState == SW_SHOWMINIMIZED) {
						if (paused && systray==2) mp_input_queue_cmd(mp_input_parse_cmd("pause"));
						PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
					}
					SetForegroundWindow(hWnd);
					break;
				case WM_LBUTTONDBLCLK:
					SendMessage(hWnd, WM_COMMAND, IDM_OPEN, 0);
					SetForegroundWindow(hWnd);
					break;
				case WM_RBUTTONDOWN: {
					POINT p;
					SetForegroundWindow(hWnd);
					GetCursorPos(&p);
					TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
					break;
				}
			}
			return 0;
		}
		case WM_SYSCHAR:
		{
			if (wParam == 13) {
				control(VOCTRL_FULLSCREEN, NULL);
			} else
			if (wParam>0x30 && wParam<0x34 && !vo_fs && !vidmode && filename) {
				static float scale = 1.0;

				if (wParam == 0x32) {
					scale = 1.0;
				}
				else if (wParam == 0x31 && scale > 0.25) {
					scale -= 0.25;
				}
				else if (wParam == 0x33 && scale < 3.0) {
					scale += 0.25;
				}
				ShowWindow(hWnd, SW_NORMAL);
				WndState = SW_NORMAL;
				SetWindowPos(hWnd, NULL, 0, 0,
					d_image_width*scale+xborder,
					d_image_height*scale+yborder, SWP_NOMOVE);
			} else
			if(systray && (wParam == 's' || wParam == 'S' || wParam == 'h' || wParam == 'H'))
			{
				if (WndState != SW_SHOWMINIMIZED)
					PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			} else
				break;
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			mplayer_put_key(KEY_CLOSE_WIN);
			return 0;
		}
		case WM_WINDOWPOSCHANGED:
		{
			//printf("Windowposchange\n");
			//if(g_lpddsBack != NULL)  //or it will crash with -vm
			{
				Directx_ManageDisplay();
			}
			break;
		}
		case WM_COPYDATA:
		{
			char tmps[DMAX_PATH+20];
			COPYDATASTRUCT*   pcd = (COPYDATASTRUCT*)lParam;
			char *str = pcd->lpData;
			int len = pcd->cbData -1;
			if(!str || len < 4)
				break;
			str[len] = 0;
			if(wParam == 8695) {
				snprintf(tmps, DMAX_PATH+20, "sub_load \"%s\"", (char *)str);
			} else
				snprintf(tmps, DMAX_PATH+20, "loadfile \"%s\"", (char *)str);
			mp_input_queue_cmd(mp_input_parse_cmd(tmps));
			break;
		}
		case WM_SYSCOMMAND:
		{
			LRESULT ret;
			WINDOWPLACEMENT wp;

			switch (wParam)
			{   //kill screensaver etc.
				//note: works only when the window is active
				//you can workaround this by disabling the allow screensaver option in
				//the link to the app
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					if(!disable_screensaver)
						break;
					mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>killing screensaver\n" );
					return 0;
				case SC_MINIMIZE:
					if (hWndP) {
						ShowWindow(hWnd, SW_MINIMIZE);
						ShowWindow(hWnd, SW_HIDE);
						if (!paused && systray==2) mp_input_queue_cmd(mp_input_parse_cmd("pause"));
					}
					break;
			}

			ret = DefWindowProc(hWnd, message, wParam, lParam);
			GetWindowPlacement(hWnd, &wp);
			WndState = wp.showCmd;

			return ret;
		}
		case WM_KEYDOWN:
		{
			if ( (vo_fs | vidmode) && ((wParam >= VK_NUMPAD1) && (wParam <= VK_DECIMAL) || (wParam > 0x30 && wParam <= 0x39))) {
				if (GetKeyState(VK_LCONTROL) < 0) {
					switch (wParam) {
						case 0x31:
						case VK_NUMPAD1 : delta_x -= 4; delta_y += 4; break;
						case 0x32:
						case VK_NUMPAD2 : delta_y += 4; break;
						case 0x33:
						case VK_NUMPAD3 : delta_x += 4; delta_y += 4; break;
						case 0x34:
						case VK_NUMPAD4 : delta_x -= 4; break;
						case 0x35:
						case VK_NUMPAD5 : delta_x = delta_y = 0; break;
						case 0x36:
						case VK_NUMPAD6 : delta_x += 4; break;
						case 0x37:
						case VK_NUMPAD7 : delta_x -= 4; delta_y -= 4; break;
						case 0x38:
						case VK_NUMPAD8 : delta_y -= 4; break;
						case 0x39:
						case VK_NUMPAD9 : delta_x += 4; delta_y -= 4; break;
					}
					Directx_ManageDisplay();
					break;
				}
				else if (GetKeyState(VK_RCONTROL) < 0) {
					vo_keepaspect = 0;
					switch (wParam) {
						case 0x31:
						case VK_NUMPAD1 : factor_x = (1 + factor_x) * 0.98 - 1; factor_y = (1 + factor_y) * 0.98 - 1; break;
						case 0x32:
						case VK_NUMPAD2 : factor_y -= 0.02; break;
						case 0x33:
						case VK_NUMPAD3 : factor_x = 0; factor_y = factors[factor_index = (factor_index+1) % 3]; break;
						case 0x34:
						case VK_NUMPAD4 : factor_x -= 0.02; break;
						case 0x35:
						case VK_NUMPAD5 : factor_x = factor_y = 0; vo_keepaspect = 1; break;
						case 0x36:
						case VK_NUMPAD6 : factor_x += 0.02; break;
						case 0x37:
						case VK_NUMPAD7 : factor_x = 0; factor_y = factors[factor_index = (factor_index+2) % 3]; break;
						case 0x38:
						case VK_NUMPAD8 : factor_y += 0.02; break;
						case 0x39:
						case VK_NUMPAD9 : factor_x = (1 + factor_x) * 1.02 - 1; factor_y = (1 + factor_y) * 1.02 - 1; break;
					}
					if(show_menubar)
						CheckMenuItem(hMenu, IDM_KEEP_ASPECT, vo_keepaspect?MF_CHECKED:MF_UNCHECKED);
					Directx_ManageDisplay();
					break;
				}
			}
#ifdef CONFIG_DVDNAV
			else if(GetKeyState(VK_CONTROL) < 0)
			{
				switch (wParam) {
					case VK_NUMPAD8:
					case VK_UP:
						{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav up")); break;}
					case VK_NUMPAD2:
					case VK_DOWN:
						{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav down")); break;}
					case VK_NUMPAD4:
					case VK_LEFT:
						{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav left")); break;}
					case VK_NUMPAD6:
					case VK_RIGHT:
						{mp_input_queue_cmd(mp_input_parse_cmd("dvdnav right")); break;}
					case VK_NUMPAD1:
					case VK_RETURN:
						{
							mp_input_queue_cmd(mp_input_parse_cmd("dvdnav select"));
							KillTimer(hWnd, TIMER_DVDNAV);
							SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
							dvdnav_checktime=0;
							break;
						}
					case VK_NUMPAD5:
					case VK_HOME:
						{
							mp_input_queue_cmd(mp_input_parse_cmd("dvdnav menu"));
							KillTimer(hWnd, TIMER_DVDNAV);
							SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
							dvdnav_checktime=0;
							break;
						}
					case VK_NUMPAD7:
					case VK_END:
						{
							mp_input_queue_cmd(mp_input_parse_cmd("dvdnav prev"));
							KillTimer(hWnd, TIMER_DVDNAV);
							SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
							dvdnav_checktime=0;
							break;
						}
				}
				break;
			}
#else
			else if(GetKeyState(VK_CONTROL) < 0)
				break;
#endif
			switch (wParam)
			{
				case VK_LEFT:
					{mplayer_put_key(KEY_LEFT);break;}
				case VK_UP:
					{mplayer_put_key(KEY_UP);break;}
				case VK_RIGHT:
					{mplayer_put_key(KEY_RIGHT);break;}
				case VK_DOWN:
					{mplayer_put_key(KEY_DOWN);break;}
				case VK_TAB:
					{mplayer_put_key(KEY_TAB);break;}
				case VK_BACK:
					{mplayer_put_key(KEY_BS);break;}
				case VK_DELETE:
					{mplayer_put_key(KEY_DELETE);break;}
				case VK_INSERT:
					{mplayer_put_key(KEY_INSERT);break;}
				case VK_HOME:
					{mplayer_put_key(KEY_HOME);break;}
				case VK_END:
					{mplayer_put_key(KEY_END);break;}
				case VK_PRIOR:
					{mplayer_put_key(KEY_PAGE_UP);break;}
				case VK_NEXT:
					{mplayer_put_key(KEY_PAGE_DOWN);break;}
				case VK_ESCAPE:
					{mplayer_put_key(KEY_ESC);break;}
				default:
					if (wParam >= VK_F1 && wParam <= VK_F12)
						mplayer_put_key((KEY_F+1-VK_F1)+wParam);
			}
			break;
		}
		case WM_CHAR:
		{
			mplayer_put_key(wParam);
			break;
		}
		case WM_MOVE:
		{
			start_lbutton_down = 0;
			start_lbutton_dbcheck = 0;
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if(!no_dvdnav && mouse_dvdnav && filename && is_dvdnav)
			{
				char tmps[32];
				snprintf(tmps, 32, "set_mouse_pos %d %d", LOWORD(lParam), HIWORD(lParam));
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				mp_input_queue_cmd(mp_input_parse_cmd("dvdnav mouse"));
				KillTimer(hWnd, TIMER_DVDNAV);
				SetTimer(hWnd, TIMER_DVDNAV, 800, NULL);
				dvdnav_checktime=0;
				break;
			}
			nothide_times = 0;
			start_lbutton_down = GetTickCount();
			if (!bMoving) {
				int h;
				old_x = LOWORD(lParam);
				old_y = HIWORD(lParam);
				h = old_y - disp_top;
				if (get_sub_size() && (FullHide||abs(h*100/disp_height-sub_pos)<10))
					bMoving = MOVING_SUB;
				else if (!vo_fs && !vidmode && WndState != SW_SHOWMAXIMIZED)
				{
					ReleaseCapture();
					SetCapture(hWnd);
					bMoving = MOVING_WINDOW;
					//PostMessage(hWnd, WM_SYSCOMMAND, 0xF012, 0);
				}
			}
			break;
			}
		case WM_LBUTTONUP:
		{
			if(seek_dropping || volume_dropping) {
				PostMessage(hWndControlbar, WM_LBUTTONUP, wParam, lParam);
				break;
			}
			bMoving = MOVING_NONE;
			ReleaseCapture();
			if(vo_nomouse_input || !video_inited)
				break;
			if (GetTickCount()-start_lbutton_down < 500)
				mplayer_put_key(MOUSE_BTN0);
			if(GetTickCount() - start_lbutton_dbcheck < 500)
			{
				mplayer_put_key(MOUSE_BTN0_DBL);
				break;
			}
			start_lbutton_dbcheck = GetTickCount();
			break;
		}
		case WM_MBUTTONDOWN:
		{
			if (!vo_nomouse_input)
				mplayer_put_key(MOUSE_BTN1);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			if(use_rightmenu)
			{
				show_cursor(6);
				POINT p;
				GetCursorPos(&p);
				TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
				break;
			}
			if(vo_fs && PopMenuShow)
			{
				POINT p;
				GetCursorPos(&p);
				if(p.y > menuHeight)
					PopMenuShow = FALSE;
				show_cursor(6);
				TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
				break;
			}
			bRBDown = TRUE;
			bWheel = FALSE;
			break;
		 }
		case WM_RBUTTONUP:
		{
			bRBDown = FALSE;
			if (!vo_nomouse_input && !bWheel && !use_rightmenu)
				mplayer_put_key(MOUSE_BTN2);
			break;
		}
		case WM_LBUTTONDBLCLK:
		{
			if (!filename)
				SendMessage(hWnd, WM_COMMAND, IDM_OPEN, 0);
			break;
		}
		case WM_MBUTTONDBLCLK:
		{
			if (!vo_nomouse_input)
				mplayer_put_key(MOUSE_BTN1_DBL);
			break;
		}
		case WM_RBUTTONDBLCLK:
		{
			if (!vo_nomouse_input)
				mplayer_put_key(MOUSE_BTN2_DBL);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int x;

			bWheel = TRUE;
			if (vo_nomouse_input)
				break;
			x = GET_WHEEL_DELTA_WPARAM(wParam);
			if (x > 0)
				mplayer_put_key(bRBDown?MOUSE_BTN8:MOUSE_BTN3);
			else
				mplayer_put_key(bRBDown?MOUSE_BTN9:MOUSE_BTN4);
			break;
		}
		case WM_XBUTTONDOWN:
		{
			if (vo_nomouse_input)
				break;
			if (HIWORD(wParam) == 1)
				mplayer_put_key(MOUSE_BTN5);
			else
				mplayer_put_key(MOUSE_BTN6);
			break;
		}
		case WM_XBUTTONDBLCLK:
		{
			if (vo_nomouse_input)
				break;
			if (HIWORD(wParam) == 1)
				mplayer_put_key(MOUSE_BTN5_DBL);
			else
				mplayer_put_key(MOUSE_BTN6_DBL);
			break;
		}

	 }
	return DefWindowProc(hWnd, message, wParam, lParam);
}

static void InitSubtitleMenu()
{
	char download[16];

	hSubtitleMenu = CreatePopupMenu();
	if(show_menubar)
		InsertMenu(hMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSubtitleMenu, subMenu);
	InsertMenu(hPopupMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSubtitleMenu, subMenu);

	if(sys_Language == 1) {
		LoadString(hInstance, IDS_SUB_DOWNLOAD_SC, download, 16);
	} else if(sys_Language == 3 || sys_Language == 4){ 
		LoadString(hInstance, IDS_SUB_DOWNLOAD_TC, download, 16);
	} else {
		sprintf(download, "Do&wnload\tCtrl+W");
	}

	mi.wID = IDM_SUB_DOWNLOAD;
	mi.dwTypeData = download;
	mi.cch = strlen(download);
	InsertMenuItem(hSubtitleMenu, 0xFFFFFFFF, TRUE, &mi);

	subdownloader = get_path("tools/subdownloader.exe");
	if (GetFileAttributes(subdownloader) == 0xFFFFFFFF) {
		EnableMenuItem(hSubtitleMenu, IDM_SUB_DOWNLOAD, MF_GRAYED);
		free(subdownloader);
		subdownloader = NULL;
	}

	mi.wID = IDM_SUBTITLE;
	mi.dwTypeData = subMenuItem;
	mi.cch = strlen(subMenuItem);
	InsertMenuItem(hSubtitleMenu, 0xFFFFFFFF, TRUE, &mi);
	AppendMenu(hSubtitleMenu, MF_SEPARATOR, 0, NULL);
	CheckMenuRadioItem(hSubtitleMenu, IDM_SUBTITLE, mi.wID, IDM_SUBTITLE, MF_BYCOMMAND);
}

static void InitFavoriteMenu()
{
	if(auto_resume <= 0)
		return;
	hFavoriteMenu = CreatePopupMenu();
	if(show_menubar)
		InsertMenu(hMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hFavoriteMenu, favMenu);
	InsertMenu(hPopupMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hFavoriteMenu, favMenu);

	memset(&minf2, 0, sizeof(minf2));
	minf2.cbSize = sizeof(minf2);
	minf2.fMask = MIIM_ID | MIIM_TYPE;
	minf2.fType = MFT_STRING;
	minf2.wID = IDM_FAVORITES_MANAGE;
	minf2.dwTypeData = favMenuManage;
	minf2.cch = strlen(favMenuManage);
	InsertMenuItem(hFavoriteMenu, 0xFFFFFFFF, TRUE, &minf2);
	minf2.wID = IDM_FAVORITES;
	if(auto_resume > 1)
	{
		minf2.dwTypeData = favMenuItem;
		minf2.cch = strlen(favMenuItem);
		InsertMenuItem(hFavoriteMenu, IDM_FAVORITES_MANAGE, FALSE, &minf2);
		AppendMenu(hFavoriteMenu, MF_SEPARATOR, 0, NULL);
	}
	if(initStatus())
	{
		int num = getStatusNum();
		int i;
		for(i = 0; i < num; i++)
		{
			char s[DMAX_PATH],t[16];
			char *str = getNameById(i);
			if(str) {
				int time = getStatusById(i);
				if(time >= 3600)
					snprintf(t, 16, " [%02d:%02d:%02d]", time/3600, (time/60)%60, time%60);
				else if(time < 3600 && time > 0)
					snprintf(t, 16, " [%02d:%02d]", time/60, time%60);
				else
					snprintf(t, 16, " [00:00]");
				snprintf(s, DMAX_PATH, "%s%s", _mbsrchr(str,'\\') + 1, t);
				free(str);
				AddFavorite(s);
			} else {
				snprintf(s, DMAX_PATH, "Unknown file");
				AddFavorite(s);
			}
		}
		freeStatus();
	}
	DrawMenuBar(hWnd);
}

static void InitSkinMenu(){
    char *skindir = NULL;
	char path[MAX_PATH + 1], skins[16], appskin[32], ctlskin[32];
	DWORD t;
	int i, CheckID;
	WIN32_FIND_DATA ff;
	HANDLE f;

	if(!show_controlbar)
		return;

	if(sys_Language == 1) {
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINS_SC, skins, 16);
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINSB_SC, appskin, 16);
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINSC_SC, ctlskin, 16);
	} else if(sys_Language == 3 || sys_Language == 4) {
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINS_TC, skins, 16);
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINSB_TC, appskin, 16);
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINSC_TC, ctlskin, 16);
	} else {
		sprintf(skins, "Skin");
		sprintf(appskin, "Skined border");
		sprintf(ctlskin, "Skined controlbar");
	}

	hSkinMenu = CreatePopupMenu();
	if(show_menubar)
		InsertMenu(hMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSkinMenu, skins);
	InsertMenu(hPopupMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSkinMenu, skins);

	memset(&minf3, 0, sizeof(minf3));
	minf3.cbSize = sizeof(minf3);
	minf3.fMask = MIIM_ID | MIIM_TYPE;
	minf3.fType = MFT_STRING;

	minf3.wID = IDM_SKINS+101;
	minf3.dwTypeData = appskin;
	minf3.cch = strlen(appskin);
	InsertMenuItem(hSkinMenu, 0xFFFFFFFF, TRUE, &minf3);

	minf3.wID = IDM_SKINS+102;
	minf3.dwTypeData = ctlskin;
	minf3.cch = strlen(ctlskin);
	InsertMenuItem(hSkinMenu, 0xFFFFFFFF, TRUE, &minf3);

	AppendMenu(hSkinMenu, MF_SEPARATOR, 0, NULL);

	skindir = get_path("skin");
	if(GetFileAttributes(skindir) != 0xFFFFFFFF) {
		GetCurrentDirectory(MAX_PATH, path);
		SetCurrentDirectory(skindir);
		i=0;
		minf3.wID = CheckID = IDM_SKINS;
		f = FindFirstFile("*.*", &ff);
		do {
			if(i >= 100) break;
			t = ff.dwFileAttributes;
			if((t & 0x10) && strcmp(ff.cFileName, ".") && strcmp(ff.cFileName, "..")) {
				skins_name[i] = strdup(ff.cFileName);
				minf3.dwTypeData = skins_name[i];
				minf3.cch = strlen(skins_name[i]);
				InsertMenuItem(hSkinMenu, 0xFFFFFFFF, TRUE, &minf3);
				if(!stricmp(skins_name[i], skin_name))
					CheckID = minf3.wID;
				minf3.wID++;
				++i;
			}
		} while ( FindNextFile(f, &ff) );
		FindClose(f);
		SetCurrentDirectory(path);
	}

	CheckMenuItem(hSkinMenu, IDM_SKINS+101, skinned_player?MF_CHECKED:MF_UNCHECKED);
	EnableMenuItem(hSkinMenu, IDM_SKINS+101, gui_skin.background.state?MF_ENABLED:MF_GRAYED);
	CheckMenuItem(hSkinMenu, IDM_SKINS+102, skin_controlbar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuRadioItem(hSkinMenu, IDM_SKINS, minf3.wID,	CheckID, MF_BYCOMMAND);

	free(skindir);
	DrawMenuBar(hWnd);
}

static int InitWindow(void)
{
	HINSTANCE user32;
	hInstance = GetModuleHandle(NULL);
	WNDCLASS wc;
	RECT rd;
	BITMAP bm;
	memset(&rd, 0, sizeof(rd));
	AdjustWindowRect(&rd,WNDSTYLE,TRUE);
	xborder = rd.right - rd.left;
	yborder = rd.bottom - rd.top;
	show_menubar_w32 = show_menubar;

	if (systray) hWndP = CreateWindow("Static",NULL,WS_POPUP,0,0,0,0,NULL,NULL,hInstance,NULL);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	hLogo = LoadImage(hInstance, MAKEINTRESOURCE(IDI_MPLAYER_LOGO), IMAGE_BITMAP, 0, 0, 0);
	GetObject(hLogo, sizeof(BITMAP), &bm);
	logoW = bm.bmWidth;
	logoH = bm.bmHeight;

	/*load icon from the main app*/
	mplayericon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	if(!mplayericon) mplayericon = LoadIcon(NULL, IDI_APPLICATION);
	mplayercursor = LoadCursor(NULL, MAKEINTRESOURCE(0x7F89));
	vo_screenwidth = monitor_rect.right = GetSystemMetrics(SM_CXSCREEN);
	vo_screenheight = monitor_rect.bottom = GetSystemMetrics(SM_CYSCREEN);

	windowcolor = vo_colorkey;
	bkgndbrush = CreateSolidBrush(gui_logo.bg_color);
	colorbrush = CreateSolidBrush(windowcolor);
	wc.style		 =  CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   =  WndProc;
	wc.cbClsExtra	 =  0;
	wc.cbWndExtra	 =  0;
	wc.hInstance	 =  hInstance;
	wc.hCursor	     =  mplayercursor;
	wc.hIcon		 =  mplayericon;
	wc.hbrBackground =  colorbrush;
	wc.lpszClassName =  WNDCLASSNAME_WINDOWED;
	char file_str[32] , option_str[32] , play_str[32] , help_str[32];
	if(sys_Language == 1) {
		wc.lpszMenuName  =  vidmode?NULL:MAKEINTRESOURCE(IDR_MAINMENU);
		LoadString(hInstance, IDS_MENU_SC, subMenu, 10);
		LoadString(hInstance, IDS_MENUITEM_SC, subMenuItem, 10);
		LoadString(hInstance, IDS_FAVMENU_SC, favMenu, 20);
		LoadString(hInstance, IDS_FAVMENUITEM_SC, favMenuItem, 30);
		LoadString(hInstance, IDS_FAVMENUMANAGE_SC, favMenuManage, 30);
		LoadString(hInstance, IDS_FILE_SC, file_str, 32);
		LoadString(hInstance, IDS_OPTION_SC, option_str, 32);
		LoadString(hInstance, IDS_PLAY_SC, play_str, 32);
		LoadString(hInstance, IDS_HELP_SC, help_str, 32);
	} else if(sys_Language == 3 || sys_Language == 4) {
		wc.lpszMenuName  =  vidmode?NULL:MAKEINTRESOURCE(IDR_MAINMENU_TC);
		LoadString(hInstance, IDS_MENU_TC, subMenu, 10);
		LoadString(hInstance, IDS_MENUITEM_TC, subMenuItem, 10);
		LoadString(hInstance, IDS_FAVMENU_TC, favMenu, 20);
		LoadString(hInstance, IDS_FAVMENUITEM_TC, favMenuItem, 30);
		LoadString(hInstance, IDS_FAVMENUMANAGE_TC, favMenuManage, 30);
		LoadString(hInstance, IDS_FILE_TC, file_str, 32);
		LoadString(hInstance, IDS_OPTION_TC, option_str, 32);
		LoadString(hInstance, IDS_PLAY_TC, play_str, 32);
		LoadString(hInstance, IDS_HELP_TC, help_str, 32);
	} else {
		wc.lpszMenuName  =  vidmode?NULL:MAKEINTRESOURCE(IDR_MAINMENU_EN);
		LoadString(hInstance, IDS_MENU_EN, subMenu, 10);
		LoadString(hInstance, IDS_MENUITEM_EN, subMenuItem, 10);
		LoadString(hInstance, IDS_FAVMENU_EN, favMenu, 20);
		LoadString(hInstance, IDS_FAVMENUITEM_EN, favMenuItem, 30);
		LoadString(hInstance, IDS_FAVMENUMANAGE_EN, favMenuManage, 30);
		strcpy(file_str,"&File");
		strcpy(option_str,"&Option");
		strcpy(play_str,"&Play");
		strcpy(help_str,"&Help");
	}
	RegisterClass(&wc);
	if(min_video_width < gui_skin.background_width)
		min_video_width = gui_skin.background_width;
	if (WinID != -1) hWnd = (HWND)WinID;
	else{
			hWnd = CreateWindowEx(vidmode?WS_EX_TOPMOST:0,
				WNDCLASSNAME_WINDOWED,"MPlayer",
				/*WS_VISIBLE|*/((vidmode)?WS_POPUP:WNDSTYLE),
				(monitor_rect.right - min_video_width)>>1, (monitor_rect.bottom - min_video_width*3/4)>>1,
				min_video_width, min_video_width/window_aspect,hWndP,NULL,hInstance,NULL);

	}
	DragAcceptFiles(hWnd, TRUE);
	hMenu = GetMenu(hWnd);
	menuHeight = GetSystemMetrics(SM_CYMENU);
	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID | MIIM_TYPE;
	mi.fType = MFT_STRING | MFT_RADIOCHECK;

	hPopupMenu = CreatePopupMenu();
	AppendMenu(hPopupMenu, MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hMenu, 0), file_str);
	AppendMenu(hPopupMenu, MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hMenu, 1), option_str);
	AppendMenu(hPopupMenu, MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hMenu, 2),play_str);
	AppendMenu(hPopupMenu, MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hMenu, 3), help_str);

	strcpy(title_text, "MPlayer");

	loadlogo(&gui_logo, hWnd);

	if(loadappskin(&gui_skin, hWnd)) {
		show_menubar_w32 = 0;
		hMHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC) MouseHookProc, hInstance, GetCurrentThreadId());
		HINSTANCE h;
		if(h = LoadLibrary("dwmapi.dll")) {
			DWORD dwAttr = 1;
			DwmAttributeFunct DwmSetWindowAttribute = (DwmAttributeFunct)GetProcAddress(h, "DwmSetWindowAttribute");
			if(DwmSetWindowAttribute) DwmSetWindowAttribute(hWnd, 2, &dwAttr, 4);
			FreeLibrary(h);
		}
		if(h = LoadLibrary("uxtheme.dll")) {
		   SetWindowThemeFunct SetWindowTheme = (SetWindowThemeFunct)GetProcAddress(h, "SetWindowTheme");
		   if(SetWindowTheme) SetWindowTheme(hWnd, L" ",L" ");
		   FreeLibrary(h);
		}
	}

	if (hWndP) {
	WM_TASKBARCREATED = RegisterWindowMessage("TaskbarCreated");
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 1973;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nid.hIcon = mplayericon;
	nid.uCallbackMessage = WM_NOTIFYICON;
	strcpy(nid.szTip, "MPlayer");
	Shell_NotifyIcon(NIM_ADD, &nid);
	}

	auto_hide_control_save = auto_hide_control;

	if(show_controlbar) {
		status[0] = 0;
		volpercent = save_volume;
		controlbar_offset = skin_controlbar ? 0 : 4;
		if(skin_controlbar) {
			hWndControlbar = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONTROL_SKIN), hWnd, ControlbarProcSkin);
			loadskin(&gui_skin, hWndControlbar);
			controlbar_height_fixed = gui_skin.background_height;
			if(gui_skin.auto_hide) auto_hide_control = 1;
			if(!auto_hide_control) controlbar_height = controlbar_height_fixed;
		} else {
			controlbar_height_fixed = volumebar_height;
			if(!auto_hide_control) controlbar_height = controlbar_height_fixed;
			hWndControlbar = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONTROL_SKIN), hWnd, ControlbarProcNormal);
			hWndSeekbar = GetDlgItem(hWndControlbar, IDC_CTL_SEEK);
			hWndVolumebar = GetDlgItem(hWndControlbar, IDC_CTL_VOLUME);
			PostMessage(hWndSeekbar, TBM_SETRANGEMAX, FALSE, 100);
			PostMessage(hWndVolumebar, TBM_SETRANGEMAX, FALSE, 100);
			PostMessage(hWndVolumebar, TBM_SETPOS, TRUE, volpercent);
			ShowWindow(hWndSeekbar, SW_SHOW);
			ShowWindow(hWndVolumebar, SW_SHOW);
		}
	}

	if(gui_skin.enable_appskin && gui_skin.border_ok) {
		xborder = gui_skin.border_left.img->width + gui_skin.border_right.img->width;
		yborder = gui_skin.title.img->height + gui_skin.border_bottom.img->height;
	}
	yborder += controlbar_height;
	xborder0 = xborder;
	yborder0 = yborder;
	memset(&rd, 0, sizeof(rd));
	AdjustWindowRect(&rd,WS_POPUP|WS_THICKFRAME,FALSE);
	xborder1 = (rd.right - rd.left) >> 1;
	yborder1 = (rd.bottom - rd.top) >> 1;
	if(!show_menubar_w32) {
		SetMenu(hWnd, NULL);
		if(!gui_skin.enable_appskin || !gui_skin.border_ok) {
			yborder -= menuHeight;
			yborder0 -= menuHeight;
		}
	}
	window_aspect = (float)(min_video_width-xborder) / (float)(min_video_width/window_aspect-yborder);

	wc.lpszMenuName  = NULL;
	wc.hbrBackground = colorbrush;
	wc.lpszClassName = WNDCLASSNAME_FULLSCREEN;
	RegisterClass(&wc);
	if(!vidmode)hWndFS = CreateWindow(WNDCLASSNAME_FULLSCREEN,"MPlayer Fullscreen",WS_POPUP,monitor_rect.left,monitor_rect.top,monitor_rect.right-monitor_rect.left,monitor_rect.bottom-monitor_rect.top,hWnd,NULL,hInstance,NULL);

	InitSkinMenu();
    InitFavoriteMenu();
	InitSubtitleMenu();
	Directx_ManageDisplay();

	user32 = GetModuleHandle("user32.dll");
	if(is_win7 && user32) ChangeWindowMessageFilter = GetProcAddress(user32, "ChangeWindowMessageFilter");
	if(is_win7 && user32 && ChangeWindowMessageFilter) {
		s_uTBBC = RegisterWindowMessage("TaskbarButtonCreated");
		ChangeWindowMessageFilter(s_uTBBC, MSGFLT_ADD);
		ChangeWindowMessageFilter(WM_COMMAND, MSGFLT_ADD);
	}

	if(vo_fs) {
		ShowWindow(hWnd , SW_HIDE);
	} else {
		ShowWindow(hWndControlbar , SW_SHOW);
		if(gui_skin.enable_appskin && gui_skin.border_ok && bFullView && !using_theme)
			SetWindowPos(hWnd,NULL,0,0,0,0,SWP_FRAMECHANGED|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE);
	}

	return 0;
}

static int preinit(const char *arg)
{
	int i = 0;

	if(arg)
	{
		if(strstr(arg,"noaccel"))
		{
			mp_msg(MSGT_VO,MSGL_V,"<vo_directx><INFO>disabled overlay\n");
			nooverlay = 1;
		}
	}

	while(!hWnd || !hWndFS) {
		if(i > 1000) break;
		Sleep(30);
		i++;
	}

	if (Directx_InitDirectDraw()!= 0)return 1;		  //init DirectDraw

	mp_msg(MSGT_VO, MSGL_DBG3 ,"<vo_directx><INFO>initial mplayer windows created\n");

	if (Directx_CheckPrimaryPixelformat()!=0)return 1;
	if (!nooverlay && Directx_CheckOverlayPixelformats() == 0)		//check for supported hardware
	{
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>hardware supports overlay\n");
		nooverlay = 0;
	}
	else   //if we can't have overlay we create a backpuffer with the same imageformat as the primary surface
	{
		mp_msg(MSGT_VO, MSGL_V ,"<vo_directx><INFO>using backpuffer\n");
		nooverlay = 1;
	}
	mp_msg(MSGT_VO, MSGL_DBG3 ,"<vo_directx><INFO>preinit succesfully finished\n");
	return 0;
}

static int draw_slice(uint8_t *src[], int stride[], int w,int h,int x,int y )
{
	uint8_t *s;
	uint8_t *d;
	uint32_t uvstride=dstride/2;
	// copy Y
	d=image+dstride*y+x;
	s=src[0];
	mem2agpcpy_pic(d,s,w,h,dstride,stride[0]);

	w/=2;h/=2;x/=2;y/=2;

	// copy U
	d=image+dstride*image_height + uvstride*y+x;
	if(image_format == IMGFMT_YV12)s=src[2];
	else s=src[1];
	mem2agpcpy_pic(d,s,w,h,uvstride,stride[1]);

	// copy V
    d=image+dstride*image_height +uvstride*(image_height/2) + uvstride*y+x;
    if(image_format == IMGFMT_YV12)s=src[1];
	else s=src[2];
    mem2agpcpy_pic(d,s,w,h,uvstride,stride[2]);
    return 0;
}

static void flip_page(void)
{
   	HRESULT dxresult;
	g_lpddsBack->lpVtbl->Unlock (g_lpddsBack,NULL);
	if (vo_doublebuffering)
    {
		// flip to the next image in the sequence
		dxresult = g_lpddsOverlay->lpVtbl->Flip( g_lpddsOverlay,NULL, DDFLIP_WAIT);
		if(dxresult == DDERR_SURFACELOST)
		{
			mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR><vo_directx><INFO>Restoring Surface\n");
			g_lpddsBack->lpVtbl->Restore( g_lpddsBack );
			// restore overlay and primary before calling
			// Directx_ManageDisplay() to avoid error messages
			g_lpddsOverlay->lpVtbl->Restore( g_lpddsOverlay );
			g_lpddsPrimary->lpVtbl->Restore( g_lpddsPrimary );
			// update overlay in case we return from screensaver
			Directx_ManageDisplay();
		    dxresult = g_lpddsOverlay->lpVtbl->Flip( g_lpddsOverlay,NULL, DDFLIP_WAIT);
		}
		if(dxresult != DD_OK)mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>can't flip page\n");
    }
	if(nooverlay)
	{
		DDBLTFX  ddbltfx;
        // ask for the "NOTEARING" option
	    memset( &ddbltfx, 0, sizeof(DDBLTFX) );
        ddbltfx.dwSize = sizeof(DDBLTFX);
        ddbltfx.dwDDFX = DDBLTFX_NOTEARING;
        g_lpddsPrimary->lpVtbl->Blt(g_lpddsPrimary, &rd, g_lpddsBack, NULL, DDBLT_WAIT, &ddbltfx);
	}
	if (g_lpddsBack->lpVtbl->Lock(g_lpddsBack,NULL,&ddsdsf, DDLOCK_NOSYSLOCK | DDLOCK_WAIT , NULL) == DD_OK) {
	    if(vo_directrendering && (dstride != ddsdsf.lPitch)){
	        mp_msg(MSGT_VO,MSGL_WARN,"<vo_directx><WARN>stride changed !!!! disabling direct rendering\n");
	        vo_directrendering=0;
	    }
	    free(tmp_image);
	    tmp_image = NULL;
	    dstride = ddsdsf.lPitch;
	    image = ddsdsf.lpSurface;
	} else if (!tmp_image) {
		mp_msg(MSGT_VO, MSGL_WARN, "<vo_directx><WARN>Locking the surface failed, rendering to a hidden surface!\n");
		tmp_image = image = calloc(1, image_height * dstride * 2);
	}
}

static int draw_frame(uint8_t *src[])
{
  	fast_memcpy( image, *src, dstride * image_height );
	return 0;
}

static uint32_t get_image(mp_image_t *mpi)
{
    if(mpi->flags&MP_IMGFLAG_READABLE) {mp_msg(MSGT_VO, MSGL_V,"<vo_directx><ERROR>slow video ram\n");return VO_FALSE;}
    if(mpi->type==MP_IMGTYPE_STATIC) {mp_msg(MSGT_VO, MSGL_V,"<vo_directx><ERROR>not static\n");return VO_FALSE;}
    if((mpi->width==dstride) || (mpi->flags&(MP_IMGFLAG_ACCEPT_STRIDE|MP_IMGFLAG_ACCEPT_WIDTH)))
	{
		if(mpi->flags&MP_IMGFLAG_PLANAR)
		{
		    if(image_format == IMGFMT_YV12)
			{
				mpi->planes[2]= image + dstride*image_height;
	            mpi->planes[1]= image + dstride*image_height+ dstride*image_height/4;
                mpi->stride[1]=mpi->stride[2]=dstride/2;
			}
			else if(image_format == IMGFMT_IYUV || image_format == IMGFMT_I420)
			{
				mpi->planes[1]= image + dstride*image_height;
	            mpi->planes[2]= image + dstride*image_height+ dstride*image_height/4;
			    mpi->stride[1]=mpi->stride[2]=dstride/2;
			}
			else if(image_format == IMGFMT_YVU9)
			{
				mpi->planes[2] = image + dstride*image_height;
				mpi->planes[1] = image + dstride*image_height+ dstride*image_height/16;
			    mpi->stride[1]=mpi->stride[2]=dstride/4;
			}
		}
		mpi->planes[0]=image;
        mpi->stride[0]=dstride;
   		mpi->width=image_width;
		mpi->height=image_height;
        mpi->flags|=MP_IMGFLAG_DIRECT;
        mp_msg(MSGT_VO, MSGL_DBG3, "<vo_directx><INFO>Direct Rendering ENABLED\n");
        return VO_TRUE;
    }
    return VO_FALSE;
}

static uint32_t put_image(mp_image_t *mpi){

    uint8_t   *d;
	uint8_t   *s;
    uint32_t x = mpi->x;
	uint32_t y = mpi->y;
	uint32_t w = mpi->w;
	uint32_t h = mpi->h;

    if (WinID != -1) Directx_ManageDisplay();

    if((mpi->flags&MP_IMGFLAG_DIRECT)||(mpi->flags&MP_IMGFLAG_DRAW_CALLBACK))
	{
		mp_msg(MSGT_VO, MSGL_DBG3 ,"<vo_directx><INFO>put_image: nothing to do: drawslices\n");
		return VO_TRUE;
    }

    if (mpi->flags&MP_IMGFLAG_PLANAR)
	{

		if(image_format!=IMGFMT_YVU9)draw_slice(mpi->planes,mpi->stride,mpi->w,mpi->h,0,0);
		else
		{
			// copy Y
            d=image+dstride*y+x;
            s=mpi->planes[0];
            mem2agpcpy_pic(d,s,w,h,dstride,mpi->stride[0]);
            w/=4;h/=4;x/=4;y/=4;
    	    // copy V
            d=image+dstride*image_height + dstride*y/4+x;
	        s=mpi->planes[2];
            mem2agpcpy_pic(d,s,w,h,dstride/4,mpi->stride[1]);
  	        // copy U
            d=image+dstride*image_height + dstride*image_height/16 + dstride/4*y+x;
		    s=mpi->planes[1];
            mem2agpcpy_pic(d,s,w,h,dstride/4,mpi->stride[2]);
		}
	}
	else //packed
	{
		mem2agpcpy_pic(image, mpi->planes[0], w * (mpi->bpp / 8), h, dstride, mpi->stride[0]);
	}
	return VO_TRUE;
}

static int
config(uint32_t width, uint32_t height, uint32_t d_width, uint32_t d_height, uint32_t options, char *title, uint32_t format)
{
	static int start_mplayer = 1;
	int prev_width, prev_height;
	RECT rt;

	if ( -1 == sub_pos ) {
		if (expand > 0)
			sub_pos = 75 + expand * 25;
		else if (expand)
			sub_pos = 50 - expand * 50;
		else
			sub_pos = 90;
	}

	CheckMenuItem(hMenu, IDM_KEEP_ASPECT, vo_keepaspect?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_FIXED_SIZE, fixedsize?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_SCREENSAVER, disable_screensaver?MF_CHECKED:MF_UNCHECKED);
	CheckMenuRadioItem(hMenu, IDM_ASPECT_FILE, IDM_ASPECT_169, IDM_ASPECT_FILE, MF_BYCOMMAND);

	if (start_mplayer) vo_fs = options & 0x01;
	vo_fs = save_vo_fs>-1? save_vo_fs: vo_fs;

	image_format =  format;
	image_width = width;
	image_height = height;
	prev_width = d_image_width = d_width;
	prev_height = d_image_height = d_height;
	if(format != primary_image_format)nooverlay = 0;
	window_aspect= (float)d_image_width / (float)d_image_height;

	SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
	rt.bottom -= rt.top;
	rt.right -= rt.left;

	if(prev_width+xborder >  rt.right) {
		prev_width = rt.right-xborder;
		prev_height = (int)((float)prev_width / window_aspect);
	}
	if(prev_height+yborder > rt.bottom) {
		prev_height = rt.bottom-yborder;
		prev_width = (int)((float)prev_height * window_aspect);
	}
	if(prev_width < min_video_width) {
		prev_width = min_video_width - xborder;
		prev_height = (int)((float)prev_width / window_aspect);
	}

#ifdef CONFIG_GUI
    if(use_gui){
        guiGetEvent(guiSetShVideo, 0);
    }
#endif
    /*release all directx objects*/
    if (g_cc != NULL)g_cc->lpVtbl->Release(g_cc);
    g_cc=NULL;
    if(g_lpddclipper)g_lpddclipper->lpVtbl->Release(g_lpddclipper);
        g_lpddclipper=NULL;
    if (g_lpddsBack != NULL) g_lpddsBack->lpVtbl->Release(g_lpddsBack);
    g_lpddsBack = NULL;
    if(vo_doublebuffering)
        if (g_lpddsOverlay != NULL)g_lpddsOverlay->lpVtbl->Release(g_lpddsOverlay);
    g_lpddsOverlay = NULL;
    if (g_lpddsPrimary != NULL) g_lpddsPrimary->lpVtbl->Release(g_lpddsPrimary);
    g_lpddsPrimary = NULL;
    mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>overlay surfaces released\n");

	if(!vidmode){
		if(!vo_geometry){
			GetWindowRect(hWnd,&rd);
			vo_dx=( rd.right + rd.left - xborder - (int)prev_width ) / 2;
			vo_dy=( rd.bottom + rd.top - yborder - (int)prev_height ) / 2;
		}

		vo_dx += monitor_rect.left; /* move position to global window space */
		vo_dy += monitor_rect.top;

		if (WndState != SW_SHOWMAXIMIZED) {
				SetWindowPos(hWnd,vo_ontop?HWND_TOPMOST:(vo_rootwin?HWND_BOTTOM:HWND_NOTOPMOST),
					vo_dx,vo_dy,prev_width+xborder,prev_height+yborder,
					((start_mplayer||!fixedsize)?0:(SWP_NOMOVE|SWP_NOSIZE))|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);

		}
		if (vo_fs) {
			ShowWindow(hWndFS,SW_SHOW);
			SetForegroundWindow(hWndFS);
			SetParent(hWndControlbar, hWndFS);
			ShowWindow(hWndControlbar , SW_HIDE);
			nothide_times = 0;
		}
		else {
			ShowWindow(hWnd, SW_SHOW);
			SetForegroundWindow(hWnd);
		}
		if(g_pTaskbarList && !gui_thread) {
			need_recreate = 1;
			g_pTaskbarList = NULL;
		}
	}
	else {
		ShowWindow(hWnd,SW_SHOW);
		vo_fs = 0;
	}
	start_mplayer = 0;
	SetFocus(vo_fs?hWndFS:hWnd);
	if (!vidmode && switch_view == 2)
		SetTimer(hWnd, TIMER_SWITCH_VIEW, 700, NULL);
	else
		KillTimer(hWnd, TIMER_SWITCH_VIEW);

	/*create the surfaces*/
    if(Directx_CreatePrimarySurface())return 1;

	//create palette for 256 color mode
	if(image_format==IMGFMT_BGR8){
		LPDIRECTDRAWPALETTE ddpalette=NULL;
		char* palette=malloc(4*256);
		int i;
		for(i=0; i<256; i++){
			palette[4*i+0] = ((i >> 5) & 0x07) * 255 / 7;
			palette[4*i+1] = ((i >> 2) & 0x07) * 255 / 7;
			palette[4*i+2] = ((i >> 0) & 0x03) * 255 / 3;
			palette[4*i+3] = PC_NOCOLLAPSE;
		}
		g_lpdd->lpVtbl->CreatePalette(g_lpdd,DDPCAPS_8BIT|DDPCAPS_INITIALIZE,palette,&ddpalette,NULL);
		g_lpddsPrimary->lpVtbl->SetPalette(g_lpddsPrimary,ddpalette);
		free(palette);
		ddpalette->lpVtbl->Release(ddpalette);
	}

	if (!nooverlay && Directx_CreateOverlay(image_format))
	{
			if(format == primary_image_format)nooverlay=1; /*overlay creation failed*/
			else {
              mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't use overlay mode: please use -vo directx:noaccel\n");
			  return 1;
            }
	}
	if(nooverlay)
	{
		if(Directx_CreateBackpuffer())
		{
			mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't get the driver to work on your system :(\n");
			return 1;
		}
		mp_msg(MSGT_VO, MSGL_V,"<vo_directx><INFO>back surface created\n");
		vo_doublebuffering = 0;
		/*create clipper for nonoverlay mode*/
	    if(g_lpdd->lpVtbl->CreateClipper(g_lpdd, 0, &g_lpddclipper,NULL)!= DD_OK){mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't create clipper\n");return 1;}
        if(g_lpddclipper->lpVtbl->SetHWnd (g_lpddclipper, 0, hWnd)!= DD_OK){mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't associate clipper with window\n");return 1;}
        if(g_lpddsPrimary->lpVtbl->SetClipper (g_lpddsPrimary,g_lpddclipper)!=DD_OK){mp_msg(MSGT_VO, MSGL_FATAL,"<vo_directx><FATAL ERROR>can't associate primary surface with clipper\n");return 1;}
	    mp_msg(MSGT_VO, MSGL_DBG3,"<vo_directx><INFO>clipper succesfully created\n");
	}else{
		if(DD_OK != g_lpddsOverlay->lpVtbl->QueryInterface(g_lpddsOverlay,&IID_IDirectDrawColorControl,(void**)&g_cc))
			mp_msg(MSGT_VO, MSGL_V,"<vo_directx><WARN>unable to get DirectDraw ColorControl interface\n");
	}
	if(vo_fs && hWnd)
	{
		ShowWindow(hWndControlbar , SW_HIDE);
		ShowWindow(hWnd , SW_HIDE);
	}
	Directx_ManageDisplay();
	memset(&ddsdsf, 0,sizeof(DDSURFACEDESC2));
	ddsdsf.dwSize = sizeof (DDSURFACEDESC2);
	if (g_lpddsBack->lpVtbl->Lock(g_lpddsBack,NULL,&ddsdsf, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL) == DD_OK) {
        dstride = ddsdsf.lPitch;
        image = ddsdsf.lpSurface;
        video_inited = 1;
        return 0;
	}
	mp_msg(MSGT_VO, MSGL_V, "<vo_directx><ERROR>Initial Lock on the Surface failed.\n");
	return 1;
}

//function to set color controls
//  brightness	[0, 10000]
//  contrast	[0, 20000]
//  hue		[-180, 180]
//  saturation	[0, 20000]
static uint32_t color_ctrl_set(char *what, int value)
{
	uint32_t	r = VO_NOTIMPL;
	DDCOLORCONTROL	dcc;
	//printf("\n*** %s = %d\n", what, value);
	if (!g_cc) {
		//printf("\n *** could not get color control interface!!!\n");
		return VO_NOTIMPL;
	}
	ZeroMemory(&dcc, sizeof(dcc));
	dcc.dwSize = sizeof(dcc);

	if (!strcmp(what, "brightness")) {
		dcc.dwFlags = DDCOLOR_BRIGHTNESS;
		dcc.lBrightness = (value + 100) * 10000 / 200;
		r = VO_TRUE;
	} else if (!strcmp(what, "contrast")) {
		dcc.dwFlags = DDCOLOR_CONTRAST;
		dcc.lContrast = (value + 100) * 20000 / 200;
		r = VO_TRUE;
	} else if (!strcmp(what, "hue")) {
		dcc.dwFlags = DDCOLOR_HUE;
		dcc.lHue = value * 180 / 100;
		r = VO_TRUE;
	} else if (!strcmp(what, "saturation")) {
		dcc.dwFlags = DDCOLOR_SATURATION;
		dcc.lSaturation = (value + 100) * 20000 / 200;
		r = VO_TRUE;
	}

	if (r == VO_TRUE) {
		g_cc->lpVtbl->SetColorControls(g_cc, &dcc);
	}
	return r;
}

//analoguous to color_ctrl_set
static uint32_t color_ctrl_get(char *what, int *value)
{
	uint32_t	r = VO_NOTIMPL;
	DDCOLORCONTROL	dcc;
	if (!g_cc) {
		//printf("\n *** could not get color control interface!!!\n");
		return VO_NOTIMPL;
	}
	ZeroMemory(&dcc, sizeof(dcc));
	dcc.dwSize = sizeof(dcc);

	if (g_cc->lpVtbl->GetColorControls(g_cc, &dcc) != DD_OK) {
		return r;
	}

	if (!strcmp(what, "brightness") && (dcc.dwFlags & DDCOLOR_BRIGHTNESS)) {
		*value = dcc.lBrightness * 200 / 10000 - 100;
		r = VO_TRUE;
	} else if (!strcmp(what, "contrast") && (dcc.dwFlags & DDCOLOR_CONTRAST)) {
		*value = dcc.lContrast * 200 / 20000 - 100;
		r = VO_TRUE;
	} else if (!strcmp(what, "hue") && (dcc.dwFlags & DDCOLOR_HUE)) {
		*value = dcc.lHue * 100 / 180;
		r = VO_TRUE;
	} else if (!strcmp(what, "saturation") && (dcc.dwFlags & DDCOLOR_SATURATION)) {
		*value = dcc.lSaturation * 200 / 20000 - 100;
		r = VO_TRUE;
	}
//	printf("\n*** %s = %d\n", what, *value);

	return r;
}

int guiCmd(GUI_CMD cmd, int v)
{
	int ret = 0;

	switch (cmd) {
	case CMD_INIT_WINDOW:
		ret = InitWindow();
		if (!filename)
		{
			if(vo_ontop == 1)
				SetWindowPos(hWnd, HWND_TOPMOST, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			ShowWindow(hWnd, SW_SHOW);
		}
		break;
	case CMD_UNINITONLY_WINDOW:
		uninitwin();
		ShowWindow(hWnd,SW_HIDE);
		break;
	case CMD_UNINIT_WINDOW:
		uninit();
		break;
	case CMD_ACTIVE_WINDOW:
		if(hWnd)
			SetForegroundWindow(hWnd);
		break;
	case CMD_SWITCH_ASPECT:
		PostMessage(hWnd, WM_COMMAND, IDM_KEEP_ASPECT, 0);
		break;
	case CMD_SWITCH_VIEW: {
		if (switch_view == 1 && !vidmode && !vo_fs && WndState != SW_SHOWMAXIMIZED) {
			last_rect.left = 0xDEADC0DE;   // reset window position cache
			bFullView = !bFullView;
			SetView();
		}
		break;
	}
	case CMD_UPDATE_VOLUME:
		if (v < 0) v=0;
		if (show_controlbar && !volume_dropping && hWndControlbar) {
			volpercent=v;
			if(!skin_controlbar && hWndVolumebar)
				PostMessage(hWndVolumebar, TBM_SETPOS, TRUE, volpercent);
			else
				InvalidateRect(hWndControlbar, NULL, FALSE);
		}
		break;
	case CMD_UPDATE_SEEK:
		if(g_pTaskbarList && v >= 0) {
			ITaskbarList3_SetProgressValue(g_pTaskbarList, hWnd, (ULONGLONG)v, (ULONGLONG)percent_value);
		}
		if (show_controlbar && !bSeeking && !seek_dropping && hWndControlbar) {
			seekpercent=v;
			if(!skin_controlbar && hWndSeekbar)
				PostMessage(hWndSeekbar, TBM_SETPOS, TRUE, (int)(seekpercent*100.0/percent_value));
			else
				InvalidateRect(hWndControlbar, NULL, FALSE);
		}
		break;
	case CMD_UPDATE_TITLE: {
		is_dvdnav = 0;
		play_finished = 0;
		char *basename= get_rar_stream_filename((stream_t *)v);
		if (basename) {
			snprintf(title_text,500, "[RAR] %s - MPlayer", basename);
		} else if (filename) {
			basename = strrchr(filename, '/');
			if (!basename) basename = _mbsrchr(filename, '\\');
			if (strlen(++basename) < 3) basename = filename;
			if (strstr(basename, "dvd://"))
				snprintf(title_text,500, "[DVD] %s - MPlayer", dvd_device);
#ifdef CONFIG_DVDNAV
			else if (strstr(basename, "dvdnav://")) {
				is_dvdnav = 1;
				snprintf(title_text,500, "[DVDNAV] %s - MPlayer", dvd_device);
				if(is_dvdnav_menu())
					mouse_dvdnav = 1;
				else
					mouse_dvdnav = 0;
				CheckMenuItem(hMenu, IDM_MOUSE_DVDNAV, mouse_dvdnav?MF_CHECKED:MF_UNCHECKED);
			}
#endif
			else
				snprintf(title_text,500, "%s - MPlayer", basename);
			if(show_controlbar || g_pTaskbarList)
				SetTimer(hWnd, TIMER_PLAY_PAUSE, 200, NULL);
		} else {
			strcpy(title_text, "MPlayer");
			if(show_controlbar) {
				KillTimer(hWnd, TIMER_PLAY_PAUSE);
				play_finished = 1;
				if(!vo_fs && vo_ontop == 2)
					SetWindowPos(hWnd, HWND_NOTOPMOST, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			}
			if(show_status)
				sprintf(status, "");
			if(show_status2)
				sprintf(status_text_timer2, "");
			UpdateThumbnailToolbar(hWnd, FALSE);
			shutdown_when_necessary(hWnd);
		}
		if(!use_filename_title)
			strcpy(title_text, "MPlayer");
		if(using_theme || vo_fs || !bFullView || !gui_skin.enable_appskin || !gui_skin.border_ok)
			while(!SetWindowText(hWnd, title_text));
		bIdle = !v;
		seekpercent = 0;
		InvalidateRect(hWnd, NULL, TRUE);
		InvalidateRect(hWndFS, NULL, TRUE);
		if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView)
			draw_title_text(hWnd, &gui_skin, title_text);
		if (show_controlbar && hWndControlbar)
			InvalidateRect(hWndControlbar, NULL, FALSE);
		break;
	}
	case CMD_UPDATE_STATUS:
		if (show_controlbar && show_status && v && hWndControlbar && (!seek_dropping || seek_realtime)) {
			switch(((char *)v)[0]) {
			case OSD_STOP:
				sprintf(status, "");
				break;
			default:
				snprintf(status, 128, "%s ", v + 1);
				break;
			}
			InvalidateRect(hWndControlbar, NULL, FALSE);
		}
		break;
	case CMD_ADD_SUBMENU:
		mi.wID++;
		mi.dwTypeData = (LPSTR)v;
		mi.cch = strlen((LPSTR)v);
		InsertMenuItem(hSubtitleMenu, 0xFFFFFFFF, TRUE, &mi);
		break;
	case CMD_UPDATE_SUBMENU:
		CheckMenuRadioItem(hSubtitleMenu, IDM_SUBTITLE, IDM_SUBTITLE + 99,
			IDM_SUBTITLE + v + 1, MF_BYCOMMAND);
		break;
	case CMD_DEL_SUBMENU:
		if (hSubtitleMenu) {
			if(show_menubar)
				DeleteMenu(hMenu, 3, MF_BYPOSITION);
			DeleteMenu(hPopupMenu, 3, MF_BYPOSITION);
			hSubtitleMenu = NULL;
			InitSubtitleMenu();
			DrawMenuBar(hWnd);
		}
		break;
	}

	return ret;
}

static int control(uint32_t request, void *data)
{
    switch (request) {

	case VOCTRL_PAUSE:
	case VOCTRL_RESUME:
		paused = request == VOCTRL_PAUSE;
		if (vo_ontop==2 && !vidmode && !vo_fs && WndState != SW_SHOWMINIMIZED)
			SetWindowPos(hWnd,paused?(vo_rootwin?HWND_BOTTOM:HWND_NOTOPMOST):HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
		if (show_controlbar && hWndControlbar)
			InvalidateRect(hWndControlbar, NULL, FALSE);
		return paused;
	case VOCTRL_GET_IMAGE:
      	return get_image(data);
    case VOCTRL_QUERY_FORMAT:
        last_rect.left = 0xDEADC0DE;   // reset window position cache
        return query_format(*((uint32_t*)data));
	case VOCTRL_DRAW_IMAGE:
		return put_image(data);
	case VOCTRL_ONTOP:
		if(WinID != -1) return VO_TRUE;
		if(vidmode)
		{
			mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>ontop has no meaning in exclusive mode\n");
		}
		else
		{
			last_rect.left = 0xDEADC0DE;   // reset window position cache
			vo_ontop = (vo_ontop + 1) % 3;
			SetWindowPos(hWnd,((vo_ontop==2)?!paused:vo_ontop)?HWND_TOPMOST:(vo_rootwin?HWND_BOTTOM:HWND_NOTOPMOST),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
		}
		return VO_TRUE;
	case VOCTRL_ROOTWIN:
		if(WinID != -1) return VO_TRUE;
		if(vidmode)
		{
			mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>rootwin has no meaning in exclusive mode\n");
		}
		else
		{
			if(vo_rootwin) vo_rootwin = 0;
			else vo_rootwin = 1;
			last_rect.left = 0xDEADC0DE;   // reset window position cache
			Directx_ManageDisplay();
		}
		return VO_TRUE;
	case VOCTRL_FULLSCREEN:
		if(vidmode)
			mp_msg(MSGT_VO, MSGL_ERR,"<vo_directx><ERROR>currently we do not allow to switch from exclusive to windowed mode\n");
		else
		{
			vo_fs=1-vo_fs;
			save_vo_fs = vo_fs;
			if (vo_fs) {
				ShowWindow(hWnd, SW_HIDE);
				ShowWindow(hWndFS, SW_SHOW);
				BringWindowToTop(hWndFS);
				ShowWindow(hWndControlbar, SW_HIDE);
				SetParent(hWndControlbar, hWndFS);
				ControlbarShow = FALSE;
				PostMessage(hWndFS, WM_MOUSEMOVE, 0, 0x400000);
				set_window_rgn(hWndFS, &gui_skin, FALSE, FALSE);
			} else {
				KillTimer(hWndFS, TIMER_HIDE_CURSOR);
				SetParent(hWndControlbar, hWnd);
				ShowWindow(hWndControlbar, SW_SHOW);
				if(show_menubar_w32 && bFullView && !GetMenu(hWnd)) SetMenu(hWnd,hMenu);
				ShowWindow(hWnd, SW_SHOW);
				ShowWindow(hWndFS, SW_HIDE);
				BringWindowToTop(hWnd);
				SetForegroundWindow(hWnd);
				PostMessage(hWnd, WM_SHOWCURSOR, 0, 0);
				if ((paused || play_finished) && vo_ontop==2 && !vidmode)
				{
					SetWindowPos(hWnd,vo_rootwin?HWND_BOTTOM:HWND_NOTOPMOST,0,0,0,0
					,SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
				}
				else if (vo_ontop==2 && !vidmode)
				{
					SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0
					,SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
				}
				UpdateThumbnailToolbar(hWnd, TRUE);
			}
			PopMenuShow = FALSE;
			last_rect.left = 0xDEADC0DE;   // reset window position cache
			SetFocus(vo_fs?hWndFS:hWnd);
		}
		return VO_TRUE;
	case VOCTRL_SET_EQUALIZER: {
        vf_equalizer_t *eq=data;
		return color_ctrl_set(eq->item, eq->value);
	}
	case VOCTRL_GET_EQUALIZER: {
        vf_equalizer_t *eq=data;
		return color_ctrl_get(eq->item, &eq->value);
	}
    case VOCTRL_UPDATE_SCREENINFO:
        if (vidmode) {
            vo_screenwidth = vm_width;
            vo_screenheight = vm_height;
        } else {
            vo_screenwidth = monitor_rect.right - monitor_rect.left;
            vo_screenheight = monitor_rect.bottom - monitor_rect.top;
        }
        aspect_save_screenres(vo_screenwidth, vo_screenheight);
        return VO_TRUE;
    case VOCTRL_RESET:
        last_rect.left = 0xDEADC0DE;   // reset window position cache
        // fall-through intended
    };
    return VO_NOTIMPL;
}
