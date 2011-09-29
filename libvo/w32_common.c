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
#include <limits.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <pthread.h>
#include <mbstring.h>
#include "mp_msg.h"
#include "video_out.h"
#include "aspect.h"
#include "w32_common.h"
#include "mp_fifo.h"
#include "winstuff.h"
#include "win7shellapi.h"
#include "resource.h"
#include "version.h"
#include "path.h"

#include "stream/stream.h"
#include "osdep/keycodes.h"
#include "input/input.h"
#include "input/mouse.h"
#include "sub/sub.h"
#include "skin/skin.h"

#ifndef MONITOR_DEFAULTTOPRIMARY
#define MONITOR_DEFAULTTOPRIMARY 1
#endif

#ifndef PFD_SUPPORT_COMPOSITION
#define PFD_SUPPORT_COMPOSITION 0x00008000
#endif

#define WNDSTYLE WS_OVERLAPPEDWINDOW|WS_SIZEBOX

#define IDTB_PLAY_PAUSE WM_USER + 956
#define IDTB_PREVIOUS   WM_USER + 695
#define IDTB_NEXT       WM_USER + 895
#define UPDATE_VOSUB    WM_USER + 586

extern int vo_dirver;
static BOOL using_d3d = FALSE;
static const char title_d3d[] = "MPlayer Direct3D";
static const char title_gl[] = "MPlayer OpenGL";
static char title_w32[32] = "MPlayer WW";

static const char* classname = "MPlayer WW for Windows";
int vo_vm = 0;

// last non-fullscreen extends
static int prev_width;
static int prev_height;
static int prev_x;
static int prev_y;
static int initwin_finished = 0;
static int confige_finished = 0;
static int save_width = -1;
static int save_height = -1;
static int save_x = -1000;
static int save_y = -1000;
static uint32_t o_dwidth;
static uint32_t o_dheight;
static BOOL bRBDown = FALSE;
static BOOL bWheel = FALSE;

static HINSTANCE hInstance;
HWND vo_w32_window = 0;
HWND vo_window = 0;
/** HDC used when rendering to a device instead of window */
static HDC dev_hdc = 0;
static int event_flags;
static int mon_cnt;
static HICON mplayerIcon = 0;
static HWND vo_full_window = 0;
static HWND hWndControlbar = NULL;
static HWND hWndSeekbar = NULL;
static HWND hWndVolumebar = NULL;
extern int save_volume;
extern int open_with_dshow_demux;
static int seekbar_width = 0;
static const int volumebar_width = 60;
static const int volumebar_height = 20;
static int min_video_width=320;
int controlbar_height_gl = 0;
static int controlbar_alpha_save = 0;
static int auto_hide_control_save = 0;
static int controlbar_height_fixed = 0;
extern int controlbar_height;
extern int controlbar_alpha;
extern int skinned_player;
extern int have_audio;
extern skin_t gui_skin;
static int ControlbarShow = TRUE;
static int PopMenuShow = TRUE;
static int controlbar_offset = 0;
static int is_dvdnav = 0;
static int dvdnav_checktime = 0;
extern int mouse_dvdnav;
extern int auto_shutdown;
extern logo_t gui_logo;
extern int seek_dropping;
extern int volume_dropping;
extern int one_player;
extern int skin_controlbar;
extern char status[128];
extern char *new_audio_stream;
static char *subdownloader = NULL;
extern int get_video_length();
extern char *get_help_text(void);
extern void update_subtitle_menu();
extern void shutdown_when_necessary(HWND hWnd);

extern void ShowEqualizer(HWND hWnd);
extern int GetDVDDriver(HWND hWnd);
extern int GeBlurayDriver(HWND hWnd);
extern BOOL GetURL(HINSTANCE hInstance, HWND hWndParent, char **url);
extern BOOL GetOpenFile2(HINSTANCE hInstance, HWND hWndParent, char **file, char **audio);
extern BOOL GetPlayCD(HINSTANCE hInstance, HWND hWndParent);

static HMONITOR (WINAPI* myMonitorFromWindow)(HWND, DWORD);
static BOOL (WINAPI* myGetMonitorInfo)(HMONITOR, LPMONITORINFO);
static BOOL (WINAPI* myEnumDisplayMonitors)(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
static BOOL (WINAPI* SetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD) = NULL;
static BOOL (WINAPI* ChangeWindowMessageFilter)(UINT message, DWORD dwFlag) = NULL;
typedef HRESULT (__stdcall * SetWindowThemeFunct)(HWND, LPCWSTR, LPCWSTR);
typedef HRESULT (__stdcall * DwmAttributeFunct)(HWND,DWORD,LPCVOID,DWORD);

static const struct mp_keymap vk_map[] = {
    // special keys
    {VK_ESCAPE, KEY_ESC}, {VK_BACK, KEY_BS}, {VK_TAB, KEY_TAB}, {VK_CONTROL, KEY_CTRL},

    // cursor keys
    {VK_LEFT, KEY_LEFT}, {VK_UP, KEY_UP}, {VK_RIGHT, KEY_RIGHT}, {VK_DOWN, KEY_DOWN},

    // navigation block
    {VK_INSERT, KEY_INSERT}, {VK_DELETE, KEY_DELETE}, {VK_HOME, KEY_HOME}, {VK_END, KEY_END},
    {VK_PRIOR, KEY_PAGE_UP}, {VK_NEXT, KEY_PAGE_DOWN},

    // F-keys
    {VK_F1, KEY_F+1}, {VK_F2, KEY_F+2}, {VK_F3, KEY_F+3}, {VK_F4, KEY_F+4},
    {VK_F5, KEY_F+5}, {VK_F6, KEY_F+6}, {VK_F7, KEY_F+7}, {VK_F8, KEY_F+8},
    {VK_F9, KEY_F+9}, {VK_F10, KEY_F+10}, {VK_F11, KEY_F+11}, {VK_F12, KEY_F+12},
    // numpad
    {VK_NUMPAD0, KEY_KP0}, {VK_NUMPAD1, KEY_KP1}, {VK_NUMPAD2, KEY_KP2},
    {VK_NUMPAD3, KEY_KP3}, {VK_NUMPAD4, KEY_KP4}, {VK_NUMPAD5, KEY_KP5},
    {VK_NUMPAD6, KEY_KP6}, {VK_NUMPAD7, KEY_KP7}, {VK_NUMPAD8, KEY_KP8},
    {VK_NUMPAD9, KEY_KP9}, {VK_DECIMAL, KEY_KPDEC},

    {0, 0}
};

static long WM_TASKBARCREATED_W32;
static NOTIFYICONDATA nid_w32;
static HWND hWndP_W32 = NULL;
static HMENU hWinMenu, hSubtitleMenu, hFavoriteMenu, hSkinMenu, hPopupMenu;
static int menu_height = 0;
static float window_aspect = 1.2;
static int xborder,yborder;
static int xborder0,yborder0;
#define MOVING_NONE	 0
#define MOVING_SUB	  1
#define MOVING_WINDOW   2
#define WM_NOTIFYICON	 (WM_USER+1973)
#define TIMER_SWITCH_VIEW 100
#define TIMER_HIDE_CURSOR 101
#define TIMER_PLAY_PAUSE 107
#define TIMER_DVDNAV 119

#ifndef WM_XBUTTONDOWN
# define WM_XBUTTONDOWN    0x020B
# define WM_XBUTTONUP      0x020C
# define WM_XBUTTONDBLCLK  0x020D
#endif

static int nothide_times = 0;
static int FullHide;
static DWORD start_lbutton_down = 0;
static DWORD start_lbutton_dbcheck = 0;
static DWORD start_flbutton_dbcheck = 0;
static DWORD start_frbutton_dbcheck = 0;
static int old_x, old_y, bMoving = MOVING_NONE;
static float factors[3] = {0, -0.1875, -0.2975};
static int factor_index = 0;
float w32_factor_x=0,w32_factor_y=0;
int w32_delta_x=0,w32_delta_y=0;
extern int systray;
extern int switch_view;
extern int vidmode;
int full_view = 1;
int max_height = 0;
int max_width = 0;

extern int auto_hide;
static BOOL bFullView = TRUE;
static BOOL bOldView = TRUE;
static int WndState = SW_NORMAL;
static int startplayer = 0;
static COLORREF windowcolor = RGB(0,0,16);  //windowcolor == colorkey
static HBRUSH   colorbrush = NULL;  // Handle to colorkey brush
static HBRUSH   blackbrush= NULL;
static HBRUSH   bkgndbrush = NULL;
static HBITMAP  hLogo;
static int  logoW, logoH;
static HCURSOR  mplayercursor = NULL;   // Handle to mplayer cursor
static int bIdle;
extern int force_load_font;
extern char *sub_font_name;
extern char *sub_cp;
#define DMAX_PATH MAX_PATH*2
#ifdef CONFIG_ICONV
static char AlternativeFont[DMAX_PATH];
#endif
extern char *filename;
extern char *dvd_device;
extern int sys_Language;
extern int gui_thread;
extern int is_vista;
extern int is_win7;
extern int use_windowblinds;
extern int osd_level;
extern int gl_fs_flash;
extern int using_theme;
extern int using_aero;
extern int show_menubar;
static int show_menubar_w32;
extern int show_controlbar;
extern int show_status;
extern int show_status2;
extern int use_filename_title;
extern int use_rightmenu;
extern int disable_screensaver;
extern BOOL bScrActiveSave;
extern char status_text_timer2[64];

extern void check_resize(void);
extern int ExitGuiThread;
HANDLE hAccelTableW32;
int fs_start = -1;
extern void resize_fs_gl();
extern void resize_fs_gl2();
static int last_vo_fs = -12345;
static int need_adjust = 0;
static MENUITEMINFO minf,minf2,minf3;
extern int auto_resume;
static char subMenu[16]={0},subMenuItem[16]={0};
static char favMenu[32]={0},favMenuItem[32]={0},favMenuManage[32]={0};

HANDLE hAeroThread = NULL;
DWORD AeroThreadID;
HANDLE hguiThread = NULL;
DWORD guiThreadID;
int vo_paused = 0;
int w32_inited = 0;
static int play_finished = 0;
extern char *skins_name[100];
extern char *skin_name;
extern char *last_filename;
static int vo_paused_save = 1;
extern int bSeeking;
extern int no_dvdnav;
extern int reload_when_open;
extern float percent_value;
extern int volpercent;
extern int seekpercent;
extern int seek_realtime;
extern int auto_hide_control;
static void SetView(void);
static HHOOK hMHook = NULL;
static char title_text[512] = {0};
static BOOL bNcLButtonDown = FALSE;
static int need_recreate = 0;
static unsigned int s_uTBBC = WM_NULL;
static ITaskbarList3 *g_pTaskbarList = NULL;

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

	while(i >= j) {
		DeleteMenu(hFavoriteMenu, i, MF_BYPOSITION);
		i--;
	}
	minf2.wID = IDM_FAVORITES;
	if(initStatus()) {
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
	if(show_menubar_w32)
		DrawMenuBar(vo_window);
}

extern int ShowFavDlg(HINSTANCE hInstance,HWND hWndParent);
static BOOL CALLBACK ControlbarProcSkin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK ControlbarProcNormal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
extern void display_playlist(int x,int y,HWND parent,HWND layer);
extern void close_playlistwindow(void);
extern int is_dvdnav_menu();
extern void mp_parse_commandline(char *cmdline);

static DWORD WINAPI osdThreadProc(LPVOID lpParam)
{
	Sleep(60);
	force_load_font = 1;
	vo_osd_changed(OSDTYPE_SUBTITLE);
	return 0;
}

static void MoveContralBarItem(int width)
{
	if(!show_controlbar || skin_controlbar || !hWndSeekbar || !hWndVolumebar)
		return;
	seekbar_width = width - volumebar_width;
	MoveWindow(hWndSeekbar, 0, 0, seekbar_width, volumebar_height, TRUE);
	MoveWindow(hWndVolumebar, seekbar_width, 0, volumebar_width, volumebar_height,TRUE);
}

static void ShowContralBar()
{
	if(!show_controlbar)
		return;
	if(gl_fs_flash || (vo_w32_window != vo_window && using_d3d)) {
		if(ControlbarShow)
			ShowWindow(hWndControlbar,SW_SHOW);
		else {
			ShowWindow(hWndControlbar,SW_HIDE);
			(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
		}
	} else {
		if(ControlbarShow) {
			MoveWindow(hWndControlbar, xinerama_x,xinerama_y+vo_screenheight-controlbar_height_fixed+controlbar_offset,vo_screenwidth,controlbar_height_fixed, TRUE);
		} else {
			MoveWindow(hWndControlbar, 1 - vo_screenwidth,vo_screenheight-1,vo_screenwidth,controlbar_height_fixed, TRUE);
			InvalidateRect(hWndControlbar, NULL, FALSE);
		}
	}
}

static void resize_fs_d3d()
{
	int w, h, x, y;
	RECT rl, rr, rt, rb;

	if(!vo_fs || !w32_inited)
		return;

	if ( (w32_factor_x == 0) && (w32_factor_y == 0)) {
		w = vo_screenwidth;
		h = vo_screenheight;
	} else {
		w = vo_screenwidth;
		h = vo_screenheight;
		w = (int)(w*(1+w32_factor_x))>>1<<1;
		h = (int)(h*(1+w32_factor_y))>>1<<1;
	}

	x = (vo_screenwidth-w)/2 + w32_delta_x;
	y = (vo_screenheight-h)/2 + w32_delta_y;

	rl.top = 0; rl.left = 0; rl.right = x; rl.bottom = vo_screenheight;
	rt.top = 0; rt.left = 0; rt.right = vo_screenwidth; rt.bottom = y;
	rr.top = 0; rr.left = x+w; rr.right = vo_screenwidth; rr.bottom = vo_screenheight;
	rb.top = y+h; rb.left = 0; rb.right = vo_screenwidth; rb.bottom = vo_screenheight;

	MoveWindow(vo_w32_window, x, y, w, h, FALSE);

	if(x > 0) InvalidateRect(vo_window, &rl, TRUE);
	if(y > 0) InvalidateRect(vo_window, &rt, TRUE);
	if(x+w < vo_screenwidth) InvalidateRect(vo_window, &rr, TRUE);
	if(y+h < vo_screenheight) InvalidateRect(vo_window, &rb, TRUE);

  	mp_msg(MSGT_VO, MSGL_V, "[d3d] Resize_FS: %d, %d %dx%d\n", x, y, w, h);
}

static void show_cursor(int times)
{
	if(vo_fs && auto_hide) {
		nothide_times = times;
		while (ShowCursor(TRUE)<0);
		if (auto_hide)
			SetTimer(vo_window, TIMER_HIDE_CURSOR, auto_hide, NULL);
	}
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
		else if(vo_paused) {
			ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_PAUSED);
			ITaskbarList3_SetProgressValue(g_pTaskbarList, vo_window, (ULONGLONG)seekpercent, (ULONGLONG)percent_value);
		} else {
			ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_NORMAL);
			ITaskbarList3_SetProgressValue(g_pTaskbarList, vo_window, (ULONGLONG)seekpercent, (ULONGLONG)percent_value);
		}

		THUMBBUTTON buttons[3] = {};

		buttons[0].dwMask = THB_BITMAP | THB_FLAGS;
		buttons[0].dwFlags = THBF_ENABLED;
		buttons[0].iId = IDTB_PREVIOUS;
		buttons[0].iBitmap = 2;

		buttons[1].dwMask = THB_BITMAP | THB_FLAGS;
		buttons[1].dwFlags = THBF_ENABLED;
		buttons[1].iId = IDTB_PLAY_PAUSE;
		if(vo_paused || play_finished || !filename)
			buttons[1].iBitmap = 0;
		else
			buttons[1].iBitmap = 1;
		buttons[2].dwMask = THB_BITMAP | THB_FLAGS;
		buttons[2].dwFlags = THBF_ENABLED;
		buttons[2].iId = IDTB_NEXT;
		buttons[2].iBitmap = 3;

		if(ReAdd) {
			HIMAGELIST hImglist = ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TASK_THUMBNAIL),
				16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_SHARED);
			if (hImglist) {
				ITaskbarList3_ThumbBarSetImageList(g_pTaskbarList, hWnd, hImglist);
				ImageList_Destroy(hImglist);
			}
			ITaskbarList3_ThumbBarAddButtons(g_pTaskbarList, hWnd, 3, buttons);
		} else
			ITaskbarList3_ThumbBarUpdateButtons(g_pTaskbarList, hWnd, 3, buttons);
	}
}

