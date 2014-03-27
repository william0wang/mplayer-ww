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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "render.h"
#include "gui/interface.h"
#include "gui/skin/font.h"
#include "gui/util/string.h"

#include "access_mpcontext.h"
#include "help_mp.h"
#include "libavutil/avstring.h"
#include "osdep/timer.h"
#include "stream/stream.h"

#define DLABEL_DELAY 2500   // in milliseconds

static char *image_buffer;
static int image_width;

/**
 * @brief Convert #guiInfo member Filename.
 *
 * @param how 0 (cut file path and extension),
 *            1 (additionally, convert lower case) or
 *            2 (additionally, convert upper case)
 * @param fname pointer to a buffer to receive the converted Filename
 * @param maxlen size of @a fname buffer
 *
 * @return pointer to @a fname buffer
 */
static char *TranslateFilename(int how, char *fname, size_t maxlen)
{
    char *p;
    size_t len;
    stream_t *stream;

    switch (guiInfo.StreamType) {
    case STREAMTYPE_FILE:

        if (guiInfo.Filename && *guiInfo.Filename) {
            p = strrchr(guiInfo.Filename, '/');

            if (p)
                av_strlcpy(fname, p + 1, maxlen);
            else
                av_strlcpy(fname, guiInfo.Filename, maxlen);

            len = strlen(fname);

            if (len > 3 && fname[len - 3] == '.')
                fname[len - 3] = 0;
            else if (len > 4 && fname[len - 4] == '.')
                fname[len - 4] = 0;
            else if (len > 5 && fname[len - 5] == '.')
                fname[len - 5] = 0;
        } else
            av_strlcpy(fname, MSGTR_GUI_MSG_NoFileLoaded, maxlen);

        break;

    case STREAMTYPE_STREAM:

        av_strlcpy(fname, guiInfo.Filename, maxlen);
        break;

    case STREAMTYPE_CDDA:

        snprintf(fname, maxlen, MSGTR_GUI_TitleN, guiInfo.Track);
        break;

    case STREAMTYPE_VCD:

        snprintf(fname, maxlen, MSGTR_GUI_TitleN, guiInfo.Track - 1);
        break;

    case STREAMTYPE_DVD:

        if (guiInfo.Chapter)
            snprintf(fname, maxlen, MSGTR_GUI_ChapterN, guiInfo.Chapter);
        else
            av_strlcpy(fname, MSGTR_GUI_NoChapter, maxlen);

        break;

    case STREAMTYPE_TV:
    case STREAMTYPE_DVB:

        p      = MSGTR_GUI_NoChannelName;
        stream = mpctx_get_stream(guiInfo.mpcontext);

        if (stream)
            stream_control(stream, STREAM_CTRL_GET_CURRENT_CHANNEL, &p);

        av_strlcpy(fname, p, maxlen);
        break;

    default:

        av_strlcpy(fname, MSGTR_GUI_MSG_NoMediaOpened, maxlen);
        break;
    }

    if (how == 1)
        strlower(fname);
    if (how == 2)
        strupper(fname);

    return fname;
}

/**
 * @brief Translate all variables in the @a text.
 *
 * @param text text containing variables
 *
 * @return new text with all variables translated
 */
