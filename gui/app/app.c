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
 * @brief GUI application helpers
 */

#include "app.h"
#include "gui.h"
#include "gui/skin/font.h"

#include "libavutil/common.h"

/**
 * @brief Initialize item counters.
 */
guiItems guiApp = {
    .IndexOfMainItems    = -1,
    .IndexOfPlaybarItems = -1,
    .IndexOfMenuItems    = -1
};

/**
 * @brief Event messages belonging to event names.
 */
static const evName evNames[] = {
    { evNone,              "evNone"              },
    { evPlay,              "evPlay"              },
    { evStop,              "evStop"              },
    { evPause,             "evPause"             },
    { evPrev,              "evPrev"              },
    { evNext,              "evNext"              },
    { evLoad,              "evLoad"              },
    { evLoadPlay,          "evLoadPlay"          },
    { evLoadAudioFile,     "evLoadAudioFile"     },
    { evLoadSubtitle,      "evLoadSubtitle"      },
    { evDropSubtitle,      "evDropSubtitle"      },
    { evPlaylist,          "evPlaylist"          },
    { evPlayCD,            "evPlayCD"            },
    { evPlayVCD,           "evPlayVCD"           },
    { evPlayDVD,           "evPlayDVD"           },
    { evLoadURL,           "evSetURL"            }, // legacy
    { evLoadURL,           "evLoadURL"           },
    { evPlayTV,            "evPlayTV"            },
    { evPlaySwitchToPause, "evPlaySwitchToPause" },
    { evPauseSwitchToPlay, "evPauseSwitchToPlay" },
    { evBackward10sec,     "evBackward10sec"     },
    { evForward10sec,      "evForward10sec"      },
    { evBackward1min,      "evBackward1min"      },
    { evForward1min,       "evForward1min"       },
    { evBackward10min,     "evBackward10min"     },
    { evForward10min,      "evForward10min"      },
    { evSetMoviePosition,  "evSetMoviePosition"  },
    { evHalfSize,          "evHalfSize"          },
    { evDoubleSize,        "evDoubleSize"        },
    { evFullScreen,        "evFullScreen"        },
    { evNormalSize,        "evNormalSize"        },
    { evSetAspect,         "evSetAspect"         },
    { evIncVolume,         "evIncVolume"         },
    { evDecVolume,         "evDecVolume"         },
    { evSetVolume,         "evSetVolume"         },
    { evMute,              "evMute"              },
    { evSetBalance,        "evSetBalance"        },
    { evEqualizer,         "evEqualizer"         },
    { evAbout,             "evAbout"             },
    { evPreferences,       "evPreferences"       },
    { evSkinBrowser,       "evSkinBrowser"       },
    { evMenu,              "evMenu"              },
    { evIconify,           "evIconify"           },
    { evExit,              "evExit"              }
};

/**
 * @brief Free all memory allocated to an item and set all its pointers to NULL.
 *
 * @param item item to be freed
 */
static void appClearItem(guiItem *item)
{
    bpFree(&item->Bitmap);
    bpFree(&item->Mask);
    free(item->label);
    free(item->text);
    memset(item, 0, sizeof(*item));
}

/**
 * @brief Free all memory allocated to all GUI items and reset all item counters.
 */
void appFreeStruct(void)
{
    int i;

    appClearItem(&guiApp.main);
    guiApp.mainDecoration = False;

    appClearItem(&guiApp.video);

    appClearItem(&guiApp.playbar);
    guiApp.playbarIsPresent = False;

    appClearItem(&guiApp.menu);
    appClearItem(&guiApp.menuSelected);
    guiApp.menuIsPresent = False;

    for (i = 0; i <= guiApp.IndexOfMainItems; i++)
        appClearItem(&guiApp.mainItems[i]);
    for (i = 0; i <= guiApp.IndexOfPlaybarItems; i++)
        appClearItem(&guiApp.playbarItems[i]);
    for (i = 0; i <= guiApp.IndexOfMenuItems; i++)
        appClearItem(&guiApp.menuItems[i]);

    guiApp.IndexOfMainItems    = -1;
    guiApp.IndexOfPlaybarItems = -1;
    guiApp.IndexOfMenuItems    = -1;

    fntFreeFont();
}

/**
 * @brief Find the event belonging to an event name.
 *
 * @param name event name
 *
 * @return event >= 0 (ok) or -1 (not found)
 */
int appFindMessage(const char *name)
{
    unsigned int i;

    for (i = 0; i < FF_ARRAY_ELEMS(evNames); i++)
        if (!strcmp(evNames[i].name, name))
            return evNames[i].message;

    return -1;
}

/**
 * @brief Find the item belonging to an event.
 *
 * @param event event
 *
 * @return pointer to the item (ok) or NULL (not found)
 */
guiItem *appFindItem(int event)
{
    guiItem *item;
    int i, n;

    if (guiApp.videoWindow.isFullScreen && guiApp.playbarIsPresent) {
        item = guiApp.playbarItems;
        n    = guiApp.IndexOfPlaybarItems;
    } else {
        item = guiApp.mainItems;
        n    = guiApp.IndexOfMainItems;
    }

    for (i = 0; i <= n; i++)
        if (item[i].message == event)
            return &item[i];

    return NULL;
}

/**
 * @brief Modify the value of the item belonging to an event.
 *
 * @param event event
 * @param value new value
 */
void btnModify(int event, float value)
{
    int i;

    for (i = 0; i <= guiApp.IndexOfMainItems; i++)
        if (guiApp.mainItems[i].message == event)
            if (hasValue(guiApp.mainItems[i])) {
                if (value < 0.0f)
                    value = 0.0f;
                if (value > 100.0f)
                    value = 100.0f;

                guiApp.mainItems[i].value = value;
            }

    for (i = 0; i <= guiApp.IndexOfPlaybarItems; i++)
        if (guiApp.playbarItems[i].message == event)
            if (hasValue(guiApp.playbarItems[i])) {
                if (value < 0.0f)
                    value = 0.0f;
                if (value > 100.0f)
                    value = 100.0f;

                guiApp.playbarItems[i].value = value;
            }
}

/**
 * @brief Set the @a pressed state (i.e. a new value) to the item belonging to an event.
 *
 * @param event event
 * @param state new state
 */
void btnSet(int event, int state)
{
    int i;

    for (i = 0; i <= guiApp.IndexOfMainItems; i++)
        if (guiApp.mainItems[i].message == event)
            if (hasButton(guiApp.mainItems[i]))
                guiApp.mainItems[i].pressed = state;

    for (i = 0; i <= guiApp.IndexOfPlaybarItems; i++)
        if (guiApp.playbarItems[i].message == event)
            if (hasButton(guiApp.playbarItems[i]))
                guiApp.playbarItems[i].pressed = state;
}

/**
 * @brief Retrieve the value of the (main) item belonging to an event.
 *
 * @param event event
 * @param value pointer to a variable to store the value (if event has been found)
 */
void btnValue(int event, float *value)
{
    int i;

    for (i = 0; i <= guiApp.IndexOfMainItems; i++)
        if (guiApp.mainItems[i].message == event)
            if (hasValue(guiApp.mainItems[i]) && hasButton(guiApp.mainItems[i])) {
                *value = guiApp.mainItems[i].value;
                return;
            }
}
