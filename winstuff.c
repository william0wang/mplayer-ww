#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <ctype.h>
#include <commctrl.h>
#include <locale.h>
#include <d3d9.h>
#include <shlobj.h>
#include "m_config.h"
#include "mp_msg.h"
#include "winstuff.h"
#include "resource.h"
#include "unrar.h"
#include "version.h"
#include "skin/skin.h"
#include "input/input.h"
#include "libavutil/common.h"
#include "libiniparser/iniparser.h"
#include "osdep/priority.h"

#define MAXSAVESTATS 200

#ifndef BIF_NONEWFOLDERBUTTON
#define BIF_NONEWFOLDERBUTTON 0x200
#endif

#define MEDIA_FILES "Media Files\0*.avi;*.as*;*.mp*;*.dat;*.m*v;*.m*a;*.n*v;*.qt;*.rm*;*.ra*;*.ts;*.wm*;*.mk*;*.pmp;*.og*;*.vp*;*.vivo;*.vob;*.pls;*.m3u;*.img;*.iso;*.bin;*.3gp;*.flv;*.ape;*.amr;*.aac;*.tp;\0Any File\0*.*\0"
#define LMEDIA_FILES L"Media Files\0*.avi;*.as*;*.mp*;*.dat;*.m*v;*.m*a;*.n*v;*.qt;*.rm*;*.ra*;*.ts;*.wm*;*.mk*;*.pmp;*.og*;*.vp*;*.vivo;*.vob;*.pls;*.m3u;*.img;*.iso;*.bin;*.3gp;*.flv;*.ape;*.amr;*.aac;*.tp;\0Any File\0*.*\0"


extern m_config_t* mconfig;
extern char *new_dvd_device;
extern char *new_bluray_device;
extern char *font_name;
extern char *filename, *save_filename, *last_filename;
extern int save_volume,save_sec,save_audio_id,save_dvdsub_id,save_vobsub_id,save_set_of_sub_pos,save_sub_pos;;
extern int reload, auto_play;
extern int player_idle_mode;
extern int _map_buffer_factor;

extern void save_status(void);

//int __stdcall (*RAROpenArchive)(struct RAROpenArchiveData *ArchiveData);
int __stdcall (*RAROpenArchiveEx)(struct RAROpenArchiveDataEx *ArchiveData);
int __stdcall (*RARCloseArchive)(int hArcData);
//int __stdcall (*RARReadHeader)(int hArcData,struct RARHeaderData *HeaderData);
int __stdcall (*RARReadHeaderEx)(int hArcData,struct RARHeaderDataEx *HeaderData);
int __stdcall (*RARProcessFile)(int hArcData,int Operation,char *DestPath,char *DestName);
//int __stdcall (*RARProcessFileW)(int hArcData,int Operation,wchar_t *DestPath,wchar_t *DestName);
void   __stdcall (*RARSetCallback)(int hArcData,UNRARCALLBACK Callback,long UserData);
//void __stdcall (*RARSetChangeVolProc)(int hArcData,CHANGEVOLPROC ChangeVolProc);
//void __stdcall (*RARSetProcessDataProc)(int hArcData,PROCESSDATAPROC ProcessDataProc);
void __stdcall (*RARSetPassword)(int hArcData,char *Password);
//int __stdcall (*RARGetDllVersion)();
unsigned int unrardll=0;

static char our_path[MAX_PATH];
static char codec_path[MAX_PATH];
static char cmdline[MAX_PATH];
static char *mplayer_avs;
char *tempsub;
char *mplayer_pls;
char *default_dir;
char *font_path=NULL;
char *skin_name = NULL;
char *skins_name[100];
int auto_resume = 0;
int save_stats_num = 3;
int autoplay_fuzziness = 1;
int sys_Language = 0;
int info_to_html = 0;

const CLSID CLSID_TaskbarList={0x56fdf344, 0xfd6d, 0x11d0,
    {0x95, 0x8a, 0x00, 0x60, 0x97, 0xc9, 0xa0, 0x90}};;

const IID IID_ITaskbarList3Vtbl={0xea1afb91, 0x9e28, 0x4b86,
    {0x90, 0xe9, 0x9e, 0x9f, 0x8a, 0x5e, 0xef, 0xaf}};

#define _debug(x,y) //printf("DEBUG >> "x"\n\n",y)

static FILE *logfile = NULL;
static HANDLE hEqThread = NULL;
static HWND hwnd_eq;
static int bScrActiveSave = 0;
static int bPowerActiveSave = 0;
static int lowpower = 0;
static int poweroff = 0;
static int screensaver = 0;
static BOOL state_saved = FALSE;
static int support_unicode = 1;
int fatal = 0;
char *log_filename;
HANDLE hStdOut;
int showlog = 0;
int gui_thread = 0;
int end_pos = 0;
int osd_percent = 0;
int osd_systime = 0;
int one_player = 0;
int generate_preview = 0;
int is_vista = -1;
int is_win7 = 0;
int using_theme = -1;
int using_aero = -1;
int always_thread = 0;
int use_windowblinds = 0;
char *screenshot_dir;
int use_ss_dir = 0;
int gl_fs_flash = 0;
int gl_new_window = 1;
int always_quit = 1;
int have_audio = 1;
int loop_all = 0;
int no_dvdnav = 0;
int mouse_dvdnav = 0;
int show_menubar = 0;
int show_controlbar = 1;
int skin_controlbar = 1;
int show_status = 0;
int skinned_player = 1;
int use_rightmenu = 0;
int ass_auto_expand = 0;
int ass_expand_aspect = 0;
int disable_screensaver = 1;
int gui_priority_lowest = 1;
int reload_when_open = 0;
int reload_confirm = 1;
int adjust_ts_offset = 1;
int controlbar_height = 0;
int controlbar_alpha = -1;
int seek_realtime = 1;
int detect_directx = 1;
int detect_cache = 1;
int auto_hide_control = 1;
int always_use_ass = 1;
logo_t gui_logo;
skin_t gui_skin;

HANDLE hThread = NULL;
DWORD dwThreadID;
extern int InitWindow(void);
extern int UninitWindow(void);
extern HANDLE hAccelTable;
extern HANDLE hAccelTableW32;
int ExitGuiThread = 0;
int vo_dirver = VO_DIRV_DIRECTX;
int high_accuracy_timer = 0;
static char cd_dirver[16], cd_dirverlist[26];
static char *full_url = NULL;
static char *audio_f = NULL;

char *video_exts[] = {"avi", "mkv", "rmvb", "mp4", "mpg",
						"mpeg", "wmv", "asf", "rm", "flv",
						"ts", "tp","ogm", "mov", "pmp",
						"m4v", "3gp", "vob", "avs", NULL};

char *audio_exts[] = {"aac", "mp3", "m4a", "ogg", "ape",
						"flac", "wma", "mpc", "wav", "ra",
						"amr", "mka", NULL};

char *get_path(const char *path){
	char *temp = (char *)malloc(MAX_PATH);
	strcpy(temp, our_path);
	if (path)
		strcat(temp, path);
	return temp;
}

// I like mine better than the block in mplayer.c
static void init_paths(){
	char *path;
	int len;

	GetModuleFileName(NULL, our_path, MAX_PATH);
	*(strrchr(our_path, '\\')+1) = '\0';

	strcpy(codec_path, our_path);
	strcat(codec_path, "codecs\\");

	mplayer_pls = get_path("mplayer.pls");
	mplayer_avs = get_path("mplayer.avs");

	// get the PATH env var
	len = GetEnvironmentVariable("PATH", path, 1)+strlen(our_path)+strlen(codec_path)+3;
	path = (char *)malloc(len);
	GetEnvironmentVariable("PATH", path, len);

	// append our_path and codec_path
	strcat(path, ";");
	strcat(path, our_path);
	strcat(path, ";");
	strcat(path, codec_path);

	// set the PATH env var
	SetEnvironmentVariable("PATH", path);

	GetWindowsDirectory(path, len);
	strcat(path, "\\fonts\\");
	font_path = strdup(path);

	GetTempPath(len, path);
	strcat(path, "\\mpvobsub.tmp");
	tempsub = strdup(path);

	free(path);
}

BOOL CALLBACK CDDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG :
		{
			int itemp, drivernum = 0, cd_number = 0;
			UINT IsLogicalCDROM;
			int drives = GetLogicalDrives();
			while(drives) {
				drives >>= 1;
				drivernum++;
			};
			if(drivernum > 26) drivernum = 26;
			for(itemp = 0; itemp < drivernum; itemp++)
			{
				snprintf(cd_dirver, 16, "%c:", 'A' + itemp);
				IsLogicalCDROM = GetDriveType(cd_dirver);
				if (IsLogicalCDROM == DRIVE_CDROM) {
					cd_dirverlist[cd_number] = 'A' + itemp;
					SendDlgItemMessage(hDlg, IDC_LIST_CDROM, LB_ADDSTRING, 0, (LPARAM)cd_dirver);
					cd_number++;
				}
			}
			cd_dirver[0] = 0;
			SendDlgItemMessage(hDlg, IDC_LIST_CDROM, LB_SETCURSEL, 0, 0);
			SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
			return TRUE ;
		}
		case WM_CLOSE:
			EndDialog( hDlg, 0);
			break;
		case WM_COMMAND :
			switch (LOWORD (wParam))
			{
				case IDOK :{
					int selected = SendDlgItemMessage(hDlg, IDC_LIST_CDROM, LB_GETCURSEL, 0, 0);
					if(selected >= 0) snprintf(cd_dirver, 16, "%c:", cd_dirverlist[selected]);
				}
				case IDCANCEL :
					EndDialog (hDlg, wParam) ;
					return TRUE ;
			}
			break ;
	}
	return FALSE ;
}

BOOL GetPlayCD(HINSTANCE hInstance, HWND hWndParent)
{
	if(IDOK == DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CDROM), hWndParent, CDDlgProc))
	{
		if(strlen(cd_dirver) > 0){
			m_config_set_option(mconfig, "cdrom-device", cd_dirver);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CALLBACK URLDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CLOSE:
			EndDialog( hDlg, 0);
			break;
		case WM_COMMAND :
			switch (LOWORD (wParam))
			{
				case IDOK :
				{
					char *url = (char *)calloc(10*1024, 1);
					GetDlgItemText(hDlg, IDC_CMDLINE, url, 10*1024);
					if(strlen(url) > 0) {
						full_url = (char *)malloc(10*1024 + 20);
						snprintf(full_url, 10*1024, "loadfile \"%s\"", url);
					}
					free(url);
				}
				case IDCANCEL :
					EndDialog (hDlg, wParam);
					return TRUE ;
			}
			break ;
	}
	return FALSE ;
}

BOOL GetURL(HINSTANCE hInstance, HWND hWndParent, char **url)
{
	if(IDOK == DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_URL), hWndParent, URLDlgProc))
	{
		if(full_url) {
			if(url)
				*url = strdup(full_url);
			free(full_url);
			full_url = NULL;
		}
		return TRUE;
	}
	return FALSE;
}

// ========== OpenFileDialog ============
#define OFN_BUFFER_SIZE (256 * MAX_PATH)
OPENFILENAME ofn;
char ofn_buffer[OFN_BUFFER_SIZE];
wchar_t wofn_buffer[OFN_BUFFER_SIZE];