static LRESULT CALLBACK MouseHookProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode == HC_ACTION) {
		// get a pointer to the mouse hook struct.
		PMOUSEHOOKSTRUCT mhs = (PMOUSEHOOKSTRUCT) lParam;

		// intercept messages for left button down and up
		switch(wParam)
		{
		case WM_NCLBUTTONDOWN:
			{
				if (!bNcLButtonDown && HTCAPTION == mhs->wHitTestCode && mhs->hwnd == vo_window)
					bNcLButtonDown = TRUE;
			}
			break;
		case WM_LBUTTONUP:
			{
				if(bNcLButtonDown && mhs->hwnd == vo_window) {
					PostMessage(vo_window, WM_NCLBUTTONUP, HTCAPTION, MAKELONG(mhs->pt.x,mhs->pt.y));
					bNcLButtonDown = FALSE;
				}
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			if(vo_fs && vo_full_window && mhs->hwnd == vo_window) {
				PostMessage(vo_window, wParam, 0, 0);
				return TRUE;
			}
		default:
			break;
		}
	}
	return CallNextHookEx(hMHook, nCode, wParam, lParam);
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
	controlbar_alpha = controlbar_alpha_save;

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
		if(!vo_full_window) {
			UnhookWindowsHookEx(hMHook);
			hMHook = NULL;
		}
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
			SetMenu(hWnd, hWinMenu);
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
			if(gui_skin.alpha > 0 && gui_skin.alpha < 255)
				controlbar_alpha = gui_skin.alpha;
			controlbar_height_fixed = gui_skin.background_height;
		}
		if(auto_hide_control) {
			controlbar_height = 0;
			ControlbarShow = FALSE;
			ShowWindow(hWndctrl, SW_HIDE);
			if(SetLayeredWindowAttributes && (using_d3d || is_vista)) {
				if(controlbar_alpha > 0)
					SetLayeredWindowAttributes(hWndctrl, RGB(255,0,255), controlbar_alpha, 3);
				else
					SetLayeredWindowAttributes(hWndctrl, RGB(255,0,255), 255, 1);
			}
		} else {
			controlbar_height = controlbar_height_fixed;
			ShowWindow(hWndctrl, SW_SHOW);
			if(SetLayeredWindowAttributes && (using_d3d || is_vista))
				SetLayeredWindowAttributes(hWndctrl, RGB(255,0,255), 255, 1);
		}
		InvalidateRect(hWndctrl, NULL, FALSE);
	}

	if(gui_skin.background_width > 320)
		min_video_width = gui_skin.background_width;
	else
		min_video_width = 320;

	if(gui_skin.enable_appskin && gui_skin.border_ok) {
		controlbar_height_gl = 0;
		xborder0 = xborder = gui_skin.border_left.img->width + gui_skin.border_right.img->width;
		yborder0 = yborder = gui_skin.title.img->height + gui_skin.border_bottom.img->height + controlbar_height;
	} else {
		controlbar_height_gl = controlbar_height;
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
	if(show_controlbar && hWndctrl) {
		RECT r;
		POINT pt;
		GetClientRect(hWnd, &r);
		width = r.right;
		height = r.bottom;
		if(bFullView && (auto_hide_control || !(gui_skin.enable_appskin && gui_skin.border_ok)))
			height -= controlbar_height_fixed;
		pt.x = 0;
		pt.y = height;
		ClientToScreen(hWnd,&pt);
		MoveWindow(hWndctrl,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
	}

	set_window_rgn(hWnd, &gui_skin, !vo_fs & bFullView, TRUE);

	UpdateThumbnailToolbar(hWnd, TRUE);
	return 1;
}

static LRESULT CALLBACK W32FullFocusPrc (HWND hWnd, UINT message,  WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
		return TRUE;
	case WM_MOUSEMOVE:
	case WM_COMMAND:
	case WM_SYSCHAR:
	case WM_SYSCOMMAND:
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
		PostMessage(vo_window, message, wParam, lParam);
		return TRUE;
	}
    return DefWindowProc(hWnd, message, wParam, lParam);
}

