#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "vd_internal.h"

#include "loader/wine/winbase.h"

#ifdef WIN32_LOADER
#include "loader/ldt_keeper.h"
#endif

#define DSN_OK 0
#define DSN_API_VERSION 7

typedef struct _DSVideoCodec DSVideoCodec;
typedef DSVideoCodec * (WINAPI *funcDSOpenVideoCodec)(const char *dll, const GUID guid, BITMAPINFOHEADER* bih,
                                                        unsigned int outfmt, float fps, const char *filename, int mpegts, int *err);
typedef void (WINAPI *funcDSCloseVideoCodec)(DSVideoCodec *codec);
typedef int (WINAPI *funcDSVideoDecode)(DSVideoCodec *vcodec, const BYTE *src, int size, double pts,
                                          double *newpts, BYTE *pImage, int keyframe);
typedef int (WINAPI *funcDSVideoResync)(DSVideoCodec *codec, double pts);
typedef const char * (WINAPI *funcDSStrError)(int error);
typedef unsigned int (WINAPI *funcDSGetApiVersion)(void);

static const vd_info_t info = {
    "DirectShow video codecs (native win32)",
    "dshownative",
    "Gianluigi Tiesi",
    "using dshownative external dll wrapper",
    "win32 codecs"
};

static struct
{
    HMODULE hLib;
    funcDSOpenVideoCodec DSOpenVideoCodec;
    funcDSCloseVideoCodec DSCloseVideoCodec;
    funcDSVideoDecode DSVideoDecode;
    funcDSVideoResync DSVideoResync;
    funcDSGetApiVersion DSGetApiVersion;
    funcDSStrError DSStrError;
    DSVideoCodec *codec;
#ifdef WIN32_LOADER
    ldt_fs_t* ldt_fs;
#endif
} dsn;

LIBVD_EXTERN(dshownative)

int special_codec = 0;
extern int is_mpegts_format;

static int control(sh_video_t *sh, int cmd, void *arg, ...)
{
    switch (cmd)
    {
        case VDCTRL_RESYNC_STREAM:
            dsn.DSVideoResync(dsn.codec, sh->pts);
            return CONTROL_TRUE;
        default:
            return CONTROL_UNKNOWN;
    }
}

static int init(sh_video_t *sh)
{
    int err;
    char fname[MAX_PATH + 1] = "";
    memset(&dsn, 0, sizeof(dsn));
#ifdef WIN32_LOADER
    dsn.ldt_fs = Setup_LDT_Keeper();
#endif
    dsn.hLib = LoadLibraryA("dshownative.dll");

    if (!dsn.hLib)
    {
        mp_msg(MSGT_DECVIDEO, MSGL_ERR, "[dshownative] Cannot find dshownative.dll: %ld\n", GetLastError());
        return 0;
    }

    dsn.DSOpenVideoCodec = (funcDSOpenVideoCodec) GetProcAddress(dsn.hLib, "DSOpenVideoCodec");
    dsn.DSCloseVideoCodec = (funcDSCloseVideoCodec) GetProcAddress(dsn.hLib, "DSCloseVideoCodec");
    dsn.DSVideoDecode = (funcDSVideoDecode) GetProcAddress(dsn.hLib, "DSVideoDecode");
    dsn.DSVideoResync = (funcDSVideoResync) GetProcAddress(dsn.hLib, "DSVideoResync");
    dsn.DSStrError = (funcDSStrError) GetProcAddress(dsn.hLib, "DSStrError");
    dsn.DSGetApiVersion = (funcDSGetApiVersion) GetProcAddress(dsn.hLib, "DSGetApiVersion");

    if (!(dsn.DSOpenVideoCodec && dsn.DSCloseVideoCodec && dsn.DSVideoDecode && dsn.DSVideoResync && dsn.DSStrError && dsn.DSGetApiVersion))
    {
        mp_msg(MSGT_DECVIDEO, MSGL_ERR, "[dshownative] dshownative dll symbol mismatch\n");
        return 0;
    }
    if (dsn.DSGetApiVersion() != DSN_API_VERSION)
    {
        mp_msg(MSGT_DECVIDEO, MSGL_ERR, "[dshownative] Incompatible API version\n");
        return 0;
    }

    if (sh->ds->demuxer->filename)
        GetFullPathNameA(sh->ds->demuxer->filename, MAX_PATH, fname, NULL);

    if (!(dsn.codec = dsn.DSOpenVideoCodec(sh->codec->dll, sh->codec->guid, sh->bih,
                                           sh->codec->outfmt[sh->outfmtidx], sh->fps, fname, is_mpegts_format, &err)))
    {
        mp_msg(MSGT_DECVIDEO, MSGL_ERR, "[dshownative] Codec init failed: %s\n", dsn.DSStrError(err));
        return 0;
    }

    if (!mpcodecs_config_vo(sh, sh->disp_w, sh->disp_h, IMGFMT_YUY2))
    {
        mp_msg(MSGT_DECVIDEO, MSGL_ERR, "[dshownative] mpcodecs_config_vo() failed\n");
        return 0;
    }

	special_codec = 1;
    mp_msg(MSGT_DECVIDEO, MSGL_V, "INFO: [dshownative] video codec init OK.\n");
    return 1;
}

static void uninit(sh_video_t *sh)
{
    if (dsn.codec) dsn.DSCloseVideoCodec(dsn.codec);
    if (dsn.hLib) FreeLibrary(dsn.hLib);
#ifdef WIN32_LOADER
    Restore_LDT_Keeper(dsn.ldt_fs);
#endif
}

// decode a frame
static mp_image_t* decode(sh_video_t *sh, void* data,int len, int flags)
{
    mp_image_t* mpi;
    unsigned char *planes;
    int err;
    double newpts;
    int keyframe;

    if (len <= 0)
        return NULL; // skipped frame

    mpi = mpcodecs_get_image(sh, MP_IMGTYPE_TEMP, MP_IMGFLAG_COMMON_PLANE /*MP_IMGFLAG_PRESERVE*/, sh->disp_w, sh->disp_h);
    if (!mpi) return NULL;

    keyframe = sh->ds->flags & 1;

    if (!keyframe && (flags & 3))
        planes = NULL;
    else
        planes = mpi->planes[0];

    if ((err = dsn.DSVideoDecode(dsn.codec, data, len, sh->pts, &newpts, planes, keyframe) == DSN_OK))
    {
        sh->pts = newpts;
        return mpi;
    }
    else
    {
        //mp_msg(MSGT_DECVIDEO, MSGL_ERR, "[dshownative] Decoding failed: %s\n", dsn.DSStrError(err)); 
        return NULL;
    }
}