UINT APIENTRY ofnhook(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch (uMsg)
	{
		case WM_SIZE: {
			RECT rc;
			GetClientRect(hDlg, &rc);
			SetWindowPos(GetDlgItem(hDlg, IDC_CMDLINE), 0, 0, 0,
				rc.right - 120, 20, SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
			break;
		}
		case WM_DESTROY:
			GetDlgItemText(hDlg, IDC_CMDLINE, cmdline, MAX_PATH);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

int OpenFileDialog(char *title, char *fname, char *filter, char *initdir, DWORD flags){
	memset(&ofn, 0, sizeof(ofn));
	strcpy(ofn_buffer, fname);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = ofn_buffer;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = initdir;
	ofn.nMaxFile = OFN_BUFFER_SIZE;
	ofn.Flags = flags;
	ofn.lpstrTitle = title;
	if (flags & OFN_ENABLETEMPLATE) {
		ofn.hInstance = GetModuleHandle(NULL);
		ofn.lpfnHook = (LPOFNHOOKPROC)ofnhook;
		if(sys_Language == 1)
			ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE_SC);
		else if(sys_Language == 3 || sys_Language == 4)
			ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE_TC);
		else
			ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE);
	}
	return GetOpenFileName(&ofn);
}

int OpenFileDialogW(wchar_t *title, wchar_t *fname, wchar_t *filter, wchar_t *initdir, DWORD flags){
	memset(&ofn, 0, sizeof(ofn));
	wcscpy(wofn_buffer, fname);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = wofn_buffer;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = initdir;
	ofn.nMaxFile = OFN_BUFFER_SIZE;
	ofn.Flags = flags;
	ofn.lpstrTitle = title;
	if (flags & OFN_ENABLETEMPLATE) {
		ofn.hInstance = GetModuleHandle(NULL);
		ofn.lpfnHook = (LPOFNHOOKPROC)ofnhook;
		if(sys_Language == 1)
			ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE_SC);
		else if(sys_Language == 3 || sys_Language == 4)
			ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE_TC);
		else
			ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE);
	}
	return GetOpenFileNameW(&ofn);
}

int GetSubFile(char **name){
	char title_str[32];
	if(sys_Language == 1)
		LoadString(GetModuleHandle(NULL), IDS_LOADSUB_SC, title_str, 20);
	else if(sys_Language == 3 || sys_Language == 4)
		LoadString(GetModuleHandle(NULL), IDS_LOADSUB_TC, title_str, 20);
	else
		strcpy(title_str,"Load Subtitle");

	if (OpenFileDialog(title_str,"",
		"Subtitle Files\0*.utf*;*.srt;*.smi;*.rt;*.txt;*.ssa;*.aqt;*.jss;*.js;*.ass\0Any File\0*.*\0",
		default_dir, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST)) {
		*name = ofn_buffer;
		return 1;
	}
	return 0;
}

static CHOOSEFONT cf;
static LOGFONT font;

int GetFontFile(char *name){
	int ret = 0;

	font.lfHeight = 20;
	cf.hwndOwner = GetForegroundWindow();
	if(ChooseFont(&cf))
	{
		HKEY key;
		DWORD cbSize = MAX_PATH;

		strcpy(name, font.lfFaceName);
		strcat(name, " (TrueType)");
		RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", &key);
		ret = !RegQueryValueEx(key, name, NULL, NULL, name, &cbSize);
		RegCloseKey(key);
	}

	return ret;
}

int GetOpenFile(char *title, char **name, char **pCmdline){
	int result = 0;

	if (OpenFileDialog(title,"", MEDIA_FILES,
		default_dir, OFN_HIDEREADONLY | OFN_NOVALIDATE | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLESIZING | (pCmdline?(OFN_ENABLEHOOK|OFN_ENABLETEMPLATE):0))) {
		char *p, *filename = ofn_buffer;
		result = 2;
		if (pCmdline) *pCmdline = cmdline;

		if (p = strstr(filename, "://") ) {
			while (p>filename && *(p-1) != '\\') p--;
			filename = p + 16;
			memmove(filename, p, strlen(p) + 1);
			if (strstr(filename, "dvd://")) {
				DWORD t;
				strcpy(p, "VIDEO_TS.VOB");
				t = GetFileAttributes(ofn_buffer);
				if (t != 0xffffffff && !(t & 0x10)) { // is file
					*(--p) = 0;
					if(new_dvd_device)
						free(new_dvd_device);
					new_dvd_device = strdup(ofn_buffer);
				}
			}
		} else {
			FILE * fp;
			if (SetCurrentDirectory(filename)) {
				p = filename+strlen(filename)+1;
				fp = fopen(mplayer_pls, "w");
				while (*p) {
					fprintf(fp, "%s\\%s\n",filename, p);
					p=p+strlen(p)+1;
				}
				fclose(fp);
				*name = mplayer_pls;
				return 1;
			}
			p = strrchr(filename, '.');
			if (p && (!stricmp(p, ".pls") || !stricmp(p, ".m3u") || !stricmp(p, ".m3u8") || !stricmp(p, ".fls"))) {
//#ifdef CHECK_FILE_SIZE
				HANDLE hFile;
				DWORD filesize = 0, filesizehigh = 0;
				hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if(hFile) {
					filesize = GetFileSize(hFile, &filesizehigh);
					CloseHandle(hFile);
				}
				if(!filesizehigh && filesize < 20480)
//#endif
					result = 1;
			}
		}
		*name = filename;
	}

	return result;
}

int GetOpenFileW(char *title, char **name, char **pCmdline){
	int result = 0;
	char *p, *filename;
	wchar_t wdefault_dir[MAX_PATH];
	wchar_t wtitle[MAX_PATH];
	mbstowcs(wdefault_dir,default_dir,MAX_PATH);
	mbstowcs(wtitle,title,MAX_PATH);

	if (OpenFileDialogW(wtitle, L"", LMEDIA_FILES, wdefault_dir,
			OFN_HIDEREADONLY | OFN_NOVALIDATE | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLESIZING | (pCmdline?(OFN_ENABLEHOOK|OFN_ENABLETEMPLATE):0))) {

			wcstombs(ofn_buffer,wofn_buffer,OFN_BUFFER_SIZE);

			filename = ofn_buffer;
			result = 2;
			if (pCmdline) *pCmdline = cmdline;

			if (p = strstr(filename, "://") ) {
				while (p>filename && *(p-1) != '\\') p--;
				filename = p + 16;
				memmove(filename, p, strlen(p) + 1);
				if (strstr(filename, "dvd://")) {
					DWORD t;
					strcpy(p, "VIDEO_TS.VOB");
					t = GetFileAttributes(ofn_buffer);
					if (t != 0xffffffff && !(t & 0x10)) { // is file
						*(--p) = 0;
						if(new_dvd_device)
							free(new_dvd_device);
						new_dvd_device = strdup(ofn_buffer);
					}
				}
			} else {
				FILE * fp;
				if (SetCurrentDirectory(filename)) {
					wchar_t *q = wofn_buffer+wcslen(wofn_buffer)+1;
					fp = fopen(mplayer_pls, "w");
					char fname[MAX_PATH];
					while (*q) {
						wcstombs(fname,q,MAX_PATH);
						if(GetFileAttributes(fname) == 0xffffffff)
						{
							wchar_t tmpshot[MAX_PATH];
							GetShortPathNameW(q,tmpshot,MAX_PATH);
							wcstombs(fname,tmpshot,MAX_PATH);
						}
						fprintf(fp, "%s\\%s\n",filename, fname);
						q=q+wcslen(q)+1;
					}
					fclose(fp);
					*name = mplayer_pls;
					return 1;
				}
				p = strrchr(filename, '.');
				if (p && (!stricmp(p, ".pls") || !stricmp(p, ".m3u") || !stricmp(p, ".m3u8") || !stricmp(p, ".fls"))) {
//#ifdef CHECK_FILE_SIZE
					HANDLE hFile;
					DWORD filesize = 0, filesizehigh = 0;
					hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if(hFile) {
						filesize = GetFileSize(hFile, &filesizehigh);
						CloseHandle(hFile);
					}
					if(!filesizehigh && filesize < 20480)
//#endif
						result = 1;
				}
			}
			if(strstr(filename ,":\\") && GetFileAttributes(filename) == 0xffffffff) {
				wchar_t tmpshot[MAX_PATH];
				GetShortPathNameW(wofn_buffer,tmpshot,MAX_PATH);
				wcstombs(filename,tmpshot,MAX_PATH);
			}
			*name = filename;
	}

	return result;
}

BOOL GetOpenAVSFileW(char *title, char **name, char **pCmdline){
	char *p, *filename;
	wchar_t wdefault_dir[MAX_PATH];
	wchar_t wtitle[MAX_PATH];
	FILE * fp = NULL;
	BOOL result = FALSE;

	mbstowcs(wdefault_dir,default_dir,MAX_PATH);
	mbstowcs(wtitle,title,MAX_PATH);

	if (OpenFileDialogW(wtitle, L"", LMEDIA_FILES, wdefault_dir,
		OFN_HIDEREADONLY | OFN_NOVALIDATE | OFN_EXPLORER | OFN_ENABLESIZING | (pCmdline?(OFN_ENABLEHOOK|OFN_ENABLETEMPLATE):0))) {

			wcstombs(ofn_buffer,wofn_buffer,OFN_BUFFER_SIZE);

			filename = ofn_buffer;
			if (pCmdline) *pCmdline = cmdline;

			if(strstr(filename ,":\\") && GetFileAttributes(filename) == 0xffffffff) {
				wchar_t tmpshot[MAX_PATH];
				GetShortPathNameW(wofn_buffer,tmpshot,MAX_PATH);
				wcstombs(filename,tmpshot,MAX_PATH);
			}
			if(GetFileAttributes(filename) != 0xffffffff) {
				fp = fopen(mplayer_avs, "w");
				if(fp) {
					fprintf(fp, "DirectShowSource(\"%s\",convertfps=true)\n",filename);
					fclose(fp);
					*name = mplayer_avs;
					result = TRUE;
				}
			}
	}

	return result;
}

BOOL CALLBACK FileDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG :
			if(filename)
				SetDlgItemText(hDlg, IDC_CMDLINE, filename);
			break;
		case WM_CLOSE:
			EndDialog( hDlg, 0);
			break;
		case WM_COMMAND :
			switch (LOWORD (wParam))
			{
				case IDC_BUTTON_DELETE:
					{
						wchar_t wdefault_dir[MAX_PATH];

						mbstowcs(wdefault_dir,default_dir,MAX_PATH);
						if (OpenFileDialogW(NULL, L"", LMEDIA_FILES, wdefault_dir,
							OFN_HIDEREADONLY | OFN_NOVALIDATE | OFN_EXPLORER | OFN_ENABLESIZING)) {
								SetDlgItemTextW(hDlg, IDC_CMDLINE, wofn_buffer);
						}
					}
					break;
				case IDC_BUTTON_CLEAN:
					{
						wchar_t wdefault_dir[MAX_PATH];

						mbstowcs(wdefault_dir,default_dir,MAX_PATH);
						if (OpenFileDialogW(NULL, L"", LMEDIA_FILES, wdefault_dir,
							OFN_HIDEREADONLY | OFN_NOVALIDATE | OFN_EXPLORER | OFN_ENABLESIZING)) {
								SetDlgItemTextW(hDlg, IDC_PASSWORD, wofn_buffer);
						}
					}
					break;
				case IDOK :
				{
					char *url = (char *)calloc(MAX_PATH*2, 1);
					GetDlgItemText(hDlg, IDC_CMDLINE, url, MAX_PATH*2);
					if(strlen(url) > 0) {
						full_url = (char *)malloc(MAX_PATH*2 + 20);
						snprintf(full_url, MAX_PATH*2+20, "loadfile \"%s\"", url);
					}
					wchar_t *urlw = (wchar_t *)calloc(MAX_PATH*2, 2);
					GetDlgItemTextW(hDlg, IDC_PASSWORD, urlw, MAX_PATH*2);
					if(wcslen(urlw) > 0) {
						wchar_t tmpshot[MAX_PATH*2];
						if(GetShortPathNameW(urlw, tmpshot, MAX_PATH*2)) {
							wcstombs(url, tmpshot, MAX_PATH*2);
							if(strlen(url) > 0)
								audio_f = strdup(url);
						}
					}
					free(urlw);
					free(url);
				}
				case IDCANCEL :
					EndDialog (hDlg, wParam);
					return TRUE;
			}
			break ;
	}
	return FALSE;
}

