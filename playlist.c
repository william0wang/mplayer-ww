#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <winuser.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <commctrl.h>

#include "playlist.h"
#include "playtree.h"
#include "m_config.h"
#include "help_mp.h"
#include "resource.h"
#include "mp_msg.h"
#include "winstuff.h"
#include "input/input.h"
#include "mp_core.h"

#ifndef LVS_EX_FULLROWSELECT
#define LVS_EX_FULLROWSELECT 0x20
#endif

#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
#define LVM_SETEXTENDEDLISTVIEWSTYLE LVM_FIRST + 54
#endif

#ifndef LVM_GETSELECTIONMARK
#define LVM_GETSELECTIONMARK LVM_FIRST + 66
#endif

#ifndef LVM_SETSELECTIONMARK
#define LVM_SETSELECTIONMARK LVM_FIRST + 67
#endif


extern m_config_t* mconfig;
extern char* filename;
extern need_update_playtree;

static MPContext *mpctx;

//static LONG_PTR plptrWin;
static playlist_t *playlist = NULL;
HMENU playlistmenu;
static HICON mplayericon = NULL;
static WNDPROC wdgProc;
static const int WinWidth = 600;
static const int WinHeight = 320;
static BOOL button_down = FALSE;
static BOOL mouse_move = FALSE;
static int old_selected = 0;
static int playlist_x = 0;
static int playlist_y = 0;
static HWND hPlaylistWnd = NULL;
static HWND playlist_layer = NULL;
static HWND playlist_parent = NULL;
static HANDLE hPlaylistThread = NULL;
static HBRUSH bkgndbrush = NULL;
static int transparent = 0;
static int up_y = -1;
static int down_y = 0x7fffffff;
static unsigned int code_page = CP_ACP;
static BOOL (WINAPI* SetLayeredWinAttributes)(HWND, COLORREF, BYTE, DWORD) = NULL;

extern int loop_break;
extern int vo_dirver;
extern int is_vista;
extern int controlbar_alpha;
extern int sys_Language;
extern char *video_exts[];
extern char *audio_exts[];

static char TitalPlaylist[30]={0};
static char BottonPlayAll[30]={0};
static char BottonUp[30]={0};
static char BottonDown[30]={0};
static char BottonAdd[30]={0};
static char BottonRemove[30]={0};
static char BottonSave[30]={0};
static char BottonClose[30]={0};

static char MenuAdd[30]={0};
static char MenuRemove[30]={0};
static char MenuClear[30]={0};
static char MenuClose[30]={0};

int set_playlist_mpctx(MPContext *mpctx_in)
{
	mpctx = mpctx_in;
}

static void add_track(playlist_t *playlist, const char *lfilename, const char *artist, const char *title, int duration)
{
	(playlist->trackcount)++;
	playlist->tracks = realloc(playlist->tracks, playlist->trackcount * sizeof(pl_track_t *));
	playlist->tracks[playlist->trackcount - 1] = calloc(1, sizeof(pl_track_t));
	if(lfilename)
		playlist->tracks[playlist->trackcount - 1]->filename = strdup(lfilename);
	if(artist)
		playlist->tracks[playlist->trackcount - 1]->artist = strdup(artist);
	if(title)
		playlist->tracks[playlist->trackcount - 1]->title = strdup(title);
	if(duration)
		playlist->tracks[playlist->trackcount - 1]->duration = duration;
}

static void remove_track(playlist_t *playlist, int number)
{
	pl_track_t **tmp = calloc(1, playlist->trackcount * sizeof(pl_track_t *));
	int i, p = 0;
	memcpy(tmp, playlist->tracks, playlist->trackcount * sizeof(pl_track_t *));
	(playlist->trackcount)--;
	playlist->tracks = realloc(playlist->tracks, playlist->trackcount * sizeof(pl_track_t *));
	for(i=0; i<playlist->trackcount + 1; i++)
	{
		if(i != (number - 1))
		{
			playlist->tracks[p] = tmp[i];
			p++;
		}
		else
		{
			if(tmp[i]->filename)
			free(tmp[i]->filename);
			if(tmp[i]->artist) free(tmp[i]->artist);
			if(tmp[i]->title) free(tmp[i]->title);
			free(tmp[i]);
		}
	}
	free(tmp);
}

static void moveup_track(playlist_t *playlist, int number)
{
	pl_track_t *tmp;
	if(number == 1) return; /* already first */
	tmp = playlist->tracks[number - 2];
	playlist->tracks[number - 2] = playlist->tracks[number - 1];
	playlist->tracks[number - 1] = tmp;
}

static void movedown_track(playlist_t *playlist, int number)
{
	pl_track_t *tmp;
	if(number == playlist->trackcount) return; /* already latest */
	tmp = playlist->tracks[number];
	playlist->tracks[number] = playlist->tracks[number - 1];
	playlist->tracks[number - 1] = tmp;
}

static void sort_playlist(playlist_t *playlist, int opt) {}

static void clear_playlist(playlist_t *playlist)
{
	while(playlist->trackcount) playlist->remove_track(playlist, 1);
	playlist->tracks = NULL;
	playlist->current = 0;
}

static void free_playlist(playlist_t *playlist)
{
	if(playlist->tracks) playlist->clear_playlist(playlist);
	free(playlist);
}

