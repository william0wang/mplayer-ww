/*
 * MPlayer GUI for Win32
 * Copyright (C) 2003 Sascha Sommer <saschasommer@freenet.de>
 * Copyright (C) 2006 Erik Augustson <erik_27can@yahoo.com>
 * Copyright (C) 2006 Gianluigi Tiesi <sherpya@netfarm.it>
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

#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <windows.h>

#include "mp_msg.h"
#include "help_mp.h"
#include "cpudetect.h"
#include "libswscale/swscale.h"
#include "libavutil/attributes.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "gui.h"
#include "gui/util/mem.h"
#include "gui/util/bitmap.h"

#define MAX_LINESIZE 256

typedef struct
{
    int msg;
    char *name;
} evName;

static const evName evNames[] =
{
    {   evNone,                 "evNone"                },
    {   evPlay,                 "evPlay"                },
    {   evStop,                 "evStop"                },
    {   evPause,                "evPause"               },
    {   evPrev,                 "evPrev"                },
    {   evNext,                 "evNext"                },
    {   evLoad,                 "evLoad"                },
    {   evLoadPlay,             "evLoadPlay"            },
    {   evLoadAudioFile,        "evLoadAudioFile"       },
    {   evLoadSubtitle,         "evLoadSubtitle"        },
    {   evDropSubtitle,         "evDropSubtitle"        },
    {   evPlaylist,             "evPlaylist"            },
    {   evPlayCD,               "evPlayCD"              },
    {   evPlayVCD,              "evPlayVCD"             },
    {   evPlayDVD,              "evPlayDVD"             },
    {   evLoadURL,              "evSetURL"              }, // legacy
    {   evLoadURL,              "evLoadURL"             },
    {   evPlayTV,               "evPlayTV"              },
    {   evPlaySwitchToPause,    "evPlaySwitchToPause"   },
    {   evPauseSwitchToPlay,    "evPauseSwitchToPlay"   },
    {   evBackward10sec,        "evBackward10sec"       },
    {   evForward10sec,         "evForward10sec"        },
    {   evBackward1min,         "evBackward1min"        },
    {   evForward1min,          "evForward1min"         },
    {   evBackward10min,        "evBackward10min"       },
    {   evForward10min,         "evForward10min"        },
    {   evSetMoviePosition,     "evSetMoviePosition"    },
    {   evHalfSize,             "evHalfSize"            },
    {   evDoubleSize,           "evDoubleSize"          },
    {   evFullScreen,           "evFullScreen"          },
    {   evNormalSize,           "evNormalSize"          },
    {   evSetAspect,            "evSetAspect"           },
    {   evSetRotation,          "evSetRotation"         },
    {   evIncVolume,            "evIncVolume"           },
    {   evDecVolume,            "evDecVolume"           },
    {   evSetVolume,            "evSetVolume"           },
    {   evMute,                 "evMute"                },
    {   evSetBalance,           "evSetBalance"          },
    {   evEqualizer,            "evEqualizer"           },
    {   evAbout,                "evAbout"               },
    {   evPreferences,          "evPreferences"         },
    {   evSkinBrowser,          "evSkinBrowser"         },
    {   evMenu,                 "evMenu"                },
    {   evIconify,              "evIconify"             },
    {   evExit,                 "evExit"                }
};

static const int evBoxs = sizeof(evNames) / sizeof(evName);

static int linenumber;

/**
 * @brief Print a legacy information on an entry.
 *
 * @param old identifier (and deprecated entry)
 * @param data string necessary for checking and to print the information on @a old
 */
static void skin_legacy (const char *old, const char *data)
{
    const char *p;

    if (strcmp(old, "fontid") == 0)
    {
        p = strchr(data, ',');

        if (p) mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, p, "font = fontfile");
    }
    else if (strcmp(old, "$l") == 0)
    {
        p = strstr(old, data);

        if (p && (p == data || p[-1] != '$')) mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, old, "$p");
    }
    else if (strcmp(old, "evSetURL") == 0 && strcmp(data, old) == 0)
        mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, old, "evLoadURL");
    else if (strcmp(old, "sub") == 0 || strcmp(old, "potmeter") == 0)
        mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, old, data);
}

