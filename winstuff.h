
#ifndef _WIN_STUFF_H
#define _WIN_STUFF_H

#define PAUSE_HACK 1

typedef enum {
	CMD_INIT_WINDOW,
	CMD_UNINIT_WINDOW,
	CMD_ACTIVE_WINDOW,
	CMD_SWITCH_ASPECT,
	CMD_SWITCH_VIEW,
	CMD_UPDATE_VOLUME,
	CMD_UPDATE_SEEK,
	CMD_UPDATE_TITLE,
	CMD_UPDATE_STATUS,
	CMD_UPDATE_SUBMENU,
	CMD_INIT_SUBMENU,
	CMD_ADD_SUBMENU,
	CMD_DEL_SUBMENU,
	CMD_PAUSE_CONTINUE,
	CMD_UNINITONLY_WINDOW,
	CMD_CONFIGURE_FINISHED,
} GUI_CMD;

typedef enum {
	VO_DIRV_DIRECTX,
	VO_DIRV_DIRECT3D,
	VO_DIRV_OPENGL,
	VO_DIRV_SDL,
	VO_DIRV_OTHER,
} VO_DIRVER;

int win_init(int argc,char** argv);
void win_uninit(int);
int detect_vo_system(void);
int initStatus(void);
int getStatusNum(void);
char * getNameById(int id);
int getStatusById(int id);
int addStatus(const char *file);
void freeStatus(void);
void SaveStatus(void);
int GetFontFile(char *name);
int GetSubFile(char **name);
int GetOpenFile(char *title, char **name, char **pCmdline);
int GetOpenFileW(char *title, char **name, char **pCmdline);
char *GetNextEpisode(char *filename);
void directory_to_playlist(char *dir);
int guiCommand(GUI_CMD cmd, int v);
int GetStatus(char *name);
void show_media_info(void);
void seek2time(int pos);
void check_update(void);
int get_seek_pos(float percent);
int get_seek_offset(float percent);
int TestUnicodeFilename();
void SaveOldStatus();
int ConfirmStatus();
int get_video_equalizer(const char * name);
void disable_screen_saver(int disable);
int w32Cmd(GUI_CMD cmd, int v);
int guiCmd(GUI_CMD cmd, int v);

extern char *mplayer_pls;
#endif

