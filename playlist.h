#ifndef _MPLAYER_WW_PLAYLIST_WINDOW_H
#define _MPLAYER_WW_PLAYLIST_WINDOW_H
	
#define TBS_TOOLTIPS   0x0100
#define WM_SYSTRAY     (WM_USER+1)
#define UDM_SETRANGE32 (WM_USER+111)
#define UDM_GETRANGE32 (WM_USER+112)
#define UDM_SETPOS32   (WM_USER+113)
#define UDM_GETPOS32   (WM_USER+114)

#define SOLID_GREY (HBRUSH) CreateSolidBrush(RGB(232, 232, 232))
#define SOLID_GREY2 (HBRUSH) CreateSolidBrush(RGB(175, 175, 175))

#define gfree free

#define MAXFILE 1024


#define ID_OK                12
#define ID_APPLY             13
#define ID_CANCEL            14
#define ID_CLOSE             15
#define ID_DEFAULTS          16
#define ID_VO_DRIVER         17
#define ID_AO_DRIVER         18
#define ID_DOUBLE            19
#define ID_DIRECT            20
#define ID_FRAMEDROP         21
#define ID_NORMALIZE         22
#define ID_SOFTMIX           23
#define ID_EXTRASTEREO       91
#define ID_TRACKBAR1         25
#define ID_TRACKBAR2         26
#define ID_UPDOWN1           27
#define ID_UPDOWN2           28
#define ID_EDIT1             29
#define ID_EDIT2             30
#define ID_CACHE             31
#define ID_AUTOSYNC          32
#define ID_NONE              33
#define ID_OSD1              34
#define ID_OSD2              35
#define ID_OSD3              36
#define ID_DVDDEVICE         37
#define ID_CDDEVICE          38
#define ID_PRIO              39
#define ID_URL               40
#define ID_TITLESEL          41
#define ID_UP                42
#define ID_DOWN              43
#define ID_REMOVE            44
#define ID_ADDFILE           45
#define ID_TRACKLIST         46
#define ID_SUBTITLE          47
#define ID_PLAYLISTLOAD      48
#define ID_PLAYLISTSAVE      49
#define ID_ADDURL            50
#define ID_DIR               51
#define ID_PLAY              52
#define ID_STOP              53
#define ID_SEEKF             54
#define ID_SEEKB             55
#define ID_NTRACK            56
#define ID_PTRACK            57
#define ID_PLAYLIST          58
#define ID_CLEAR             59
#define ID_SHOWHIDE          60
#define ID_SKINBROWSER       61
#define ID_KEYHELP           62
#define ID_ONLINEHELP        63
#define ID_CHAPTERSEL        64
#define ID_PREFS             65
#define TRAYMENU             66
#define IDFILE_OPEN          67
#define IDEXIT               68
#define IDURL_OPEN           69
#define IDDIR_OPEN           70
#define IDFILE_NEW           71
#define IDFILE_SAVE          72
#define IDHELP_ABOUT         73
#define IDSUBTITLE_OPEN      74
#define IDPLAYDISK           75
#define ID_CONSOLE           76
#define ID_EQ0               77
#define ID_EQ1               78
#define ID_EQ2               79
#define ID_EQ3               80
#define IDSUB_TOGGLE         81
#define IDSUB_CYCLE          82
#define ID_ASPECT1           83
#define ID_ASPECT2           84
#define ID_ASPECT3           85
#define ID_ASPECT4           86
#define ID_SUBWINDOW         87
#define ID_TIMER             88
#define ID_MUTE              89
#define ID_FULLSCREEN        90

typedef struct
{
    char *filename;
    char *artist;
    char *title;
    int duration;
} pl_track_t;

typedef struct playlist_t playlist_t;
struct playlist_t
{
    int current;                  /* currently used track */
    int trackcount;               /* number of tracknumber */
    pl_track_t **tracks;             /* tracklist */
    void (*add_track)(playlist_t* playlist, const char *filename, const char *artist, const char *title, int duration);
    void (*remove_track)(playlist_t* playlist, int number);
    void (*moveup_track)(playlist_t* playlist, int number);
    void (*movedown_track)(playlist_t* playlist, int number);
    void (*dump_playlist)(playlist_t* playlist);
    void (*sort_playlist)(playlist_t* playlist, int opt);
    void (*clear_playlist)(playlist_t* playlist);
    void (*free_playlist)(playlist_t* playlist);
};

#endif