static void dump_playlist(playlist_t *playlist)
{
	int i;
	for (i=0; i<playlist->trackcount; i++)
	{
		mp_msg(MSGT_GPLAYER, MSGL_V, "track %i %s ", i + 1, playlist->tracks[i]->filename);
		if(playlist->tracks[i]->artist) mp_msg(MSGT_GPLAYER, MSGL_V, "%s ", playlist->tracks[i]->artist);
		if(playlist->tracks[i]->title) mp_msg(MSGT_GPLAYER, MSGL_V, "- %s ", playlist->tracks[i]->title);
		if(playlist->tracks[i]->duration) mp_msg(MSGT_GPLAYER, MSGL_V, "%i ", playlist->tracks[i]->duration);
		mp_msg(MSGT_GPLAYER, MSGL_V, "\n");
	}
}

static playlist_t *create_playlist(void)
{
	playlist_t *playlist = calloc(1, sizeof(playlist_t));
	playlist->add_track = add_track;
	playlist->remove_track = remove_track;
	playlist->moveup_track = moveup_track;
	playlist->movedown_track = movedown_track;
	playlist->dump_playlist = dump_playlist;
	playlist->sort_playlist = sort_playlist;
	playlist->clear_playlist = clear_playlist;
	playlist->free_playlist = free_playlist;
	return playlist;
}

int import_playtree_into_playlist(play_tree_t *tree, m_config_t *config)
{
	char *lfilename;
	int result = 0;
	play_tree_iter_t *pi = NULL;
	if(!tree) return 0;

	if((pi = pt_iter_create(&tree, config)))
	{
		while ((lfilename = pt_iter_get_next_file(pi)) != NULL)
		{
			playlist->add_track(playlist, lfilename, NULL, NULL, 0);
			result = 1;
		}
		pt_iter_destroy(&pi);
	}
	lfilename = NULL;
	return result;
}

static int playtree_add_file(play_tree_t* tree, char *lfilename)
{
	int result = 0;
	if(!tree) return -1;
	play_tree_iter_t *pi = NULL;
	if((pi = pt_iter_create(&tree, mconfig)))
	{
		pt_iter_add_file(pi , lfilename);
		pt_iter_destroy(&pi);
		result = 1;
	}
	return result;
}

int playtree_update(MPContext * mpctx)
{
	int i, seek = 0, n = 0;
  	play_tree_t *list = NULL, *entry = NULL, *last_entry = NULL;
  	need_update_playtree = 0;

  	if(playlist->trackcount <= 0)
  		return -1;

  	if(!mpctx->playtree_iter || !mpctx->playtree)
  		return -1;

	for(i = 0; i<(playlist->trackcount); i++) {
		if(!seek) n++;
		if(!stricmp(filename, playlist->tracks[i]->filename))
			seek = 1;
		entry = play_tree_new();
		play_tree_add_file(entry, playlist->tracks[i]->filename);
		if(list)
			play_tree_append_entry(last_entry,entry);
		else
			list = entry;
		last_entry = entry;
	}
	entry = play_tree_new();
	play_tree_set_child(entry,list);
	if (entry) {
		// Go back to the starting point.
		while (play_tree_iter_up_step(mpctx->playtree_iter, 0, 1) != PLAY_TREE_ITER_END)
		    /* NOP */ ;
		play_tree_free_list(mpctx->playtree->child, 1);
		play_tree_set_child(mpctx->playtree, entry);
		pt_iter_goto_head(mpctx->playtree_iter);
		mpctx->eof = PT_NEXT_SRC;

		if(seek && n > 0) {
			play_tree_iter_t *i = play_tree_iter_new_copy(mpctx->playtree_iter);
			if (play_tree_iter_step(i, n, 0) == PLAY_TREE_ITER_ENTRY)
			    mpctx->eof = PT_NEXT_ENTRY;
			play_tree_iter_free(i);
			if (mpctx->eof == PT_NEXT_ENTRY)
				mpctx->play_tree_step = n;
		}
	}
	return 0;
}

int playtree_update_play(MPContext * mpctx, int index)
{
	int i;
  	play_tree_t *list = NULL, *entry = NULL, *last_entry = NULL;
  	need_update_playtree = 0;

  	if(playlist->trackcount <= 0)
  		return -1;

	for(i = 0; i<(playlist->trackcount); i++) {
		entry = play_tree_new();
		play_tree_add_file(entry, playlist->tracks[i]->filename);
		if(list)
			play_tree_append_entry(last_entry,entry);
		else
			list = entry;
		last_entry = entry;
	}
	entry = play_tree_new();
	play_tree_set_child(entry,list);
	if (entry) {
	  	if(!mpctx->playtree_iter || !mpctx->playtree) {
			if (mpctx->playtree) // the playtree is always a node with one child. let's clear it
				play_tree_free_list(mpctx->playtree->child, 1);
			else mpctx->playtree=play_tree_new(); // .. or make a brand new playtree

			if (!mpctx->playtree) return -1; // couldn't make playtree!

			play_tree_set_child(mpctx->playtree, entry);

			/* Make iterator start at the top the of tree. */
			mpctx->playtree_iter = play_tree_iter_new(mpctx->playtree, mconfig);
			if (!mpctx->playtree_iter) return;

			// find the first real item in the tree
			if (play_tree_iter_step(mpctx->playtree_iter,0,0) != PLAY_TREE_ITER_ENTRY) {
				// no items!
				play_tree_iter_free(mpctx->playtree_iter);
				mpctx->playtree_iter = NULL;
			} else {
				mpctx->eof = PT_NEXT_ENTRY;
				if(index > 0 && index < playlist->trackcount) {
					mpctx->play_tree_step = index;
					play_tree_iter_step(mpctx->playtree_iter,mpctx->play_tree_step,0);
				}
				filename = play_tree_iter_get_file(mpctx->playtree_iter, mpctx->eof);
				mp_input_queue_cmd(mp_input_parse_cmd("stop"));
			}

	  	} else {
			// Go back to the starting point.
			while (play_tree_iter_up_step(mpctx->playtree_iter, 0, 1) != PLAY_TREE_ITER_END)
			    /* NOP */ ;
			play_tree_free_list(mpctx->playtree->child, 1);
			play_tree_set_child(mpctx->playtree, entry);
			pt_iter_goto_head(mpctx->playtree_iter);
			mpctx->eof = PT_NEXT_SRC;
			loop_break = 1;

			if(index > 0 && index < playlist->trackcount)
			{
				play_tree_iter_t *i = play_tree_iter_new_copy(mpctx->playtree_iter);
				if (play_tree_iter_step(i, index, 0) == PLAY_TREE_ITER_ENTRY)
				    mpctx->eof = PT_NEXT_ENTRY;
				play_tree_iter_free(i);
				if (mpctx->eof == PT_NEXT_ENTRY)
					mpctx->play_tree_step = index;
			}
		}
	}
	return 0;
}

