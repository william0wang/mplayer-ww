#include "message.h"

#if defined(__MINGW32__) || defined(__CYGWIN__)
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define WNDCLASSNAME_MESSAGE  "Message Windows"
static HWND hwnd_msg;
static HINSTANCE hInst;
static int exit_message = 0;
static int exit_time = 0;
static char str_msg[256];
static HANDLE hMessageThread = NULL;
extern int sys_Language;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		KillTimer(hwnd, 0);
		exit_message = 1;
		break;
	case WM_TIMER:
		KillTimer(hwnd, 0);
		exit_message = 1;
		return TRUE;
	default:
		return DefWindowProc (hwnd, message, wParam, lParam);
	}
	return 0;
}

static int init_message()
{
	hInst = GetModuleHandle(NULL);
	char title[64];
	if(sys_Language == 1)
		strcpy(title,"MPlayer 提示");
	else if(sys_Language == 3 || sys_Language == 4)
		LoadString(hInst, IDS_MSGTITLE_TC, title, 64);
	else
		strcpy(title,"MPlayer Prompted");
	
	WNDCLASS wc;
	wc.hInstance = hInst;
	wc.lpszClassName = WNDCLASSNAME_MESSAGE;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.style = CS_DBLCLKS;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	if(!RegisterClass(&wc))
		return 0;

	int w = 260;
	int h = 100;
	int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

	hwnd_msg = CreateWindowEx (0,  WNDCLASSNAME_MESSAGE, title,
		WS_POPUP|WS_CAPTION, x,  y, w, h,HWND_DESKTOP,  NULL, hInst, NULL );

	if(!hwnd_msg)
		return 0;
	HWND hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CONTROL_MESSAGE), hwnd_msg, NULL);
	if(!hDlg)
		return 0;
	if(exit_time > 20) {
		SetWindowLong(hwnd_msg, GWL_STYLE, CS_DBLCLKS | WS_CAPTION | WS_SYSMENU);
		SetTimer(hwnd_msg, 0, exit_time, 0);
	}
	MoveWindow(hDlg, 0, 0, w, h, FALSE);
	SetDlgItemText(hDlg,IDC_TEXT_MESSAGE,str_msg);
	ShowWindow(hwnd_msg, SW_SHOW);
	UpdateWindow(hwnd_msg);
	SetWindowPos(hwnd_msg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	return 1;
}