static char *geteventname(int event)
{
    int i;
    for(i=0; i<evBoxs; i++)
        if(evNames[i].msg == event)
            return evNames[i].name;
    return NULL;
}

/* reads a complete image as is into image buffer */
static image *pngRead(skin_t *skin, const char *fname)
{
    unsigned int i;
    guiImage bmp;
    image *bf;
    char *filename = NULL;
    FILE *fp;

    if(!strcasecmp(fname, "NULL")) return 0;

    /* find filename in order file file.png */
    if(!(fp = fopen(fname, "rb")))
    {
        filename = calloc(1, strlen(skin->skindir) + strlen(fname) + 6);
        sprintf(filename, "%s/%s.png", skin->skindir, fname);
        if(!(fp = fopen(filename, "rb")))
        {
            mp_msg(MSGT_GPLAYER, MSGL_ERR, "[png] cannot find image %s\n", filename);
            free(filename);
            return 0;
        }
    }
    fclose(fp);

    for (i=0; i < skin->imagecount; i++)
        if(!strcmp(fname, skin->images[i]->name))
        {
            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[png] skinfile %s already exists\n", fname);
            free(filename);
            return skin->images[i];
        }
    (skin->imagecount)++;
    skin->images = realloc(skin->images, sizeof(image *) * skin->imagecount);
    bf = skin->images[(skin->imagecount) - 1] = calloc(1, sizeof(image));
    bf->name = strdup(fname);
    bpRead(filename ? filename : fname, &bmp);
    free(filename);
    bf->width = bmp.Width; bf->height = bmp.Height;

    bf->size = bf->width * bf->height * skin->desktopbpp / 8;
    if (skin->desktopbpp == 32)
      bf->data = bmp.Image;
    else {
      const uint8_t *src[4] = { bmp.Image, NULL, NULL, NULL};
      int src_stride[4] = { 4 * bmp.Width, 0, 0, 0 };
      uint8_t *dst[4] = { NULL, NULL, NULL, NULL };
      int dst_stride[4];
      enum AVPixelFormat out_pix_fmt = PIX_FMT_NONE;
      struct SwsContext *sws;
      if      (skin->desktopbpp == 16) out_pix_fmt = PIX_FMT_RGB555;
      else if (skin->desktopbpp == 24) out_pix_fmt = PIX_FMT_RGB24;
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
static void freeimages(skin_t *skin)
{
    unsigned int i;
    for (i=0; i<skin->imagecount; i++)
    {
        if(skin->images && skin->images[i])
        {
            free(skin->images[i]->data);
            free(skin->images[i]->name);
            free(skin->images[i]);
        }
    }
    free(skin->images);
}

static void dumpwidgets(skin_t *skin)
{
    unsigned int i;
    for (i=0; i<skin->widgetcount; i++)
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "widget %p id %i\n", skin->widgets[i], skin->widgets[i]->id);
}

static int counttonextchar(const char *s1, char c)
{
    unsigned int i;
    for (i=0; i<strlen(s1); i++)
        if(s1[i] == c) return i;
    return 0;
}

static char *findnextstring(char *temp, const char *desc, int *base)
{
    int len = counttonextchar(*base + desc, ',');
    memset(temp, 0, strlen(desc) + 1);
    if(!len) len = strlen(desc);
    memcpy(temp, *base + desc, len);
    *base += (len+1);
    return temp;
}

static void freeskin(skin_t *skin)
{
    unsigned int i;

    nfree(skin->skindir);

    for (i=1; i<=skin->lastusedid; i++)
        skin->removewidget(skin, i);

    nfree(skin->widgets);

    freeimages(skin);
    for(i=0; i<skin->windowcount; i++)
    {
        nfree(skin->windows[i]->name);
        free(skin->windows[i]);
    }

    nfree(skin->windows);

    for (i=0; i<skin->fontcount; i++)
    {
        unsigned int x;

        nfree(skin->fonts[i]->name);

        for (x=0; x<skin->fonts[i]->charcount; x++)
            nfree(skin->fonts[i]->chars[x]);

        nfree(skin->fonts[i]->chars);

        nfree(skin->fonts[i]);
    }
    nfree(skin->fonts);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN FREE] skin freed\n");
    nfree(skin);
}

