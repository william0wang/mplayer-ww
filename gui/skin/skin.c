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

/**
 * @file
 * @brief Skin parser
 */

#include <stdio.h>
#include <string.h>

#include "skin.h"
#include "font.h"
#include "gui/interface.h"
#include "gui/app/app.h"
#include "gui/app/gui.h"
#include "gui/dialog/dialog.h"
#include "gui/util/misc.h"
#include "gui/util/string.h"

#include "help_mp.h"
#include "mp_msg.h"
#include "libavutil/avstring.h"
#include "libavutil/common.h"

typedef struct {
    const char *name;
    int (*func)(char *in);
} _item;

char *skinDirInHome;
char *skinDirInData;

static guiItems *skin;

static int linenumber;
static unsigned char currItem[32];
static unsigned char path[512];

static unsigned char currWinName[32];
static guiItem *currWin;
static int *currWinItemIdx;
static guiItem *currWinItems;

/**
 * @brief Print a legacy information on an entry.
 *
 * @param old identifier (and deprecated entry)
 * @param data pointer to additional data necessary for checking and
 *             to print the information on @a old
 */
static void skin_legacy(const char *old, const char *data)
{
    const char *p;

    if (strcmp(old, "fontid") == 0) {
        p = strchr(data, ',');

        if (p)
            mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, p, "font = fontfile");
    } else if (strcmp(old, "$l") == 0) {
        p = strstr(old, data);

        if (p && (p == data || p[-1] != '$'))
            mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, old, "$p");
    } else if (strcmp(old, "evSetURL") == 0 && strcmp(data, old) == 0)
        mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, old, "evLoadURL");
    else if (strcmp(old, "sub") == 0 || strcmp(old, "potmeter") == 0)
        mp_msg(MSGT_GPLAYER, MSGL_INFO, MSGTR_GUI_MSG_SkinLegacy, linenumber, old, data);
}

/**
 * @brief Display a skin error message.
 *
 * @param format format string
 * @param ... arguments
 */
static void skin_error(const char *format, ...)
{
    char p[512];
    va_list ap;

    va_start(ap, format);
    vsnprintf(p, sizeof(p), format, ap);
    va_end(ap);

    gmp_msg(MSGT_GPLAYER, MSGL_ERR, MSGTR_GUI_MSG_SkinErrorMessage, linenumber, p);
}

/**
 * @brief Check whether a @a section definition has started.
 *
 * @param item name of the item to be put in a message in case of an error
 *
 * @return #True (ok) or #False (error)
 */
static int section_item(char *item)
{
    if (!skin) {
        skin_error(MSGTR_GUI_MSG_SkinErrorSection, item);
        return False;
    }

    return True;
}

/**
 * @brief Check whether a @a window definition has started.
 *
 * @param item name of the item to be put in a message in case of an error
 *
 * @return #True (ok) or #False (error)
 */
static int window_item(char *item)
{
    if (!currWinName[0]) {
        skin_error(MSGTR_GUI_MSG_SkinErrorWindow, item);
        return False;
    }

    return True;
}

/**
 * @brief Check whether a specific @a window definition has started.
 *
 * @param name name of the window to be checked
 *
 * @return 0 (ok) or 1 (error)
 */
static int in_window(char *name)
{
    if (strcmp(currWinName, name) == 0) {
        skin_error(MSGTR_GUI_MSG_SkinErrorItem, name);
        return 1;
    }

    return 0;
}

/**
 * @brief Get next free item in current @a window.
 *
 * @return pointer to next free item (ok) or NULL (error)
 */
static guiItem *next_item(void)
{
    guiItem *item = NULL;

    if (*currWinItemIdx < MAX_ITEMS - 1) {
        (*currWinItemIdx)++;
        item = &currWinItems[*currWinItemIdx];
    } else
        skin_error(MSGTR_GUI_MSG_SkinTooManyItems);

    return item;
}