static void create_playlistmenu()
{
	playlistmenu = CreatePopupMenu();
	AppendMenu(playlistmenu, MF_STRING, ID_ADDFILE, MenuAdd);
	AppendMenu(playlistmenu, MF_SEPARATOR, 0, 0);
	AppendMenu(playlistmenu, MF_STRING, ID_REMOVE, MenuRemove);
	AppendMenu(playlistmenu, MF_STRING, ID_CLEAR, MenuClear);
	AppendMenu(playlistmenu, MF_SEPARATOR, 0, 0);
	AppendMenu(playlistmenu, MF_STRING, ID_CLOSE,BottonClose);
}

static int display_openfilewindow( int add)
{
	OPENFILENAME fileopen;
	int result = 0;
	char filelist[MAXFILE];
	char lfilename[MAX_PATH];
	char directory[MAX_PATH];
	char *filespec = NULL;
	char *filepart = NULL;

	memset(&fileopen, 0, sizeof(OPENFILENAME));
	memset(filelist, 0, sizeof(filelist));

	fileopen.lStructSize = sizeof(OPENFILENAME);
	fileopen.hwndOwner = hPlaylistWnd;
	fileopen.hInstance = GetModuleHandle(NULL);
	fileopen.lpstrFilter = "Media Files\0*.avi;*.asf;*.mp*;*.dat;*.m*v;*.n*v;*.qt;*.rm*;*.ra*;"
						"*.ts;*.wm*;*.mk*;*.pmp;*.og*;*.vp*;*.vivo;*.vob;*.pls;*.m3u;*.img;*.iso;"
						"*.bin;*.3gp;*.flv;*.ape;*.amr;*.aac;*.tp;\0Any File\0*.*\0";
	fileopen.nFilterIndex = 0;
	fileopen.lpstrTitle = "Add file(s)...";
	fileopen.Flags = OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST| OFN_LONGNAMES | OFN_EXPLORER| OFN_READONLY | OFN_HIDEREADONLY;
	fileopen.lpstrFile = filelist;
	fileopen.lpstrCustomFilter = NULL;
	fileopen.nMaxFile = MAXFILE;

	if(GetOpenFileName(&fileopen))
	{
		/* clear playlist */
		if(!add) playlist->clear_playlist(playlist);

		memcpy(directory, fileopen.lpstrFile, fileopen.nFileOffset - 1);
		directory[fileopen.nFileOffset - 1] = 0;

		do
		{
			filespec = &fileopen.lpstrFile[fileopen.nFileOffset];
			lfilename[0] = 0;
			strcat(lfilename, directory);
			strcat(lfilename, "\\");
			strcat(lfilename, filespec);

			if (GetFileAttributes(lfilename) & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else if (GetFullPathName(lfilename, MAX_PATH, lfilename, &filepart))
			{
				playlist->add_track(playlist, lfilename, NULL, NULL, 0);
				playtree_add_file(mpctx->playtree, lfilename);
				result++;
			}
			fileopen.nFileOffset += strlen(filespec) + 1;
		} while (*filespec);
	}
	return result;
}

static void display_loadplaylistwindow()
{
	OPENFILENAME playlistopen;
	char playlistfile[MAX_PATH];

	memset(&playlistopen, 0, sizeof(OPENFILENAME));
	memset(playlistfile, 0, sizeof(playlistfile));

	playlistopen.lStructSize = sizeof(OPENFILENAME);
	playlistopen.hInstance = GetModuleHandle(NULL);
	playlistopen.hwndOwner = hPlaylistWnd;
	playlistopen.lpstrFilter = "All Files (*.*)\0*.*\0"
							   "Playlist Files (*.m3u;*.pls;*.txt)\0*.m3u;*.pls;*.txt\0";
	playlistopen.nFilterIndex = 0;
	playlistopen.lpstrTitle = "Load Playlist...";
	playlistopen.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_READONLY | OFN_HIDEREADONLY;
	playlistopen.lpstrFile = playlistfile;
	playlistopen.lpstrCustomFilter = NULL;
	playlistopen.nMaxFile = MAXFILE;

	if(GetOpenFileName(&playlistopen))
    {
	}
}

static void display_saveplaylistwindow()
{
	OPENFILENAME playlistsave;
	int i=0;
	static FILE *playlist_file = NULL;
	char playlistname[MAX_PATH];

	memset(&playlistsave, 0, sizeof(OPENFILENAME));
	memset(playlistname, 0, sizeof(playlistname));

	playlistsave.lStructSize = sizeof(OPENFILENAME);
	playlistsave.hInstance = GetModuleHandle(NULL);
	playlistsave.hwndOwner = hPlaylistWnd;
	playlistsave.lpstrFilter = "Playlist Files (*.pls)\0*.pls\0";
	playlistsave.nFilterIndex = 0;
	playlistsave.lpstrTitle = "Save Playlist...";
	playlistsave.Flags = OFN_LONGNAMES | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	playlistsave.lpstrFile = playlistname;
	playlistsave.lpstrCustomFilter = NULL;
	playlistsave.nMaxFile = MAXFILE;

	if(GetSaveFileName(&playlistsave))
	{
		HANDLE my_playlist;

		if(!strstr(playlistname, ".pls")) strcat(playlistname, ".pls");

		my_playlist = CreateFile(playlistname,
								 GENERIC_WRITE,
								 0,
								 NULL,
								 CREATE_ALWAYS,
								 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
								 NULL);

		if(my_playlist != INVALID_HANDLE_VALUE)
		{
			CloseHandle(my_playlist); /* close the file first so we can write to it */
			playlist_file = fopen(playlistsave.lpstrFile, "w");
			fprintf(playlist_file, "[playlist]\n");
			fprintf(playlist_file, "numberofentries=%d\n", playlist->trackcount);

			for(i=0; i<(playlist->trackcount); i++)
			{
				fprintf(playlist_file, "File%i=%s\n", i + 1, playlist->tracks[i]->filename);
				fprintf(playlist_file, "Length%i=-1\n", i + 1);
			}
			fclose(playlist_file);
		}
	}
}

void update_playlist_status(int v)
{
	int i=0;
	char num[16], *str = NULL;
	LV_ITEM item;
	if(!playlist) return;
	ZeroMemory(&item, sizeof(LV_ITEM));
	item.mask = LVIF_TEXT;
	for (i=0; i < playlist->trackcount; i++) {
		str = playlist->tracks[i]->title ? playlist->tracks[i]->title : playlist->tracks[i]->filename;
		if(filename && !stricmp(str, filename))
			sprintf(num, ">>");
		else
			sprintf(num, "");
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = num;
		item.cchTextMax = 16;
		SendDlgItemMessage(hPlaylistWnd, ID_TRACKLIST, LVM_SETITEMTEXT, i, (LPARAM)(&item));
	}
}

static void updatetracklist(HWND hwnd)
{
	int i=0;
	char *str = NULL;
	wchar_t num[16], wstr[MAX_PATH*2], *wstr_n;
	LV_ITEMW item;
	if(!playlist) return;
	ZeroMemory(&item, sizeof(LV_ITEMW));
	item.mask = LVIF_TEXT;
	SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_DELETEALLITEMS, 0, 0);
	for (i=0; i < playlist->trackcount; i++) {
		str = playlist->tracks[i]->title ? playlist->tracks[i]->title : playlist->tracks[i]->filename;
		if(filename && !stricmp(str, filename))
			swprintf(num, L">>");
		else
			swprintf(num, L"");
		MultiByteToWideChar(code_page, 0, str, -1, wstr, MAX_PATH*2);
		wstr_n = wcsrchr(wstr, L'\\');
		if(wstr_n)
			*(wstr_n++) = L'\0';
		else
			wstr_n = wstr;
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = num;
		item.cchTextMax = 16;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_INSERTITEMW, 0, (LPARAM)(&item));
		item.iSubItem = 1;
		item.pszText = wstr_n;
		item.cchTextMax = MAX_PATH*2;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETITEMTEXTW, i, (LPARAM)(&item));
		item.iSubItem = 2;
		item.pszText = wstr;
		item.cchTextMax = MAX_PATH*2;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETITEMTEXTW, i, (LPARAM)(&item));
	}
}