BOOL GetOpenFile2(HINSTANCE hInstance, HWND hWndParent, char **file, char **audio)
{
	UINT IDD = IDD_DIALOG_OPEN;
	if(sys_Language == 1)
		IDD = IDD_DIALOG_OPEN_SC;
	else if(sys_Language == 3 || sys_Language == 4)
		IDD = IDD_DIALOG_OPEN_TC;
	if(IDOK == DialogBox(hInstance, MAKEINTRESOURCE(IDD), hWndParent, FileDlgProc)) {
		if(full_url) {
			if(file)
				*file = strdup(full_url);
			free(full_url);
			full_url = NULL;
		}
		if(audio_f) {
			if(audio)
				*audio = strdup(audio_f);
			free(audio_f);
			audio_f = NULL;
		}
		return TRUE;
	}
	return FALSE;
}

static int GetDriverDevice(HWND hWnd, char **device, wchar_t *title)
{
	int result = 0, len;
	char path[MAX_PATH];
	wchar_t szDisplayName[MAX_PATH];
	wchar_t szBuffer[MAX_PATH];
	wchar_t tmpshot[MAX_PATH];
	LPITEMIDLIST lpItemIDList;
	BROWSEINFOW browseInfo;
	LPITEMIDLIST ppidl;

	if(!device)
		return 0;

	if(*device)
		free(*device);
	*device = NULL;

	browseInfo.lpszTitle = title;
	browseInfo.hwndOwner = hWnd;
	browseInfo.lpfn = NULL;
	browseInfo.lParam = NULL;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.ulFlags = BIF_NEWDIALOGSTYLE |BIF_RETURNONLYFSDIRS| BIF_DONTGOBELOWDOMAIN | BIF_NONEWFOLDERBUTTON;

	if(SHGetFolderLocation(hWnd, CSIDL_DRIVES, NULL, 0, &ppidl) == S_OK)
		browseInfo.pidlRoot = ppidl;

	if ((lpItemIDList = SHBrowseForFolderW(&browseInfo)) != NULL) {
		if (SHGetPathFromIDListW(lpItemIDList, szBuffer)) {
			if (!szBuffer[0] == '\0') {
				GetShortPathNameW(szBuffer, tmpshot, MAX_PATH);
				wcstombs(path, tmpshot, MAX_PATH);
				len = strlen(path) - 1;
				if(len > 1) {
					if(path[len] == '\\')
						path[len] = '\0';
					*device = strdup(path);
					result = 1;
				}
			}
		}
		CoTaskMemFree(lpItemIDList);
	}
	return result;
}

int GeBlurayDriver(HWND hWnd)
{
	return GetDriverDevice(hWnd, &new_bluray_device, L"Bluray Driver or Folder");
}

int GetDVDDriver(HWND hWnd)
{
	return GetDriverDevice(hWnd, &new_dvd_device, L"DVD Driver or Folder");
}

void getFindname(const char *name, char *ret)
{
	BOOL finded = FALSE;
	BOOL lastStar = FALSE;
	BOOL haveMatchString = FALSE;
	int x, i, j;
	int slen = strlen(name);
	char filter[] = "0123456789!@#$%^&*()_+[]{}\\/|?.,<>`~ -=;:\'\"\0";
	char filter2[] = "abcdefghABCDEFGH\0";

	memset(ret, '\0', MAX_PATH);
	for(i = 0, j = 0; i < slen; i++) {
		finded = FALSE;
		x = 0;
		while (filter[x]) {
			if(filter[x] == name[i]) {
				finded = TRUE;
				break;
			}
			++x;
		}

		if(!finded && haveMatchString && i > 3) {
			x = 0;
			while (filter2[x]) {
				if(filter2[x] == name[i]) {
					finded = TRUE;
					break;
				}
				++x;
			}
		}

		if(finded){
			if(!lastStar){
				ret[j] = '*';
				ret[++j] = '\0';
				lastStar = TRUE;
			}
			if(i > 4 && haveMatchString){
				break;
			}
			continue;
		} else {
			lastStar = FALSE;
			haveMatchString = TRUE;
			ret[j] = name[i];
			ret[++j] = '\0';
		}
	}
}

char *GetNextEpisode(char *filename){
	WIN32_FIND_DATA ff;
	HANDLE f;
	char *p, *basename, *newname, *sss, s[MAX_PATH] , path[MAX_PATH]
		, s1[MAX_PATH], s2[MAX_PATH] ,ex[MAX_PATH] ;
	char *pCDx = NULL;
	int i, j, n, diff, exi, maxl;
	BOOL bValid,bDir,bCmpd,bCDx;
	DWORD t;

	t = GetFileAttributes(filename);
	if (t == 0xffffffff) return NULL;
	bDir = (t & 0x10) != 0;
	strcpy(s, filename);
	if(autoplay_fuzziness) {
		strcpy(path, filename);
		basename = _mbsrchr(path, '\\');
		if(basename && strlen(basename) > 1) {
			basename[1] = 0;
			SetCurrentDirectory(path);
		}
		bCDx = FALSE;
	}
	basename = _mbsrchr(s, '\\');

	if ( basename ) {
		if (basename-3>=s && !strnicmp(basename-3,"CD",2)) {
			pCDx=basename;
			bCDx = TRUE;
		}
		basename++;
	} else if (bDir) {
		return NULL;
	} else {
		basename = s;
	}

try_CDx:
	p = strrchr(filename, '.');
	if (bDir || !p) {
		strcpy(basename, "*.*");
	} else {
		*basename = '*';
		strcpy(basename+1, p);
	}

	strcpy(ex, basename + 1);
	basename = filename + (basename-s);
	strcpy(s2, basename);

	if(!bCDx) {
		sss = strrchr(s2, '.');
		if(sss)
			sss[0] = 0;
		getFindname(s2, s);
		if(s[0])
			strcat(s, ".*");
		else
			strcpy(s, "*.*");
	}

	f = FindFirstFile(s, &ff);
	do {
		p = ff.cFileName;
		strcpy(s1, p);
		if(!bCDx) {
			sss = strrchr(s1, '.');
			bCmpd = FALSE;
			if(strcmp(ex,".*") && sss) {
				if(stricmp(ex, sss) == 0)
					bCmpd = TRUE;
				else {
					for(exi = 0; video_exts[exi]; exi++)
					{
						if(stricmp(sss+1 ,video_exts[exi]) == 0) {
							bCmpd = TRUE;
							break;
						}
					}
				}
				if(!bCmpd) continue;
			}
			if(sss) sss[0] = 0;
		}

        i = strlen(s1);
        j = strlen(s2);
        maxl = FFMAX(FFMIN(i,j)-5, FFABS(i-j)+12);

		bValid = FALSE;
		diff = i = j = 0;
		n = (auto_play > 1)?auto_play:-1;
		while ( s2[i] && s1[j] && n-- ) {
			if ( s2[i] != s1[j] ) {
				if ( !diff++ ) {
					if ( atoi(s1+j) == atoi(s2+i)+1 ) {
						bValid = TRUE;
						while (isdigit(s1[j])) j++,n--;
						while (isdigit(s2[i])) i++;
						continue;
					} else if ( s1[j] == s2[i]+1 /*&& !isdigit(p[j+1])*/ ) {
						bValid = TRUE;
					}
				}
			}
			i++;
			j++;
		}

		if ( bValid && diff > 0 && diff < maxl) {
			FindClose(f);
			newname = (char *)malloc(strlen(filename)+i+10);
			i = strlen(p) - strlen(basename);
			if ( i <= 0 ) {
				strcpy(basename, p);
				strcpy(newname, filename);
			} else {
				strcpy(newname, filename);
				strcpy(newname + (basename - filename), p);
			}
			strcpy(s1, newname);
			p = strrchr(s1, '.');
			if(p && ex) {
				p[0] = 0;
				strcat(s1, ex);
				t = GetFileAttributes(s1);
				if (t != 0xffffffff && !(t & 0x10))
					strcpy(newname, s1);
			}
			return newname;
		}
	} while ( FindNextFile(f, &ff) );
	FindClose(f);

	if (pCDx) {
		strcpy(s, filename);
		*pCDx = 0;   // temp change
		(*(pCDx-1))++;  // CD number++
		t = GetFileAttributes(s);
		if ((t!=0xffffffff) && (t & 0x10)) { // is a Directory?
			*pCDx = '\\';  // restore
			strcpy(filename, s);
			basename = pCDx + 1;
			pCDx = NULL;
			goto try_CDx; // try next CD folder
		}
	}
	return NULL;
}

void directory_to_playlist(char *dir)
{
	WIN32_FIND_DATA ff;
	HANDLE f;
	char *ex, *p;
	int i;
	FILE * fp = fopen(mplayer_pls, "w");
	if(!fp)
		return;
	SetCurrentDirectory(dir);
	f = FindFirstFile("*.*", &ff);
	do {
		p = ff.cFileName;
		ex =  strrchr(p, '.');
		if(!ex)
			continue;
		for(i = 0; video_exts[i]; i++)
		{
			if(stricmp(ex+1 ,video_exts[i]) == 0)
			{
				fprintf(fp, "%s\\%s\n",dir , p);
				continue;
			}
		}
		for(i = 0; audio_exts[i]; i++)
		{
			if(stricmp(ex+1 ,audio_exts[i]) == 0)
			{
				fprintf(fp, "%s\\%s\n",dir , p);
				continue;
			}
		}
	} while ( FindNextFile(f, &ff) );
	FindClose(f);
	fclose(fp);
}

extern void update_playlist_status(int v);

int guiCommand(GUI_CMD cmd, int v)
{
	if(cmd == CMD_UPDATE_TITLE)
		update_playlist_status(v);

	if(vo_dirver == VO_DIRV_OPENGL || vo_dirver == VO_DIRV_DIRECT3D)
		return w32Cmd(cmd, v);
	else
		return guiCmd(cmd, v);
}

