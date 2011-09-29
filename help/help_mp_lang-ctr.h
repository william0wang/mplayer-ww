// $Revision: 24423 $
// MASTER FILE. Use this file as base for translations.
// Translated files should be sent to the mplayer-DOCS mailing list or
// to the help messages maintainer, see DOCS/tech/MAINTAINERS.
// The header of the translated file should contain credits and contact
// information. Before major releases we will notify all translators to update
// their files. Please do not simply translate and forget this, outdated
// translations quickly become worthless. To help us spot outdated files put a
// note like "sync'ed with help_mp-en.h XXX" in the header of the translation.
// Do NOT translate the above lines, just follow the instructions.

// ========================= MPlayer help ===========================

static char en_help_text[]=
"Usage:   mplayer [options] [url|path/]filename\n"
"\n"
"Basic options: (complete list in the man page)\n"
" -vo <drv>        select video output driver ('-vo help' for a list)\n"
" -ao <drv>        select audio output driver ('-ao help' for a list)\n"
#ifdef CONFIG_VCD
" vcd://<trackno>  play (S)VCD (Super Video CD) track (raw device, no mount)\n"
#endif
#ifdef USE_DVDREAD
" dvd://<titleno>  play DVD title from device instead of plain file\n"
" -alang/-slang    select DVD audio/subtitle language (by 2-char country code)\n"
#endif
" -ss <position>   seek to given (seconds or hh:mm:ss) position\n"
" -nosound         do not play sound\n"
" -fs              fullscreen playback (or -vm, -zoom, details in the man page)\n"
" -x <x> -y <y>    set display resolution (for use with -vm or -zoom)\n"
" -sub <file>      specify subtitle file to use (also see -subfps, -subdelay)\n"
" -playlist <file> specify playlist file\n"
" -vid x -aid y    select video (x) and audio (y) stream to play\n"
" -fps x -srate y  change video (x fps) and audio (y Hz) rate\n"
" -pp <quality>    enable postprocessing filter (details in the man page)\n"
" -framedrop       enable frame dropping (for slow machines)\n"
"\n"
"Basic keys: (complete list in the man page, also check input.conf)\n"
" <-  or  ->       seek backward/forward 10 seconds\n"
" down or up       seek backward/forward  1 minute\n"
" pgdown or pgup   seek backward/forward 10 minutes\n"
" < or >           step backward/forward in playlist\n"
" p or SPACE       pause movie (press any key to continue)\n"
" q or ESC         stop playing and quit program\n"
" + or -           adjust audio delay by +/- 0.1 second\n"
" o                cycle OSD mode:  none / seekbar / seekbar + timer\n"
" * or /           increase or decrease PCM volume\n"
" x or z           adjust subtitle delay by +/- 0.1 second\n"
" r or t           adjust subtitle position up/down, also see -vf expand\n"
"\n"
" * * * SEE THE MAN PAGE FOR DETAILS, FURTHER (ADVANCED) OPTIONS AND KEYS * * *\n"
"\n";