static LRESULT CALLBACK W32WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        event_flags |= VO_EVENT_EXPOSE;
        break;
	case WM_WINDOWPOSCHANGING:
		return 0;
	case WM_MOUSEMOVE:
	case WM_COMMAND:
	case WM_SYSCHAR:
	case WM_SYSCOMMAND:
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
	case WM_DROPFILES:
		PostMessage(vo_window, message, wParam, lParam);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT r;
	POINT p;
    int mpkey;
    if (WinID >= 0) {
	    switch (message) {
	        case WM_MOUSEMOVE:
	        case WM_LBUTTONDOWN:
	        case WM_LBUTTONUP:
	        case WM_LBUTTONDBLCLK:
	        case WM_MBUTTONDOWN:
	        case WM_MBUTTONUP:
	        case WM_MBUTTONDBLCLK:
	        case WM_RBUTTONDOWN:
	        case WM_RBUTTONUP:
	        case WM_RBUTTONDBLCLK:
	            SendNotifyMessage(WinID, message, wParam, lParam);
	    }
    } else {
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
							if(vo_paused)
								ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_PAUSED);
							else 
								ITaskbarList3_SetProgressState(g_pTaskbarList, hWnd, TBPF_NORMAL);
						}
						HIMAGELIST hImglist = ImageList_LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TASK_THUMBNAIL),
							16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_SHARED);
						if (hImglist) {
							ITaskbarList3_ThumbBarSetImageList(g_pTaskbarList, hWnd, hImglist);
							ImageList_Destroy(hImglist);
						}
						THUMBBUTTON buttons[3] = {};

						buttons[0].dwMask = THB_BITMAP | THB_FLAGS;
						buttons[0].dwFlags = THBF_ENABLED;
						buttons[0].iId = IDTB_PREVIOUS;
						buttons[0].iBitmap = 2;

						buttons[1].dwMask = THB_BITMAP | THB_FLAGS;
						buttons[1].dwFlags = THBF_ENABLED;
						buttons[1].iId = IDTB_PLAY_PAUSE;
						if(vo_paused || play_finished || !filename)
							buttons[1].iBitmap = 0;
						else
							buttons[1].iBitmap = 1;

						buttons[2].dwMask = THB_BITMAP | THB_FLAGS;
						buttons[2].dwFlags = THBF_ENABLED;
						buttons[2].iId = IDTB_NEXT;
						buttons[2].iBitmap = 3;

						ITaskbarList3_ThumbBarAddButtons(g_pTaskbarList, hWnd, 3, buttons);
					}
				}
			}
    	} else switch (message) {
			case WM_NCCALCSIZE:
			{
				gui_skin.dlg_frame = 0;
				if(!vo_fs) {
					if(!bFullView) {
						RECT *rc;
						if(wParam)
							rc = &((LPNCCALCSIZE_PARAMS)lParam)->rgrc[0];
						else
							rc = (LPRECT)lParam;
						rc->top += 1;
						rc->left += 1;
						rc->right -= 1;
						rc->bottom -= 1;
						return 0;
					} else if(gui_skin.enable_appskin && gui_skin.border_ok) {
						RECT *rc;
						if(wParam)
							rc = &((LPNCCALCSIZE_PARAMS)lParam)->rgrc[0];
						else
							rc = (LPRECT)lParam;
						rc->top += gui_skin.title.img->height;
						rc->left += gui_skin.border_left.img->width;
						rc->right -= gui_skin.border_right.img->width;
						rc->bottom -= gui_skin.border_bottom.img->height + controlbar_height;
						if(IsZoomed(hWnd)) {
							gui_skin.dlg_frame = GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYDLGFRAME);
							rc->top += gui_skin.dlg_frame;
							rc->bottom -= gui_skin.dlg_frame;
						}
						return 0;
					}
				}
				break;
			}
			case WM_NCHITTEST:
			{
				if(!vo_fs) {
					if(!bFullView) {
						p.x = LOWORD(lParam);
						p.y = HIWORD(lParam);
						GetWindowRect(hWnd, &r);
						if(p.x <= r.left+3 && p.y <= r.top+3)
							return HTTOPLEFT;
						else if(p.x >= r.right-3 && p.y <= r.top+3)
							return HTTOPRIGHT;
						else if(p.x <= r.left+3 && p.y >= r.bottom-3)
							return HTBOTTOMLEFT;
						else if(p.x >= r.right-3 && p.y >= r.bottom-3)
							return HTBOTTOMRIGHT;
						else if(p.x <= r.left+3)
							return HTLEFT;
						else if(p.x >= r.right-3)
							return HTRIGHT;
						else if(p.y <= r.top+3)
							return HTTOP;
						else if(p.y >= r.bottom-3)
							return HTBOTTOM;
					} else if(gui_skin.enable_appskin && gui_skin.border_ok) {
						LRESULT lRet = DefWindowProc(hWnd, message, wParam, lParam);
						if (lRet == HTSYSMENU || lRet == HTMAXBUTTON || lRet == HTMINBUTTON || lRet == HTCLOSE)
							return HTCAPTION;
						else
							return lRet;
					}
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
				if(vo_fs || !bFullView || !hPopupMenu || show_menubar_w32 ||
					wParam == HTMAXBUTTON || wParam == HTMINBUTTON || wParam == HTCLOSE)
					break;
				p.x = LOWORD(lParam);
				p.y = HIWORD(lParam);
				TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
				return TRUE;
			}
			case WM_NCACTIVATE:
				if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView) {
					draw_border(hWnd, &gui_skin, title_text, TRUE);
					return TRUE;
				}
				break;
			case 0x00AE/*WM_NCUAHDRAWCAPTION*/:
			case 0x00AF/*WM_NCUAHDRAWFRAME*/:
				if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView)
					return 0;
				break;
			case WM_ACTIVATEAPP:
				if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView)
					draw_border(hWnd, &gui_skin, title_text, TRUE);
				break;
			case WM_NCPAINT:
			{
				if (!vo_fs) {
					if(gui_skin.enable_appskin && gui_skin.border_ok && bFullView) {
						draw_border(hWnd, &gui_skin, title_text, TRUE);
						return 0;
					} else if(!bFullView) {
						HDC hdc;
						GetWindowRect(hWnd, &r);
						r.right -= r.left;
						r.bottom -= r.top;
						r.left = 0;
						r.top = 0;
						hdc = GetWindowDC(hWnd);
						FillRect(hdc, &r, bIdle?bkgndbrush:colorbrush);
						ReleaseDC(hWnd, hdc);
						return 0;
					}
				}
				break;
			}
			case WM_ERASEBKGND:
			{
				if(w32_inited && filename) {
					if(is_vista && using_theme && using_aero && !using_d3d && vo_w32_window == vo_window)
						return TRUE;
					if(!vo_fs && vo_w32_window != vo_window) {
						GetClientRect(hWnd, &r);
						FillRect((HDC)wParam, &r, colorbrush);
					} else if(vo_fs && using_d3d && vo_w32_window != vo_window) {
						GetClientRect(hWnd, &r);
						FillRect((HDC)wParam, &r, blackbrush);
					}
					return FALSE;
				}
				GetClientRect(hWnd, &r);
				if (!vo_fs) {
					if (bIdle) {
						int width, height, y;
						HDC dc1 = CreateCompatibleDC((HDC)wParam);
						HDC dc2 = CreateCompatibleDC((HDC)wParam);
						HBITMAP bmp = CreateCompatibleBitmap((HDC)wParam, r.right, r.bottom);
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
						FillRect(dc1, &r, bkgndbrush);
						if(gui_skin.enable_appskin && gui_skin.border_ok)
							y = (r.bottom+r.top-height)>>1;
						else
							y = (r.bottom+r.top-height-controlbar_height)>>1;
						BitBlt(dc1, (r.right+r.left-width)>>1, y, width, height, dc2, 0, 0, SRCCOPY);
						BitBlt((HDC)wParam, r.left, r.top, r.right-r.left, r.bottom-r.top,
							dc1, r.left, r.top, SRCCOPY);
						DeleteObject(bmp);
						DeleteDC(dc1);
						DeleteDC(dc2);
					} else {
						FillRect((HDC)wParam, &r, colorbrush);
					}
				}
				return FALSE;
			}
			case WM_PAINT:
				event_flags |= VO_EVENT_EXPOSE;
				break;
			case UPDATE_VOSUB:
				force_load_font = 1;
				vo_osd_changed(OSDTYPE_SUBTITLE);
				break;
			case WM_MOVE:
			{
                event_flags |= VO_EVENT_MOVE;
				start_lbutton_down = 0;
				start_lbutton_dbcheck = 0;
				p.x = 0;
				p.y = 0;
				ClientToScreen(vo_window, &p);
				vo_dx = p.x;
				vo_dy = p.y;
				if(!vo_fs && startplayer) {
					save_x = vo_dx;
					save_y = vo_dy;
					if(show_menubar_w32 && GetMenu(vo_window))
						save_y -= menu_height;
				}
				if(!vo_fs && show_controlbar && hWndControlbar) {
					POINT pt;
					GetClientRect(vo_window, &r);
					pt.x = 0;
					pt.y = r.bottom;
					if(bFullView && (auto_hide_control || !(gui_skin.enable_appskin && gui_skin.border_ok)))
						pt.y -= controlbar_height_fixed;
					ClientToScreen(vo_window,&pt);
					MoveWindow(hWndControlbar,pt.x,pt.y,r.right,controlbar_height_fixed,TRUE);
					InvalidateRect(hWndControlbar, NULL, FALSE);
				}
				break;
			}
			case WM_SIZING:
			{
				if(vo_fs)
					break;
				LPRECT rc = (LPRECT)lParam;
				if(bFullView && rc->right - rc->left < min_video_width)
					rc->right = rc->left + min_video_width;
				if (vo_keepaspect) {
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
						rc->right = rc->left+(rc->bottom-rc->top-yborder)*window_aspect +xborder+0.5;
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
				} else if(rc->bottom - rc->top < yborder)
					rc->bottom = rc->top + yborder;
				if(show_controlbar && hWndControlbar) {
					GetClientRect(vo_window, &r);
					int width = r.right;
					int height = r.bottom;
					if(bFullView && (auto_hide_control || !(gui_skin.enable_appskin && gui_skin.border_ok)))
						height -= controlbar_height_fixed;
					POINT pt;
					pt.x = 0;
					pt.y = height;
					ClientToScreen(vo_window,&pt);
					MoveWindow(hWndControlbar,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
					MoveContralBarItem(width);
					InvalidateRect(hWndControlbar, NULL, FALSE);
				}
				set_window_rgn(vo_window, &gui_skin, !vo_fs & bFullView, TRUE);
				if(!w32_inited)
					return 0;
				if(vo_w32_window != vo_window) {
					GetClientRect(vo_window, &r);
					int width = r.right;
					int height = r.bottom;
					if(!vo_fs && bFullView && !(gui_skin.enable_appskin && gui_skin.border_ok))
						height -= controlbar_height;
					MoveWindow(vo_w32_window, 0, 0, width, height, filename?FALSE:TRUE);
				}
				max_height = 0;
				max_width = 0;
				(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
				return 0;
			}
			case WM_SIZE:
			{
				if(!vo_fs && !w32_inited && bIdle) {
					if(show_controlbar && hWndControlbar){
						GetClientRect(vo_window, &r);
						int width = r.right;
						int height = r.bottom;
						if(bFullView && (auto_hide_control || !(gui_skin.enable_appskin && gui_skin.border_ok)))
							height -= controlbar_height_fixed;
						POINT pt;
						pt.x = 0;
						pt.y = height;
						ClientToScreen(vo_window,&pt);
						MoveWindow(hWndControlbar,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
						MoveContralBarItem(width);
						InvalidateRect(hWndControlbar, NULL, FALSE);
					}
					InvalidateRect(hWnd, NULL, TRUE);
					set_window_rgn(vo_window, &gui_skin, !vo_fs & bFullView, FALSE);
					break;
				}
				event_flags |= VO_EVENT_RESIZE;
				GetClientRect(vo_window, &r);
				vo_dwidth = r.right;
				vo_dheight = r.bottom;
				if(!vo_fs && confige_finished && wParam != SIZE_MAXIMIZED) {
					save_width = vo_dwidth;
					save_height = vo_dheight;
					if(bFullView && !(gui_skin.enable_appskin && gui_skin.border_ok)) {
						save_height -= controlbar_height;
						if(vo_w32_window != vo_window)
							vo_dheight -= controlbar_height;
					}
				}
				if (!vo_fs && wParam == SIZE_MAXIMIZED) {
					if(vo_w32_window == vo_window) {
						max_height = r.bottom;
						max_width = r.right;
					}
					WndState = SW_MAXIMIZE;
					if(vo_keepaspect)
						aspect(&vo_dwidth, &vo_dheight, A_ZOOM);
					if(r.bottom < (int)((float)vo_dwidth/window_aspect)) {
						vo_dheight = r.bottom;
						if(bFullView && !(gui_skin.enable_appskin && gui_skin.border_ok))
							vo_dheight -= controlbar_height;
						vo_dwidth = (int)((float)vo_dheight * window_aspect);
					}
					int width = r.right;
					int height = r.bottom;
					if(show_controlbar && hWndControlbar) {
						if(bFullView && (auto_hide_control || !(gui_skin.enable_appskin && gui_skin.border_ok)))
							height -= controlbar_height_fixed;
						POINT pt;
						pt.x = 0;
						pt.y = height;
						ClientToScreen(vo_window,&pt);
						MoveWindow(hWndControlbar,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
						MoveContralBarItem(width);
						InvalidateRect(hWndControlbar, NULL, FALSE);
					}
				} else if(!vo_fs && wParam == SIZE_RESTORED) {
					max_height = 0;
					max_width = 0;
					WndState = SW_NORMAL;
					int width = r.right;
					int height = r.bottom;
					if(show_controlbar && hWndControlbar) {
						if(bFullView && (auto_hide_control || !(gui_skin.enable_appskin && gui_skin.border_ok)))
							height -= controlbar_height_fixed;
						POINT pt;
						pt.x = 0;
						pt.y = height;
						ClientToScreen(vo_window,&pt);
						MoveWindow(hWndControlbar,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
						MoveContralBarItem(width);
						InvalidateRect(hWndControlbar, NULL, FALSE);
					}
				} else if(!vo_fs && wParam == SIZE_MINIMIZED)
					WndState = SW_MINIMIZE;
				if(gui_thread && !vo_fs && !vo_paused && w32_inited)
				  mp_input_queue_cmd(mp_input_parse_cmd("vo_gui_resize"));
				if(vo_w32_window != vo_window) {
					GetClientRect(vo_window, &r);
					int width = r.right;
					int height = r.bottom;
					int x=0, y=0;
					if(!vo_fs && bFullView && !(gui_skin.enable_appskin && gui_skin.border_ok))
						height -= controlbar_height;
					if(wParam == SIZE_MAXIMIZED) {
						if(vo_dwidth < width) {
							x = (width - vo_dwidth) / 2;
							width = vo_dwidth;
						}
						if(vo_dheight < height) {
							y = (height - vo_dheight) / 2;
							height = vo_dheight;
						}
					}
					MoveWindow(vo_w32_window,  x, y, width, height, filename?FALSE:TRUE);
				}
				if(w32_inited && (!filename || wParam == SIZE_MAXIMIZED))
					InvalidateRect(vo_window, NULL, TRUE);
				set_window_rgn(vo_window, &gui_skin, !vo_fs & bFullView, FALSE);
				(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
				break;
			}
			case WM_TIMER:
			{
				switch (wParam) {
				case TIMER_HIDE_CURSOR:
					if(nothide_times) {
						--nothide_times;
						break;
					}
					if (!GetMenu(hWnd))
						while(ShowCursor(FALSE)>=0);
					KillTimer(vo_window, TIMER_HIDE_CURSOR);
					break;
				case TIMER_DVDNAV:
#ifdef CONFIG_DVDNAV
					if (!no_dvdnav && filename && is_dvdnav) {
						int is_menu = is_dvdnav_menu();
						if(mouse_dvdnav && !is_menu) {
							KillTimer(hWnd, TIMER_DVDNAV);
							mouse_dvdnav = 0;
							dvdnav_checktime = 0;
							CheckMenuItem(hWinMenu, IDM_MOUSE_DVDNAV, MF_UNCHECKED);
							update_subtitle_menu();
						} else if(is_menu && !mouse_dvdnav) {
							KillTimer(hWnd, TIMER_DVDNAV);
							mouse_dvdnav = 1;
							dvdnav_checktime = 0;
							CheckMenuItem(hWinMenu, IDM_MOUSE_DVDNAV, MF_CHECKED);
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
				case TIMER_PLAY_PAUSE:
					if( vo_paused_save == vo_paused)
						break;
					vo_paused_save = vo_paused;
					if (show_controlbar && hWndControlbar)
						InvalidateRect(hWndControlbar, NULL, FALSE);
					UpdateThumbnailToolbar(hWnd, FALSE);
					break;
				}
				return 0;
			}
			case WM_MOUSEMOVE:
				{
					if(!no_dvdnav && mouse_dvdnav && filename && is_dvdnav) {
						char tmps[32];
						snprintf(tmps, 32, "set_mouse_pos %d %d", LOWORD(lParam), HIWORD(lParam));
						mp_input_queue_cmd(mp_input_parse_cmd(tmps));
						break;
					}
					static DWORD lastp=0;
					if (lastp != lParam) {
						lastp = lParam;
						if (vo_fs) {
							if (auto_hide) {
								while (ShowCursor(TRUE)<0);
								SetTimer(vo_window, TIMER_HIDE_CURSOR, auto_hide, NULL);
							}
							if(HIWORD(lParam) < menu_height)
								PopMenuShow = TRUE;
							else if(HIWORD(lParam) >= menu_height)
								PopMenuShow = FALSE;
							if(show_controlbar) {
								if (ControlbarShow && HIWORD(lParam) < vo_screenheight-controlbar_height_fixed+controlbar_offset) {
									ControlbarShow = FALSE;
									ShowContralBar();
								} else
								if (!ControlbarShow && HIWORD(lParam) >= vo_screenheight-controlbar_height_fixed+controlbar_offset) {
									ControlbarShow = TRUE;
									ShowContralBar();
									if(auto_hide_control && skin_controlbar) SetCapture(hWndControlbar);
								}
							}
						} else if ( bMoving == MOVING_WINDOW) {
							if(gui_thread)
								ShowCursor(TRUE);
							RECT r;
							GetWindowRect(hWnd, &r);
							int new_x = r.left+LOWORD(lastp)-old_x;
							int new_y = r.top+HIWORD(lastp)-old_y;
							if(new_x < vo_screenwidth && new_y < vo_screenheight )
								SetWindowPos(hWnd, NULL,	new_x , new_y , 0,0,
									SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
						} else if(!vo_fs && bFullView && auto_hide_control && show_controlbar) {
							int inctrl;
							POINT pt;
							if(gui_thread)
								ShowCursor(TRUE);
							pt.x = LOWORD(lParam);
							pt.y = HIWORD(lParam);
							GetClientRect(vo_window, &r);
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
						} else if(gui_thread){
							ShowCursor(TRUE);
						}
					}
					return 0;
				}
			case WM_NOTIFYICON:
			{
				switch (lParam)
				{
					case WM_LBUTTONDOWN:
						if (WndState == SW_SHOWMINIMIZED) {
							if (vo_paused && systray==2)
								mp_input_queue_cmd(mp_input_parse_cmd("pause"));
							PostMessage(vo_window, WM_SYSCOMMAND, SC_RESTORE, 0);
						}
						SetForegroundWindow(vo_window);
						break;
					case WM_LBUTTONDBLCLK:
						PostMessage(vo_window, WM_COMMAND, IDM_OPEN, 0);
						SetForegroundWindow(vo_window);
						break;
					case WM_RBUTTONDOWN:
					{
						POINT p;
						SetForegroundWindow(vo_window);
						GetCursorPos(&p);
						TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, vo_window, NULL);
						break;
					}
				}
				return 0;
			}
			case WM_WINDOWPOSCHANGING:
				return 0;
			case WM_CLOSE:
				mplayer_put_key(KEY_CLOSE_WIN);
				break;
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
			case WM_COMMAND:
			{
				char *nfilename;
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
					if(id == -1 && filename && !strstr(filename, "://")) {
						initStatus();
						int nid = addStatus(filename);
						if(nid >= 0) {
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
					} else if(id >= 0) {
						initStatus();
						char *file = getNameById(id);
						int time = getStatusById(id);
						freeStatus();
						if(file) {
							snprintf(tmps,DMAX_PATH, "loadfile \"%s\"", file);
							free(file);
							mp_input_queue_cmd(mp_input_parse_cmd(tmps));
							seek2time(time);
						}
					}
					return 0;
				} else if (LOWORD(wParam) >= IDM_SUBTITLE) {
					CheckMenuRadioItem(hSubtitleMenu, IDM_SUBTITLE, IDM_SUBTITLE + 99,
						LOWORD(wParam), MF_BYCOMMAND);
					sprintf(tmps, "sub_select %d", LOWORD(wParam) - IDM_SUBTITLE - 1);
					mp_input_queue_cmd(mp_input_parse_cmd(tmps));
					return 0;
				} else {
					switch (LOWORD(wParam))
					{
						case IDM_OPEN:
						case IDM_ADD: {
							show_cursor(6);
							if(!vo_fs && !bFullView) {
								bFullView = TRUE;
								SetView();
							}
							char *cmdline = NULL;
							BOOL bOpen = LOWORD(wParam)==IDM_OPEN;
							char play_str[64], add_str[64];
							if(sys_Language == 1) {
								LoadString(hInstance, IDS_LISTPLAY_SC, play_str, 60);
								LoadString(hInstance, IDS_LISTADD_SC, add_str, 60);
							} else if(sys_Language == 3 || sys_Language == 4) {
								LoadString(hInstance, IDS_LISTPLAY_TC, play_str, 60);
								LoadString(hInstance, IDS_LISTADD_TC, add_str, 60);
							} else {
								strcpy(play_str,"Open File(s) to Play");
								strcpy(add_str,"Add File(s) to Playlist");
							}
							int i = GetOpenFileW(bOpen? play_str : add_str,&nfilename, bOpen?&cmdline:NULL);
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
							if(ShowFavDlg(hInstance,vo_window))
								ReflashFavorite();
							break;
						}
						case IDM_PLAYLIST:{
							POINT pt;
							HWND parent;
							HWND layer = HWND_NOTOPMOST;
							show_cursor(6);
							if(!vo_fs) {
								GetWindowRect(vo_window, &r);
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
							if(vo_fs || vo_ontop)
								layer = HWND_TOPMOST;
							display_playlist(pt.x,pt.y,hWndControlbar?hWndControlbar:hWnd,layer);
							break;
						}
						case IDM_OPENDVD:
						{
							if(!GetDVDDriver(hWnd))
								break;
							close_playlistwindow();
#ifdef CONFIG_DVDNAV
							if(!no_dvdnav) {
								mp_parse_commandline("-nocache");
								mp_input_queue_cmd(mp_input_parse_cmd("loadfile dvdnav://"));
							} else
#endif
							  mp_input_queue_cmd(mp_input_parse_cmd("loadfile dvd://"));
							break;
						}
						case IDM_OPENBR:
						{
							if(!GeBlurayDriver(hWnd))
								break;
							close_playlistwindow();
							mp_input_queue_cmd(mp_input_parse_cmd("loadfile br://"));
							break;
						}
						case IDM_OPENCD:
						{
							close_playlistwindow();
							if(GetPlayCD(hInstance, hWnd))
								mp_input_queue_cmd(mp_input_parse_cmd("loadfile cdda://"));
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
								if(play_finished && last_filename) {
									DWORD lfa = GetFileAttributes(last_filename);
									if(lfa == 0xFFFFFFFF || (lfa & FILE_ATTRIBUTE_DIRECTORY))
										break;
									snprintf(tmps, DMAX_PATH + 20 , "loadfile \"%s\"", last_filename);
									mp_input_queue_cmd(mp_input_parse_cmd(tmps));
									vo_paused_save = !vo_paused_save;
									if(show_controlbar || g_pTaskbarList)
										SetTimer(vo_window, TIMER_PLAY_PAUSE, 200, NULL);
								} else
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
						case IDM_KEEP_ASPECT:
							vo_keepaspect = !vo_keepaspect;
							CheckMenuItem(hWinMenu, IDM_KEEP_ASPECT, vo_keepaspect?MF_CHECKED:MF_UNCHECKED);
							if(vo_keepaspect && !vo_fs && filename) {
								HWND layer = HWND_NOTOPMOST;
								if (vo_fs || vo_ontop) layer = HWND_TOPMOST;
								vo_dheight  = (int)((float)vo_dwidth / window_aspect);
								SetWindowPos(vo_window, layer, 0, 0,vo_dwidth+xborder, vo_dheight+yborder,SWP_NOMOVE|SWP_SHOWWINDOW);
								if(vo_w32_window != vo_window)
									MoveWindow(vo_w32_window,  0, 0, vo_dwidth, vo_dheight, filename?FALSE:TRUE);
								if(show_controlbar && hWndControlbar) {
									POINT pt;
									pt.x = 0;
									pt.y = vo_dheight;
									if(auto_hide_control)
										pt.y -= controlbar_height_fixed;
									ClientToScreen(vo_window,&pt);
									MoveWindow(hWndControlbar,pt.x,pt.y,vo_dwidth,controlbar_height_fixed,TRUE);
									MoveContralBarItem(vo_dwidth);
									InvalidateRect(hWndControlbar, NULL, FALSE);
								}
								(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
							}
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
							CheckMenuRadioItem(hWinMenu, IDM_ASPECT_FILE, IDM_ASPECT_169, LOWORD(wParam), MF_BYCOMMAND);
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
						case IDM_MOUSE_DVDNAV:
							mouse_dvdnav = !mouse_dvdnav;
							CheckMenuItem(hWinMenu, IDM_MOUSE_DVDNAV, mouse_dvdnav?MF_CHECKED:MF_UNCHECKED);
							break;
						case IDM_SHUTDOWN:
							auto_shutdown = !auto_shutdown;
							CheckMenuItem(hWinMenu, IDM_SHUTDOWN, auto_shutdown?MF_CHECKED:MF_UNCHECKED);
							break;
						case IDM_FIXED_SIZE:
							break;
						case IDM_SCREENSAVER:
							disable_screensaver = !disable_screensaver;
							disable_screen_saver(disable_screensaver);
							CheckMenuItem(hWinMenu, IDM_SCREENSAVER, disable_screensaver?MF_CHECKED:MF_UNCHECKED);
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
							MessageBox(hWnd, get_help_text(), title_w32, MB_OK | MB_ICONEXCLAMATION | (vo_ontop?MB_TOPMOST:0));
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
							if (GetFileAttributes(nfilename) != 0xFFFFFFFF) {
								ShellExecute(0,NULL , nfilename, "--open-ontop", NULL, SW_SHOW);
								free(nfilename);
								break;
							} else
								free(nfilename);
						case IDM_INPUT_CONF:
							show_cursor(6);
							nfilename = get_path("meditor.exe");
							if (GetFileAttributes(nfilename) != 0xFFFFFFFF) {
								ShellExecute(0,NULL , nfilename, "--open-hotkey", NULL, SW_SHOW);
								free(nfilename);
								break;
							} else
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
				}
				return 0;
			}
			case WM_SYSCHAR:
				{
					if (wParam == 13) {
						if(gui_thread)
							mp_input_queue_cmd(mp_input_parse_cmd("vo_fullscreen"));
						else
							vo_w32_fullscreen();
					} else
					if (wParam>0x30 && wParam<0x34 && !vo_fs && filename) {
						static float scale = 1.0;
						if (wParam == 0x32) {
							scale = 1.0;
						} else if (wParam == 0x31 && scale > 0.25) {
							scale -= 0.25;
						} else if (wParam == 0x33 && scale < 3.0) {
							scale += 0.25;
						}
						ShowWindow(vo_window, SW_NORMAL);
						WndState = SW_NORMAL;
						SetWindowPos(vo_window, NULL, 0, 0,
							o_dwidth*scale+xborder,
							o_dheight*scale+yborder, SWP_NOMOVE);
						if(vo_w32_window != vo_window) {
							GetClientRect(vo_window, &r);
							int width = r.right;
							int height = r.bottom;
							if(!vo_fs && bFullView && !(gui_skin.enable_appskin && gui_skin.border_ok))
								height -= controlbar_height;
							MoveWindow(vo_w32_window,  0, 0, width, height, filename?FALSE:TRUE);
						}
						if(gui_thread && !vo_paused)
							mp_input_queue_cmd(mp_input_parse_cmd("vo_gui_resize"));
					} else
					if(systray && (wParam == 's' || wParam == 'S' || wParam == 'h' || wParam == 'H')) {
						if (WndState != SW_SHOWMINIMIZED)
							PostMessage(vo_window, WM_SYSCOMMAND, SC_MINIMIZE, 0);
					} else
						break;
					return 0;
				}
			case WM_SYSCOMMAND:
				{
					WINDOWPLACEMENT wp;
					switch (wParam) {
					case SC_SCREENSAVE:
					case SC_MONITORPOWER:
						if(!disable_screensaver)
							break;
						mp_msg(MSGT_VO, MSGL_V, "vo: win32: killing screensaver\n");
						return 0;
					case SC_MINIMIZE:
						if (systray && hWndP_W32) {
							ShowWindow(hWnd, SW_MINIMIZE);
							ShowWindow(hWnd, SW_HIDE);
							if (!vo_paused && systray==2)
								mp_input_queue_cmd(mp_input_parse_cmd("pause"));
						}
						break;
					}
					GetWindowPlacement(hWnd, &wp);
					WndState = wp.showCmd;
					break;
				}
			case WM_KEYDOWN:
				{
					if ((vo_fs ) &&( (wParam >= VK_NUMPAD1) && (wParam <= VK_DECIMAL) || (wParam > 0x30 && wParam <= 0x39)) ) {
						if (GetKeyState(VK_LCONTROL) < 0) {
							switch (wParam) {
								case 0x31:
								case VK_NUMPAD1 : w32_delta_x -= 4; w32_delta_y += 4; break;
								case 0x32:
								case VK_NUMPAD2 : w32_delta_y += 4; break;
								case 0x33:
								case VK_NUMPAD3 : w32_delta_x += 4; w32_delta_y += 4; break;
								case 0x34:
								case VK_NUMPAD4 : w32_delta_x -= 4; break;
								case 0x35:
								case VK_NUMPAD5 : w32_delta_x = w32_delta_y = 0; break;
								case 0x36:
								case VK_NUMPAD6 : w32_delta_x += 4; break;
								case 0x37:
								case VK_NUMPAD7 : w32_delta_x -= 4; w32_delta_y -= 4; break;
								case 0x38:
								case VK_NUMPAD8 : w32_delta_y -= 4; break;
								case 0x39:
								case VK_NUMPAD9 : w32_delta_x += 4; w32_delta_y -= 4; break;
								default:
									break;
							}
							if(using_d3d && vo_w32_window != vo_window) {
								resize_fs_d3d();
							} else if(gui_thread && !vo_paused) {
								mp_input_queue_cmd(mp_input_parse_cmd("vo_gui_resize"));
							} else {
								resize_fs_gl();
								resize_fs_gl2();
							}
							break;
						} else if (GetKeyState(VK_RCONTROL) < 0) {
							switch (wParam) {
								case 0x31:
								case VK_NUMPAD1 : w32_factor_x = (1 + w32_factor_x) * 0.98 - 1; w32_factor_y = (1 + w32_factor_y) * 0.98 - 1; break;
								case 0x32:
								case VK_NUMPAD2 : w32_factor_y -= 0.02; break;
								case 0x33:
								case VK_NUMPAD3 : w32_factor_x = 0; w32_factor_y = factors[factor_index = (factor_index+1) % 3]; break;
								case 0x34:
								case VK_NUMPAD4 : w32_factor_x -= 0.02; break;
								case 0x35:
								case VK_NUMPAD5 : w32_factor_x = w32_factor_y = 0;  break;
								case 0x36:
								case VK_NUMPAD6 : w32_factor_x += 0.02; break;
								case 0x37:
								case VK_NUMPAD7 : w32_factor_x = 0; w32_factor_y = factors[factor_index = (factor_index+2) % 3]; break;
								case 0x38:
								case VK_NUMPAD8 : w32_factor_y += 0.02; break;
								case 0x39:
								case VK_NUMPAD9 : w32_factor_x = (1 + w32_factor_x) * 1.02 - 1; w32_factor_y = (1 + w32_factor_y) * 1.02 - 1; break;
							}
							if(using_d3d && vo_w32_window != vo_window) {
								resize_fs_d3d();
							} else if(gui_thread && !vo_paused) {
								mp_input_queue_cmd(mp_input_parse_cmd("vo_gui_resize"));
							} else {
								resize_fs_gl();
								resize_fs_gl2();
							}
							break;
						}
					}
#ifdef CONFIG_DVDNAV
					else if(GetKeyState(VK_CONTROL) < 0) {
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
					mpkey = lookup_keymap_table(vk_map, wParam);
		            if (mpkey)
		                mplayer_put_key(mpkey);
					break;
				}
			case WM_CHAR:
				mplayer_put_key(wParam);
				break;
			case WM_LBUTTONDOWN:
				{
					if(!no_dvdnav && mouse_dvdnav && filename && is_dvdnav) {
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
					if (!bMoving) {
						old_x = LOWORD(lParam);
						old_y = HIWORD(lParam);
						if (!vo_fs && WndState != SW_SHOWMAXIMIZED) {
							bMoving = MOVING_WINDOW;
							ReleaseCapture();
							SetCapture(hWnd);
						}
					}
					if (w32_inited && !filename && vo_w32_window != vo_window)
						break;
					start_lbutton_down = GetTickCount();
					break;
				}
			case WM_LBUTTONUP:
				{
					bMoving = MOVING_NONE;
					if(seek_dropping || volume_dropping) {
						PostMessage(hWndControlbar, WM_LBUTTONUP, wParam, lParam);
						break;
					}
					ReleaseCapture();
					if(vo_nomouse_input || !w32_inited)
						break;
					if (GetTickCount() - start_lbutton_down < 500)
						mplayer_put_key(MOUSE_BTN0);
					if(GetTickCount() - start_lbutton_dbcheck < 500) {
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
					if(use_rightmenu) {
						show_cursor(6);
						POINT p;
						GetCursorPos(&p);
						TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, 0, hWnd, NULL);
						break;
					}
					if(vo_fs && PopMenuShow) {
						POINT p;
						GetCursorPos(&p);
						if(p.y > menu_height)
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
					if (!w32_inited || !filename)
						PostMessage(vo_window, WM_COMMAND, IDM_OPEN, 0);
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
					bWheel = TRUE;
					if (vo_nomouse_input)
						break;
					int x = GET_WHEEL_DELTA_WPARAM(wParam);
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
			case WM_WINDOWPOSCHANGED:
			{
				break;
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
						if(t == 0xFFFFFFFF) {
							DragQueryFileW((HDROP)wParam, 0, tmpw, DMAX_PATH-1);
							GetShortPathNameW(tmpw,tmpshot,DMAX_PATH);
							wcstombs(tmps,tmpshot,DMAX_PATH);
						} else if (t & 0x10) { // is a Directory? 
							directory_to_playlist(tmps);
							if (GetKeyState(VK_CONTROL) < 0)
								sprintf(tmps, "addlist \"%s\"", mplayer_pls);
							else
								sprintf(tmps, "loadlist \"%s\"", mplayer_pls);
							if(!vo_fs && !bFullView) {
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
						if (tmp_fname_ext = strrchr(tmps,'.')) {
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
						if(!vo_fs && !bFullView) {
							bFullView = TRUE;
							SetView();
						}
					} else {
						FILE * fp = fopen(mplayer_pls, "w");
						while (i<j) {
							DragQueryFileA((HDROP)wParam, i++, tmps, DMAX_PATH);
							if(GetFileAttributes(tmps) == 0xFFFFFFFF) {
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
						if(!vo_fs && !bFullView) {
							bFullView = TRUE;
							SetView();
						}
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
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/**
 * \brief Dispatch incoming window events and handle them.
 *
 * This function should be placed inside libvo's function "check_events".
 *
 * Global libvo variables changed:
 * vo_dwidth:  new window client area width
 * vo_dheight: new window client area height
 *
 * \return int with these flags possibly set, take care to handle in the right order
 *         if it matters in your driver:
 *
 * VO_EVENT_RESIZE = The window was resized. If necessary reinit your
 *                   driver render context accordingly.
 * VO_EVENT_EXPOSE = The window was exposed. Call e.g. flip_frame() to redraw
 *                   the window if the movie is paused.
 */
int vo_w32_check_events(void) {
	MSG msg;
	event_flags = 0;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTableW32, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if (WinID >= 0) {
        BOOL res;
        RECT r;
        POINT p;
        res = GetClientRect(vo_window, &r);
        if (res && (r.right != vo_dwidth || r.bottom != vo_dheight)) {
            vo_dwidth = r.right; vo_dheight = r.bottom;
            event_flags |= VO_EVENT_RESIZE;
        }
        p.x = 0; p.y = 0;
        ClientToScreen(vo_window, &p);
        if (p.x != vo_dx || p.y != vo_dy) {
            vo_dx = p.x; vo_dy = p.y;
            event_flags |= VO_EVENT_MOVE;
        }
        res = GetClientRect(WinID, &r);
        if (res && (r.right != vo_dwidth || r.bottom != vo_dheight))
            MoveWindow(vo_window, 0, 0, r.right, r.bottom, FALSE);
        if (!IsWindow(WinID))
            // Window has probably been closed, e.g. due to program crash
            mplayer_put_key(KEY_CLOSE_WIN);
    }

    return event_flags;
}

static BOOL CALLBACK mon_enum(HMONITOR hmon, HDC hdc, LPRECT r, LPARAM p) {
    // this defaults to the last screen if specified number does not exist
    xinerama_x = r->left;
    xinerama_y = r->top;
    vo_screenwidth = r->right - r->left;
    vo_screenheight = r->bottom - r->top;
    if (mon_cnt == xinerama_screen)
        return FALSE;
    mon_cnt++;
    return TRUE;
}

/**
 * \brief Update screen information.
 *
 * This function should be called in libvo's "control" callback
 * with parameter VOCTRL_UPDATE_SCREENINFO.
 * Note that this also enables the new API where geometry and aspect
 * calculations are done in video_out.c:config_video_out
 *
 * Global libvo variables changed:
 * xinerama_x
 * xinerama_y
 * vo_screenwidth
 * vo_screenheight
 */
void w32_update_xinerama_info(void) {
    xinerama_x = xinerama_y = 0;
    if (xinerama_screen < -1) {
        int tmp;
        xinerama_x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        xinerama_y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        tmp = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        if (tmp) vo_screenwidth = tmp;
        tmp = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if (tmp) vo_screenheight = tmp;
    } else if (xinerama_screen == -1 && myMonitorFromWindow && myGetMonitorInfo) {
        MONITORINFO mi;
        HMONITOR m = myMonitorFromWindow(vo_window, MONITOR_DEFAULTTOPRIMARY);
        mi.cbSize = sizeof(mi);
        myGetMonitorInfo(m, &mi);
        xinerama_x = mi.rcMonitor.left;
        xinerama_y = mi.rcMonitor.top;
        vo_screenwidth = mi.rcMonitor.right - mi.rcMonitor.left;
        vo_screenheight = mi.rcMonitor.bottom - mi.rcMonitor.top;
    } else if (xinerama_screen > 0 && myEnumDisplayMonitors) {
        mon_cnt = 0;
        myEnumDisplayMonitors(NULL, NULL, mon_enum, 0);
    }
    aspect_save_screenres(vo_screenwidth, vo_screenheight);
}

static void updateScreenProperties(void) {
    DEVMODE dm;
    dm.dmSize = sizeof dm;
    dm.dmDriverExtra = 0;
    dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    if (!EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm)) {
        mp_msg(MSGT_VO, MSGL_ERR, "vo: win32: unable to enumerate display settings!\n");
        return;
    }

    vo_screenwidth = dm.dmPelsWidth;
    vo_screenheight = dm.dmPelsHeight;
    vo_depthonscreen = dm.dmBitsPerPel;
    w32_update_xinerama_info();
}

static void changeMode(void) {
    DEVMODE dm;
    dm.dmSize = sizeof dm;
    dm.dmDriverExtra = 0;

    dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    dm.dmBitsPerPel = vo_depthonscreen;
    dm.dmPelsWidth = vo_screenwidth;
    dm.dmPelsHeight = vo_screenheight;

    if (vo_vm) {
        int bestMode = -1;
        int bestScore = INT_MAX;
        int i;
        for (i = 0; EnumDisplaySettings(0, i, &dm); ++i) {
            int score = (dm.dmPelsWidth - o_dwidth) * (dm.dmPelsHeight - o_dheight);
            if (dm.dmBitsPerPel != vo_depthonscreen) continue;
            if (dm.dmPelsWidth < o_dwidth) continue;
            if (dm.dmPelsHeight < o_dheight) continue;

            if (score < bestScore) {
                bestScore = score;
                bestMode = i;
            }
        }

        if (bestMode != -1)
            EnumDisplaySettings(0, bestMode, &dm);

        ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
    }
}

static void resetMode(void) {
    if (vo_vm)
    ChangeDisplaySettings(0, 0);
}
extern void setGlWindow_thread();

static int createRenderingContext(void) {
    HWND layer = HWND_NOTOPMOST;
    RECT r;
    int style = (vo_border && !vo_fs) ?
                (WS_OVERLAPPEDWINDOW | WS_SIZEBOX) : WS_POPUP;

    if (WinID >= 0)
        return 1;

	int maxmize = (WndState == SW_SHOWMAXIMIZED) ? 1 : 0;
	int minimize = (WndState == SW_SHOWMINIMIZED) ? 1 : 0;

	if (vo_fs || vo_ontop) layer = HWND_TOPMOST;
	if (vo_fs) {
		changeMode();
		FullHide = !auto_hide;
		while (ShowCursor(FALSE)>=0);
	} else {
		KillTimer(vo_window, TIMER_HIDE_CURSOR);
		resetMode();
		FullHide = FALSE;
		while (ShowCursor(TRUE)<0);
	}
	updateScreenProperties();
	ShowWindow(vo_window, SW_HIDE);
	if(show_controlbar)
		ShowWindow(hWndControlbar, SW_HIDE);
	SetWindowLong(vo_window, GWL_STYLE, style);
	if (vo_fs) {
		prev_width = save_width;
		prev_height = save_height;
		prev_x = vo_dx;
		prev_y = vo_dy;
		vo_dwidth = vo_screenwidth;
		vo_dheight = vo_screenheight;
		vo_dx = xinerama_x;
		vo_dy = xinerama_y;

		need_adjust = 1;
		if(show_controlbar && hWndControlbar) {
			SetWindowPos(hWndControlbar,HWND_TOPMOST, xinerama_x,
				xinerama_y+vo_screenheight-controlbar_height_fixed+controlbar_offset,
				vo_screenwidth ,controlbar_height_fixed,SWP_SHOWWINDOW);
			MoveContralBarItem(vo_screenwidth);
			InvalidateRect(hWndControlbar, NULL, FALSE);
		}
	} else {
		// make sure there are no "stale" resize events
		// that would set vo_d* to wrong values
		if(fs_start == 1) {
			fs_start = 0;
			prev_width = o_dwidth;
			prev_height = o_dheight;
			if(prev_width >  vo_screenwidth) {
				prev_width = vo_screenwidth;
				prev_height = (int)((float)prev_width / window_aspect);
			}
			if(prev_height > vo_screenheight) {
				prev_height = vo_screenheight;
				prev_width = (int)((float)prev_height * window_aspect);
			}
			prev_x = (vo_screenwidth - prev_width) / 2;
			prev_y = (vo_screenheight - prev_height) / 2;
		}
		vo_w32_check_events();
		vo_dwidth = prev_width;
		vo_dheight = prev_height;
		vo_dx = prev_x;
		vo_dy = prev_y;
		if(vo_keepaspect && vo_dheight > (int)((float)vo_dwidth / window_aspect)+1)
			vo_dheight  = (int)((float)vo_dwidth / window_aspect);
		// HACK around what probably is a windows focus bug:
		// when pressing 'f' on the console, then 'f' again to
		// return to windowed mode, any input into the video
		// window is lost forever.
	}
	r.left = vo_dx;
	r.right = r.left + vo_dwidth;
	r.top = vo_dy;
	r.bottom = r.top + vo_dheight;
	if(show_menubar_w32) {
		if (vo_fs)
			SetMenu(vo_window, NULL);
		else if(!GetMenu(vo_window))
			SetMenu(vo_window, hWinMenu);
		if(GetMenu(vo_window)) {
			menu_height = GetSystemMetrics(SM_CYMENU);
			r.bottom += menu_height;
		}
	}
	if(!vo_fs) {
		int width = r.right - r.left;
		int height = r.bottom - r.top;
		if(gui_skin.enable_appskin && gui_skin.border_ok) {
			r.top -= gui_skin.title.img->height;
			r.left -= gui_skin.border_left.img->width;
			r.right += gui_skin.border_right.img->width;
			r.bottom += gui_skin.border_bottom.img->height;
		}
		if(show_menubar_w32)
			height -= menu_height;
		if(show_controlbar && hWndControlbar) {
			r.bottom += controlbar_height;
			POINT pt;
			pt.x = 0;
			pt.y = height;
			if(auto_hide_control)
				pt.y -= controlbar_height_fixed;
			ClientToScreen(vo_window,&pt);
			MoveWindow(hWndControlbar,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
			MoveContralBarItem(width);
			InvalidateRect(hWndControlbar, NULL, FALSE);
		}
		if ((vo_paused || play_finished) && vo_ontop == 2)
			layer = HWND_NOTOPMOST;
	}
	if(!gui_skin.enable_appskin || !gui_skin.border_ok || vo_fs)
		AdjustWindowRect(&r, style, 0);
	if(!vo_fs && need_adjust && (!gui_skin.enable_appskin || !gui_skin.border_ok)) {
		if(using_theme && !using_aero) {
			int x_add = prev_x - r.left;
			int y_add = prev_y - r.top;
			if(show_menubar_w32)
				y_add -= menu_height;
			if (x_add > 0) {
				r.left += x_add;
				r.right +=x_add;
			}
			if (y_add > 0) {
				r.top += y_add;
				r.bottom +=y_add;
			}
		} else if(show_menubar_w32) {
			r.top -= menu_height;
			r.bottom -=menu_height;
		}
		need_adjust = 0;
	}

	SetWindowPos(vo_window, layer, r.left, r.top, r.right - r.left , r.bottom - r.top , SWP_SHOWWINDOW);
	if(vo_w32_window != vo_window) {
		GetClientRect(vo_window, &r);
		int width = r.right;
		int height = r.bottom;
		if(!vo_fs && bFullView && !(gui_skin.enable_appskin && gui_skin.border_ok))
			height -= controlbar_height;
		MoveWindow(vo_w32_window, 0, 0, width, height, filename?FALSE:TRUE);
	}

	if(show_controlbar) {
		if(vo_fs) {
			if(SetLayeredWindowAttributes && hWndControlbar && (using_d3d || is_vista)) {
				if(controlbar_alpha > 0)
					SetLayeredWindowAttributes(hWndControlbar, RGB(255,0,255), controlbar_alpha, 3);
				else
					SetLayeredWindowAttributes(hWndControlbar, RGB(255,0,255), 255, 1);
			}
			ShowWindow(hWndControlbar, SW_SHOW);
			ControlbarShow = FALSE;
			ShowContralBar();
			if(vo_full_window) {
				if(hMHook)
					SetWindowPos(vo_full_window,HWND_TOPMOST,xinerama_x,xinerama_y, 1, 1,SWP_SHOWWINDOW);
				else
					SetWindowPos(vo_full_window,HWND_TOPMOST,xinerama_x,xinerama_y, vo_screenwidth,
						vo_screenheight-controlbar_height_fixed+controlbar_offset ,SWP_SHOWWINDOW);
				ShowWindow(vo_full_window,SW_SHOW);
			}
		} else {
			if(SetLayeredWindowAttributes && hWndControlbar && (using_d3d || is_vista)) {
				if(auto_hide_control && controlbar_alpha > 0)
					SetLayeredWindowAttributes(hWndControlbar, RGB(255,0,255), controlbar_alpha, 3);
				else
					SetLayeredWindowAttributes(hWndControlbar, RGB(255,0,255), 255, 1);
			}
			if(auto_hide_control) {
				ControlbarShow = FALSE;
				ShowWindow(hWndControlbar, SW_HIDE);
			} else
				ShowWindow(hWndControlbar, SW_SHOW);
			if(vo_full_window) {
				SetWindowPos(vo_full_window,HWND_TOPMOST,0,0,0,0 ,SWP_SHOWWINDOW);
				ShowWindow(vo_full_window,SW_HIDE);
			}
		}
		PopMenuShow = FALSE;
	}

	if(!vo_fs) {
		set_window_rgn(vo_window, &gui_skin, !vo_fs & bFullView, TRUE);
		if(maxmize) ShowWindow(vo_window, SW_SHOWMAXIMIZED);
		if(minimize) {
			ShowWindow(vo_window, SW_MINIMIZE);
			if(systray && hWndP_W32) ShowWindow(vo_window, SW_HIDE);
		}
	}

	(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
	UpdateThumbnailToolbar(vo_window, TRUE);

	return 1;
}

/**
 * \brief Configure and show window on the screen.
 *
 * This function should be called in libvo's "config" callback.
 * It configures a window and shows it on the screen.
 *
 * Global libvo variables changed:
 * vo_fs
 * vo_vm
 *
 * \return 1 - Success, 0 - Failure
 */
int vo_w32_config(uint32_t width, uint32_t height, uint32_t flags) {
    PIXELFORMATDESCRIPTOR pfd;
    int pf;
    HDC vo_hdc = vo_w32_get_dc(vo_w32_window);

    memset(&pfd, 0, sizeof pfd);
    pfd.nSize = sizeof pfd;
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
    if (flags & VOFLAG_STEREO)
        pfd.dwFlags |= PFD_STEREO;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pf = ChoosePixelFormat(vo_hdc, &pfd);
    if (!pf) {
        mp_msg(MSGT_VO, MSGL_ERR, "vo: win32: unable to select a valid pixel format!\n");
        vo_w32_release_dc(vo_w32_window, vo_hdc);
        return 0;
    }

    SetPixelFormat(vo_hdc, pf, &pfd);
    vo_w32_release_dc(vo_w32_window, vo_hdc);

	// store original size for videomode switching
	RECT rd, rt;
	int changed =1;
	float new_aspact;

    // we already have a fully initialized window, so nothing needs to be done
    if (flags & VOFLAG_HIDDEN)
        return 1;

	o_dwidth = width;
	o_dheight = height;

    if (WinID < 0) {
        // the desired size is ignored in wid mode, it always matches the window size.
		window_aspect = (float)width / (float)height;
		new_aspact = (float)save_width / (float)save_height;

		SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
		rt.bottom -= rt.top;
		rt.right -= rt.left;

		if(gui_skin.enable_appskin && gui_skin.border_ok) {
			controlbar_height_gl = 0;
			rt.top += gui_skin.title.img->height;
			xborder0 = xborder = gui_skin.border_left.img->width + gui_skin.border_right.img->width;
			yborder0 = yborder = gui_skin.title.img->height + gui_skin.border_bottom.img->height + controlbar_height;
		} else {
			rt.top += GetSystemMetrics(SM_CYCAPTION);
			controlbar_height_gl = controlbar_height;
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

		if(new_aspact > window_aspect -0.01 && new_aspact < window_aspect + 0.01)
			changed = 0;
		if(!changed && save_width > 0 && save_height > 0){
			prev_width = vo_dwidth = save_width;
			prev_height = vo_dheight = save_height;
		} else {
			save_width = prev_width = vo_dwidth = width;
			save_height = prev_height = vo_dheight = height;
			if(prev_width+xborder >  rt.right) {
				save_width = prev_width = rt.right-xborder;
				save_height = prev_height = (int)((float)prev_width / window_aspect);
				vo_dx = (rt.right - prev_width) / 2 + rt.left;
				vo_dy = (rt.bottom - prev_height - yborder) / 2 + rt.top;
			}
			if(prev_height+yborder > rt.bottom) {
				save_height = prev_height = rt.bottom-yborder;
				save_width = prev_width = (int)((float)prev_height * window_aspect);
				vo_dx = (rt.right - prev_width) / 2 + rt.left;
				vo_dy = (rt.bottom - prev_height - yborder) / 2 + rt.top;
			}
			if(prev_width < min_video_width) {
				save_width = prev_width = min_video_width;
				save_height = prev_height = (int)((float)prev_width / window_aspect);
			}
		}
		if(save_x > -500 && save_y > -500 && startplayer){
			prev_x = vo_dx = save_x;
			prev_y = vo_dy = save_y;
		} else {
			save_x = prev_x = vo_dx;
			save_y = prev_y = vo_dy;
		}
		confige_finished = 1;

		vo_fs = flags & VOFLAG_FULLSCREEN;
		vo_vm = flags & VOFLAG_MODESWITCHING;

		if(last_vo_fs != -12345)
			vo_fs = last_vo_fs;

		if(startplayer <=0) {
			if(vo_fs)
				fs_start = 1;
			else if(gui_thread && use_windowblinds && !is_vista) {
				vo_fs = 1;
				mp_input_queue_cmd(mp_input_parse_cmd("vo_fullscreen"));
			}
		}
		if(g_pTaskbarList && !gui_thread) {
			g_pTaskbarList = NULL;
			need_recreate = 1;
		}
		startplayer = 1;
		if (!vidmode && switch_view == 2)
			SetTimer(vo_window, TIMER_SWITCH_VIEW, 700, NULL);
		else
			KillTimer(vo_window, TIMER_SWITCH_VIEW);
		w32_inited = 1;
		if(!vo_fs && !bFullView) {
			bFullView = TRUE;
			SetView();
		}
	} else {
		vo_fs = flags & VOFLAG_FULLSCREEN;
		vo_vm = flags & VOFLAG_MODESWITCHING;
	}

	return createRenderingContext();
}

/**
 * \brief return the name of the selected device if it is indepedant
 */
static char *get_display_name(void) {
    DISPLAY_DEVICE disp;
    disp.cb = sizeof(disp);
    EnumDisplayDevices(NULL, vo_adapter_num, &disp, 0);
    if (disp.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
        return NULL;
    return disp.DeviceName;
}

static void SetView(void){
	if(vo_fs)
		return;
	RECT rc;

	if (bOldView == bFullView) return;

	if (bFullView) {
		full_view = 1;
		xborder = xborder0;
		yborder = yborder0;
		GetWindowRect(vo_window, &rc);
		if(gui_skin.enable_appskin && gui_skin.border_ok) {
			rc.top -= gui_skin.title.img->height;
			rc.left -= gui_skin.border_left.img->width;
			rc.right += gui_skin.border_right.img->width;
			rc.bottom += gui_skin.border_bottom.img->height;
		} else {
			AdjustWindowRect(&rc, WNDSTYLE, TRUE);
			if(!show_menubar_w32)
				rc.top += GetSystemMetrics(SM_CYMENU);
		}
		SetWindowLong(vo_window, GWL_STYLE, WNDSTYLE|WS_VISIBLE);
		SetWindowPos(vo_window, NULL, rc.left, rc.top, rc.right-rc.left,
			rc.bottom-rc.top+controlbar_height, SWP_NOZORDER|SWP_NOOWNERZORDER);
		if(vo_w32_window != vo_window) {
			GetClientRect(vo_window, &rc);
			SetWindowPos(vo_w32_window, NULL, 0, 0, rc.right-rc.left,
			rc.bottom-rc.top-controlbar_height_gl, SWP_NOZORDER|SWP_NOOWNERZORDER);
		}
		if(show_menubar_w32)
			SetMenu(vo_window, hWinMenu);
		if(auto_hide_control) {
			ControlbarShow = FALSE;
			ShowWindow(hWndControlbar, SW_HIDE);
		} else
			ShowWindow(hWndControlbar, SW_SHOW);
	} else {
		POINT p;
		p.x = p.y = 0;
		full_view = 0;
		ClientToScreen(vo_window, &p);
		xborder = yborder = 0;
		GetClientRect(vo_window,&rc);
		if(show_menubar_w32)
			SetMenu(vo_window, NULL);
		ShowWindow(hWndControlbar, SW_HIDE);
		SetWindowLong(vo_window, GWL_STYLE, WS_POPUP|WS_VISIBLE);
		SetWindowPos(vo_window, NULL, p.x, p.y, rc.right,
			rc.bottom-controlbar_height_gl, SWP_NOZORDER|SWP_NOOWNERZORDER);
		if(vo_w32_window != vo_window) {
			GetClientRect(vo_window, &rc);
			SetWindowPos(vo_w32_window, NULL, 0, 0, rc.right-rc.left,
			rc.bottom-rc.top, SWP_NOZORDER|SWP_NOOWNERZORDER);
		}
	}
	bOldView = bFullView;

	if(is_vista && show_controlbar && using_aero && !vo_paused)
		mp_input_queue_cmd(mp_input_parse_cmd("vo_gui_resize"));

	SetFocus(vo_window);
}

static void InitSubtitleMenu()
{
	char download[16];

	hSubtitleMenu = CreatePopupMenu();
	if(show_menubar)
		InsertMenu(hWinMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSubtitleMenu, subMenu);
	InsertMenu(hPopupMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSubtitleMenu, subMenu);

	if(sys_Language == 1) {
		LoadString(hInstance, IDS_SUB_DOWNLOAD_SC, download, 16);
	} else if(sys_Language == 3 || sys_Language == 4){ 
		LoadString(hInstance, IDS_SUB_DOWNLOAD_TC, download, 16);
	} else {
		sprintf(download, "Do&wnload\tCtrl+W");
	}

	minf.wID = IDM_SUB_DOWNLOAD;
	minf.dwTypeData = download;
	minf.cch = strlen(download);
	InsertMenuItem(hSubtitleMenu, 0xFFFFFFFF, TRUE, &minf);

	subdownloader = get_path("tools/subdownloader.exe");
	if (GetFileAttributes(subdownloader) == 0xFFFFFFFF) {
		EnableMenuItem(hSubtitleMenu, IDM_SUB_DOWNLOAD, MF_GRAYED);
		free(subdownloader);
		subdownloader = NULL;
	}

	minf.wID = IDM_SUBTITLE;
	minf.dwTypeData = subMenuItem;
	minf.cch = strlen(subMenuItem);
	InsertMenuItem(hSubtitleMenu, 0xFFFFFFFF, TRUE, &minf);
	AppendMenu(hSubtitleMenu, MF_SEPARATOR, 0, NULL);
	CheckMenuRadioItem(hSubtitleMenu, IDM_SUBTITLE, minf.wID, IDM_SUBTITLE, MF_BYCOMMAND);
}

static void InitFavoriteMenu()
{
	if(auto_resume <= 0)
		return;
	hFavoriteMenu = CreatePopupMenu();
	if(show_menubar)
		InsertMenu(hWinMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hFavoriteMenu, favMenu);
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
	if(auto_resume > 1) {
		minf2.dwTypeData = favMenuItem;
		minf2.cch = strlen(favMenuItem);
		InsertMenuItem(hFavoriteMenu, IDM_FAVORITES_MANAGE, FALSE, &minf2);
		AppendMenu(hFavoriteMenu, MF_SEPARATOR, 0, NULL);
	}

	if(initStatus()) {
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
	if(show_menubar_w32)
		DrawMenuBar(vo_window);
}

static void InitSkinMenu()
{
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
		LoadString(GetModuleHandle(NULL), IDS_MU_SKINSC_SC, ctlskin, 16);;
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
		InsertMenu(hWinMenu, 3, MF_STRING|MF_BYPOSITION|MF_POPUP, (UINT)hSkinMenu, skins);
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
	DrawMenuBar(vo_window);
}

int InitW32Window(void)
{
	HINSTANCE user32;
	myMonitorFromWindow = NULL;
	myGetMonitorInfo = NULL;
	myEnumDisplayMonitors = NULL;
	SetLayeredWindowAttributes = NULL;
	BITMAP bm;
	char exedir[DMAX_PATH];

	if(vo_dirver == VO_DIRV_DIRECT3D) {
		using_d3d = TRUE;
		strncpy(title_w32, title_d3d, 32);
	} else
		strncpy(title_w32, title_gl, 32);
	strcpy(title_text, title_w32);
	show_menubar_w32 = show_menubar;
	windowcolor = vo_colorkey;
	bkgndbrush = CreateSolidBrush(gui_logo.bg_color);
	colorbrush = CreateSolidBrush(windowcolor);
	blackbrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (vo_window)
		return 1;

	hInstance = GetModuleHandle(0);
	if(min_video_width < gui_skin.background_width)
		min_video_width = gui_skin.background_width;

	hAccelTableW32 = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	hLogo = LoadImage(hInstance, MAKEINTRESOURCE(IDI_MPLAYER_LOGO), IMAGE_BITMAP, 0, 0, 0);
	GetObject(hLogo, sizeof(BITMAP), &bm);
	logoW = bm.bmWidth;
	logoH = bm.bmHeight;

	if (GetModuleFileName(0, exedir, DMAX_PATH))
		mplayerIcon = ExtractIcon(hInstance, exedir, 0);
	if (!mplayerIcon)
		mplayerIcon = LoadIcon(0, IDI_APPLICATION);

	WNDCLASSEX wcex = { sizeof wcex, CS_OWNDC | CS_DBLCLKS, WndProc, 0, 0
	, hInstance, mplayerIcon, LoadCursor(0, MAKEINTRESOURCE(0x7F89)), NULL, 0, classname, mplayerIcon };

	if(sys_Language == 1) {
		wcex.lpszMenuName  =  MAKEINTRESOURCE(IDR_MAINMENU);
		LoadString(hInstance, IDS_MENU_SC, subMenu, 10);
		LoadString(hInstance, IDS_MENUITEM_SC, subMenuItem, 10);
		LoadString(hInstance, IDS_FAVMENU_SC, favMenu, 20);
		LoadString(hInstance, IDS_FAVMENUITEM_SC, favMenuItem, 30);
		LoadString(hInstance, IDS_FAVMENUMANAGE_SC, favMenuManage, 30);
	} else if(sys_Language == 3 || sys_Language == 4) {
		wcex.lpszMenuName  =  MAKEINTRESOURCE(IDR_MAINMENU_TC);
		LoadString(hInstance, IDS_MENU_TC, subMenu, 10);
		LoadString(hInstance, IDS_MENUITEM_TC, subMenuItem, 10);
		LoadString(hInstance, IDS_FAVMENU_TC, favMenu, 20);
		LoadString(hInstance, IDS_FAVMENUITEM_TC, favMenuItem, 30);
		LoadString(hInstance, IDS_FAVMENUMANAGE_TC, favMenuManage, 30);
	} else {
		wcex.lpszMenuName  =  MAKEINTRESOURCE(IDR_MAINMENU_EN);
		LoadString(hInstance, IDS_MENU_EN, subMenu, 10);
		LoadString(hInstance, IDS_MENUITEM_EN, subMenuItem, 10);
		LoadString(hInstance, IDS_FAVMENU_EN, favMenu, 20);
		LoadString(hInstance, IDS_FAVMENUITEM_EN, favMenuItem, 30);
		LoadString(hInstance, IDS_FAVMENUMANAGE_EN, favMenuManage, 30);
	}

	if (!RegisterClassEx(&wcex)) {
		mp_msg(MSGT_VO, MSGL_ERR, "vo: win32: unable to register window class!\n");
		return 0;
	}

	if (WinID >= 0)
		vo_w32_window = vo_window = WinID;
	else {
		if (systray)
			hWndP_W32 = CreateWindow("Static",NULL,WS_POPUP,0,0,0,0,NULL,NULL,hInstance,NULL);
		RECT monitor_rect;
		monitor_rect.right = GetSystemMetrics(SM_CXSCREEN);
		monitor_rect.bottom = GetSystemMetrics(SM_CYSCREEN);
		vo_window = CreateWindowEx(0, classname, classname,
			vo_border? (WS_OVERLAPPEDWINDOW | WS_SIZEBOX) : WS_POPUP,
			(monitor_rect.right - min_video_width)>>1, (monitor_rect.bottom - min_video_width*3/4)>>1,
					min_video_width, (int)((float)min_video_width/window_aspect),hWndP_W32,NULL,hInstance,NULL);

		if (!vo_window) {
			mp_msg(MSGT_VO, MSGL_ERR, "vo: win32: unable to create window!\n");
			return 0;
		}

		if(loadappskin(&gui_skin, vo_window)) {
			show_menubar_w32 = 0;
			hMHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC) MouseHookProc, hInstance, GetCurrentThreadId());
			HINSTANCE h;
			if(h = LoadLibrary("dwmapi.dll")) {
				DWORD dwAttr = 1;
				DwmAttributeFunct DwmSetWindowAttribute = (DwmAttributeFunct)GetProcAddress(h, "DwmSetWindowAttribute");
				if(DwmSetWindowAttribute) DwmSetWindowAttribute(vo_window, 2, &dwAttr, 4);
				FreeLibrary(h);
			}
			if(h = LoadLibrary("uxtheme.dll")) {
			   SetWindowThemeFunct SetWindowTheme = (SetWindowThemeFunct)GetProcAddress(h, "SetWindowTheme");
			   if(SetWindowTheme) SetWindowTheme(vo_window, L" ",L" ");
			   FreeLibrary(h);
			}
		}

		WNDCLASSEX wcex2 = { sizeof wcex2, CS_OWNDC|CS_DBLCLKS, W32WndProc, 0, 0, hInstance, 0, LoadCursor(0, MAKEINTRESOURCE(0x7F89)), 0, 0, "CLASS_VO_WIN32", 0};
		RegisterClassEx(&wcex2);
		vo_w32_window = CreateWindow("CLASS_VO_WIN32", NULL, WS_CHILD|WS_VISIBLE,0,0,0,0, vo_window, NULL, hInstance, NULL);

		if (!using_d3d && is_vista && using_theme && using_aero && !gl_fs_flash) {
			vo_full_window = CreateWindow("Static",NULL,WS_POPUP ,0,0,0,0, vo_window,NULL,hInstance,NULL);
			SetWindowLong(vo_full_window, GWL_WNDPROC, (LONG)W32FullFocusPrc);
			ShowWindow(vo_full_window,SW_HIDE);
			if(!hMHook) hMHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC) MouseHookProc, hInstance, GetCurrentThreadId());
		}
		if(!vo_w32_window)
			vo_w32_window = vo_window;

		loadlogo(&gui_logo, vo_w32_window);
		DragAcceptFiles(vo_window, TRUE);

		hWinMenu = GetMenu(vo_window);
		EnableMenuItem(hWinMenu, IDM_FIXED_SIZE, MF_BYCOMMAND | MF_DISABLED| MF_GRAYED);
		CheckMenuItem(hWinMenu, IDM_KEEP_ASPECT, vo_keepaspect?MF_CHECKED:MF_UNCHECKED);
		CheckMenuItem(hWinMenu, IDM_SCREENSAVER, disable_screensaver?MF_CHECKED:MF_UNCHECKED);
		CheckMenuRadioItem(hWinMenu, IDM_ASPECT_FILE, IDM_ASPECT_169, IDM_ASPECT_FILE, MF_BYCOMMAND);

		if(show_menubar_w32)
			menu_height = GetSystemMetrics(SM_CYMENU);
		else
			SetMenu(vo_window, NULL);

		auto_hide_control_save = auto_hide_control;
		controlbar_alpha_save = controlbar_alpha;

		if(show_controlbar) {
			status[0] = 0;
			volpercent = save_volume;
			controlbar_offset = skin_controlbar ? 0 : 4;
			if(skin_controlbar) {
				hWndControlbar = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONTROL_SKIN_POP), vo_window, ControlbarProcSkin);
				loadskin(&gui_skin, hWndControlbar);
				controlbar_height_fixed = gui_skin.background_height;
				if(gui_skin.auto_hide) auto_hide_control = 1;
				if(gui_skin.alpha > 0 && gui_skin.alpha < 255)
					controlbar_alpha = gui_skin.alpha;
				if(!auto_hide_control) controlbar_height = controlbar_height_fixed;
				if(gui_skin.enable_appskin && gui_skin.border_ok)
					controlbar_height_gl = 0;
				else
					controlbar_height_gl = controlbar_height;
			} else {
				controlbar_height_fixed = volumebar_height;
				if(!auto_hide_control) controlbar_height = controlbar_height_fixed;
				hWndControlbar = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONTROL_SKIN_POP), vo_window, ControlbarProcNormal);
				hWndSeekbar = GetDlgItem(hWndControlbar, IDC_CTL_SEEK);
				hWndVolumebar = GetDlgItem(hWndControlbar, IDC_CTL_VOLUME);
				PostMessage(hWndSeekbar, TBM_SETRANGEMAX, FALSE, 100);
				PostMessage(hWndVolumebar, TBM_SETRANGEMAX, FALSE, 100);
				PostMessage(hWndVolumebar, TBM_SETPOS, TRUE, volpercent);
				ShowWindow(hWndSeekbar, SW_SHOW);
				ShowWindow(hWndVolumebar, SW_SHOW);
			}
			if(!filename && hWndControlbar) {
				RECT rc;
				POINT pt;
				GetClientRect(vo_window, &rc);
				int width = rc.right;
				int height = rc.bottom - controlbar_height_fixed;
				pt.x = 0;
				pt.y = height;
				ClientToScreen(vo_window,&pt);
				if(auto_hide_control) {
					ControlbarShow = FALSE;
					ShowWindow(hWndControlbar, SW_HIDE);
				} else
					ShowWindow(hWndControlbar, SW_SHOWNOACTIVATE);
				MoveWindow(hWndControlbar,pt.x,pt.y,width,controlbar_height_fixed,TRUE);
				MoveContralBarItem(width);
				InvalidateRect(hWndControlbar, NULL, FALSE);
			}
		}
	}

	user32 = GetModuleHandle("user32.dll");
	if(user32) {
		myMonitorFromWindow = (void *)GetProcAddress(user32, "MonitorFromWindow");
		myGetMonitorInfo = GetProcAddress(user32, "GetMonitorInfoA");
		myEnumDisplayMonitors = GetProcAddress(user32, "EnumDisplayMonitors");
		SetLayeredWindowAttributes = GetProcAddress(user32, "SetLayeredWindowAttributes");
		if(SetLayeredWindowAttributes && vo_full_window) {
			SetWindowLong(vo_full_window, GWL_EXSTYLE, GetWindowLong(vo_full_window ,GWL_EXSTYLE) | WS_EX_LAYERED );
			SetLayeredWindowAttributes(vo_full_window, windowcolor, 1, 2);
		}
		if(SetLayeredWindowAttributes && hWndControlbar && (using_d3d || is_vista)) {
			SetWindowLong(hWndControlbar, GWL_EXSTYLE, GetWindowLong(hWndControlbar ,GWL_EXSTYLE) | WS_EX_LAYERED );
			if(auto_hide_control && controlbar_alpha > 0)
				SetLayeredWindowAttributes(hWndControlbar, RGB(255,0,255), controlbar_alpha, 3);
			SetLayeredWindowAttributes(hWndControlbar, RGB(255,0,255), 255, 1);
		}
		if(is_win7 && using_d3d) ChangeWindowMessageFilter = GetProcAddress(user32, "ChangeWindowMessageFilter");
		if(is_win7 && using_d3d && ChangeWindowMessageFilter) {
			s_uTBBC = RegisterWindowMessage("TaskbarButtonCreated");
			ChangeWindowMessageFilter(s_uTBBC, MSGFLT_ADD);
			ChangeWindowMessageFilter(WM_COMMAND, MSGFLT_ADD);
		}
	}
	initwin_finished = 1;

	memset(&minf, 0, sizeof(minf));
	minf.cbSize = sizeof(minf);
	minf.fMask = MIIM_ID | MIIM_TYPE;
	minf.fType = MFT_STRING | MFT_RADIOCHECK;
	char file_str[32] , option_str[32] , play_str[32] , help_str[32];
	if(sys_Language == 1) {
		LoadString(hInstance, IDS_FILE_SC, file_str, 32);
		LoadString(hInstance, IDS_OPTION_SC, option_str, 32);
		LoadString(hInstance, IDS_PLAY_SC, play_str, 32);
		LoadString(hInstance, IDS_HELP_SC, help_str, 32);
	} else if(sys_Language == 3 || sys_Language == 4) {
		LoadString(hInstance, IDS_FILE_TC, file_str, 32);
		LoadString(hInstance, IDS_OPTION_TC, option_str, 32);
		LoadString(hInstance, IDS_PLAY_TC, play_str, 32);
		LoadString(hInstance, IDS_HELP_TC, help_str, 32);
	} else {
		strcpy(file_str,"&File");
		strcpy(option_str,"&Option");
		strcpy(play_str,"&Play");
		strcpy(help_str,"&Help");
	}
	hPopupMenu = CreatePopupMenu();
	AppendMenu(hPopupMenu,  MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hWinMenu, 0),file_str);
	AppendMenu(hPopupMenu,MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hWinMenu, 1),option_str);
	AppendMenu(hPopupMenu,  MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hWinMenu, 2),play_str);
	AppendMenu(hPopupMenu,  MF_POPUP|MF_STRING,
		(UINT)GetSubMenu(hWinMenu, 3),help_str);
	InitSkinMenu();
	InitFavoriteMenu();
	InitSubtitleMenu();

	if(gui_skin.enable_appskin && gui_skin.border_ok) {
		xborder = gui_skin.border_left.img->width + gui_skin.border_right.img->width;
		yborder = gui_skin.title.img->height + gui_skin.border_bottom.img->height + controlbar_height;
	} else {
		RECT rd;
		memset(&rd, 0, sizeof(rd));
		AdjustWindowRect(&rd,WNDSTYLE,TRUE);
		xborder = rd.right - rd.left;
		yborder = rd.bottom - rd.top;
		if(!show_menubar_w32)
			yborder -= GetSystemMetrics(SM_CYMENU);
		yborder += controlbar_height;
	}

	if(!filename) {
		window_aspect = (float)(min_video_width-xborder) / (float)(min_video_width/window_aspect - yborder);
		updateScreenProperties();
	}

	if (hWndP_W32) {
		WM_TASKBARCREATED_W32 = RegisterWindowMessage("TaskbarCreated");
		nid_w32.cbSize = sizeof(nid_w32);
		nid_w32.hWnd = vo_window;
		nid_w32.uID = 1973;
		nid_w32.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
		nid_w32.hIcon = mplayerIcon;
		nid_w32.uCallbackMessage = WM_NOTIFYICON;
		strcpy(nid_w32.szTip, title_w32);
		Shell_NotifyIcon(NIM_ADD, &nid_w32);
	}

}

/**
 * \brief Initialize w32_common framework.
 *
 * The first function that should be called from the w32_common framework.
 * It handles window creation on the screen with proper title and attributes.
 * It also initializes the framework's internal variables. The function should
 * be called after your own preinit initialization and you shouldn't do any
 * window management on your own.
 *
 * Global libvo variables changed:
 * vo_w32_window
 * vo_depthonscreen
 * vo_screenwidth
 * vo_screenheight
 *
 * \return 1 = Success, 0 = Failure
 */
int vo_w32_init(void) {
    char *dev;
	int i;

	while(!initwin_finished) {
		if(i > 2000) break;
		Sleep(30);
		++i;
	}

    dev_hdc = 0;
    dev = get_display_name();
    if (dev) dev_hdc = CreateDC(dev, NULL, NULL, NULL);
    free(dev);
    updateScreenProperties();

    mp_msg(MSGT_VO, MSGL_V, "vo: win32: running at %dx%d with depth %d\n", vo_screenwidth, vo_screenheight, vo_depthonscreen);

    return 1;
}

/**
 * \brief Toogle fullscreen / windowed mode.
 *
 * Should be called on VOCTRL_FULLSCREEN event. The window is
 * always resized after this call, so the rendering context
 * should be reinitialized with the new dimensions.
 * It is unspecified if vo_check_events will create a resize
 * event in addition or not.
 *
 * Global libvo variables changed:
 * vo_dwidth
 * vo_dheight
 * vo_fs
 */
void vo_w32_fullscreen(void) {
	if(!filename && !vo_fs)
		return;

	if(!vo_fs && !bFullView) {
		bFullView = TRUE;
		SetView();
	}
	vo_fs = !vo_fs;

	createRenderingContext();
	last_vo_fs = vo_fs;

	if(vo_fs) {
		if(gui_thread)
			CreateThread(NULL, 0, osdThreadProc, NULL, 0, 0);
		else
		    PostMessage(vo_window, UPDATE_VOSUB, 0, 0);			
	}
}

/**
 * \brief Toogle window border attribute.
 *
 * Should be called on VOCTRL_BORDER event.
 *
 * Global libvo variables changed:
 * vo_border
 */
void vo_w32_border(void) {
	vo_border = !vo_border;
	createRenderingContext();
}

/**
 * \brief Toogle window ontop attribute.
 *
 * Should be called on VOCTRL_ONTOP event.
 *
 * Global libvo variables changed:
 * vo_ontop
 */
void vo_w32_ontop( void ) {
	vo_ontop = (vo_ontop + 1) % 3;
	if (!vo_fs) {
		HWND layer = HWND_NOTOPMOST;
		if (vo_ontop) layer = HWND_TOPMOST;
		SetWindowPos(vo_window, layer, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}

/**
 * \brief Uninitialize w32_common framework.
 *
 * Should be called last in video driver's uninit function. First release
 * anything built on top of the created window e.g. rendering context inside
 * and call vo_w32_uninit at the end.
 */
void vo_w32_uninit(void) {
	mp_msg(MSGT_VO, MSGL_V, "vo: win32: uninit\n");
	resetMode();
	while (ShowCursor(TRUE)<0);
	vo_depthonscreen = 0;
    if (dev_hdc) DeleteDC(dev_hdc);
    dev_hdc = 0;
	if(vo_w32_window && vo_w32_window != vo_window)
		DestroyWindow(vo_w32_window);
	if (WinID < 0)
		DestroyWindow(vo_window);
	vo_w32_window = 0;
	vo_window = 0;
	UnregisterClass(classname, 0);
	if (g_pTaskbarList) {
		ITaskbarList3_Release(g_pTaskbarList);
		g_pTaskbarList = NULL;
	}
	if (hMHook) {
		UnhookWindowsHookEx(hMHook);
		hMHook = NULL;
	}
	if(hWndControlbar) {
		DestroyWindow(hWndControlbar);
		hWndControlbar = NULL;
	}
	if(vo_full_window) {
		DestroyWindow(vo_full_window);
		vo_full_window = NULL;
	}
	if(hWndP_W32) {
		DestroyWindow(hWndP_W32);
		Shell_NotifyIcon(NIM_DELETE, &nid_w32);
		hWndP_W32 = NULL;
	}
	if (hPopupMenu) {
		DestroyMenu(hPopupMenu);
		hPopupMenu = NULL;
	}
	if (mplayerIcon) {
		DestroyIcon(mplayerIcon);
		mplayerIcon = NULL;
	}
	if (mplayercursor) {
		DestroyCursor(mplayercursor);
		mplayercursor = NULL;
	}
	if (blackbrush) {
		DeleteObject(blackbrush);
		blackbrush = NULL;
	}
	if (colorbrush) {
		DeleteObject(colorbrush);
		colorbrush = NULL;
	}
	if (bkgndbrush) {
		DeleteObject(bkgndbrush);
		bkgndbrush = NULL;
	}
	if (subdownloader) {
		free(subdownloader);
		subdownloader = NULL;
	}
}

/**
 * \brief get a device context to draw in
 *
 * \param wnd window the DC should belong to if it makes sense
 */
HDC vo_w32_get_dc(HWND wnd) {
    if (dev_hdc) return dev_hdc;
    return GetDC(wnd);
}

/**
 * \brief release a device context
 *
 * \param wnd window the DC probably belongs to
 */
void vo_w32_release_dc(HWND wnd, HDC dc) {
    if (dev_hdc) return;
    ReleaseDC(wnd, dc);
}

int w32Cmd(GUI_CMD cmd, int v) {
	int ret = 0;
	switch (cmd)
	{
	case CMD_INIT_WINDOW:
		InitW32Window();
		if (!filename) {
			HWND layerx = HWND_NOTOPMOST;
			if(vo_ontop == 1)
				layerx = HWND_TOPMOST;
			SetWindowPos(vo_window, layerx, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			ShowWindow(vo_window, SW_SHOW);
		} else
			SetForegroundWindow(vo_window);
		break;
	case CMD_UNINIT_WINDOW:
		vo_w32_uninit();
		break;
	case CMD_ACTIVE_WINDOW:
		if(vo_window)
			SetForegroundWindow(vo_window);
		break;
	case CMD_PAUSE_CONTINUE:
		if(!vo_fs && vo_ontop == 2) {
			if(vo_paused)
				SetWindowPos(vo_window, HWND_NOTOPMOST, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			else
				SetWindowPos(vo_window, HWND_TOPMOST, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}
		if(gui_thread && !vo_fs && !vo_paused)
			mp_input_queue_cmd(mp_input_parse_cmd("vo_gui_resize"));
		if (show_controlbar && hWndControlbar)
			InvalidateRect(hWndControlbar, NULL, FALSE);
		break;
	case CMD_SWITCH_ASPECT:
		PostMessage(vo_window, WM_COMMAND, IDM_KEEP_ASPECT, 0);
		break;
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
		if(g_pTaskbarList && vo_window && v >= 0) {
			ITaskbarList3_SetProgressValue(g_pTaskbarList, vo_window, (ULONGLONG)v, (ULONGLONG)percent_value);
		}
		if (show_controlbar && !bSeeking && !seek_dropping && hWndControlbar) {
			seekpercent=v;
			if(!skin_controlbar && hWndSeekbar)
				PostMessage(hWndSeekbar, TBM_SETPOS, TRUE, (int)(seekpercent*100.0/percent_value));
			else
				InvalidateRect(hWndControlbar, NULL, FALSE);
		}
		break;
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
	case CMD_UPDATE_TITLE:
	{
		is_dvdnav = 0;
		play_finished = 0;
		char *basename = get_rar_stream_filename((stream_t *)v);
		if (basename) {
			if(use_filename_title)
			snprintf(title_text,500, "[RAR] %s - %s", basename, title_w32);
		} else if (filename) {
			basename = strrchr(filename, '/');
			if (!basename) basename = _mbsrchr(filename, '\\');
			if (strlen(++basename) < 3) basename = filename;
			if (strstr(basename, "dvd://"))
				snprintf(title_text,500, "[DVD] %s - %s", dvd_device, title_w32);
#ifdef CONFIG_DVDNAV
			else if (strstr(basename, "dvdnav://")) {
				is_dvdnav = 1;
				snprintf(title_text,500, "[DVDNAV] %s - %s", dvd_device, title_w32);
				if(is_dvdnav_menu())
					mouse_dvdnav = 1;
				else
					mouse_dvdnav = 0;
				CheckMenuItem(hWinMenu, IDM_MOUSE_DVDNAV, mouse_dvdnav?MF_CHECKED:MF_UNCHECKED);
			}
#endif
#ifdef CONFIG_LIBBLURAY
			else if (strstr(basename, "br://")) {
				snprintf(title_text,500, "[BluRay] %s - %s", bluray_device, title_w32);
			} else if (strstr(basename, "bd://")) {
				snprintf(title_text,500, "[BluRay] %s - %s", bluray_device, title_w32);
			}
#endif
			else
				snprintf(title_text,500, "%s - %s", basename, title_w32);
			if((show_controlbar || g_pTaskbarList) && vo_window)
				SetTimer(vo_window, TIMER_PLAY_PAUSE, 200, NULL);
		} else {
			strcpy(title_text, title_w32);
			if(show_controlbar) {
				KillTimer(vo_window, TIMER_PLAY_PAUSE);
				play_finished = 1;
				if(!vo_fs && vo_ontop == 2)
					SetWindowPos(vo_window, HWND_NOTOPMOST, 0,0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			}
			if(show_status)
				sprintf(status, "");
			if(show_status2)
				sprintf(status_text_timer2, "");
			UpdateThumbnailToolbar(vo_window, FALSE);
			shutdown_when_necessary(vo_window);
		}
		if(!use_filename_title)
			strcpy(title_text, title_w32);
		if(using_theme || vo_fs || !bFullView || !gui_skin.enable_appskin || !gui_skin.border_ok)
			while(!SetWindowText(vo_window, title_text));
		bIdle = !v;
		seekpercent = 0;
		if(vo_window) InvalidateRect(vo_window, NULL, TRUE);
		if(gui_skin.enable_appskin && gui_skin.border_ok && !vo_fs && bFullView)
			draw_title_text(vo_window, &gui_skin, title_text);
		if(vo_w32_window && vo_w32_window != vo_window)
			InvalidateRect(vo_w32_window, NULL, FALSE);
		if(show_controlbar && hWndControlbar)
			InvalidateRect(hWndControlbar, NULL, FALSE);
		break;
	}
	case CMD_ADD_SUBMENU:
		minf.wID++;
		minf.dwTypeData = (LPSTR)v;
		minf.cch = strlen((LPSTR)v);
		InsertMenuItem(hSubtitleMenu, 0xFFFFFFFF, TRUE, &minf);
		break;
	case CMD_UPDATE_SUBMENU:
		CheckMenuRadioItem(hSubtitleMenu, IDM_SUBTITLE, IDM_SUBTITLE + 99,
			IDM_SUBTITLE + v + 1, MF_BYCOMMAND);
		break;
	case CMD_DEL_SUBMENU:
		if (hSubtitleMenu) {
			if(show_menubar_w32)
				DeleteMenu(hWinMenu, 3, MF_BYPOSITION);
			DeleteMenu(hPopupMenu, 3, MF_BYPOSITION);
			hSubtitleMenu = NULL;
			InitSubtitleMenu();
			if(show_menubar_w32)
				DrawMenuBar(vo_window);
		}
		break;
	case CMD_SWITCH_VIEW:
		if (switch_view == 1 && !vidmode && !vo_fs && WndState != SW_SHOWMAXIMIZED) {
			bFullView = !bFullView;
			SetView();
		}
		break;
	}
	return ret;
}

static BOOL CALLBACK ControlbarProcNormal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
		case WM_PAINT:
			if(vo_fs && !ControlbarShow)
			{
				  RECT rect;
				  PAINTSTRUCT ps;
				  HDC hdc;
				  HBRUSH brush,oldbrush;
				  hdc = BeginPaint(hDlg, &ps); 
				  brush=CreateSolidBrush(RGB(0,0,0));
				  GetClientRect(hDlg,&rect);
				  oldbrush=(HBRUSH)SelectObject(hdc,brush);
				  Rectangle(hdc,rect.left, rect.top, rect.right, rect.bottom);
				  SelectObject(hdc,oldbrush);
				  EndPaint(hDlg, &ps);
				  return FALSE;
			}
			break;
		case WM_HSCROLL:
		{
			char tmps[20];
			POINT p;
			float x;
			(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
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
				on_paint(&gui_skin, hdc, hDlg, bkgndbrush, r, vo_paused || play_finished);
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
			(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);

			if(auto_hide_control && !(p.x > 0 && p.x < r.right && p.y > 0 && p.y < r.bottom)) {
				ReleaseCapture();
				ControlbarShow = FALSE;
				on_mouse_leave(&gui_skin);
				if(vo_fs)
					ShowContralBar();
				else
					ShowWindow(hDlg, SW_HIDE);
				break;
			}

			if (vo_fs && auto_hide) {
				KillTimer(vo_window, TIMER_HIDE_CURSOR);
				while (ShowCursor(TRUE)<0);
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

			if(on_lbutton_down(&gui_skin, hDlg, r, p, vo_window))
				InvalidateRect(hDlg, NULL, FALSE);

			if(auto_hide_control) SetCapture(hDlg);
			(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
			return FALSE;
		}
	case WM_LBUTTONUP:
		{
			RECT r;
			POINT p;
			p.x = LOWORD(lParam);
			p.y = HIWORD(lParam);
			GetClientRect(hDlg, &r);

			if(on_lbutton_up(&gui_skin, hDlg, r, p, vo_window))
				InvalidateRect(hDlg, NULL, FALSE);

			if(auto_hide_control) SetCapture(hDlg);
			(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
			return FALSE;
		}
	case WM_SYSCHAR:
	case WM_SYSCOMMAND:
	case WM_KEYDOWN:
	case WM_MOUSEWHEEL:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
		PostMessage(vo_window, message, wParam, lParam);
		break;
	case WM_COMMAND:
		(vo_full_window && vo_fs) ? SetFocus(vo_full_window) : SetFocus(vo_window);
		break;
	}

    return FALSE;
}

