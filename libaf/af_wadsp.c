/*
 * Winamp DSP plugin audio filter - This code only works on Win32
 * License: GPL
 * Copyright (c) 2005,06 Stanley Huang <stanleyhuangyc@yahoo.com.cn>
 * Copyright (c) 2005 Erik Augustson <erik_27can@yahoo.com>
 * Copyright (c) 2005-2009 Gianluigi Tiesi <sherpya@netfarm.it>
 *
 * 11/17/2009 - fixed code to work with current svn, cleanups
 *
 */

#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <windows.h>
#include "af.h"
#include "af_wadsp.h"
#include "mp_msg.h"

#define MAX_PLUGINS  8

static winampDSPModule* mod[MAX_PLUGINS];
static winampDSPHeader* hdr[MAX_PLUGINS];

static int class_count = 1;
static int init_state = 0;
static int dsp_count = 0;
static HANDLE hPluginThread = NULL;
static void* hParentWindow = NULL;
static HANDLE hEventInit;

BOOL APIENTRY WaDSPWndProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
    switch (message)
    {
        case WM_QUIT:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hDlg, message, wParam, lParam);
    }
    return 0;
}

/* Main thread proc */
DWORD WINAPI mainProc(LPVOID lpParam)
{
    WNDCLASS wc;
    MSG msg;
    int i, working = 0;
    const char *szWindowClass = class_count?"wadspclass":"wadspclassx";
    class_count = !class_count;

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) WaDSPWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = szWindowClass;

    if (!RegisterClass(&wc))
    {
        mp_msg(MSGT_AFILTER, MSGL_ERR, "[wadsp] Unable to register window class\n");
        SetEvent(hEventInit);
        return -1;
    }

    hParentWindow = CreateWindow(szWindowClass, "mp_wadsp", WS_DISABLED, 0, 0, 0, 0, HWND_DESKTOP, NULL, wc.hInstance, NULL);

    if (!hParentWindow)
    {
        mp_msg(MSGT_AFILTER, MSGL_ERR, "[wadsp] Unable to create window\n");
        UnregisterClass("wadspclass", 0);
        UnregisterClass("wadspclassx", 0);
        SetEvent(hEventInit);
        return -1;
    }

    ShowWindow(hParentWindow, SW_HIDE);

    mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] CreateWindow OK!\n");

    for (i = 0; i < dsp_count; i++)
    {
        int ret = 0;
        mod[i]->hwndParent = hParentWindow;
        if ((ret = mod[i]->Init(mod[i])) == 0)
            working++;
        mp_msg(MSGT_AFILTER, MSGL_INFO, "[wadsp] %s: %s\n", mod[i]->description, ret ? "failed" : "ok");
    }

    if (working < 1)
    {
        mp_msg(MSGT_AFILTER, MSGL_INFO, "[wadsp] No plugin started\n");
        SetEvent(hEventInit);
        return -1;
    }

    init_state = 1;
    SetEvent(hEventInit);
    mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] message loop started\n");

    /* the almighty Windows message loop */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] message loop ended\n");
    hParentWindow = NULL;
    init_state = 0;
    UnregisterClass("wadspclass", 0);
    UnregisterClass("wadspclassx", 0);
    return 0;
}

static int wadsp_load_plugins(char *arg)
{
    winampDSPHeader *(*pfnGetHeader)(void);
    char *p, *s;
    char path[MAX_PATH];
    unsigned char dsp_flags[MAX_PLUGINS];
    HMODULE hDllPlugin;
    int mod_index;

    memset(mod, 0, sizeof(mod));

    /* urgh */
    for (p = strtok(arg, ";"); p; p = strtok(NULL, ";"))
    {
        mod_index = 0;
        s = p;
        while (*p && !(*p == ':' && (*(p + 1) != '\\'))) p++;

        dsp_flags[dsp_count] = 0;
        while(*p)
        {
            char ch, *s2;
            *(p++) = 0;
            s2 = p;
            while (*p && *p != ':') p++;
            ch = *p;
            *p = 0;
            if (!strncmp(s2, "mod=", 4))
                mod_index = atoi(s2 + 4);
            else if (!strncmp(s2, "cfg=", 4))
                dsp_flags[dsp_count] = 1;
            *p = ch;
        }

        mp_msg(MSGT_AFILTER, MSGL_INFO, "[wadsp] loading %s\n", s);
        if (!(strchr(s, '\\') || strchr(s, '/')))
        {
            snprintf(path, MAX_PATH, "codecs/plugins/%s.dll", s);
            s = path;
        }

        if (!(hDllPlugin = LoadLibraryExA(s, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)))
        {
            mp_msg(MSGT_AFILTER, MSGL_ERR, "[wadsp] Cannot load %s: GetLastError() returned %ld\n", s, GetLastError());
            continue;
        }

        pfnGetHeader = (void *) GetProcAddress(hDllPlugin, "winampDSPGetHeader2");
        if (!pfnGetHeader) continue;

        hdr[dsp_count] = (*pfnGetHeader)();
        mod[dsp_count] = hdr[dsp_count]->getModule(mod_index);

        if (!mod[dsp_count])
        {
            FreeLibrary(hDllPlugin);
            mp_msg(MSGT_AFILTER, MSGL_ERR, "[wadsp] invalid module %s\n", s);
            continue;
        }

        mod[dsp_count]->hDllInstance = hDllPlugin;
        mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] adding %d: %s\n", mod_index, mod[dsp_count]->description);
        dsp_count++;
    }

    if (dsp_count > 0)
    {
        DWORD ret;
        int i;
        hEventInit = CreateEvent(0, 0, 0, 0);
        hPluginThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) mainProc, NULL, 0, &ret);
        WaitForSingleObject(hEventInit, INFINITE);
        CloseHandle(hEventInit);
        mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] thread done\n");

        if (!init_state) return -1;

        for (i = 0; i < dsp_count; i++)
        {
            if (dsp_flags[i])
            {
                mp_msg(MSGT_AFILTER, MSGL_INFO, "[wadsp] Requesting config dialog for %s\n", mod[i]->description);
                mod[i]->Config(mod[i]);
            }
        }
    }

    return 0;
}