static void up_list_item(HWND hwnd, int sel)
{
	char *str = NULL;
	wchar_t num[16], wstr[MAX_PATH*2], *wstr_n;
	LV_ITEMW item;
	if(!playlist) return;
	if(0 < sel) {
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_DELETEITEM, sel-1, 0);
		str = playlist->tracks[sel]->title ? playlist->tracks[sel]->title : playlist->tracks[sel]->filename;
		if(filename && !stricmp(str, filename))
			swprintf(num, L">>");
		else
			swprintf(num, L"");
		MultiByteToWideChar(code_page, 0, str, -1, wstr, MAX_PATH*2);
		wstr_n = wcsrchr(wstr, L'\\');
		if(wstr_n)
			*(wstr_n++) = L'\0';
		else
			wstr_n = wstr;
		ZeroMemory(&item, sizeof(LV_ITEMW));
		item.mask = LVIF_TEXT;
		item.iItem = sel;
		item.iSubItem = 0;
		item.pszText = num;
		item.cchTextMax = 16;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_INSERTITEMW, 0, (LPARAM)(&item));
		item.iSubItem = 1;
		item.pszText = wstr_n;
		item.cchTextMax = MAX_PATH*2;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETITEMTEXTW, sel, (LPARAM)(&item));
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, sel - 1);
		item.iSubItem = 2;
		item.pszText = wstr;
		item.cchTextMax = MAX_PATH*2;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETITEMTEXTW, sel, (LPARAM)(&item));
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, sel - 1);
	}

}