static char *TranslateVariables(const char *text)
{
    static char translation[512];
    char trans[512];
    unsigned int i, c;
    int t;

    *translation = 0;

    for (c = 0, i = 0; i < strlen(text); i++) {
        if (text[i] != '$') {
            if (c + 1 < sizeof(translation)) {
                translation[c++] = text[i];
                translation[c]   = 0;
            }
        } else {
            switch (text[++i]) {
            case '1':
                t = guiInfo.ElapsedTime;
HH_MM_SS:       snprintf(trans, sizeof(trans), "%02d:%02d:%02d", t / 3600, t / 60 % 60, t % 60);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case '2':
                t = guiInfo.ElapsedTime;
MMMM_SS:        snprintf(trans, sizeof(trans), "%04d:%02d", t / 60, t % 60);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case '3':
                snprintf(trans, sizeof(trans), "%02d", guiInfo.ElapsedTime / 3600);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case '4':
                snprintf(trans, sizeof(trans), "%02d", guiInfo.ElapsedTime / 60 % 60);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case '5':
                snprintf(trans, sizeof(trans), "%02d", guiInfo.ElapsedTime % 60);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case '6':
                t = guiInfo.RunningTime;
                goto HH_MM_SS;

            case '7':
                t = guiInfo.RunningTime;
                goto MMMM_SS;

            case '8':
                snprintf(trans, sizeof(trans), "%01d:%02d:%02d", guiInfo.ElapsedTime / 3600, (guiInfo.ElapsedTime / 60) % 60, guiInfo.ElapsedTime % 60);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'a':
                switch (guiInfo.AudioChannels) {
                case 0:
                    av_strlcat(translation, "n", sizeof(translation));
                    break;

                case 1:
                    av_strlcat(translation, "m", sizeof(translation));
                    break;

                case 2:
                    av_strlcat(translation, guiInfo.AudioPassthrough ? "r" : "t", sizeof(translation));
                    break;

                default:
                    av_strlcat(translation, "r", sizeof(translation));
                    break;
                }
                break;

            case 'b':
                snprintf(trans, sizeof(trans), "%3.2f%%", guiInfo.Balance);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'B':
                snprintf(trans, sizeof(trans), "%3.1f", guiInfo.Balance);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'C':
                snprintf(trans, sizeof(trans), "%s", guiInfo.CodecName ? guiInfo.CodecName : "");
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'D':
                snprintf(trans, sizeof(trans), "%3.0f", guiInfo.Balance);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'e':
                if (guiInfo.Playing == GUI_PAUSE)
                    av_strlcat(translation, "e", sizeof(translation));
                break;

            case 'f':
                TranslateFilename(1, trans, sizeof(trans));
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'F':
                TranslateFilename(2, trans, sizeof(trans));
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'o':
                TranslateFilename(0, trans, sizeof(trans));
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'l': // legacy
            case 'p':
                if (guiInfo.Playing == GUI_PLAY)
                    av_strlcat(translation, "p", sizeof(translation));
                break;

            case 'P':
                switch (guiInfo.Playing) {
                case GUI_STOP:
                    av_strlcat(translation, "s", sizeof(translation));
                    break;

                case GUI_PLAY:
                    av_strlcat(translation, "p", sizeof(translation));
                    break;

                case GUI_PAUSE:
                    av_strlcat(translation, "e", sizeof(translation));
                    break;
                }
                break;

            case 's':
                if (guiInfo.Playing == GUI_STOP)
                    av_strlcat(translation, "s", sizeof(translation));
                break;

            case 't':
                snprintf(trans, sizeof(trans), "%02d", guiInfo.Track);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'T':
                switch (guiInfo.StreamType) {
                case STREAMTYPE_FILE:
                    av_strlcat(translation, "f", sizeof(translation));
                    break;

                case STREAMTYPE_STREAM:
                    av_strlcat(translation, "u", sizeof(translation));
                    break;

                case STREAMTYPE_CDDA:
                    av_strlcat(translation, "a", sizeof(translation));
                    break;

                case STREAMTYPE_VCD:
                    av_strlcat(translation, "v", sizeof(translation));
                    break;

                case STREAMTYPE_DVD:
                    av_strlcat(translation, "d", sizeof(translation));
                    break;

                case STREAMTYPE_TV:
                case STREAMTYPE_DVB:
                    av_strlcat(translation, "b", sizeof(translation));
                    break;

                default:
                    av_strlcat(translation, " ", sizeof(translation));
                    break;
                }
                break;

            case 'U':
                snprintf(trans, sizeof(trans), "%3.0f", guiInfo.Volume);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'v':
                snprintf(trans, sizeof(trans), "%3.2f%%", guiInfo.Volume);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'V':
                snprintf(trans, sizeof(trans), "%3.1f", guiInfo.Volume);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'x':
                snprintf(trans, sizeof(trans), "%d", guiInfo.VideoWidth);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case 'y':
                snprintf(trans, sizeof(trans), "%d", guiInfo.VideoHeight);
                av_strlcat(translation, trans, sizeof(translation));
                break;

            case '$':
                av_strlcat(translation, "$", sizeof(translation));
                break;

            default:
                continue;
            }

            c = strlen(translation);
        }
    }

    return translation;
}