static void removewidget(skin_t *skin, int id)
{
    unsigned int i;
    unsigned int pos=0;
    widget **temp = calloc(skin->widgetcount - 1, sizeof(widget *));

    for (i=0; i<skin->widgetcount; i++)
    {
        if(skin->widgets[i]->id == id)
        {
            free(skin->widgets[i]->label);
            nfree(skin->widgets[i]);
        }
        else
        {
            temp[pos] = skin->widgets[i];
            pos++;
        }
    }
    if (pos != i)
    {
        (skin->widgetcount)--;
        free(skin->widgets);
        skin->widgets = temp;
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "removed widget %i\n", id);
        return;
    }
    free(temp);
    mp_msg(MSGT_GPLAYER, MSGL_ERR, "widget %i not found\n", id);
}

static void addwidget(skin_t *skin, window *win, const char *desc)
{
    widget *mywidget;
    char *temp = calloc(1, strlen(desc) + 1);
    (skin->widgetcount)++;
    (skin->lastusedid)++;
    skin->widgets = realloc(skin->widgets, sizeof(widget *) * skin->widgetcount);
    mywidget = skin->widgets[(skin->widgetcount) - 1] = calloc(1, sizeof(widget));
    mywidget->id = skin->lastusedid;
    mywidget->window = win->type;
    /* parse and fill widget specific info */
    if(!strncmp(desc, "base", 4))
    {
        int base = counttonextchar(desc, '=') + 1;
        mywidget->type = tyBase;
        mywidget->bitmap[0] = pngRead(skin, findnextstring(temp, desc, &base));
        mywidget->wx = mywidget->x = atoi(findnextstring(temp, desc, &base));
        mywidget->wy = mywidget->y = atoi(findnextstring(temp, desc, &base));
        mywidget->wwidth = mywidget->width = atoi(findnextstring(temp, desc, &base));
        mywidget->wheight = mywidget->height = atoi(findnextstring(temp, desc, &base));
        win->base = mywidget;
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [BASE] %s %i %i %i %i\n",
              (mywidget->bitmap[0]) ? mywidget->bitmap[0]->name : NULL,
               mywidget->x, mywidget->y, mywidget->width, mywidget->height);
    }
    else if(!strncmp(desc, "button", 6))
    {
        int base = counttonextchar(desc, '=') + 1;
        int i;
        mywidget->type = tyButton;
        mywidget->bitmap[0] = pngRead(skin, findnextstring(temp, desc, &base));
        mywidget->wx = mywidget->x = atoi(findnextstring(temp, desc, &base));
        mywidget->wy = mywidget->y = atoi(findnextstring(temp, desc, &base));
        mywidget->wwidth = mywidget->width = atoi(findnextstring(temp, desc, &base));
        mywidget->wheight = mywidget->height = atoi(findnextstring(temp, desc, &base));
        findnextstring(temp, desc, &base);

        /* Assign corresponding event to the widget */
        mywidget->msg = evNone;
        for (i=0; i<evBoxs; i++)
        {
            if(!strcmp(temp, evNames[i].name))
            {
                // legacy
                skin_legacy("evSetURL", temp);

                mywidget->msg = evNames[i].msg;
                break;
            }
        }

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [BUTTON] %s %i %i %i %i msg %i\n",
              (mywidget->bitmap[0]) ? mywidget->bitmap[0]->name : NULL,
               mywidget->x, mywidget->y, mywidget->width, mywidget->height, mywidget->msg);
    }
    else if(!strncmp(desc, "hpotmeter", 9) || !strncmp(desc, "vpotmeter", 9) || !strncmp(desc, "rpotmeter", 9) || /* legacy */ !strncmp(desc, "potmeter", 8))
    {
        int base = counttonextchar(desc, '=') + 1;
        int i, av_uninit(x0), av_uninit(y0), av_uninit(x1), av_uninit(y1);
        /* hpotmeter = button, bwidth, bheight, phases, numphases, default, X, Y, width, height, message */
        if(!strncmp(desc, "vpotmeter", 9)) mywidget->type = tyVpotmeter;
        else if(!strncmp(desc, "rpotmeter", 9)) mywidget->type = tyRpotmeter;
        else mywidget->type = tyHpotmeter;
        if (*desc == 'p')
        {
            mywidget->bitmap[0] = NULL;
            mywidget->width = 0;
            mywidget->height = 0;

            // legacy
            skin_legacy("potmeter", "hpotmeter");
        }
        else
        {
            mywidget->bitmap[0] = pngRead(skin, findnextstring(temp, desc, &base));
            mywidget->width = atoi(findnextstring(temp, desc, &base));
            mywidget->height = atoi(findnextstring(temp, desc, &base));
        }
        mywidget->bitmap[1] = pngRead(skin, findnextstring(temp, desc, &base));
        mywidget->phases = atoi(findnextstring(temp, desc, &base));

        if (*desc == 'r')
        {
            x0 = atoi(findnextstring(temp, desc, &base));
            y0 = atoi(findnextstring(temp, desc, &base));
            x1 = atoi(findnextstring(temp, desc, &base));
            y1 = atoi(findnextstring(temp, desc, &base));
        }

        mywidget->value = atof(findnextstring(temp, desc, &base));
        mywidget->x = mywidget->wx = atoi(findnextstring(temp, desc, &base));
        mywidget->y = mywidget->wy = atoi(findnextstring(temp, desc, &base));
        mywidget->wwidth = atoi(findnextstring(temp, desc, &base));
        mywidget->wheight = atoi(findnextstring(temp, desc, &base));
        findnextstring(temp, desc, &base);
        mywidget->msg = evNone;
        for (i=0; i<evBoxs; i++)
        {
            if(!strcmp(temp, evNames[i].name))
            {
                // legacy
                skin_legacy("evSetURL", temp);

                mywidget->msg = evNames[i].msg;
                break;
            }
        }
        if (*desc == 'r')
        {
            mywidget->zeropoint = appRadian(mywidget, x0, y0);
            mywidget->arclength = appRadian(mywidget, x1, y1) - mywidget->zeropoint;

            if (mywidget->arclength < 0.0) mywidget->arclength += 2 * M_PI;
            // else check if radians of (x0,y0) and (x1,y1) only differ below threshold
            else if (mywidget->arclength < 0.05) mywidget->arclength = 2 * M_PI;
        }
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] %s %s %i %i %s %i ",
                (mywidget->type == tyHpotmeter) ? "[HPOTMETER]" : (mywidget->type == tyVpotmeter) ? "[VPOTMETER]" : "[RPOTMETER]",
                (mywidget->bitmap[0]) ? mywidget->bitmap[0]->name : NULL,
                mywidget->width, mywidget->height,
                (mywidget->bitmap[1]) ? mywidget->bitmap[1]->name : NULL,
                mywidget->phases);
        if (*desc == 'r')
            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "%i,%i %i,%i ", x0, y0, x1, y1);
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "%f %i %i %i %i msg %i\n", mywidget->value,
                mywidget->wx, mywidget->wy, mywidget->wwidth, mywidget->wwidth,
                mywidget->msg);
        if (mywidget->bitmap[0] == NULL || mywidget->width == 0 || mywidget->height == 0)
        {
            mywidget->bitmap[0] = mywidget->bitmap[1];
            mywidget->width = mywidget->wwidth;
            mywidget->height = mywidget->wheight;
        }
        if (*desc == 'r')
        {
            mywidget->maxwh = FFMAX(mywidget->width, mywidget->height);

            // clickedinsidewidget() checks with width/height, so set it
            mywidget->width = mywidget->wwidth;
            mywidget->height = mywidget->wheight;
        }
    }
    else if(!strncmp(desc, "pimage", 6))
    {
        int base = counttonextchar(desc, '=') + 1;
        int i;
        /* pimage = phases, numphases, default, X, Y, width, height, message */
        mywidget->type = tyPimage;
        mywidget->bitmap[0] = pngRead(skin, findnextstring(temp, desc, &base));
        mywidget->phases = atoi(findnextstring(temp, desc, &base));
        mywidget->value = atof(findnextstring(temp, desc, &base));
        mywidget->wx = mywidget->x = atoi(findnextstring(temp, desc, &base));
        mywidget->wy = mywidget->y = atoi(findnextstring(temp, desc, &base));
        mywidget->wwidth = mywidget->width = atoi(findnextstring(temp, desc, &base));
        mywidget->wheight = mywidget->height = atoi(findnextstring(temp, desc, &base));
        findnextstring(temp, desc, &base);
        mywidget->msg = evNone;
        for (i=0; i<evBoxs; i++)
        {
            if(!strcmp(temp, evNames[i].name))
            {
                // legacy
                skin_legacy("evSetURL", temp);

                mywidget->msg=evNames[i].msg;
                break;
            }
        }
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [PIMAGE] %s %i %i %i %f %i %i msg %i\n",
                (mywidget->bitmap[0]) ? mywidget->bitmap[0]->name : NULL,
                mywidget->width, mywidget->height,
                mywidget->phases, mywidget->value,
                mywidget->x, mywidget->y,
                mywidget->msg);
    }
    else if(!strncmp(desc, "menu", 4))
    {
        int base = counttonextchar(desc, '=') + 1;
        int i;
        mywidget->type = tyMenu;
        mywidget->wx=atoi(findnextstring(temp, desc, &base));
        mywidget->x=0;
        mywidget->wy=mywidget->y=atoi(findnextstring(temp, desc, &base));
        mywidget->wwidth=mywidget->width=atoi(findnextstring(temp, desc, &base));
        mywidget->wheight=mywidget->height=atoi(findnextstring(temp, desc, &base));
        findnextstring(temp, desc, &base);
        mywidget->msg = evNone;
        for (i=0; i<evBoxs; i++)
        {
            if(!strcmp(temp, evNames[i].name))
            {
                // legacy
                skin_legacy("evSetURL", temp);

                mywidget->msg = evNames[i].msg;
                break;
            }
        }
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [MENU] %i %i %i %i msg %i\n",
               mywidget->x, mywidget->y, mywidget->width, mywidget->height, mywidget->msg);
    }
    else if(!strncmp(desc, "selected", 8))
    {
        win->base->bitmap[1] = pngRead(skin, desc + 9);
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [BASE] added image %s\n", win->base->bitmap[1]->name);
    }
    else if(!strncmp(desc, "slabel",6))
    {
        int base = counttonextchar(desc, '=') + 1;
        unsigned int i;
        mywidget->type = tySlabel;
        mywidget->wx = mywidget->x = atoi(findnextstring(temp, desc, &base));
        mywidget->wy = mywidget->y = atoi(findnextstring(temp, desc, &base));
        findnextstring(temp, desc, &base);
        mywidget->font = NULL;
        for (i=0; i<skin->fontcount; i++)
        {
            if(!strcmp(temp, skin->fonts[i]->name))
            {
                mywidget->font = skin->fonts[i];
                break;
            }
        }
        mywidget->label = strdup(findnextstring(temp, desc, &base));
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [SLABEL] %i %i %s %s\n",
               mywidget->x, mywidget->y, mywidget->font->name, mywidget->label);
    }
    else if(!strncmp(desc, "dlabel", 6))
    {
        int base = counttonextchar(desc, '=') + 1;
        unsigned int i;
        mywidget->type = tyDlabel;
        mywidget->wx = mywidget->x = atoi(findnextstring(temp, desc, &base));
        mywidget->wy = mywidget->y = atoi(findnextstring(temp, desc, &base));
        mywidget->length = atoi(findnextstring(temp, desc, &base));
        mywidget->align = atoi(findnextstring(temp, desc, &base));
        findnextstring(temp, desc, &base);
        mywidget->font = NULL;
        for (i=0; i<skin->fontcount; i++)
        {
            if(!strcmp(temp, skin->fonts[i]->name))
            {
                mywidget->font=skin->fonts[i];
                break;
            }
        }
        mywidget->label=strdup(findnextstring(temp, desc, &base));

        // legacy
        skin_legacy("$l", mywidget->label);

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [ITEM] [DLABEL] %i %i %i %i %s \"%s\"\n",
               mywidget->x, mywidget->y, mywidget->length, mywidget->align, mywidget->font->name, mywidget->label);
    }
    free(temp);
}