static void down_list_item(HWND hwnd, int sel)
{
	char *str = NULL;
	wchar_t num[16], wstr[MAX_PATH*2], *wstr_n;
	LV_ITEMW item;
	if(!playlist) return;
	if(sel >= 0 && sel+1 < playlist->trackcount) {
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_DELETEITEM, sel+1, 0);
		str = playlist->tracks[sel]->title ? playlist->tracks[sel]->title : playlist->tracks[sel]->filename;
		if(filename && !stricmp(str, filename))
			swprintf(num, L">>");
		else
			swprintf(num, L"");
		MultiByteToWideChar(code_page, 0, str, -1, wstr, MAX_PATH*2);
		wstr_n = wcsrchr(wstr, L'\\');
		if(wstr_n)
			*(wstr_n++) = L'\0';
		else
			wstr_n = wstr;
		ZeroMemory(&item, sizeof(LV_ITEMW));
		item.mask = LVIF_TEXT;
		item.iItem = sel;
		item.iSubItem = 0;
		item.pszText = num;
		item.cchTextMax = 16;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_INSERTITEMW, 0, (LPARAM)(&item));
		item.iSubItem = 1;
		item.pszText = wstr_n;
		item.cchTextMax = MAX_PATH*2;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETITEMTEXTW, sel, (LPARAM)(&item));
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, sel + 1);
		item.iSubItem = 2;
		item.pszText = wstr;
		item.cchTextMax = MAX_PATH*2;
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETITEMTEXTW, sel, (LPARAM)(&item));
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, sel + 1);
	}
}

static void delete_list_item(HWND hwnd, int sel)
{
	if(!playlist) return;
	if(sel >= 0 && sel <= playlist->trackcount) {
		SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_DELETEITEM, sel, 0);
		if(sel < playlist->trackcount)
			SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, sel);
		else if(playlist->trackcount > 0)
			SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, sel-1);
	}
}

static void add_directory_to_playlist(char *dir)
{
	WIN32_FIND_DATA ff;
	HANDLE f;
	char file[MAX_PATH*2], path[MAX_PATH+1];
	char *ex, *p;
	int i;
	playlist_t *pl = playlist;

	if(!dir) return;

	GetCurrentDirectory(MAX_PATH, path);
	SetCurrentDirectory(dir);
	f = FindFirstFile("*.*", &ff);
	do {
		p = ff.cFileName;
		ex =  strrchr(p, '.');
		if(!ex)
			continue;

		for(i = 0; video_exts[i]; i++) {
			if(stricmp(ex+1 ,video_exts[i]) == 0) {
				sprintf(file, "%s\\%s",dir , p);
				pl->add_track(pl, file, NULL, NULL, 0);
				playtree_add_file(mpctx->playtree, file);
				continue;
			}
		}

		for(i = 0; audio_exts[i]; i++) {
			if(stricmp(ex+1 ,audio_exts[i]) == 0) {
				sprintf(file, "%s\\%s",dir , p);
				pl->add_track(pl, file, NULL, NULL, 0);
				playtree_add_file(mpctx->playtree, file);
				continue;
			}
		}

	} while ( FindNextFile(f, &ff) );
	FindClose(f);

	SetCurrentDirectory(path);
}

static int get_select_pos(HWND hwnd)
{
	int i, now_y = 0;
	POINT p;
	up_y = -1;
	down_y = 0x7fffffff;
	old_selected = 0;
	i = SendMessage(hwnd, LVM_GETSELECTIONMARK, 0, 0);
	if(0 <= i) {
		if(SendMessage(hwnd, LVM_GETITEMPOSITION, i+1, &p))
			now_y = p.y;

		if(i > 0 && SendMessage(hwnd, LVM_GETITEMPOSITION, i-1, &p))
			up_y = p.y;

		if(i+1 < playlist->trackcount && SendMessage(hwnd, LVM_GETITEMPOSITION, i+1, &p))
			down_y = p.y * 2 -now_y;

		old_selected = i+1;
		return 1;
	}
	return 0;
}

static LRESULT CALLBACK ListProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int now_y = 0;
	playlist_t *pl = playlist;
	switch (iMsg)
	{
		case WM_LBUTTONDOWN:
			button_down = TRUE;
			break;
		case WM_LBUTTONUP:
			button_down = FALSE;
			break;
		case WM_MOUSEMOVE:
			if(!pl || pl->trackcount <= 0)
				break;
			if(button_down && get_select_pos(hwnd)) {
				now_y = HIWORD(lParam);
				if(now_y > down_y-2) {
					pl->movedown_track(pl, old_selected);
					down_list_item(hPlaylistWnd, old_selected-1);
					need_update_playtree = 1;
				} else if(now_y < up_y+3) {
					pl->moveup_track(pl, old_selected);
					up_list_item(hPlaylistWnd, old_selected-1);
					need_update_playtree = 1;
				}
			}
			break;
	}
	return CallWindowProc(wdgProc, hwnd, iMsg, wParam, lParam);
}