static void wadsp_control(char *arg)
{
    char *p;
    int idx = 0;
    p = strchr(arg, ',');
    if (p)
    {
        *(p++) = 0;
        idx = atoi(arg);
    }
    else
        p = arg;

    if (!strcmp(p,"cfg"))
        mod[idx]->Config(mod[idx]);
    else if (!strncmp(p, "mod=", 4))
    {
        winampDSPModule *new_mod;
        new_mod = hdr[idx]->getModule(atoi(p + 4));
        if (new_mod) mod[idx] = new_mod;
        mp_msg(MSGT_AFILTER, MSGL_INFO, "[wadsp] module %d: %s\n",idx, mod[idx]->description);
    }
}

/* Initialization and runtime control */
static int control(struct af_instance_s *af, int cmd, void *arg)
{
    switch(cmd)
    {
        case AF_CONTROL_WADSP:
            wadsp_control(arg);
            return AF_OK;
        case AF_CONTROL_COMMAND_LINE:
        {
            if(hParentWindow) return AF_OK; /* module's already loaded */
            if(wadsp_load_plugins(arg))
                mp_msg(MSGT_AFILTER, MSGL_ERR, "[wadsp] unable to load specified plugin\n");
            return AF_OK;
        }
        case AF_CONTROL_REINIT:
        {
            memcpy(af->data, (af_data_t *) arg, sizeof(af_data_t));
            mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] reinit: %iHz/%ich/%s\n",
                   af->data->rate, af->data->nch, af_fmt2str_short(af->data->format));
            return AF_OK;
        }
    }
    return AF_UNKNOWN;
}

/* Close stuff and deallocate memory */
static void uninit(struct af_instance_s *af)
{
    int idx;
    mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] uninit\n");

    if (hParentWindow) SendMessage(hParentWindow, WM_QUIT, 0, 0);

    for (idx = 0; idx < dsp_count; idx++)
    {
        if (mod[idx]->hDllInstance)
        {
            mp_msg(MSGT_AFILTER, MSGL_V, "[wadsp] Unloading %s\n", mod[idx]->description);
            mod[idx]->Quit(mod[idx]);
            FreeLibrary(mod[idx]->hDllInstance);
        }
    }
    dsp_count = 0;

    if(af->data) free(af->data);
    if(af->setup) free(af->setup);
}

/* Filter data through filter */
static af_data_t* play(struct af_instance_s *af, af_data_t *data)
{
    int i;
    for (i=0; i<dsp_count; i++)
        if (mod[i]) mod[i]->ModifySamples(mod[i], data->audio, data->len / 4, data->bps * 8, data->nch, data->rate);
    return data;
}

/* Allocate memory and set function pointers */
static int wadsp_open(af_instance_t *af)
{
    af->control = control;
    af->uninit = uninit;
    af->play = play;
    af->mul = 1;
    af->data = malloc(sizeof(af_data_t));
    if(!af->data) return AF_ERROR;
    return AF_OK;
}

/* Description of this filter */
af_info_t af_info_wadsp = {
    "Winamp DSP plugin",
    "wadsp",
    "Stanley Huang",
    "",
    AF_FLAGS_REENTRANT,
    wadsp_open
};

#endif /* _WIN32 */