static void loadfonts(skin_t* skin)
{
    unsigned int x;
    for (x=0; x<skin->fontcount; x++)
    {
        FILE *fp;
        char *filename;
        char *tmp = calloc(1, MAX_LINESIZE);
        char *desc = calloc(1, MAX_LINESIZE);
        filename = calloc(1, strlen(skin->skindir) + strlen(skin->fonts[x]->name) + 6);
        sprintf(filename, "%s/%s.fnt", skin->skindir, skin->fonts[x]->name);
        if(!(fp = fopen(filename,"rb")))
        {
            mp_msg(MSGT_GPLAYER, MSGL_ERR, "[FONT LOAD] Font not found \"%s\"\n", skin->fonts[x]->name);
            free(tmp);
            free(desc);
            free(filename);
            return;
        }
        while(!feof(fp))
        {
            int pos = 0;
            unsigned int i;
            fgets(tmp, MAX_LINESIZE, fp);
            memset(desc, 0, MAX_LINESIZE);
            for (i=0; i<strlen(tmp); i++)
            {
                /* remove spaces and linebreaks */
                if((tmp[i] == ' ') || (tmp[i] == '\n') || (tmp[i] == '\r')) continue;
                /* remove comments */
                if((tmp[i] == ';') &&  ((i < 1) || (tmp[i-1] != '\"')))
                {
                    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[FONT LOAD] Comment: %s", tmp + i + 1);
                    break;
                }
                desc[pos] = tmp[i];
                pos++;
            }
            if(!strlen(desc)) continue;
            /* now we have "readable" output -> parse it */
            if(!strncmp(desc, "image", 5))
            {
                skin->fonts[x]->image = pngRead(skin, desc + 6);
                mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[FONT] [IMAGE] \"%s\"\n", desc + 6);
            }
            else
            {
                int base = 4;
                if(*desc != '"') break;
                if(*(desc + 1) == 0) break;
                (skin->fonts[x]->charcount)++;
                skin->fonts[x]->chars = realloc(skin->fonts[x]->chars, sizeof(char_t *) *skin->fonts[x]->charcount);
                skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]=calloc(1, sizeof(char_t));
                skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->c = ((*(desc + 1) == '"') && (*(desc + 2) != '"')) ? ' ': *(desc + 1);
                if((*(desc + 1) == '"') && (*(desc + 2) != '"')) base = 3;
                skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->x = atoi(findnextstring(tmp, desc, &base));
                skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->y = atoi(findnextstring(tmp, desc, &base));
                skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->width = atoi(findnextstring(tmp, desc, &base));
                skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->height = atoi(findnextstring(tmp, desc, &base));
                mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[FONT] [CHAR] %c %i %i %i %i\n",
                        skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->c,
                        skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->x,
                        skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->y,
                        skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->width,
                        skin->fonts[x]->chars[skin->fonts[x]->charcount - 1]->height);
            }
        }
        free(desc);
        free(filename);
        free(tmp);
        fclose(fp);
    }
}