/**
 * @brief Parse a @a section definition.
 *
 *        Syntax: section=movieplayer
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_section(char *in)
{
    if (skin) {
        skin_error(MSGTR_GUI_MSG_SkinErrorItem, currItem);
        return 1;
    }

    if (!strcmp(strlower(in), "movieplayer"))
        skin = &guiApp;
    else {
        skin_error(MSGTR_GUI_MSG_SkinUnknownName, in);
        return 1;
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]  %s: %s\n", currItem, in);

    return 0;
}

/**
 * @brief Parse an @a end definition.
 *
 *        Syntax: end
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_end(char *in)
{
    char *space, *name;

    (void)in;

    if (currWinName[0]) {
        space = " ";
        name  = currWinName;
    } else {
        space = "";
        name  = "section";
    }

    if (!section_item(currItem))
        return 1;

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]  %s%s (%s)\n", space, currItem, name);

    if (currWinName[0]) {
        currWinName[0] = 0;
        currWin = NULL;
        currWinItemIdx = NULL;
        currWinItems   = NULL;
    } else
        skin = NULL;

    return 0;
}

/**
 * @brief Parse a @a window definition.
 *
 *        Syntax: window=main|video|playbar|menu
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_window(char *in)
{
    if (!section_item(currItem))
        return 1;

    if (currWinName[0]) {
        skin_error(MSGTR_GUI_MSG_SkinErrorItem, currItem);
        return 1;
    }

    strlower(in);

    // legacy
    if (strcmp(in, "sub") == 0) {
        strcpy(in, "video");
        skin_legacy("sub", in);
    }

    if (strcmp(in, "main") == 0) {
        currWin = &skin->main;
        currWinItemIdx = &skin->IndexOfMainItems;
        currWinItems   = skin->mainItems;
    } else if (strcmp(in, "video") == 0) {
        currWin = &skin->video;
        currWinItemIdx = NULL;
        currWinItems   = NULL;
    } else if (strcmp(in, "playbar") == 0) {
        currWin = &skin->playbar;
        currWinItemIdx = &skin->IndexOfPlaybarItems;
        currWinItems   = skin->playbarItems;
    } else if (strcmp(in, "menu") == 0) {
        currWin = &skin->menu;
        currWinItemIdx = &skin->IndexOfMenuItems;
        currWinItems   = skin->menuItems;
    } else {
        skin_error(MSGTR_GUI_MSG_SkinUnknownName, in);
        return 1;
    }

    av_strlcpy(currWinName, in, sizeof(currWinName));

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]   %s: %s\n", currItem, currWinName);

    return 0;
}

/**
 * @brief Parse a @a base definition.
 *
 *        Syntax: base=image,x,y[,width,height]
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_base(char *in)
{
    unsigned char fname[256];
    unsigned char file[512];
    int x, y, w, h;
    int is_video, is_bar, is_menu;

    if (!window_item(currItem))
        return 1;

    is_video = (strcmp(currWinName, "video") == 0);
    is_bar   = (strcmp(currWinName, "playbar") == 0);
    is_menu  = (strcmp(currWinName, "menu") == 0);

    cutStr(in, fname, ',', 0);
    x = cutInt(in, ',', 1);
    y = cutInt(in, ',', 2);
    w = cutInt(in, ',', 3);
    h = cutInt(in, ',', 4);

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    image: %s", fname);

    currWin->type = itBase;

    if (!is_menu) {
        currWin->x = x;
        currWin->y = y;

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, " %d,%d", x, y);
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "\n");

    if (is_video && (strcmp(fname, "NULL") == 0)) {
        currWin->width  = 0;
        currWin->height = 0;
    } else {
        av_strlcpy(file, path, sizeof(file));
        av_strlcat(file, fname, sizeof(file));

        if (skinImageRead(file, &currWin->Bitmap) != 0)
            return 1;

        currWin->width  = currWin->Bitmap.Width;
        currWin->height = currWin->Bitmap.Height;
    }

    if (is_video) {
        if (w && h) {
            currWin->width  = w;
            currWin->height = h;
        }
    }

    if (currWin->width == 0 || currWin->height == 0)
        return 1;

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s: %dx%d\n", is_video && w && h ? "size" : " bitmap", currWin->width, currWin->height);

    if (!is_video) {
        if (!bpRenderMask(&currWin->Bitmap, &currWin->Mask)) {
            skin_error(MSGTR_GUI_MSG_SkinMemoryError);
            return 1;
        }
        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     mask: %ux%u\n", currWin->Mask.Width, currWin->Mask.Height);
    }

    if (is_bar)
        skin->playbarIsPresent = True;
    if (is_menu)
        skin->menuIsPresent = True;

    return 0;
}

/**
 * @brief Parse a @a background definition.
 *
 *        Syntax: background=R,G,B
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_background(char *in)
{
    if (!window_item(currItem))
        return 1;

    if (in_window("main"))
        return 1;
    if (in_window("playbar"))
        return 1;
    if (in_window("menu"))
        return 1;

    currWin->R = cutInt(in, ',', 0);
    currWin->G = cutInt(in, ',', 1);
    currWin->B = cutInt(in, ',', 2);

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s color: #%02x%02x%02x\n", currItem, currWin->R, currWin->G, currWin->B);

    return 0;
}

/**
 * @brief Parse a @a button definition.
 *
 *        Syntax: button=image,x,y,width,height,message
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_button(char *in)
{
    unsigned char fname[256];
    unsigned char file[512];
    int x, y, w, h, message;
    char msg[32];
    guiItem *item;

    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("menu"))
        return 1;

    cutStr(in, fname, ',', 0);
    x = cutInt(in, ',', 1);
    y = cutInt(in, ',', 2);
    w = cutInt(in, ',', 3);
    h = cutInt(in, ',', 4);
    cutStr(in, msg, ',', 5);

    message = appFindMessage(msg);

    if (message == -1) {
        skin_error(MSGTR_GUI_MSG_SkinUnknownMessage, msg);
        return 1;
    }
    // legacy
    else
        skin_legacy("evSetURL", msg);

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s image: %s %d,%d\n", currItem, fname, x, y);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     message: %s (#%d)\n", msg, message);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     size: %dx%d\n", w, h);

    item = next_item();

    if (!item)
        return 1;

    item->type    = itButton;
    item->x       = x;
    item->y       = y;
    item->width   = w;
    item->height  = h;
    item->message = message;
    item->pressed = btnReleased;

    if (item->message == evPauseSwitchToPlay)
        item->pressed = btnDisabled;

    item->Bitmap.Image = NULL;

    if (strcmp(fname, "NULL") != 0) {
        av_strlcpy(file, path, sizeof(file));
        av_strlcat(file, fname, sizeof(file));

        if (skinImageRead(file, &item->Bitmap) != 0)
            return 1;

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     (bitmap: %ux%u)\n", item->Bitmap.Width, item->Bitmap.Height);
    }

    return 0;
}

/**
 * @brief Parse a @a selected definition.
 *
 *        Syntax: selected=image
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_selected(char *in)
{
    unsigned char file[512];
    guiItem *item;

    if (!window_item(currItem))
        return 1;

    if (in_window("main"))
        return 1;
    if (in_window("video"))
        return 1;
    if (in_window("playbar"))
        return 1;

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    image %s: %s\n", currItem, in);

    item       = &skin->menuSelected;
    item->type = itBase;

    av_strlcpy(file, path, sizeof(file));
    av_strlcat(file, in, sizeof(file));

    if (skinImageRead(file, &item->Bitmap) != 0)
        return 1;

    item->width  = item->Bitmap.Width;
    item->height = item->Bitmap.Height;

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     bitmap: %dx%d\n", item->width, item->height);

    return 0;
}

/**
 * @brief Parse a @a menu definition.
 *
 *        Syntax: menu=x,y,width,height,message
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_menu(char *in)
{
    int x, y, w, h, message;
    char msg[32];
    guiItem *item;

    if (!window_item(currItem))
        return 1;

    if (in_window("main"))
        return 1;
    if (in_window("video"))
        return 1;
    if (in_window("playbar"))
        return 1;

    x = cutInt(in, ',', 0);
    y = cutInt(in, ',', 1);
    w = cutInt(in, ',', 2);
    h = cutInt(in, ',', 3);
    cutStr(in, msg, ',', 4);

    message = appFindMessage(msg);

    if (message == -1) {
        skin_error(MSGTR_GUI_MSG_SkinUnknownMessage, msg);
        return 1;
    }
    // legacy
    else
        skin_legacy("evSetURL", msg);

    item = next_item();

    if (!item)
        return 1;

    item->type    = itMenu;
    item->x       = x;
    item->y       = y;
    item->width   = w;
    item->height  = h;
    item->message = message;

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    item #%d: %d,%d %dx%d\n", *currWinItemIdx, x, y, w, h);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     message: %s (#%d)\n", msg, message);

    item->Bitmap.Image = NULL;

    return 0;
}

/**
 * @brief Parse a hpotmeter or vpotmeter definition.
 *
 *        Parameters: button,bwidth,bheight,phases,numphases,default,x,y,width,height,message
 *
 * @param item pointer to item to store the parameters in
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int parse_potmeter(guiItem *item, char *in)
{
    unsigned char bfname[256];
    unsigned char phfname[256];
    unsigned char buf[512];
    int bwidth, bheight, num, d, x, y, w, h, message;

    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("menu"))
        return 1;

    cutStr(in, bfname, ',', 0);
    bwidth  = cutInt(in, ',', 1);
    bheight = cutInt(in, ',', 2);
    cutStr(in, phfname, ',', 3);
    num = cutInt(in, ',', 4);
    d   = cutInt(in, ',', 5);
    x   = cutInt(in, ',', 6);
    y   = cutInt(in, ',', 7);
    w   = cutInt(in, ',', 8);
    h   = cutInt(in, ',', 9);
    cutStr(in, buf, ',', 10);

    message = appFindMessage(buf);

    if (message == -1) {
        skin_error(MSGTR_GUI_MSG_SkinUnknownMessage, buf);
        return 1;
    }
    // legacy
    else
        skin_legacy("evSetURL", buf);

    if (d < 0 || d > 100) {
        skin_error(MSGTR_GUI_MSG_SkinErrorDefault, d);
        return 1;
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s image: %s %d,%d %dx%d\n", currItem, phfname, x, y, w, h);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     button image: %s %dx%d\n", bfname, bwidth, bheight);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     numphases: %d, default: %d%%\n", num, d);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     message: %s (#%d)\n", buf, message);

    item->x         = x;
    item->y         = y;
    item->width     = w;
    item->height    = h;
    item->pbwidth   = bwidth;
    item->pbheight  = bheight;
    item->numphases = num;
    item->value     = (float)d;
    item->message   = message;
    item->pressed   = btnReleased;

    item->Bitmap.Image = NULL;

    if (strcmp(phfname, "NULL") != 0) {
        if (num == 0) {
            skin_error(MSGTR_GUI_MSG_SkinErrorNumphases);
            return 1;
        }

        av_strlcpy(buf, path, sizeof(buf));
        av_strlcat(buf, phfname, sizeof(buf));

        if (skinImageRead(buf, &item->Bitmap) != 0)
            return 1;

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     (%s bitmap: %ux%u)\n", currItem, item->Bitmap.Width, item->Bitmap.Height);
    }

    item->Mask.Image = NULL;

    if (strcmp(bfname, "NULL") != 0) {
        av_strlcpy(buf, path, sizeof(buf));
        av_strlcat(buf, bfname, sizeof(buf));

        if (skinImageRead(buf, &item->Mask) != 0)
            return 1;

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     (button bitmap: %ux%u)\n", item->Mask.Width, item->Mask.Height);
    }

    return 0;
}

/**
 * @brief Parse a @a hpotmeter definition.
 *
 *        Syntax: hpotmeter=button,bwidth,bheight,phases,numphases,default,x,y,width,height,message
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_hpotmeter(char *in)
{
    guiItem *item;

    item = next_item();

    if (!item)
        return 1;

    item->type = itHPotmeter;

    return parse_potmeter(item, in);
}

/**
 * @brief Parse a @a vpotmeter definition.
 *
 *        Syntax: vpotmeter=button,bwidth,bheight,phases,numphases,default,x,y,width,height,message
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_vpotmeter(char *in)
{
    guiItem *item;

    item = next_item();

    if (!item)
        return 1;

    item->type = itVPotmeter;

    return parse_potmeter(item, in);
}

/**
 * @brief Parse a @a potmeter definition.
 *
 *        Syntax: potmeter=phases,numphases,default,x,y,width,height,message
 *
 * @note THIS ITEM IS DEPRECATED.
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_potmeter(char *in)
{
    char param[256];

    // legacy
    skin_legacy(currItem, "hpotmeter");

    snprintf(param, sizeof(param), "NULL,0,0,%s", in);

    return item_hpotmeter(param);
}

/**
 * @brief Parse a @a pimage definition.
 *
 *        Syntax: pimage=phases,numphases,default,x,y,width,height,message
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_pimage(char *in)
{
    unsigned char phfname[256];
    unsigned char buf[512];
    int num, d, x, y, w, h, message;
    guiItem *item;

    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("menu"))
        return 1;

    cutStr(in, phfname, ',', 0);
    num = cutInt(in, ',', 1);
    d   = cutInt(in, ',', 2);
    x   = cutInt(in, ',', 3);
    y   = cutInt(in, ',', 4);
    w   = cutInt(in, ',', 5);
    h   = cutInt(in, ',', 6);
    cutStr(in, buf, ',', 7);

    message = appFindMessage(buf);

    if (message == -1) {
        skin_error(MSGTR_GUI_MSG_SkinUnknownMessage, buf);
        return 1;
    }
    // legacy
    else
        skin_legacy("evSetURL", buf);

    if (d < 0 || d > 100) {
        skin_error(MSGTR_GUI_MSG_SkinErrorDefault, d);
        return 1;
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s image: %s %d,%d %dx%d\n", currItem, phfname, x, y, w, h);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     numphases: %d, default: %d%%\n", num, d);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     message: %s (#%d)\n", buf, message);

    item = next_item();

    if (!item)
        return 1;

    item->type      = itPimage;
    item->x         = x;
    item->y         = y;
    item->width     = w;
    item->height    = h;
    item->numphases = num;
    item->value     = (float)d;
    item->message   = message;

    item->Bitmap.Image = NULL;

    if (strcmp(phfname, "NULL") != 0) {
        if (num == 0) {
            skin_error(MSGTR_GUI_MSG_SkinErrorNumphases);
            return 1;
        }

        av_strlcpy(buf, path, sizeof(buf));
        av_strlcat(buf, phfname, sizeof(buf));

        if (skinImageRead(buf, &item->Bitmap) != 0)
            return 1;

        mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     (bitmap: %ux%u)\n", item->Bitmap.Width, item->Bitmap.Height);
    }

    return 0;
}

/**
 * @brief Parse a @a font definition.
 *
 *        Syntax: font=fontfile
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_font(char *in)
{
    char fnt[256];

    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("menu"))
        return 1;

    cutStr(in, fnt, ',', 0);   // Note: This seems needless but isn't for compatibility
                               // reasons with a meanwhile deprecated second parameter.
    // legacy
    skin_legacy("fontid", in);

    switch (fntRead(path, fnt)) {
    case -1:
        skin_error(MSGTR_GUI_MSG_SkinMemoryError);
        return 1;

    case -2:
        skin_error(MSGTR_GUI_MSG_SkinTooManyFonts);
        return 1;

    case -3:
        skin_error(MSGTR_GUI_MSG_SkinFontFileNotFound);
        return 1;

    case -4:
        skin_error(MSGTR_GUI_MSG_SkinFontImageNotFound);
        return 1;
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s: %s (#%d)\n", currItem, fnt, fntFindID(fnt));

    return 0;
}

/**
 * @brief Parse a @a slabel definition.
 *
 *        Syntax: slabel=x,y,fontfile,"text"
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_slabel(char *in)
{
    int x, y, id;
    char fnt[256];
    char txt[256];
    guiItem *item;

    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("menu"))
        return 1;

    x = cutInt(in, ',', 0);
    y = cutInt(in, ',', 1);
    cutStr(in, fnt, ',', 2);
    cutStr(in, txt, ',', 3);
    cutStr(txt, txt, '"', 1);

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s: \"%s\"\n", currItem, txt);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     pos: %d,%d\n", x, y);

    id = fntFindID(fnt);

    if (id < 0) {
        skin_error(MSGTR_GUI_MSG_SkinFontNotFound, fnt);
        return 1;
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     font: %s (#%d)\n", fnt, id);

    item = next_item();

    if (!item)
        return 1;

    item->type   = itSLabel;
    item->x      = x;
    item->y      = y;
    item->width  = -1;
    item->height = -1;
    item->fontid = id;
    item->label  = strdup(txt);

    if (!item->label) {
        skin_error(MSGTR_GUI_MSG_SkinMemoryError);
        return 1;
    }

    return 0;
}

/**
 * @brief Parse a @a dlabel definition.
 *
 *        Syntax: dlabel=x,y,width,align,fontfile,"text"
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_dlabel(char *in)
{
    int x, y, w, a, id;
    char fnt[256];
    char txt[256];
    guiItem *item;

    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("menu"))
        return 1;

    x = cutInt(in, ',', 0);
    y = cutInt(in, ',', 1);
    w = cutInt(in, ',', 2);
    a = cutInt(in, ',', 3);
    cutStr(in, fnt, ',', 4);
    cutStr(in, txt, ',', 5);
    cutStr(txt, txt, '"', 1);

    // legacy
    skin_legacy("$l", txt);

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s: \"%s\"\n", currItem, txt);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     pos: %d,%d\n", x, y);
    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     width: %d, align: %d\n", w, a);

    id = fntFindID(fnt);

    if (id < 0) {
        skin_error(MSGTR_GUI_MSG_SkinFontNotFound, fnt);
        return 1;
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]     font: %s (#%d)\n", fnt, id);

    item = next_item();

    if (!item)
        return 1;

    item->type   = itDLabel;
    item->x      = x;
    item->y      = y;
    item->width  = w;
    item->height = -1;
    item->fontid = id;
    item->align  = a;
    item->label  = strdup(txt);

    if (!item->label) {
        skin_error(MSGTR_GUI_MSG_SkinMemoryError);
        return 1;
    }

    return 0;
}

/**
 * @brief Parse a @a decoration definition.
 *
 *        Syntax: decoration=enable|disable
 *
 * @param in definition to be analyzed
 *
 * @return 0 (ok) or 1 (error)
 */