DWORD WINAPI threadProc(LPVOID lpParam)
{
	MSG msg;
	HANDLE hAccelTableThread;

	guiCommand(CMD_INIT_WINDOW, 0);

	set_priority();

	if(gui_priority_lowest)
		SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);

	if(vo_dirver == VO_DIRV_OPENGL || vo_dirver == VO_DIRV_DIRECT3D)
		hAccelTableThread = hAccelTableW32;
	else
		hAccelTableThread = hAccelTable;

	while (1) {
		int ret = GetMessage(&msg,NULL,0,0);
		if ((!ret || ret==-1 ) && ExitGuiThread)
			break;
		if (!TranslateAccelerator(msg.hwnd, hAccelTableThread, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

void StartGuiThread(void)
{
	hThread = CreateThread(NULL, 0, threadProc, NULL, 0, &dwThreadID);
}

static dictionary * status_ini = NULL;

int initStatus(){
	if (auto_resume <= 0)
		return 0;
	char *cfg = get_path("kk.ini");
	if(!status_ini)
		status_ini = iniparser_load(cfg);
	free(cfg);
	return 1;
}

int getStatusNum(){
	int index = 0;
	char Resume[32];
	
	if(!status_ini)
		return 0;
	int have_status = iniparser_getint(status_ini , "Status:Resume", -10);
	if(have_status < 0)
		return 0;
	while(have_status >= 0) {
		index++;
		if(index > save_stats_num)
			break;
		sprintf(Resume, "Status:Resume%02d", index);
		have_status = iniparser_getint(status_ini , Resume, -1);
	}
	return index;
}

char * getNameById(int id){
	if(!status_ini || id < 0 || id > save_stats_num)
		return NULL;
	char *name = NULL, *str, Filename[32];
	if(id == 0)
		sprintf(Filename, "Status:Filename");
	else
		sprintf(Filename, "Status:Filename%02d", id);
	str = iniparser_getstring(status_ini, Filename, NULL);
	if(str && _mbsrchr(str, '\\')) {
		name = malloc(MAX_PATH);
		strncpy(name,str, MAX_PATH);
	}
	return name;
}

int getStatusById(int id){
	if(!status_ini || id < 0 || id > save_stats_num)
		return 0;
	char Time[32];
	if(id == 0)
		sprintf(Time, "Status:Time");
	else
		sprintf(Time, "Status:Time%02d", id);
	int time = iniparser_getint(status_ini, Time, 0);
	
	return time;
}

int addStatus(const char *file){
	if(!status_ini && auto_resume != 2)
		return -1;
	int nextid = getStatusNum();
	
	int i,save_type = 0;
	for(i = 0; i < nextid; i++)
	{
		char *str = getNameById(i);
		if(str && stricmp(str,file) == 0) {
			save_type = i + 1;
			nextid = i;
			break;
		}
	}
	if(nextid == 0) {
		save_status();
		iniparser_setint(status_ini ,"Status:Resume", 1);
		iniparser_setint(status_ini ,"Status:Time", save_sec-5);
		iniparser_setint(status_ini ,"Status:Audio", save_audio_id);
		iniparser_setint(status_ini ,"Status:DVDSub", save_dvdsub_id);
		iniparser_setint(status_ini ,"Status:VOBSub", save_vobsub_id);
		iniparser_setint(status_ini ,"Status:TXTSub", save_set_of_sub_pos);
		iniparser_setint(status_ini ,"Status:SubPos", save_sub_pos);
		iniparser_set(status_ini ,"Status:Filename", file);
	} else {
		char Resume[32], Time[32], Audio[32], DVDSub[32], VOBSub[32], TXTSub[32], SubPos[32], Filename[32];
		sprintf(Resume, "Status:Resume%02d", nextid);
		sprintf(Time, "Status:Time%02d", nextid);
		sprintf(Audio, "Status:Audio%02d", nextid);
		sprintf(DVDSub, "Status:DVDSub%02d", nextid);
		sprintf(VOBSub, "Status:VOBSub%02d", nextid);
		sprintf(TXTSub, "Status:TXTSub%02d", nextid);
		sprintf(SubPos, "Status:SubPos%02d", nextid);
		sprintf(Filename, "Status:Filename%02d", nextid);
		save_status();
		iniparser_setint(status_ini , Resume, 1);
		iniparser_setint(status_ini , Time, save_sec - 5);
		iniparser_setint(status_ini , Audio, save_audio_id);
		iniparser_setint(status_ini , DVDSub, save_dvdsub_id);
		iniparser_setint(status_ini , VOBSub, save_vobsub_id);
		iniparser_setint(status_ini , TXTSub, save_set_of_sub_pos);
		iniparser_setint(status_ini , SubPos, save_sub_pos);
		iniparser_set(status_ini , Filename, file);
	}
	char *tmp = get_path("kk.ini");
	iniparser_save(status_ini , tmp);
	free(tmp);
	return save_type;
}

static int have_modified = 0;

int removeStatus(int id)
{
	char Resume[32], Time[32], Audio[32], DVDSub[32], VOBSub[32], TXTSub[32], SubPos[32], Filename[32];
	char name[MAX_PATH];
	char *str = NULL;
		
	if(!status_ini || id < 0)
		return 0;
	int	count = getStatusNum();
	if(count <= id)
		return 0;
	if(count > id + 1) {
		sprintf(Resume, "Status:Resume%02d", count -1);
		sprintf(Time, "Status:Time%02d", count -1);
		sprintf(Audio, "Status:Audio%02d", count -1);
		sprintf(DVDSub, "Status:DVDSub%02d", count -1);
		sprintf(VOBSub, "Status:VOBSub%02d", count -1);
		sprintf(TXTSub, "Status:TXTSub%02d", count -1);
		sprintf(SubPos, "Status:SubPos%02d", count -1);
		sprintf(Filename, "Status:Filename%02d", count -1);
		str = iniparser_getstring(status_ini, Filename, NULL);
		if(str) {
			strncpy(name,str, MAX_PATH);
		}
		
		int x_save_sec = iniparser_getint(status_ini , Time, 0);
		int x_save_audio_id = iniparser_getint(status_ini , Audio, 0);
		int x_save_dvdsub_id = iniparser_getint(status_ini , DVDSub, 0);
		int x_save_vobsub_id = iniparser_getint(status_ini , VOBSub, 0);
		int x_save_set_of_sub_pos = iniparser_getint(status_ini , TXTSub, 0);
		int x_save_sub_pos = iniparser_getint(status_ini , SubPos, 0);
		
		iniparser_unset(status_ini,Resume);
		iniparser_unset(status_ini ,Time);
		iniparser_unset(status_ini ,Audio);
		iniparser_unset(status_ini ,DVDSub);
		iniparser_unset(status_ini ,VOBSub);
		iniparser_unset(status_ini ,TXTSub);
		iniparser_unset(status_ini ,SubPos);
		iniparser_unset(status_ini ,Filename);
		
		if(id > 0) {
			sprintf(Resume, "Status:Resume%02d", id);
			sprintf(Time, "Status:Time%02d", id);
			sprintf(Audio, "Status:Audio%02d", id);
			sprintf(DVDSub, "Status:DVDSub%02d", id);
			sprintf(VOBSub, "Status:VOBSub%02d", id);
			sprintf(TXTSub, "Status:TXTSub%02d", id);
			sprintf(SubPos, "Status:SubPos%02d", id);
			sprintf(Filename, "Status:Filename%02d", id);
		} else {
			sprintf(Resume, "Status:Resume");
			sprintf(Time, "Status:Time");
			sprintf(Audio, "Status:Audio");
			sprintf(DVDSub, "Status:DVDSub");
			sprintf(VOBSub, "Status:VOBSub");
			sprintf(TXTSub, "Status:TXTSub");
			sprintf(SubPos, "Status:SubPos");
			sprintf(Filename, "Status:Filename");
		}
		iniparser_setint(status_ini , Resume, 1);
		iniparser_setint(status_ini , Time, x_save_sec);
		iniparser_setint(status_ini , Audio, x_save_audio_id);
		iniparser_setint(status_ini , DVDSub, x_save_dvdsub_id);
		iniparser_setint(status_ini , VOBSub, x_save_vobsub_id);
		iniparser_setint(status_ini , TXTSub, x_save_set_of_sub_pos);
		iniparser_setint(status_ini , SubPos, x_save_sub_pos);
		iniparser_set(status_ini , Filename, name);
		
	} else if(count == id + 1) {
		if(id == 0) {
			iniparser_unset(status_ini ,"Status:Resume");
			iniparser_unset(status_ini ,"Status:Time");
			iniparser_unset(status_ini ,"Status:Audio");
			iniparser_unset(status_ini ,"Status:DVDSub");
			iniparser_unset(status_ini ,"Status:VOBSub");
			iniparser_unset(status_ini ,"Status:TXTSub");
			iniparser_unset(status_ini ,"Status:SubPos");
			iniparser_unset(status_ini ,"Status:Filename");
		} else {
			sprintf(Resume, "Status:Resume%02d", id);
			sprintf(Time, "Status:Time%02d", id);
			sprintf(Audio, "Status:Audio%02d", id);
			sprintf(DVDSub, "Status:DVDSub%02d", id);
			sprintf(VOBSub, "Status:VOBSub%02d", id);
			sprintf(TXTSub, "Status:TXTSub%02d", id);
			sprintf(SubPos, "Status:SubPos%02d", id);
			sprintf(Filename, "Status:Filename%02d", id);
			iniparser_unset(status_ini ,Resume);
			iniparser_unset(status_ini ,Time);
			iniparser_unset(status_ini ,Audio);
			iniparser_unset(status_ini ,DVDSub);
			iniparser_unset(status_ini ,VOBSub);
			iniparser_unset(status_ini ,TXTSub);
			iniparser_unset(status_ini ,SubPos);
			iniparser_unset(status_ini ,Filename);
		}
	}
	have_modified = 1;
	return 1;
}

int cleanStatus(){
	char Resume[32], Time[32], Audio[32], DVDSub[32], VOBSub[32], TXTSub[32], SubPos[32], Filename[32];
	int index = 1;
	
	if(!status_ini)
		return 0;
	
	iniparser_unset(status_ini ,"Status:Resume");
	iniparser_unset(status_ini ,"Status:Time");
	iniparser_unset(status_ini ,"Status:Audio");
	iniparser_unset(status_ini ,"Status:DVDSub");
	iniparser_unset(status_ini ,"Status:VOBSub");
	iniparser_unset(status_ini ,"Status:TXTSub");
	iniparser_unset(status_ini ,"Status:SubPos");
	iniparser_unset(status_ini ,"Status:Filename");
	
	sprintf(Resume, "Status:Resume%02d", index);
	int have_status = iniparser_getint(status_ini , Resume, 0);
	while(have_status > 0) {
		sprintf(Time, "Status:Time%02d", index);
		sprintf(Audio, "Status:Audio%02d", index);
		sprintf(DVDSub, "Status:DVDSub%02d", index);
		sprintf(VOBSub, "Status:VOBSub%02d", index);
		sprintf(TXTSub, "Status:TXTSub%02d", index);
		sprintf(SubPos, "Status:SubPos%02d", index);
		sprintf(Filename, "Status:Filename%02d", index);
		iniparser_unset(status_ini,Resume);
		iniparser_unset(status_ini ,Time);
		iniparser_unset(status_ini ,Audio);
		iniparser_unset(status_ini ,DVDSub);
		iniparser_unset(status_ini ,VOBSub);
		iniparser_unset(status_ini ,TXTSub);
		iniparser_unset(status_ini ,SubPos);
		iniparser_unset(status_ini ,Filename);
		index++;
		if(index > save_stats_num)
			break;
		sprintf(Resume, "Status:Resume%02d", index);
		have_status = iniparser_getint(status_ini , Resume, 0);
	}
	have_modified = 1;
	return 1;
}

void freeStatus(){
	iniparser_freedict(status_ini);
	status_ini = NULL;
}

void updatefavlist(HWND hDlg)
{
	int i, num, time;
	long len;
	SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_RESETCONTENT, 0, 0);
	num = getStatusNum();
	for(i = 0; i < num; i++)
	{
		char s[MAX_PATH],t[16];
		char *str = getNameById(i);
		if(str) {
			time = getStatusById(i);
			if(time >= 3600)
				snprintf(t, 16, " [%02d:%02d:%02d]", time/3600, (time/60)%60, time%60);
			else if(time < 3600 && time > 0)
				snprintf(t, 16, " [%02d:%02d]", time/60, time%60);
			else
				snprintf(t, 16, " [00:00]");
			snprintf(s, MAX_PATH, "%s%s", _mbsrchr(str,'\\') + 1, t);
			free(str);
			SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_ADDSTRING, 0, (LPARAM)s);
			len = lstrlen(s) * 6;
			if(len > (LONG)SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_GETHORIZONTALEXTENT, 0, 0))
				SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_SETHORIZONTALEXTENT, len, 0);
		} else {
			sprintf(s, "Unknown file");
			SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_ADDSTRING, 0, (LPARAM)s);
			len = lstrlen(s) * 6;
			if(len > (LONG)SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_GETHORIZONTALEXTENT, 0, 0))
				SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_SETHORIZONTALEXTENT, len, 0);
		}
	}
}