skin_t* loadskin(char* skindir, int desktopbpp)
{
    FILE *fp;
    int reachedendofwindow = FALSE;
    skin_t *skin = calloc(1, sizeof(skin_t));
    char *filename;
    char *tmp = calloc(1, MAX_LINESIZE);
    char *desc = calloc(1, MAX_LINESIZE);
    window* mywindow = NULL;

    linenumber = 0;

    /* setup funcs */
    skin->freeskin = freeskin;
    skin->pngRead = pngRead;
    skin->addwidget = addwidget;
    skin->removewidget = removewidget;
    skin->geteventname = geteventname;
    skin->desktopbpp = desktopbpp;
    skin->skindir = strdup(skindir);

    filename = calloc(1, strlen(skin->skindir) + strlen("skin") + 2);
    sprintf(filename, "%s/skin", skin->skindir);
    if(!(fp = fopen(filename, "rb")))
    {
        mp_msg(MSGT_GPLAYER, MSGL_FATAL, "[SKIN LOAD] Skin \"%s\" not found\n", skindir);
        skin->freeskin(skin);
        free(tmp);
        free(desc);
        free(filename);
        return NULL;
    }

    while(!feof(fp))
    {
        int pos = 0;
        unsigned int i;
        int insidequote = FALSE;
        fgets(tmp, MAX_LINESIZE, fp);
        linenumber++;
        memset(desc, 0, MAX_LINESIZE);
        for (i=0; i<strlen(tmp); i++)
        {
            if((tmp[i] == '"') && !insidequote) { insidequote=TRUE; continue; }
            else if((tmp[i] == '"') && insidequote) { insidequote=FALSE ; continue; }
            /* remove spaces and linebreaks */
            if((!insidequote && (tmp[i] == ' ')) || (tmp[i] == '\n') || (tmp[i] == '\r')) continue;
            /* remove comments */
            else if(tmp[i] == ';')
            {
                mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN LOAD] Comment: %s", tmp + i + 1);
                break;
            }
            desc[pos] = tmp[i];
            pos++;
        }

        if(!strlen(desc)) continue;
        /* now we have "readable" output -> parse it */
        /* parse window specific info */
        if(!strncmp(desc, "section", 7))
        {
            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [SECTION] \"%s\"\n", desc + 8);
        }
        else if(!strncmp(desc, "window", 6))
        {
            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [WINDOW] \"%s\"\n", desc + 7);
            reachedendofwindow = FALSE;
            (skin->windowcount)++;
            skin->windows = realloc(skin->windows, sizeof(window *) * skin->windowcount);
            mywindow = skin->windows[(skin->windowcount) - 1] = calloc(1, sizeof(window));
            mywindow->name = strdup(desc + 7);
            if(!strncmp(desc + 7, "main", 4)) mywindow->type = wiMain;
            else if(!strncmp(desc+7, "video", 5) || /* legacy */ !strncmp(desc+7, "sub", 3))
            {
                mywindow->type = wiVideo;
                mywindow->decoration = TRUE;
                // legacy
                if (desc[7] == 's') skin_legacy("sub", "video");
            }
            else if(!strncmp(desc + 7, "menu", 4)) mywindow->type = wiMenu;
            else if(!strncmp(desc + 7, "playbar", 7)) mywindow->type = wiPlaybar;
            else mp_msg(MSGT_GPLAYER, MSGL_V, "[SKIN] warning found unknown windowtype");
        }
        else if(!strncmp(desc, "decoration", 10) && !strncmp(desc + 11, "enable", 6))
        {
            mywindow->decoration = TRUE;
            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [DECORATION] enabled decoration for window \"%s\"\n", mywindow->name);
        }
        else if(!strncmp(desc, "background", 10))
        {
            int base = counttonextchar(desc, '=') + 1;
            char temp[MAX_LINESIZE];
            mywindow->backgroundcolor[0] = atoi(findnextstring(temp, desc, &base));
            mywindow->backgroundcolor[1] = atoi(findnextstring(temp, desc, &base));
            mywindow->backgroundcolor[2] = atoi(findnextstring(temp, desc, &base));
            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [BACKGROUND] window \"%s\" has backgroundcolor (%i,%i,%i)\n", mywindow->name,
                    mywindow->backgroundcolor[0],
                    mywindow->backgroundcolor[1],
                    mywindow->backgroundcolor[2]);
        }
        else if(!strncmp(desc, "end", 3))
        {
            if(reachedendofwindow)
            {
                mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [END] of section\n");
            }
            else
            {
                reachedendofwindow = TRUE;
                mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [END] of window \"%s\"\n", mywindow->name);
            }
        }
        else if(!strncmp(desc, "font", 4))
        {
            int id = 0;
            char temp[MAX_LINESIZE];
            int base = counttonextchar(desc, '=') + 1;
            findnextstring(temp, desc, &base);
            id = skin->fontcount;
            (skin->fontcount)++;
            skin->fonts = realloc(skin->fonts, sizeof(font_t *) * skin->fontcount);
            skin->fonts[id]=calloc(1, sizeof(font_t));
            skin->fonts[id]->name = strdup(temp);

            // legacy
            skin_legacy("fontid", desc);

            mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[SKIN] [FONT] name \"%s\"\n", skin->fonts[id]->name);
        }
        else
            skin->addwidget(skin, mywindow, desc);
    }

    free(desc);
    free(filename);
    free(tmp);
    fclose(fp);
    loadfonts(skin);
    mp_msg(MSGT_GPLAYER, MSGL_V, "[SKIN LOAD] loaded skin \"%s\"\n", skin->skindir);
    dumpwidgets(skin);
    return skin;
}