static int item_decoration(char *in)
{
    if (!window_item(currItem))
        return 1;

    if (in_window("video"))
        return 1;
    if (in_window("playbar"))
        return 1;
    if (in_window("menu"))
        return 1;

    strlower(in);

    if (strcmp(in, "enable") != 0 && strcmp(in, "disable") != 0) {
        skin_error(MSGTR_GUI_MSG_SkinUnknownParameter, in);
        return 1;
    }

    skin->mainDecoration = (strcmp(in, "enable") == 0);

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin]    %s: %s\n", currItem, in);

    return 0;
}

/**
 * @brief Parsing functions responsible for skin item definitions.
 */
static _item skinItem[] = {
    { "background", item_background },
    { "base",       item_base       },
    { "button",     item_button     },
    { "decoration", item_decoration },
    { "dlabel",     item_dlabel     },
    { "end",        item_end        },
    { "font",       item_font       },
    { "hpotmeter",  item_hpotmeter  },
    { "menu",       item_menu       },
    { "pimage",     item_pimage     },
    { "potmeter",   item_potmeter   }, // legacy
    { "section",    item_section    },
    { "selected",   item_selected   },
    { "slabel",     item_slabel     },
    { "vpotmeter",  item_vpotmeter  },
    { "window",     item_window     }
};