extern int stream_cache_size;
extern float stream_cache_min_percent;

DWORD WINAPI seekProc(LPVOID lpParam)
{
	char *cmd = (char *)lpParam;
	if(!cmd)
		return 1;
	Sleep(500);
	mp_input_queue_cmd(mp_input_parse_cmd(cmd));
	free(cmd);
	return 0;
}

void StartSeekThread(const char *pos)
{
	CreateThread(NULL, 0, seekProc, (LPVOID)pos, 0, NULL);
}

void seek2time(int time)
{
	if( time > 3)
	{
		char cmd[MAX_PATH];
		snprintf(cmd,MAX_PATH,"seek %d",time);
		if( stream_cache_size > 0 )
			StartSeekThread(strdup(cmd));
		else
			mp_input_queue_cmd(mp_input_parse_cmd(cmd));
	}
}

static char win_text[32],ok_text[32],del_text[32],clean_text[32];
BOOL CALLBACK FavDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG :
		{
			if(sys_Language == 1 || sys_Language == 3 || sys_Language == 4)
			{
				SetWindowText(hDlg , win_text);
				SetDlgItemText(hDlg , IDOK , ok_text);
				SetDlgItemText(hDlg , IDC_BUTTON_DELETE , del_text);
				SetDlgItemText(hDlg , IDC_BUTTON_CLEAN , clean_text);
			}
			updatefavlist(hDlg);
			return TRUE;
		}
		case WM_CLOSE:
			EndDialog( hDlg, 0);
			break;
		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
				case IDOK :
				case IDCANCEL :
					EndDialog (hDlg, wParam) ;
					return TRUE;
				case IDC_BUTTON_DELETE:	{
					int selected = SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_GETCURSEL, 0, 0);
					if(selected < 0 || selected >= getStatusNum())
						break;
					removeStatus(selected);
					updatefavlist(hDlg);
					break;
				}
				case IDC_BUTTON_CLEAN:
					cleanStatus();
					SendDlgItemMessage(hDlg, IDC_LIST_FAVORITES, LB_RESETCONTENT, 0, 0);
					break;
			}
			break ;
	}
	return FALSE ;
}

int ShowFavDlg(HINSTANCE hInstance,HWND hWndParent)
{
	if(sys_Language == 1) {
		LoadString(hInstance, IDS_FAVMENUMANAGE_SC, win_text, 20);
		LoadString(hInstance, IDS_OK_SC, ok_text, 20);
		LoadString(hInstance, IDS_BT_REMOVE_SC, del_text, 20);
		LoadString(hInstance, IDS_CLEAN_TC, clean_text, 20);
	} else if(sys_Language == 3 || sys_Language == 4){ 
		LoadString(hInstance, IDS_FAVMENUMANAGE_TC, win_text, 20);
		LoadString(hInstance, IDS_OK_TC, ok_text, 20);
		LoadString(hInstance, IDS_DELETE_TC, del_text, 20);
		LoadString(hInstance, IDS_CLEAN_TC, clean_text, 20);
	} else {
		LoadString(hInstance, IDS_FAVMENUMANAGE_EN, win_text, 20);
		strcpy(ok_text,"OK");
		strcpy(del_text,"Delete");
		strcpy(clean_text,"Clean");
	}
	initStatus();
	if(IDOK == DialogBox (hInstance , MAKEINTRESOURCE(IDD_DIALOG_FAVORITES) , hWndParent, FavDlgProc)) {
		if(have_modified == 1) {
			char *tmp = get_path("kk.ini");
			iniparser_save(status_ini , tmp);
			free(tmp);
		}
		freeStatus();
		return 1;
	}
	freeStatus();
	return 0;
}

void SaveStatus(void){
	if (auto_resume == 1 && filename && !strstr(filename, "://")) {
		char *tmp = get_path("kk.ini");

		save_status();
		dictionary * ini = NULL;
		ini = iniparser_load(tmp);
		iniparser_setint(ini ,"Status:Resume", 1);
		iniparser_setint(ini ,"Status:Time", save_sec-5);
		iniparser_setint(ini ,"Status:Audio", save_audio_id);
		iniparser_setint(ini ,"Status:DVDSub", save_dvdsub_id);
		iniparser_setint(ini ,"Status:VOBSub", save_vobsub_id);
		iniparser_setint(ini ,"Status:TXTSub", save_set_of_sub_pos);
		iniparser_setint(ini ,"Status:SubPos", save_sub_pos);
		iniparser_set(ini ,"Status:Filename", filename);
		iniparser_save(ini , tmp);
		
		iniparser_freedict(ini);
		free(tmp);
	}
}

void SaveOldStatus()
{
	if (auto_resume == 1 && save_filename && !strstr(save_filename, "://")) {
		char *tmp = get_path("kk.ini");
		
		dictionary * ini = NULL;
		ini = iniparser_load(tmp);
		int have_status = iniparser_getint(ini , "Status:Resume", -10);
		if(have_status < 0)
		{
			iniparser_freedict(ini);
			return;
		}
		have_status = 1;
		int index = 0;
		char Resume[32], Time[32], Audio[32], DVDSub[32], VOBSub[32], TXTSub[32], SubPos[32], Filename[32];
		while(have_status > 0)
		{
			index++;
			if(index > save_stats_num)
				break;
			sprintf(Resume, "Status:Resume%02d", index);
			have_status = iniparser_getint(ini , Resume, 0);
		}
		if(index > save_stats_num)
			index = 1;
		
		sprintf(Resume, "Status:Resume%02d", index);
		sprintf(Time, "Status:Time%02d", index);
		sprintf(Audio, "Status:Audio%02d", index);
		sprintf(DVDSub, "Status:DVDSub%02d", index);
		sprintf(VOBSub, "Status:VOBSub%02d", index);
		sprintf(TXTSub, "Status:TXTSub%02d", index);
		sprintf(SubPos, "Status:SubPos%02d", index);
		sprintf(Filename, "Status:Filename%02d", index);
		
		iniparser_setint(ini , Resume, 1);
		iniparser_setint(ini , Time, save_sec);
		iniparser_setint(ini , Audio, save_audio_id);
		iniparser_setint(ini , DVDSub, save_dvdsub_id);
		iniparser_setint(ini , VOBSub, save_vobsub_id);
		iniparser_setint(ini , TXTSub, save_set_of_sub_pos);
		iniparser_setint(ini , SubPos, save_sub_pos);
		iniparser_set(ini , Filename, save_filename);
		iniparser_setint(ini , "Status:Time", 0);
		iniparser_set(ini , "Status:Filename", filename);
		iniparser_save(ini , tmp);
		
		iniparser_freedict(ini);
		free(tmp);
	}
}

int GetStatus(char *name)
{
	if (auto_resume == 1 && name && !strstr(name, "://")) {
		
		char filenm[MAX_PATH];
		char *tmp = get_path("kk.ini");
		
		dictionary * ini = NULL;
		ini = iniparser_load(tmp);
		
		int have_status = iniparser_getint(ini , "Status:Resume", -10);
		if(have_status < 0)
		{
			iniparser_freedict(ini);
			return 1;
		}

		if(save_filename && stricmp(save_filename,name) == 0)
		{
			iniparser_freedict(ini);
			return 0;
		}

		have_status = 1;
		int index = 0;
		char Resume[32], Time[32], Audio[32], DVDSub[32], VOBSub[32], TXTSub[32], SubPos[32], Filename[32];
		while(have_status > 0)
		{
			index++;
			if(index > save_stats_num)
				break;
			sprintf(Filename, "Status:Filename%02d", index);
			char *str = NULL;
			str = iniparser_getstring(ini, Filename, NULL);
			if(str) {
				strncpy(filenm,str, MAX_PATH);
			} else
				filenm[0] = 0;
			if(stricmp(filenm,name) == 0) {
				sprintf(Resume, "Status:Resume%02d", index);
				sprintf(Time, "Status:Time%02d", index);
				sprintf(Audio, "Status:Audio%02d", index);
				sprintf(DVDSub, "Status:DVDSub%02d", index);
				sprintf(VOBSub, "Status:VOBSub%02d", index);
				sprintf(TXTSub, "Status:TXTSub%02d", index);
				sprintf(SubPos, "Status:SubPos%02d", index);

				int x_save_sec = iniparser_getint(ini , Time, 0);
				int x_save_audio_id = iniparser_getint(ini , Audio, 0);
				int x_save_dvdsub_id = iniparser_getint(ini , DVDSub, 0);
				int x_save_vobsub_id = iniparser_getint(ini , VOBSub, 0);
				int x_save_set_of_sub_pos = iniparser_getint(ini , TXTSub, 0);
				int x_save_sub_pos = iniparser_getint(ini , SubPos, 0);

				iniparser_setint(ini , Time, save_sec);
				iniparser_setint(ini , Audio, save_audio_id);
				iniparser_setint(ini , DVDSub, save_dvdsub_id);
				iniparser_setint(ini , VOBSub, save_vobsub_id);
				iniparser_setint(ini , TXTSub, save_set_of_sub_pos);
				iniparser_setint(ini , SubPos, save_sub_pos);
				iniparser_set(ini , Filename, save_filename);
				iniparser_save(ini , tmp);

				strncpy(save_filename,filenm,MAX_PATH);
				save_sec = x_save_sec ;
				save_audio_id =  x_save_audio_id ;
				save_dvdsub_id =  x_save_dvdsub_id ;
				save_vobsub_id =  x_save_vobsub_id;
				save_set_of_sub_pos =  x_save_set_of_sub_pos;
				save_sub_pos = x_save_sub_pos;

				iniparser_freedict(ini);
				free(tmp);
				return 0;
			}
			sprintf(Resume, "Status:Resume%02d", index);
			have_status = iniparser_getint(ini , Resume, 0);
		}
		iniparser_freedict(ini);
		free(tmp);
	}
	return 1;
}

int ConfirmStatus()
{
	char s[10], title[32], time[16], info[MAX_PATH+66], str[64], *tmp;
	if(player_idle_mode && reload_confirm) {

		if(save_sec >= 3600)
			snprintf(time, 16, " [%02d:%02d:%02d]", save_sec/3600, (save_sec/60)%60, save_sec%60);
		else if(save_sec < 3600 && save_sec > 0)
			snprintf(time, 16, " [%02d:%02d]", save_sec/60, save_sec%60);
		else
			snprintf(time, 16, " [00:00]");

		if(sys_Language == 1) {
			LoadString(GetModuleHandle(NULL), IDS_FAVMENU_SC, str, 64);
			snprintf(title, 32, "MPlayer %s", str);
			LoadString(GetModuleHandle(NULL), IDS_FAVMENU_TEXT_SC, str, 64);
		} else if(sys_Language == 3 || sys_Language == 4) {
			LoadString(GetModuleHandle(NULL), IDS_FAVMENU_TC, str, 64);
			snprintf(title, 32, "MPlayer %s", str);
			LoadString(GetModuleHandle(NULL), IDS_FAVMENU_TEXT_TC, str, 64);
		} else {
			strncpy(title, "MPlayer Favorites", 32);
			strncpy(str, "Find favorite file, Continue to play this file ?  Time", 64);
		}
		snprintf(info, MAX_PATH+80, "%s%s\n\n%s\n", str, time, save_filename);

		if(MessageBox(NULL, info, title, MB_YESNO | MB_TOPMOST) != IDYES) {
			tmp = get_path("kk.ini");
			sprintf(s, "1");
			WritePrivateProfileString("Status", "Resume", s, tmp);
			free(tmp);
			reload = 0;
			return 0;
		}
	}

	return 1;
}