static DWORD WINAPI messageProc(LPVOID lpParam)
{
	SetThreadPriority(hMessageThread, THREAD_PRIORITY_LOWEST);
	int i = 5;
	while (i > 0)
	{
		if(exit_message)
			return 0;
		Sleep(200);
		--i;
	}
	MSG messages;
	if(!init_message())
		return 0;
	while (1)
	{
		if(exit_message)
			return 0;
		GetMessage(&messages, NULL, 0, 0);
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	return 0;
}

static void StartMessageThread(void)
{
	hMessageThread = CreateThread(NULL, 0, messageProc, NULL, 0, NULL);
}

/*
* Auto Shutdown.
*/
static void ShutdownPC()
{
	TOKEN_PRIVILEGES   tkp,tkpNewButIgnored;
	HANDLE   hdlTokenHandle;
	LUID   tmpLuid;
	BOOL   rc;
	DWORD   lBufferNeeded;
	HANDLE hdlProcessHandle = GetCurrentProcess();
	rc = OpenProcessToken(hdlProcessHandle,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hdlTokenHandle);

	rc = LookupPrivilegeValue("","SeShutdownPrivilege", &tmpLuid);
	tkp.PrivilegeCount   =   1;   // One privilege to set
	tkp.Privileges[0].Luid   =   tmpLuid;
	tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;
	rc = AdjustTokenPrivileges(hdlTokenHandle, FALSE, &tkp,
		sizeof(tkpNewButIgnored), &tkpNewButIgnored, &lBufferNeeded);
	ExitWindowsEx(EWX_SHUTDOWN,0);
}

static int timer=0;
static char title[64], str_text[256], str_timer[256];

LRESULT CALLBACK ShutdownProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG :
		timer = 30;
		if(sys_Language == 1) {
			LoadString(hInst, IDS_SHUTDOWN_TITLE_SC, title, 64);
			LoadString(hInst, IDS_SHUTDOWN_TEXT_SC, str_text, 256);
		} else if(sys_Language == 3 || sys_Language == 4) {
			LoadString(hInst, IDS_SHUTDOWN_TITLE_TC, title, 64);
			LoadString(hInst, IDS_SHUTDOWN_TEXT_TC, str_text, 256);
		} else {
			strcpy(title,"Auto Shutdown");
			strcpy(str_text,"seconds left!");
		}
		sprintf(str_timer, "%d %s", timer, str_text);
		SetWindowText(hDlg, title);
		SetDlgItemText(hDlg,IDC_TEXT_SHUTDOWN,str_timer);
		if(sys_Language != 1 && sys_Language != 3 && sys_Language != 4)
			SetDlgItemText(hDlg,IDCANCEL,"Cancel");
		SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		SetTimer(hDlg, 0, 1000, 0);
		ShowCursor(TRUE);
		break;
	case WM_TIMER:
		ShowCursor(TRUE);
		timer--;
		if(timer <= 0)
		{
			KillTimer(hDlg, 0);
			EndDialog(hDlg, 0);
			ShutdownPC();
			return TRUE;
		}
		snprintf(str_timer, 256, "%d %s", timer, str_text);
		SetDlgItemText(hDlg,IDC_TEXT_SHUTDOWN,str_timer);
		SetForegroundWindow(hwnd_msg);
		break;
	case WM_COMMAND :
		switch (LOWORD (wParam))
		{
			case IDOK :
			case IDCANCEL :
				KillTimer(hDlg, 0);
				EndDialog (hDlg, wParam);
				return TRUE ;
		}
		break;
	default:
		return FALSE;   //Message not handled
	}
	return TRUE;
}
#endif

void show_message(int type)
{
#if defined(__MINGW32__) || defined(__CYGWIN__)
	switch (type)
	{
		case MESSAGE_TYPE_FONTCONFIG:
		{
			if(sys_Language == 1)
				LoadString(hInst, IDS_MSGTEXTFONT_SC, str_msg, 256);
			else if(sys_Language == 3 || sys_Language == 4)
				LoadString(hInst, IDS_MSGTEXTFONT_TC, str_msg, 256);
			else
				strcpy(str_msg,"Updating font cache, please wait...");
			break;
		}
		//case MESSAGE_TYPE_STREAM:
		//{
		//	if(sys_Language == 1)
		//		strcpy(str_msg,"正在连接网络并缓冲，请稍等...");
		//	else if(sys_Language == 3 || sys_Language == 4)
		//		LoadString(hInst, IDS_MSGTEXTSTREAM_TC, str_msg, 256);
		//	else
		//		strcpy(str_msg,"Connect network and caching, please wait...");
		//	break;
		//}
		default:
			return;
	}
	exit_time = 0;
	exit_message = 0;
	StartMessageThread();
#endif
}

void end_message(void)
{
	exit_message = 1;
}

void pop_message(int type, int time)
{
#if defined(__MINGW32__) || defined(__CYGWIN__)
	switch (type)
	{
		case MESSAGE_TYPE_D3DX9:
		{
			if(sys_Language == 1)
				LoadString(hInst, IDS_D3DX9_DLL_SC, str_msg, 256);
			else if(sys_Language == 3 || sys_Language == 4)
				LoadString(hInst, IDS_D3DX9_DLL_TC, str_msg, 256);
			else
				strcpy(str_msg,"Unable to load d3dx9_24.dll\nPlease install DirectX 9.0 c");
			break;
		}
		default:
			return;
	}
	exit_time = time;
	exit_message = 0;
	StartMessageThread();
#endif
}

int auto_shutdown=0;

void shutdown_when_necessary(HWND hWnd)
{
#if defined(__MINGW32__) || defined(__CYGWIN__)
	if(auto_shutdown) {
		hInst = GetModuleHandle(NULL);
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SHUTDOWN), hWnd, ShutdownProc);
	}
#endif
}