/**
 * @brief Read a skin @a image file.
 *
 * @param fname filename (with path)
 * @param img pointer suitable to store the image data
 *
 * @return return code of #bpRead()
 */
int skinImageRead(char *fname, guiImage *img)
{
    int i = bpRead(fname, img);

    switch (i) {
    case -1:
        skin_error(MSGTR_GUI_MSG_SkinErrorBitmap16Bit, fname);
        break;

    case -2:
        skin_error(MSGTR_GUI_MSG_SkinBitmapNotFound, fname);
        break;

    case -5:
        skin_error(MSGTR_GUI_MSG_SkinBitmapPngReadError, fname);
        break;

    case -8:
        skin_error(MSGTR_GUI_MSG_SkinBitmapConversionError, fname);
        break;
    }

    return i;
}

/**
 * @brief Build the skin file path for a skin name.
 *
 * @param dir skins directory
 * @param sname name of the skin
 *
 * @return skin file path
 *
 * @note As a side effect, variable #path gets set to the skin path.
 */
static char *setname(char *dir, char *sname)
{
    static char skinfname[512];

    av_strlcpy(skinfname, dir, sizeof(skinfname));
    av_strlcat(skinfname, "/", sizeof(skinfname));
    av_strlcat(skinfname, sname, sizeof(skinfname));
    av_strlcat(skinfname, "/", sizeof(skinfname));
    av_strlcpy(path, skinfname, sizeof(path));
    av_strlcat(skinfname, "skin", sizeof(skinfname));

    return skinfname;
}