void check_update(void) {
	char url[MAX_PATH], svn[6], date[9], *s;
	char *p = VERSION;
	s = strstr(p, "SVN-r");
	if(!s)
		return;
	strncpy(svn, s+5, 5);
	svn[5] = '\0';
	s = strstr(p, "(");
	if(!s)
		return;
	strncpy(date, s+1, 8);
	date[8] = '\0';
	char *nfilename = get_path("meditor.exe");
	if (GetFileAttributes(nfilename) != 0xFFFFFFFF) {
		sprintf(url, "--check-update --version %s --date %s", svn, date);
		ShellExecute(0, NULL, nfilename, url, NULL, SW_SHOW);
	} else {
		sprintf(url,"http://mplayer-ww.com/?update=1&svn=%s&date=%s", svn, date);
		ShellExecute(0, NULL, url, NULL, NULL, SW_NORMAL);
	}
	free(nfilename);
}

void disable_screen_saver(BOOL disable)
{
	if(disable) {
		if(!state_saved) {
			SystemParametersInfo(SPI_GETLOWPOWERTIMEOUT, 0, &lowpower, 0);
			SystemParametersInfo(SPI_GETPOWEROFFTIMEOUT, 0, &poweroff, 0);
			SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &screensaver, 0);
			SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &bScrActiveSave, 0);
			SystemParametersInfo(SPI_GETPOWEROFFACTIVE, 0, &bPowerActiveSave, 0);
			state_saved = TRUE;
		}
		SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, 0, 0, SPIF_SENDWININICHANGE);
		SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, 0, 0, SPIF_SENDWININICHANGE);
		SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, 0, SPIF_SENDWININICHANGE);
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, SPIF_SENDWININICHANGE);
		SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0, 0, SPIF_SENDWININICHANGE);

		SetThreadExecutionState(ES_CONTINUOUS|ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED);
	} else {
		if(state_saved) {
			SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT, lowpower, 0, SPIF_SENDWININICHANGE);
			SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT, poweroff, 0, SPIF_SENDWININICHANGE);
			SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, screensaver, 0, SPIF_SENDWININICHANGE);
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, bScrActiveSave, 0, SPIF_SENDWININICHANGE);
			SystemParametersInfo(SPI_SETPOWEROFFACTIVE, bPowerActiveSave, 0, SPIF_SENDWININICHANGE);
		}

		SetThreadExecutionState(ES_CONTINUOUS);
		state_saved = FALSE;
	}
}

static dictionary * kk_ini = NULL;

static BOOL detect_one_player(int argc,char** argv,char *tmp)
{
	int i , fd = 0;
	char *arg;
	char tmps[MAX_PATH*2];
	if(one_player) {
		for(i = 1 ; i < argc ; i++) {
			arg = argv[i];
			if(i < 2 && !strcmp(arg, "-generate-preview")) {
				generate_preview = 1;
				one_player = 0;
				return FALSE;
			}
			if(arg[0] == '-' || arg[1] == 0) continue;
			if(strstr(arg, ":\\") || strstr(arg, "://")) {
				fd = 1;
				break;
			}
			if(arg[0] == '-' && arg[1] == 0)
				break;
		}
		HWND hWndMP = FindWindow("MPlayer WW for Windows", NULL);
		if(hWndMP != NULL) {
			if(fd) {
				snprintf(tmps ,MAX_PATH*2, "%s", arg);
				COPYDATASTRUCT cpd;
				cpd.dwData = 0;
				cpd.cbData = strlen(tmps) + 1;
				cpd.lpData = (void*)tmps;
				SendMessage(hWndMP, WM_COPYDATA, 0, (LPARAM)&cpd);
			}
			return TRUE;
		}
	}
	return FALSE;
}

void load_config_ex(void) {
	char *cfg = NULL;
	if(!kk_ini) {
		/* read kk.ini settings */
		cfg = get_path("kk.ini");
		kk_ini = iniparser_load(cfg);
	}
	showlog = iniparser_getint(kk_ini , "Option:log", 0);
	autoplay_fuzziness = iniparser_getint(kk_ini , "Option:autoplay_fuzziness", 1);
	always_use_ass = iniparser_getint(kk_ini , "Option:always_use_ass", 1);
	ass_auto_expand = iniparser_getint(kk_ini , "Option:ass_auto_expand", 0);
	if(ass_auto_expand)
		ass_expand_aspect = iniparser_getint(kk_ini , "Option:ass_expand_aspect", 0);
	osd_percent = iniparser_getint(kk_ini , "OSD:osd_percent", 0);
	osd_systime = iniparser_getint(kk_ini , "OSD:osd_systime", 0);
	use_rightmenu = iniparser_getint(kk_ini , "Gui:use_rightmenu", 0);
	gl_fs_flash = iniparser_getint(kk_ini , "Gui:gl_fs_flash", 0);
	disable_screensaver = iniparser_getint(kk_ini , "Sysinfo:disable_screensaver", 1);
	no_dvdnav = iniparser_getint(kk_ini , "Setting:no_dvdnav", 0);
	info_to_html = iniparser_getint(kk_ini , "Setting:info_to_html", 0);
	always_quit = iniparser_getint(kk_ini , "Option:always_quit", 1);
	loop_all = iniparser_getint(kk_ini , "Setting:loop_all", 0);
	end_pos = iniparser_getint(kk_ini , "Status:end_pos", 0);
	seek_realtime = iniparser_getint(kk_ini , "Setting:seek_realtime", 1);
	reload_when_open = iniparser_getint(kk_ini , "Setting:reload_when_open", 0);
	reload_confirm = iniparser_getint(kk_ini , "Setting:reload_confirm", 1);
	adjust_ts_offset = iniparser_getint(kk_ini , "Setting:adjust_ts_offset", 1);
	use_ss_dir = 0;
	char *str = NULL;
	str = iniparser_getstring(kk_ini, "Setting:screenshot_dir", NULL);
	if(str) {
		strncpy(screenshot_dir,str, MAX_PATH);
		if(strstr(screenshot_dir, ":\\")) {
			use_ss_dir = 1;
			if(_mbsrchr(screenshot_dir, '\\') + 1 != '\0')
				strcat(screenshot_dir,"\\");
		}
	}

	if(disable_screensaver)
		disable_screen_saver(TRUE);

	default_dir =  malloc(MAX_PATH);
	str = iniparser_getstring(kk_ini, "Setting:default_dir", NULL);
	if(str) {
		strncpy(default_dir,str, MAX_PATH);
		if(!strstr(default_dir, ":\\")) {
			free(default_dir);
			default_dir = NULL;
		}
	}

	if(cfg) {
		iniparser_freedict(kk_ini);
		kk_ini = NULL;
		free(cfg);
	}
}

static void detect_theme(void)
{
	HKEY hKEY;
	OSVERSIONINFO version;
	if(using_theme < 0) {
		using_theme = 0;
		char key_theme[] = "Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager";
		if(RegOpenKeyEx(HKEY_CURRENT_USER,key_theme, 0, KEY_READ,&hKEY) == ERROR_SUCCESS) {
			char data[256];
			DWORD type=REG_SZ;
			DWORD dwLength = 256;
			if(RegQueryValueEx(hKEY, "ThemeActive", NULL,&type, data, &dwLength) ==ERROR_SUCCESS) {
				if(data[0] == '1')
					using_theme = 1;
			}
			RegCloseKey(hKEY);
		}
	}
	if(is_vista < 0) {
		is_vista = 0;
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(GetVersionEx(&version)) {
			if(version.dwMajorVersion >= 6) {
				is_vista = 1;
				if(version.dwMinorVersion >= 1)
					is_win7 = 1;
			}
		}
	} else {
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(GetVersionEx(&version)) {
			if(version.dwMajorVersion >= 6 && version.dwMinorVersion >= 1)
				is_win7 = 1;
		}
	}
	if(using_aero < 0) {
		using_aero = 0;
		if(is_vista) {
			char key_aero[] = "Software\\Microsoft\\Windows\\DWM";
			if(RegOpenKeyEx(HKEY_CURRENT_USER,key_aero, 0, KEY_READ,&hKEY) == ERROR_SUCCESS) {
				char data[256];
				DWORD type=REG_SZ;
				DWORD dwLength = 256;
				if(RegQueryValueEx(hKEY, "Composition", NULL,&type, data, &dwLength) ==ERROR_SUCCESS) {
					if(data[0] == 1) {
						if(RegQueryValueEx(hKEY, "CompositionPolicy", NULL,&type, data, &dwLength) ==ERROR_SUCCESS) {
							if(data[0] != 1)
								using_aero = 1;
						}
					}
				}
				RegCloseKey(hKEY);
			}
		}
	}

	if(use_windowblinds) {
		char key[] = "Software\\Stardock\\WindowBlinds\\WB5.ini\\WBLiteFX";
		if(RegOpenKeyEx(HKEY_CURRENT_USER,key, 0, KEY_READ,&hKEY) == ERROR_SUCCESS) {
			char data[256];
			DWORD type=REG_SZ;
			DWORD dwLength = 256;
			data[0] = 0;
			if(RegQueryValueEx(hKEY, "wbon", NULL,&type, data, &dwLength) ==ERROR_SUCCESS) {
				if(data[0] != '1')
					use_windowblinds = 0;
			}
			data[0] = 0;
			if(RegQueryValueEx(hKEY, "INSTANTON", NULL,&type, data, &dwLength) ==ERROR_SUCCESS) {
				if(data[0] != '1')
					use_windowblinds = 0;
			}
			RegCloseKey(hKEY);
		}
		char key2[] = "Software\\Stardock\\WindowBlinds\\WB5.ini\\Installed";
		if(RegOpenKeyEx(HKEY_CURRENT_USER,key2, 0, KEY_READ,&hKEY) == ERROR_SUCCESS) {
			char data[512];
			DWORD type=REG_SZ;
			DWORD dwLength = 512;
			if(RegQueryValueEx(hKEY, "Path", NULL,&type, data, &dwLength) ==ERROR_SUCCESS) {
				if (data) {
					strcat(data, "\\wbperapp.ini");
					if(GetPrivateProfileInt("mplayer.exe", "IgnoreApp", 0, data) == 1)
						use_windowblinds = 0;
				}
			}
			RegCloseKey(hKEY);
		}
	}
}

typedef IDirect3D9 * (WINAPI *ImpDirect3DCreate9)(UINT);