static LRESULT CALLBACK PlayListWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HWND wdg;
	POINT cursor;
	playlist_t *pl = playlist;
	switch (iMsg)
	{
		case WM_CREATE:
		{
			RECT r;
			LOGFONT lf;
			memset(&lf,0, sizeof(LOGFONT));
			strcpy(lf.lfFaceName,"MS Shell Dlg");
			lf.lfHeight = -10;
			lf.lfWeight = FW_NORMAL;
			HFONT   hFont=CreateFontIndirect(&lf);
			wdg = CreateWindow("button", BottonPlayAll,
							   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
							   4, 10, 80, 22, hwnd,
							   (HMENU) ID_PLAY,
							   ((LPCREATESTRUCT) lParam) -> hInstance,
							   NULL);
  			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			wdg = CreateWindow ("button", BottonUp,
								WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								4, 35, 80, 22, hwnd,
								(HMENU) ID_UP,
								((LPCREATESTRUCT) lParam) -> hInstance,
								NULL);
			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			wdg = CreateWindow ("button",BottonDown,
								WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								4, 60, 80, 22, hwnd,
								(HMENU) ID_DOWN,
								((LPCREATESTRUCT) lParam) -> hInstance,
								NULL);
  			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			wdg = CreateWindow ("button", BottonRemove,
								WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								4, 85, 80, 22, hwnd,
								(HMENU) ID_REMOVE,
								((LPCREATESTRUCT) lParam) -> hInstance,
								NULL);
  			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			wdg = CreateWindow ("button", BottonAdd,
								WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								4, 110, 80, 22, hwnd,
								(HMENU) ID_ADDFILE,
								((LPCREATESTRUCT) lParam) -> hInstance,
								NULL);
  			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			wdg = CreateWindow ("button", BottonSave,
								WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								4, 135, 80, 22, hwnd,
								(HMENU) ID_PLAYLISTSAVE,
								((LPCREATESTRUCT) lParam) -> hInstance,
								NULL);
  			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			wdg = CreateWindow ("button", BottonClose,
								WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
								4, 185, 80, 22, hwnd,
								(HMENU) ID_CLOSE,
								((LPCREATESTRUCT) lParam) -> hInstance,
								NULL);
  			SendMessage(wdg,   WM_SETFONT,   (WPARAM)hFont,   0);

			GetClientRect(hwnd, &r);
			wdg = CreateWindowEx(WS_EX_CLIENTEDGE, "SysListView32", "tracklist",
					WS_CHILD | WS_VISIBLE | LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER /*| LVS_NOCOLUMNHEADER*/,
					90, 7, r.right-90, r.bottom-7, hwnd, (HMENU) ID_TRACKLIST, ((LPCREATESTRUCT) lParam) -> hInstance, NULL);

  			SendMessage(wdg, WM_SETFONT, (WPARAM)hFont, 0);
			wdgProc = SetWindowLong(wdg, GWL_WNDPROC, (LONG)ListProc);
			LV_COLUMNA lvc, lvc1, lvc2;
			ZeroMemory(&lvc, sizeof(LV_COLUMNA));
			ZeroMemory(&lvc1, sizeof(LV_COLUMNA));
			ZeroMemory(&lvc2, sizeof(LV_COLUMNA));
			lvc.mask = LVCF_WIDTH;
			lvc.cx = 24;
			lvc1.mask = LVCF_WIDTH;
			lvc1.cx = 700;
			lvc2.mask = LVCF_WIDTH;
			lvc2.cx = 500;
			SendMessage(wdg, LVM_INSERTCOLUMN,0, &lvc);
			SendMessage(wdg, LVM_INSERTCOLUMN,1, &lvc1);
			SendMessage(wdg, LVM_INSERTCOLUMN,2, &lvc2);
			SendMessage(wdg, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
			break;
		}
		case WM_CONTEXTMENU:
		{
			GetCursorPos(&cursor);
			SetForegroundWindow(hwnd);
			TrackPopupMenu(playlistmenu, 0, cursor.x, cursor.y, 0, hwnd, NULL);
			break;
		}
		case WM_NOTIFY:
		{
			if(wParam == ID_TRACKLIST) {
				NMHDR *nmhdr = (NMHDR *)lParam;
				if(nmhdr && nmhdr->code == NM_DBLCLK) {
					int selected = SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_GETSELECTIONMARK, 0, 0);
					if(0 <= selected) {
						pl->current = selected;
						playtree_update_play(mpctx, pl->current);
						return 0;
					}
				} else if(nmhdr && nmhdr->code == NM_CLICK)
					button_down = FALSE;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(LOWORD(lParam), HIWORD(lParam)));
			break;
		case WM_NCLBUTTONUP:
		case WM_LBUTTONUP:
			button_down = FALSE;
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					ShowWindow(hwnd, SW_HIDE);
					break;
			}
			break;
		case WM_COMMAND:
		{
			int selected = 0;
			int i = SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_GETSELECTIONMARK, 0, 0);
			if(0 <= i) selected = i + 1;
			switch (LOWORD(wParam))
			{
				case ID_CLOSE:
					if(SetLayeredWinAttributes) {
						if(transparent) {
							transparent = 0;
							SetLayeredWinAttributes(hPlaylistWnd, 0, 255, 2);
						} else {
							transparent = 1;
							SetLayeredWinAttributes(hPlaylistWnd, 0, controlbar_alpha, 2);
						}
					}
					break;
				case ID_PLAY:
					if(selected) {
						pl->current = selected - 1;
						playtree_update_play(mpctx, pl->current);
					} else
						playtree_update_play(mpctx, 0);
					return 0;
				case ID_UP:
					if(selected) pl->moveup_track(pl, selected);
					selected--;
					up_list_item(hwnd, selected);
					need_update_playtree = 1;
					return 0;
				case ID_DOWN:
					if(selected) pl->movedown_track(pl, selected);
					down_list_item(hwnd, selected-1);
					selected++;
					need_update_playtree = 1;
					return 0;
				case ID_REMOVE:
					if(selected) pl->remove_track(pl, selected);
					delete_list_item(hwnd, selected-1);
					need_update_playtree = 1;
					return 0;
				case ID_ADDFILE:
					display_openfilewindow(1);
					break;
				case ID_CLEAR:
					if(!playlist->trackcount) return 0;
					playlist->clear_playlist(playlist);
					SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_DELETEALLITEMS, 0, 0);
					need_update_playtree = 1;
					return 0;
				case ID_PLAYLISTLOAD:
					display_loadplaylistwindow();
					break;
				case ID_PLAYLISTSAVE:
				{
					/* no point saving an empty playlist */
					display_saveplaylistwindow();
					break;
				}
			}
			updatetracklist(hwnd);
			if(selected < 1) selected = 1;
			else if(selected>pl->trackcount) selected = pl->trackcount;
			SendDlgItemMessage(hwnd, ID_TRACKLIST, LVM_SETSELECTIONMARK, 0, selected - 1);
			return 0;
		}
		case WM_DROPFILES:
		{
			DWORD t;
			char file[MAX_PATH];
			wchar_t tmpw[MAX_PATH], tmpshot[MAX_PATH];
			int filecount = DragQueryFile((HDROP) wParam, -1, file, MAX_PATH);
			int i;
			for (i=0; i<filecount; i++)
			{
				DragQueryFile((HDROP) wParam, i, file, MAX_PATH);
				t = GetFileAttributes(file);
				if (t & 0x10) {
					if(t == 0xFFFFFFFF) {
						DragQueryFileW((HDROP)wParam, i, tmpw, MAX_PATH);
						GetShortPathNameW(tmpw,tmpshot,MAX_PATH);
						wcstombs(file,tmpshot,MAX_PATH);
					}
					add_directory_to_playlist(file);
				} else {
					if(t == 0xFFFFFFFF) {
						DragQueryFileW((HDROP)wParam, i, tmpw, MAX_PATH);
						GetShortPathNameW(tmpw,tmpshot,MAX_PATH);
						wcstombs(file,tmpshot,MAX_PATH);
					}
					pl->add_track(pl, file, NULL, NULL, 0);
					playtree_add_file(mpctx->playtree, file);
				}
			}
			DragFinish((HDROP) wParam);
			updatetracklist(hwnd);
			break;
		}
		case WM_SIZING:
		{
			LPRECT rc = (LPRECT)lParam;
			if(rc->right - rc->left < WinWidth-100)
				rc->right = rc->left + WinWidth-100;
			if(rc->bottom - rc->top < WinHeight-30)
				rc->bottom = rc->top + WinHeight-30;
			break;
		}
		case WM_SIZE:
		{
			RECT r;
			GetClientRect(hwnd, &r);
			MoveWindow(GetDlgItem(hwnd, ID_TRACKLIST), 90, 7, r.right-90, r.bottom-7, TRUE);
			break;
		}
		case WM_SYSCOMMAND:
		{
			if (wParam == SC_MINIMIZE) {
				ShowWindow(hwnd, SW_HIDE);
				return 0;
			}
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void update_playlistwindow(void)
{
	if(!need_update_playtree) {
		playlist->clear_playlist(playlist);
		import_playtree_into_playlist(mpctx->playtree, mconfig);
	}
	if (hPlaylistWnd) updatetracklist(hPlaylistWnd);
}

void close_playlistwindow(void)
{
	if(hPlaylistWnd)
		PostMessage(hPlaylistWnd, WM_CLOSE, 0, 0);
}

static int init_playlist()
{
	WNDCLASS wc;
	HINSTANCE user32;
	LANGID _SysLangId;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	if(hPlaylistWnd) {
		updatetracklist(hPlaylistWnd);
		ShowWindow(hPlaylistWnd, SW_SHOW);
		return 0;
	}

	if(sys_Language == 1) {
		LoadString(hInstance, IDS_PLAYLIST_SC, TitalPlaylist, 20);
		LoadString(hInstance, IDS_BT_PLAY_SC, BottonPlayAll, 20);
		LoadString(hInstance, IDS_BT_UP_SC, BottonUp, 20);
		LoadString(hInstance, IDS_BT_DOWN_SC, BottonDown, 20);
		LoadString(hInstance, IDS_BT_ADD_SC, BottonAdd, 20);
		LoadString(hInstance, IDS_BT_REMOVE_SC, BottonRemove, 20);
		LoadString(hInstance, IDS_BT_SAVE_SC, BottonSave, 20);
		LoadString(hInstance, IDS_BT_CLOSE_SC, BottonClose, 20);
		LoadString(hInstance, IDS_MU_ADD_SC, MenuAdd, 20);
		LoadString(hInstance, IDS_MU_REMOVE_SC, MenuRemove, 20);
		LoadString(hInstance, IDS_MU_CLEAR_SC, MenuClear, 20);
		LoadString(hInstance, IDS_MU_CLOSE_SC, MenuClose, 20);
	} else if(sys_Language == 3 || sys_Language == 4) {
		LoadString(hInstance, IDS_PLAYLIST_TC, TitalPlaylist, 20);
		LoadString(hInstance, IDS_BT_PLAY_TC, BottonPlayAll, 20);
		LoadString(hInstance, IDS_BT_UP_TC, BottonUp, 20);
		LoadString(hInstance, IDS_BT_DOWN_TC, BottonDown, 20);
		LoadString(hInstance, IDS_BT_ADD_TC, BottonAdd, 20);
		LoadString(hInstance, IDS_BT_REMOVE_TC, BottonRemove, 20);
		LoadString(hInstance, IDS_BT_SAVE_TC, BottonSave, 20);
		LoadString(hInstance, IDS_BT_CLOSE_TC, BottonClose, 20);
		LoadString(hInstance, IDS_MU_ADD_TC, MenuAdd, 20);
		LoadString(hInstance, IDS_MU_REMOVE_TC, MenuRemove, 20);
		LoadString(hInstance, IDS_MU_CLEAR_TC, MenuClear, 20);
		LoadString(hInstance, IDS_MU_CLOSE_TC, MenuClose, 20);
	} else {
		LoadString(hInstance, IDS_PLAYLIST_EN, TitalPlaylist, 20);
		LoadString(hInstance, IDS_BT_PLAY_EN, BottonPlayAll, 20);
		LoadString(hInstance, IDS_BT_UP_EN, BottonUp, 20);
		LoadString(hInstance, IDS_BT_DOWN_EN, BottonDown, 20);
		LoadString(hInstance, IDS_BT_ADD_EN, BottonAdd, 20);
		LoadString(hInstance, IDS_BT_REMOVE_EN, BottonRemove, 20);
		LoadString(hInstance, IDS_BT_SAVE_EN, BottonSave, 20);
		LoadString(hInstance, IDS_BT_CLOSE_EN, BottonClose, 20);
		LoadString(hInstance, IDS_MU_ADD_EN, MenuAdd, 20);
		LoadString(hInstance, IDS_MU_REMOVE_EN, MenuRemove, 20);
		LoadString(hInstance, IDS_MU_CLEAR_EN, MenuClear, 20);
		LoadString(hInstance, IDS_MU_CLOSE_EN, MenuClose, 20);
	}

	code_page = CP_ACP;
	_SysLangId = GetSystemDefaultLangID();
	if(PRIMARYLANGID(_SysLangId) == LANG_CHINESE) {
		if(SUBLANGID(_SysLangId) == SUBLANG_CHINESE_SIMPLIFIED)
			code_page = 936;
		else
			code_page = 950;
	}

	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = PlayListWndProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	 = hInstance;
	wc.hCursor	   = LoadCursor(NULL,IDC_ARROW);
	mplayericon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	wc.hIcon		 = mplayericon;
	wc.hbrBackground = SOLID_GREY;
	wc.lpszClassName = "MPlayer - Playlist";
	wc.lpszMenuName  = NULL;
	RegisterClass(&wc);
	create_playlistmenu();
	bkgndbrush = CreateSolidBrush(RGB(236,236,236));
	if(playlist_x < 0)
		playlist_x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (WinWidth / 2);   /* Erik: center popup window on screen */
	if(playlist_y < 0)
		playlist_y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (WinHeight / 2);
	hPlaylistWnd = CreateWindow("MPlayer - Playlist",
						TitalPlaylist, WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
						playlist_x, playlist_y, WinWidth, WinHeight,
						playlist_parent, NULL, hInstance, NULL);

	if((vo_dirver != VO_DIRV_OPENGL || is_vista) && controlbar_alpha > 0) {
		user32 = GetModuleHandle("user32.dll");
		if (user32) {
			SetLayeredWinAttributes = GetProcAddress(user32, "SetLayeredWindowAttributes");
			if(SetLayeredWinAttributes) {
				SetWindowLong(hPlaylistWnd, GWL_EXSTYLE, GetWindowLong(hPlaylistWnd ,GWL_EXSTYLE) | WS_EX_LAYERED );
				SetLayeredWinAttributes(hPlaylistWnd, 0, 255, 2);
			}
		}
	}

	DragAcceptFiles(hPlaylistWnd, TRUE);

	if(!playlist)
		playlist = create_playlist();
	if(!need_update_playtree) {
		playlist->clear_playlist(playlist);
		import_playtree_into_playlist(mpctx->playtree, mconfig);
	}
	updatetracklist(hPlaylistWnd);
	SetWindowPos(hPlaylistWnd, playlist_layer,playlist_x,playlist_y,WinWidth,WinHeight,SWP_SHOWWINDOW);

	return 1;
}

static DWORD WINAPI playlistProc(LPVOID lpParam){
	MSG msg;

	SetThreadPriority(hPlaylistThread, THREAD_PRIORITY_LOWEST);

	if(!init_playlist())
		return 0;

	while (1) {
		int ret = GetMessage(&msg,NULL,0,0);
		if (!ret) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	hPlaylistWnd = NULL;
	hPlaylistThread = NULL;

	return 0;
}

void display_playlist(int x,int y,HWND parent,HWND layer)
{
	playlist_x = x;
	playlist_y = y;
	playlist_layer = layer;
	playlist_parent = parent;

	if(hPlaylistThread) {
		if(hPlaylistWnd) 
			SetWindowPos(hPlaylistWnd, playlist_layer, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		return;
	}

	hPlaylistThread = CreateThread(NULL, 0, playlistProc, NULL, 0, NULL);
}

void display_playlistwindow(int x,int y,HWND layer)
{
	display_playlist(x, y, NULL, layer);
}