static void PutImage(guiImage *bf, int x, int y, int max, int ofs)
{
    int i = 0, ix, iy;
    uint32_t *buf = NULL;
    uint32_t *drw = NULL;
    register uint32_t tmp;

    /* register uint32_t yc; */

    if (!bf || (bf->Image == NULL))
        return;

    i   = bf->Width * (bf->Height / max) * ofs;
    buf = (uint32_t *)image_buffer;
    drw = (uint32_t *)bf->Image;

#if 1
    for (iy = y; iy < (int)(y + bf->Height / max); iy++)
        for (ix = x; ix < (int)(x + bf->Width); ix++) {
            tmp = drw[i++];

            if (!IS_TRANSPARENT(tmp))
                buf[iy * image_width + ix] = tmp;
        }
#else
    yc = y * image_width;

    for (iy = y; iy < (int)(y + bf->Height / max); iy++) {
        for (ix = x; ix < (int)(x + bf->Width); ix++) {
            tmp = drw[i++];

            if (!IS_TRANSPARENT(tmp))
                buf[yc + ix] = tmp;
        }

        yc += image_width;
    }
#endif
}

static void SinglePhasePutImage(guiImage *bf, int x, int y, float frac)
{
    int i = 0, w, r, ix, iy;
    uint32_t *buf = NULL;
    uint32_t *drw = NULL;
    register uint32_t tmp;

    if (!bf || (bf->Image == NULL))
        return;

    buf = (uint32_t *)image_buffer;
    drw = (uint32_t *)bf->Image;
    w   = bf->Width * frac;
    r   = bf->Width - w;

    for (iy = y; iy < (int)(y + bf->Height); iy++) {
        for (ix = x; ix < (int)(x + w); ix++) {
            tmp = drw[i++];

            if (!IS_TRANSPARENT(tmp))
                buf[iy * image_width + ix] = tmp;
        }

        i += r;
    }
}

void RenderAll(wsWindow *window, guiItem *Items, int nrItems, char *db)
{
    guiItem *item;
    guiImage *image = NULL;
    int i, ofs;

    image_buffer = db;
    image_width  = window->Width;

    for (i = 0; i < nrItems + 1; i++) {
        item = &Items[i];

        switch (item->pressed) {
        case btnPressed:
            ofs = 0;
            break;

        case btnReleased:
            ofs = 1;
            break;

        default:
            ofs = 2;
            break;
        }

        switch (item->type) {
        case itButton:

            PutImage(&item->Bitmap, item->x, item->y, 3, ofs);
            break;

        case itPimage:

            if (item->numphases == 1)
                SinglePhasePutImage(&item->Bitmap, item->x, item->y, item->value / 100.0);
            else
                PutImage(&item->Bitmap, item->x, item->y, item->numphases, (item->numphases - 1) * (item->value / 100.0));

            break;

        case itHPotmeter:

            if (item->numphases == 1)
                SinglePhasePutImage(&item->Bitmap, item->x, item->y, item->value / 100.0);
            else
                PutImage(&item->Bitmap, item->x, item->y, item->numphases, (item->numphases - 1) * (item->value / 100.0));

            PutImage(&item->Mask, item->x + (item->width - item->pwidth) * (item->value / 100.0), item->y, 3, ofs);
            break;

        case itVPotmeter:

            PutImage(&item->Bitmap, item->x, item->y, item->numphases, item->numphases * (1.0 - item->value / 100.0));
            PutImage(&item->Mask, item->x, item->y + (item->height - item->pheight) * (1.0 - item->value / 100.0), 3, ofs);
            break;

        case itSLabel:

            if (item->width == -1)
                item->width = fntTextWidth(item->fontid, item->label);

            image = fntTextRender(item, 0, item->label);

            if (image)
                PutImage(image, item->x, item->y, 1, 0);

            break;

        case itDLabel:
        {
            int x;
            unsigned int d;
            char *t = TranslateVariables(item->label);

            if (!item->text || (strcmp(item->text, t) != 0)) {
                free(item->text);
                item->text      = strdup(t);
                item->textwidth = fntTextWidth(item->fontid, t);
                item->starttime = GetTimerMS();
                item->last_x    = 0;
            }

            d = GetTimerMS() - item->starttime;

            if (d < DLABEL_DELAY)
                x = item->last_x;                     // don't scroll yet
            else {
                int l;
                char c[2];

                l    = (item->textwidth ? item->textwidth : item->width);
                x    = (l ? l - ((d - DLABEL_DELAY) / 20) % l - 1 : 0);
                c[0] = *item->text;
                c[1] = '\0';

                if (x < (fntTextWidth(item->fontid, c) + 1) >> 1) {
                    item->starttime = GetTimerMS();   // stop again
                    item->last_x    = x;              // at current x pos
                }
            }

            image = fntTextRender(item, x, t);
        }

            if (image)
                PutImage(image, item->x, item->y, 1, 0);

            break;
        }
    }

    wsImageRender(window, db);
}
