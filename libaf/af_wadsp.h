/*
 * Winamp DSP plugin audio filter
 * License: GPL
 * Copyright (c) 2005,06 Stanley Huang <stanleyhuangyc@yahoo.com.cn>
 * Copyright (c) 2005 Erik Augustson <erik_27can@yahoo.com>
 * Copyright (c) 2005-2009 Gianluigi Tiesi <sherpya@netfarm.it>
 * This code only works on Win32/MinGW
 *
 * Winamp DSP API Copyright (c) NullSoft
 */

#ifndef _AF_WADSP_H_
#define _AF_WADSP_H_

#include <windows.h>

/* DSP plugin interface

    notes:
    any window that remains in foreground should optimally pass unused
    keystrokes to the parent (winamp's) window, so that the user
    can still control it. As for storing configuration,
    Configuration data should be stored in <dll directory>\plugin.ini
    (look at the vis plugin for configuration code)
*/
typedef struct winampDSPModule
{
    char *description;      // description
    HWND hwndParent;        // parent window (filled in by calling app)
    HINSTANCE hDllInstance; // instance handle to this DLL (filled in by calling app)

    void (*Config) (struct winampDSPModule *this_mod);  // configuration dialog (if needed)
    int (*Init) (struct winampDSPModule *this_mod);     // 0 on success, creates window, etc (if needed)

    /*
        modify waveform samples: returns number of samples to actually write
        (typically numsamples, but no more than twice numsamples, and no less than half numsamples)
        numsamples should always be at least 128. should, but I'm not sure
    */
    int (*ModifySamples) (struct winampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate);

    void (*Quit) (struct winampDSPModule *this_mod);    // called when unloading

    void *userData; // user data, optional
} winampDSPModule;

typedef struct
{
    int version;                            // DSP_HDRVER
    char *description;                      // description of library
    winampDSPModule *(*getModule) (int);    // module retrieval function
    int (*sf)(int key);                     // DSP_HDRVER == 0x21
} winampDSPHeader;

/* exported symbols */
typedef winampDSPHeader *(*winampDSPGetHeaderType)(void);

/* header version: 0x20 == 0.20 == winamp 2.0 */
#define DSP_HDRVER 0x20

#endif // _AF_WADSP_H_