/**
 * @brief Read and parse a skin.
 *
 * @param sname name of the skin
 *
 * @return 0 (ok), -1 (skin file not found or not readable) or -2 (parsing error)
 */
int skinRead(char *sname)
{
    char *skinfname;
    FILE *skinfile;
    unsigned char line[256];
    unsigned char param[256];
    unsigned int i;

    skinfname = setname(skinDirInHome, sname);

    if ((skinfile = fopen(skinfname, "rt")) == NULL) {
        skinfname = setname(skinDirInData, sname);

        if ((skinfile = fopen(skinfname, "rt")) == NULL) {
            mp_msg(MSGT_GPLAYER, MSGL_ERR, MSGTR_GUI_MSG_SkinFileNotFound, skinfname);
            return -1;
        }
    }

    mp_msg(MSGT_GPLAYER, MSGL_DBG2, "[skin] configuration file: %s\n", skinfname);

    appFreeStruct();

    skin = NULL;
    currWinName[0] = 0;
    linenumber     = 0;

    while (fgetstr(line, sizeof(line), skinfile)) {
        linenumber++;

        strswap(line, '\t', ' ');
        trim(line);
        decomment(line);

        if (!*line)
            continue;

        cutStr(line, currItem, '=', 0);
        cutStr(line, param, '=', 1);
        strlower(currItem);

        for (i = 0; i < FF_ARRAY_ELEMS(skinItem); i++) {
            if (!strcmp(currItem, skinItem[i].name)) {
                if (skinItem[i].func(param) != 0) {
                    fclose(skinfile);
                    return -2;
                } else
                    break;
            }
        }

        if (i == FF_ARRAY_ELEMS(skinItem)) {
            skin_error(MSGTR_GUI_MSG_SkinUnknownItem, currItem);
            fclose(skinfile);
            return -2;
        }
    }

    fclose(skinfile);

    if (linenumber == 0) {
        mp_msg(MSGT_GPLAYER, MSGL_ERR, MSGTR_GUI_MSG_SkinFileNotReadable, skinfname);
        return -1;
    }

    return 0;
}