int detect_vo_system(void)
{
	char *tmp, *cmd, module[MAX_PATH+1];
	MEMORYSTATUSEX  memstatus;
	char *cache_size = NULL, *cache_min_size = NULL;
	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
	ImpDirect3DCreate9 pDirect3DCreate9;
	LPDIRECT3D9 d3d_handle;
	HANDLE d3d9;
	int Ret, phys_mem_size;

	if(generate_preview) return 0;

	if(!is_vista && (vo_dirver != VO_DIRV_DIRECT3D))
		controlbar_alpha = 0;

	memstatus.dwLength =sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memstatus);
	phys_mem_size = (int)(memstatus.ullTotalPhys/1024/1024/100);
	if(phys_mem_size > 39)
		_map_buffer_factor = 12;
	else if(phys_mem_size >= 35)
		_map_buffer_factor = 10;
	else if(phys_mem_size >= 25)
		_map_buffer_factor = 8;
	else if(phys_mem_size > 19)
		_map_buffer_factor = 6;
	else if(phys_mem_size >= 15)
		_map_buffer_factor = 4;
	else if(phys_mem_size > 9)
		_map_buffer_factor = 3;
	else if(phys_mem_size >= 5)
		_map_buffer_factor = 2;
	else
		_map_buffer_factor = 1;
	mp_msg(MSGT_VO, MSGL_V, "System Physical memory size: %0.1f GB.\n", (float)phys_mem_size/10.0);
	if(is_vista) {
		if(detect_cache) {
			tmp = get_path("kk.ini");
			WritePrivateProfileString("sysinfo", "detect_cache", "0", tmp);
			free(tmp);
			if(stream_cache_size > 4096) {
				if(sys_Language == 1)
					LoadString(GetModuleHandle(NULL), IDS_AUTO_CACHE_SC, module, MAX_PATH);
				else if(sys_Language == 3 || sys_Language == 4)
					LoadString(GetModuleHandle(NULL), IDS_AUTO_CACHE_TC, module, MAX_PATH);
				else
					strncpy(module, "The version of MPlayer you are using have a new file read method.\nNo longer needed to enable cache when play a local file, Disable cache ?\n\n[Yes] Disable, [No] Not Disable", MAX_PATH);
				if(MessageBox(NULL, module, "MPlayer Cache", MB_YESNO | MB_TOPMOST) == IDYES) {
					tmp = get_path("mplayer.ini");
					WritePrivateProfileString("default", "cache", NULL, tmp);
					WritePrivateProfileString("default", "cache-min", NULL, tmp);
					free(tmp);
				}
			}
		}
	}
	if(detect_directx && is_vista && using_theme && using_aero && vo_dirver == VO_DIRV_DIRECTX) {
		if(sys_Language == 1)
			LoadString(GetModuleHandle(NULL), IDS_AERO_DX_SC, module, MAX_PATH);
		else if(sys_Language == 3 || sys_Language == 4)
			LoadString(GetModuleHandle(NULL), IDS_AERO_DX_TC, module, MAX_PATH);
		else
			strncpy(module, "DirectX video output are not compatible with the Aero effects !\nPlease change it to  Direct3D or OpenGL in the Preferences.\nAutomatic switch to Direct3D video output ?\n\n[Yes] Switch, [No] Not, [Cancel] Do not ask again", MAX_PATH);
		Ret = MessageBox(NULL, module, "MPlayer Directx", MB_YESNOCANCEL | MB_TOPMOST);
		if(Ret == IDYES) {
			tmp = get_path("mplayer.ini");
			WritePrivateProfileString("default", "vo", "direct3d", tmp);
			free(tmp);
			GetModuleFileName(NULL, module, MAX_PATH);
			cmd = GetCommandLine();
			tmp = strstr(cmd, ".exe\"");
			if(tmp) cmd = tmp + 5;
			ShellExecute(0, NULL, module, cmd, NULL, SW_NORMAL);
			return 1;
		} else if(Ret == IDCANCEL) {
			tmp = get_path("kk.ini");
			WritePrivateProfileString("sysinfo", "detect_directx", "0", tmp);
			free(tmp);
		}
	} else if(gl_new_window && is_vista && using_theme && using_aero && vo_dirver == VO_DIRV_OPENGL) {
		d3d9 = LoadLibraryA("d3d9.dll");
		if (!d3d9) return;

		pDirect3DCreate9 = (ImpDirect3DCreate9) GetProcAddress(d3d9, "Direct3DCreate9");
		if (!pDirect3DCreate9) return;

		d3d_handle = pDirect3DCreate9(D3D_SDK_VERSION);
		if(!d3d_handle) return;

		IDirect3D9_GetAdapterIdentifier(d3d_handle, D3DADAPTER_DEFAULT, 0, &AdapterIdentifier);
		IDirect3D9_Release(d3d_handle);
		FreeLibrary(d3d9);

		mp_msg(MSGT_VO, MSGL_INFO, "Video Card: %s\n", AdapterIdentifier.Description);
		if(!strstr(AdapterIdentifier.Description, "ATI")) {
			mp_msg(MSGT_VO, MSGL_V, "Only ATI card can use gui thread for OpenGL!\n");
			always_thread = 0;
		}
	}

	if(!always_thread && is_vista && using_aero && using_theme && vo_dirver == VO_DIRV_OPENGL)
		gui_thread = 0;

	return 0;
}

int TestUnicodeFilename()
{
	int number, i = 0;
	char fname[MAX_PATH*2], tmps[MAX_PATH*2+20];
	wchar_t tmpshot[MAX_PATH*2];

	if(!support_unicode)
		return 0;

	if(GetFileAttributes(filename) != 0xffffffff)
		return 0;



	wchar_t **szArglist = CommandLineToArgvW(GetCommandLineW(), &number);

	if(!szArglist) return 0;

	for(i = 1; i < FFMIN(number, 6); i++) {
		GetShortPathNameW(szArglist[i], tmpshot, MAX_PATH*2);
		wcstombs(fname, tmpshot, MAX_PATH*2);
		if(strstr(fname, ":\\"))
			break;
	}
	LocalFree(szArglist);

	if(GetFileAttributes(fname) != 0xffffffff) {
		free(filename);
		filename = strdup(fname);
	}
}

int win_init(int argc,char** argv) {
	char *tmp;
	int i;

	if (unrardll = (unsigned int)LoadLibrary("unrar.dll")) {
		RAROpenArchiveEx = GetProcAddress((HMODULE)unrardll, "RAROpenArchiveEx");
		RARCloseArchive = GetProcAddress((HMODULE)unrardll, "RARCloseArchive");
		RARReadHeaderEx = GetProcAddress((HMODULE)unrardll, "RARReadHeaderEx");
		RARProcessFile = GetProcAddress((HMODULE)unrardll, "RARProcessFile");
		RARSetCallback = (void __stdcall(*)())GetProcAddress((HMODULE)unrardll, "RARSetCallback");
		RARSetPassword = (void __stdcall(*)())GetProcAddress((HMODULE)unrardll, "RARSetPassword");
	}

	init_paths();

	for(i=0; i<100; i++)
		skins_name[i]=NULL;

	/* read kk.ini settings */
	tmp = get_path("kk.ini");
	screenshot_dir = malloc(MAX_PATH);

	kk_ini = iniparser_load(tmp);

	one_player = iniparser_getint(kk_ini , "Setting:one_player", 0);
	if(detect_one_player(argc, argv, tmp)) {
		iniparser_freedict(kk_ini);
		kk_ini = NULL;
		free(tmp);
		return 0;
	}

	if(generate_preview) {
		iniparser_freedict(kk_ini);
		kk_ini = NULL;
		free(tmp);

		show_controlbar = 0;
		auto_resume = 0;
		skinned_player = 0;
		disable_screensaver = 0;
		return 1;
	}

	load_config_ex();

	gui_thread = iniparser_getint(kk_ini , "Option:gui_thread", 0);
	auto_resume = iniparser_getint(kk_ini , "Option:auto_resume", 0);
	sys_Language = iniparser_getint(kk_ini , "Option:Language", 0);

	support_unicode = iniparser_getint(kk_ini , "Option:support_unicode", 1);
	high_accuracy_timer = iniparser_getint(kk_ini , "Option:high_accuracy_timer", 0);
	if(gui_thread)
		gui_priority_lowest = iniparser_getint(kk_ini , "Gui:gui_priority_lowest", 1);
	if(auto_resume)
		save_stats_num = iniparser_getint(kk_ini , "Setting:save_stats_num", 10);
	show_menubar = iniparser_getint(kk_ini , "Gui:show_menubar", 0);
	auto_hide_control =  iniparser_getint(kk_ini , "Gui:auto_hide_control", 1);
	show_controlbar = iniparser_getint(kk_ini , "Gui:show_controlbar", 1);
	skin_controlbar = iniparser_getint(kk_ini , "Gui:skin_controlbar", 1);
	controlbar_alpha = iniparser_getint(kk_ini , "Gui:controlbar_alpha", 0);
	if(controlbar_alpha > 255 || controlbar_alpha < 0) controlbar_alpha = 0;

	skin_name = strdup(iniparser_getstring(kk_ini, "Gui:skin", NULL));
	skinned_player = iniparser_getint(kk_ini, "Gui:skinned_player", 1);
	initskin(skin_name, &gui_logo, &gui_skin);
	if(show_controlbar && gui_skin.status.show)
		show_status = gui_skin.status.show;

	is_vista = iniparser_getint(kk_ini , "Sysinfo:is_vista", -1);
	using_aero = iniparser_getint(kk_ini , "Sysinfo:using_aero", -1);
	detect_directx = iniparser_getint(kk_ini , "Sysinfo:detect_directx", 1);
	detect_cache = iniparser_getint(kk_ini , "Sysinfo:detect_cache", 1);
	gl_new_window = iniparser_getint(kk_ini , "Sysinfo:gl_new_window", 1);
	always_thread = iniparser_getint(kk_ini , "Sysinfo:always_thread", 1);
	using_theme = iniparser_getint(kk_ini , "Theme:using_theme", -1);
	use_windowblinds = iniparser_getint(kk_ini , "Theme:use_windowblinds", 0);

	if(save_stats_num > MAXSAVESTATS)
		save_stats_num = MAXSAVESTATS;
	else if(save_stats_num < 0)
		save_stats_num = 1;

	save_volume = iniparser_getint(kk_ini , "Status:Volume", 100);

	if(sys_Language < 1 || sys_Language > 4) {
		LANGID _SysLangId = GetSystemDefaultLangID();
		if(PRIMARYLANGID(_SysLangId) == LANG_CHINESE) {
			if(SUBLANGID(_SysLangId) == SUBLANG_CHINESE_SIMPLIFIED)
				sys_Language = 1;
			else
				sys_Language = 4;
		} else
			sys_Language = 2;
	}

	LANGID _SysLangId = GetSystemDefaultLangID();
	if(PRIMARYLANGID(_SysLangId) == LANG_CHINESE) {
		if(SUBLANGID(_SysLangId) == SUBLANG_CHINESE_SIMPLIFIED)
			setlocale( LC_CTYPE, "chs" );
		else
			setlocale( LC_CTYPE, "cht" );
	}

	if(!last_filename)
		last_filename = malloc(MAX_PATH);
	if (auto_resume == 1) {
		reload = iniparser_getint(kk_ini , "Status:Resume", 0);
		if (reload) {
			save_sec = iniparser_getint(kk_ini , "Status:Time", 0);
			save_audio_id = iniparser_getint(kk_ini , "Status:Audio", 0);
			save_dvdsub_id = iniparser_getint(kk_ini , "Status:DVDSub", 0);
			save_vobsub_id = iniparser_getint(kk_ini , "Status:VOBSub", 0);
			save_set_of_sub_pos = iniparser_getint(kk_ini , "Status:TXTSub", 0);
			save_sub_pos = iniparser_getint(kk_ini , "Status:SubPos", 0);
			if(!save_filename)
				save_filename = malloc(MAX_PATH);
			save_filename[0] = 0;
			char *str = iniparser_getstring(kk_ini, "Status:Filename", NULL);
			if(str) {
				strncpy(save_filename,str, MAX_PATH);
			}
			if (GetFileAttributes(save_filename) == 0xFFFFFFFF)
				reload = 0;
			iniparser_setint(kk_ini , "Status:Resume", 0);
			iniparser_save(kk_ini , tmp);
		}
	}

	iniparser_freedict(kk_ini);
	kk_ini = NULL;
	free(tmp);

	if ( save_volume < 0 ) save_volume = 0;

	detect_theme();

	if (showlog == 1) {
		log_filename = get_path("log.txt");
		if (!logfile) {
			logfile = fopen(log_filename, "w");
			*stdout = *logfile;
			*stderr = *logfile;
		}
	}
	else if (showlog) {
		AllocConsole();
		hStdOut= GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle("MPlayer Log Window");
	}

	memset(&cf, 0, sizeof(cf));
	memset(&font, 0, sizeof(font));
	strcpy(font.lfFaceName, "MS Shell Dlg");
	cf.lStructSize = sizeof(cf);
	cf.lpLogFont = &font;
	cf.Flags = CF_NOSCRIPTSEL|CF_SCREENFONTS|CF_NOVERTFONTS|CF_TTONLY|CF_INITTOLOGFONTSTRUCT;

	return 1;
}

