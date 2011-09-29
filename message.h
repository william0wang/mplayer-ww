#ifndef _MPLAYER_WW_MESSAGE_WINDOW_H
#define _MPLAYER_WW_MESSAGE_WINDOW_H

typedef enum {
	MESSAGE_TYPE_FONTCONFIG = 0,
	MESSAGE_TYPE_STREAM,
	MESSAGE_TYPE_D3DX9,
} MESSAGE_TYPE;

void show_message(int type);
void end_message(void);
void pop_message(int type, int time);

#endif