void win_uninit(int rc){
	int i;

	disable_screen_saver(FALSE);

	for(i=0; i<100; i++) {
		if(skins_name[i])
			free(skins_name[i]);
	}

	if(hEqThread) {
		DestroyWindow(hwnd_eq);
		hEqThread = NULL;
	}

	if (logfile) {
		fclose(logfile);
		logfile = NULL;
		if (rc || fatal) ShellExecute(0, NULL, log_filename, NULL, NULL, SW_SHOWNORMAL);
	}
	if (unrardll) FreeLibrary((HMODULE)unrardll);

	if(save_filename)
		free(save_filename);
	if(last_filename)
		free(last_filename);
	if(screenshot_dir)
		free(screenshot_dir);
	if(default_dir)
		free(default_dir);
	if(mplayer_pls) {
		DeleteFile(mplayer_pls);
		free(mplayer_pls);
	}
	if(mplayer_avs) {
		DeleteFile(mplayer_avs);
		free(mplayer_avs);
	}
	guiCommand(CMD_UNINIT_WINDOW, 0);
	freelogo(&gui_logo);
	freeskin(&gui_skin);
	freeappskin(&gui_skin);
	if (hThread) {
		//TerminateThread(hThread, 0);
		PostThreadMessage((DWORD)hThread, WM_QUIT, 0, 0);
		CloseHandle(hThread);
	}

}

extern int vo_gamma_gamma;
extern int vo_gamma_brightness;
extern int vo_gamma_saturation;
extern int vo_gamma_contrast;
static HWND hWndSlideB = NULL;
static HWND hWndSlideC = NULL;
static HWND hWndSlideS = NULL;
static HWND hWndSlideG = NULL;
static BOOL (WINAPI* SetLayeredWinAttributes)(HWND, COLORREF, BYTE, DWORD) = NULL;

static LRESULT CALLBACK EqualizerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
			ShowWindow(hwnd_eq, SW_HIDE);
		break;
	case WM_CLOSE:
		ShowWindow(hwnd_eq, SW_HIDE);
		break;
	case WM_HSCROLL:
		if (LOWORD(wParam) == TB_ENDTRACK || LOWORD(wParam) == TB_THUMBTRACK)
		{
			int x;
			char tmps[32];
			if ((HWND)lParam == hWndSlideB) {
				x = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
				sprintf(tmps, "brightness %d 1", x);
			}
			mp_input_queue_cmd(mp_input_parse_cmd(tmps));
		}
		return TRUE;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

static LRESULT CALLBACK EqualizerDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND :
		switch (LOWORD (wParam))
		{
			case IDOK:
			{
				char tmps[32];
				PostMessage(hWndSlideB, TBM_SETPOS, TRUE, 0);
				PostMessage(hWndSlideC, TBM_SETPOS, TRUE, 0);
				PostMessage(hWndSlideS, TBM_SETPOS, TRUE, 0);
				PostMessage(hWndSlideG, TBM_SETPOS, TRUE, 0);
				sprintf(tmps, "brightness 0 1");
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				sprintf(tmps, "contrast 0 1");
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				sprintf(tmps, "saturation 0 1");
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				sprintf(tmps, "gamma 0 1");
				mp_input_queue_cmd(mp_input_parse_cmd(tmps));
				return TRUE;
			}
		}
		break;
	case WM_HSCROLL:
		if(filename && (LOWORD(wParam) == TB_ENDTRACK || LOWORD(wParam) == TB_THUMBTRACK))
		{
			int x;
			char tmps[32];
			if ((HWND)lParam == hWndSlideB) {
				x = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
				sprintf(tmps, "brightness %d 1", x);
			} else if ((HWND)lParam == hWndSlideC) {
				x = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
				sprintf(tmps, "contrast %d 1", x);
			} else if ((HWND)lParam == hWndSlideS) {
				x = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
				sprintf(tmps, "saturation %d 1", x);
			} else if ((HWND)lParam == hWndSlideG) {
				x = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
				sprintf(tmps, "gamma %d 1", x);
			}
			mp_input_queue_cmd(mp_input_parse_cmd(tmps));
		}
		return TRUE;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

static int init_equalizer(HWND hWnd)
{
	char title[64];
	int x, y, w, h, xdpi, ydpi;
	RECT rc;
	HDC dc;
	HINSTANCE hInst, user32;

	SetLayeredWinAttributes = NULL;
	hInst = GetModuleHandle(NULL);

	if(sys_Language == 1)
		strcpy(title,"均衡器");
	else
		strcpy(title,"Equalizer");

	WNDCLASS wc;
	wc.hInstance = hInst;
	wc.lpszClassName = "MPlayer Equalizer Window";
	wc.lpfnWndProc = (WNDPROC)EqualizerProc;
	wc.style = CS_DBLCLKS;
	wc.hIcon = LoadIcon(hInst, IDI_APPICON);
	wc.hCursor = NULL;
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	if(!RegisterClass(&wc))
		return 0;

	GetWindowRect(hWnd, &rc);
	w = 400;
	h = 240;
	x = (rc.right - w) / 2;
	y = (rc.bottom - h) / 2;

	dc = GetDC(hWnd);
	xdpi = GetDeviceCaps(dc, LOGPIXELSX);
	ydpi = GetDeviceCaps(dc, LOGPIXELSY);
	if(xdpi > 96)
		w = w*xdpi*105/9600;
	if(ydpi > 96)
		h = h*ydpi/96;

	hwnd_eq = CreateWindowEx (WS_EX_APPWINDOW,  "MPlayer Equalizer Window", title,
		WS_POPUP | WS_SYSMENU | WS_CAPTION, x, y, w, h, hWnd,  NULL, hInst, NULL );

	if((vo_dirver != VO_DIRV_OPENGL || is_vista) && controlbar_alpha > 0) {
		user32 = GetModuleHandle("user32.dll");
		if (user32) {
			SetLayeredWinAttributes = GetProcAddress(user32, "SetLayeredWindowAttributes");
			if(SetLayeredWinAttributes) {
				SetWindowLong(hwnd_eq, GWL_EXSTYLE, GetWindowLong(hwnd_eq ,GWL_EXSTYLE) | WS_EX_LAYERED );
				SetLayeredWinAttributes(hwnd_eq, 0, controlbar_alpha, 2);
			}
		}
	}

	if(!hwnd_eq) return 0;
	HWND hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_EQ), hwnd_eq, EqualizerDlgProc);

	if(!hDlg) return 0;

	hWndSlideB = GetDlgItem(hDlg, IDC_SLIDERB);
	hWndSlideC = GetDlgItem(hDlg, IDC_SLIDERC);
	hWndSlideS = GetDlgItem(hDlg, IDC_SLIDERS);
	hWndSlideG = GetDlgItem(hDlg, IDC_SLIDERG);

	if(sys_Language == 1) {
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_B), "亮度");
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_C), "对比度");
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_S), "饱和度");
		SetWindowText(GetDlgItem(hDlg, IDC_STATIC_G), "伽玛");
		SetWindowText(GetDlgItem(hDlg, IDOK), "重置");
	}

	x = get_video_equalizer("brightness");
	if(x < -100 || x > 100) x = 0;
	PostMessage(hWndSlideB, TBM_SETRANGEMAX, FALSE, 100);
	PostMessage(hWndSlideB, TBM_SETRANGEMIN, FALSE, -100);
	PostMessage(hWndSlideB, TBM_SETPOS, TRUE, 1);
	PostMessage(hWndSlideB, TBM_SETPOS, TRUE, x);

	x = get_video_equalizer("contrast");
	if(x < -100 || x > 100) x = 0;
	PostMessage(hWndSlideC, TBM_SETRANGEMAX, FALSE, 100);
	PostMessage(hWndSlideC, TBM_SETRANGEMIN, FALSE, -100);
	PostMessage(hWndSlideC, TBM_SETPOS, TRUE, 1);
	PostMessage(hWndSlideC, TBM_SETPOS, TRUE, x);

	x = get_video_equalizer("saturation");
	if(x < -100 || x > 100) x = 0;
	PostMessage(hWndSlideS, TBM_SETRANGEMAX, FALSE, 100);
	PostMessage(hWndSlideS, TBM_SETRANGEMIN, FALSE, -100);
	PostMessage(hWndSlideS, TBM_SETPOS, TRUE, 1);
	PostMessage(hWndSlideS, TBM_SETPOS, TRUE, x);

	x = get_video_equalizer("gamma");
	if(x < -100 || x > 100) x = 0;
	PostMessage(hWndSlideG, TBM_SETRANGEMAX, FALSE, 100);
	PostMessage(hWndSlideG, TBM_SETRANGEMIN, FALSE, -100);
	PostMessage(hWndSlideG, TBM_SETPOS, TRUE, 1);
	PostMessage(hWndSlideG, TBM_SETPOS, TRUE, x);

	ShowWindow(hwnd_eq, SW_SHOW);

	UpdateWindow(hwnd_eq);

	SetWindowPos(hwnd_eq,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

	return 1;
}

static DWORD WINAPI EqualizerThread(LPVOID lpParam)
{
	HWND hWnd = (HWND) lpParam;
	SetThreadPriority(hEqThread, THREAD_PRIORITY_LOWEST);

	MSG messages;
	if(!init_equalizer(hWnd))
		return 0;

	while (hEqThread && GetMessage(&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return 0;
}

void ShowEqualizer(HWND hWnd)
{
	int eq;
	if(!filename)
		return;

	if(hEqThread) {
		eq = get_video_equalizer("brightness");
		if(eq < -100 || eq > 100) eq = 0;
		PostMessage(hWndSlideB, TBM_SETPOS, TRUE, eq);
		eq = get_video_equalizer("contrast");
		if(eq < -100 || eq > 100) eq = 0;
		PostMessage(hWndSlideC, TBM_SETPOS, TRUE, eq);
		eq = get_video_equalizer("saturation");
		if(eq < -100 || eq > 100) eq = 0;
		PostMessage(hWndSlideS, TBM_SETPOS, TRUE, eq);
		eq = get_video_equalizer("gamma");
		if(eq < -100 || eq > 100) eq = 0;
		PostMessage(hWndSlideG, TBM_SETPOS, TRUE, eq);
		ShowWindow(hwnd_eq, SW_SHOW);
	} else {
		hEqThread = CreateThread(NULL, 0, EqualizerThread, hWnd, 0, NULL);
	}
}
