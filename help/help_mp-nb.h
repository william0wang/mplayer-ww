// Transated by:  Andreas Berntsen  <andreasb@kvarteret.org>
// Updated for 0.60 by: B. Johannessen <bob@well.com>

// ========================= MPlayer hjelp ===========================

#ifdef CONFIG_VCD
#define MSGTR_HelpVCD " vcd://<sporno>   spill VCD (video cd) spor fra enhet i stedet for fil\n"
#else
#define MSGTR_HelpVCD
#endif

#ifdef CONFIG_DVDREAD
#define MSGTR_HelpDVD " dvd://<tittelno> spill DVD tittel/spor fra enhet i stedet for fil\n"
#else
#define MSGTR_HelpDVD
#endif

#define MSGTR_Help \
"Bruk:    mplayer [valg] [sti/]filnavn\n"\
"\n"\
"Valg:\n"\
" -vo <drv[:dev]> velg video-ut driver og enhet (se '-vo help' for liste)\n"\
" -ao <drv[:dev]> velg lyd-ut driver og enhet (se '-ao help' for liste)\n"\
MSGTR_HelpVCD \
MSGTR_HelpDVD \
" -ss <timepos>   søk til gitt (sekunder eller hh:mm:ss) posisjon\n"\
" -nosound        ikke spill av lyd\n"\
" -channels <n>   målnummer for lyd output kanaler\n"\
" -fs -vm -zoom   fullskjerm avspillings valg (fullscr,vidmode chg,softw.scale)\n"\
" -x <x> -y <y>   skaler bildet til <x> * <y> oppløsning [hvis -vo driver støtter det!]\n"\
" -sub <fil>      spesifiser hvilken subtitle fil som skal brukes (se også -subfps, -subdelay)\n"\
" -vid x -aid y   spesifiser hvilken video (x) og lyd (y) stream som skal spilles av\n"\
" -fps x -srate y spesifiser video (x fps) og lyd (y Hz) hastiget\n"\
" -pp <quality>   slå på etterbehandlingsfilter (0-4 for DivX, 0-63 for mpeg)\n"\
" -nobps          bruk alternativ A-V sync metode for AVI filer (kan være nyttig!)\n"\
" -framedrop      slå på bilde-dropping (for trege maskiner)\n"\
" -wid <window id> bruk eksisterende vindu for video output (nytting med plugger!)\n"\
"\n"\
"Tastatur:\n"\
" <- eller ->       søk bakover/fremover 10 sekunder\n"\
" opp eller ned     søk bakover/fremover 1 minutt\n"\
" < or >            søk bakover/fremover i playlisten\n"\
" p eller MELLOMROM pause filmen (trykk en tast for å fortsette)\n"\
" q eller ESC       stopp avspilling og avslutt programmet\n"\
" + eller -         juster lyd-forsinkelse med +/- 0.1 sekund\n"\
" o                 gå gjennom OSD modi:  ingen / søkelinje / søkelinje+tidsvisning\n"\
" * eller /         øk eller mink volumet (trykk 'm' for å velge master/pcm)\n"\
" z or x            juster undertittelens forsinkelse med +/- 0.1 sekund\n"\
"\n"\
" * * * SE PÅ MANSIDE FOR DETALJER, FLERE (AVANSERTE) VALG OG TASTER! * * *\n"\
"\n"

static const char help_text[] = MSGTR_Help;

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\nAvslutter...\n"
#define MSGTR_ExitingHow "\nAvslutter... (%s)\n"
#define MSGTR_Exit_quit "Avslutt"
#define MSGTR_Exit_eof "Slutt på filen"
#define MSGTR_Exit_error "Fatal feil"
#define MSGTR_IntBySignal "\nMPlayer avbrutt av signal %d i modul: %s \n"
#define MSGTR_NoHomeDir "Kan ikke finne HOME katalog\n"
#define MSGTR_GetpathProblem "get_path(\"config\") problem\n"
#define MSGTR_CreatingCfgFile "Oppretter konfigurasjonsfil: %s\n"
#define MSGTR_CantLoadFont "Kan ikke laste skrifttype: %s\n"
#define MSGTR_CantLoadSub "Kan ikke laste undertitler: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATALT: valgte stream mangler!\n"
#define MSGTR_CantOpenDumpfile "Kan ikke åpne dump fil!!!\n"
#define MSGTR_CoreDumped "core dumpet :)\n"
#define MSGTR_FPSnotspecified "FPS ikke spesifisert (eller ugyldig) i headeren! Bruk -fps valget!\n"
#define MSGTR_CantFindAudioCodec "Kan ikke finne codec for lydformat 0x%X!\n"
#define MSGTR_CantFindVideoCodec "Kan ikke finne codec for videoformat 0x%X!\n"
#define MSGTR_VOincompCodec "Desverre, valgt video_out enhet er inkompatibel med denne codec'en.\n"
#define MSGTR_CannotInitVO "FATALT: Kan ikke initialisere video driver!\n"
#define MSGTR_CannotInitAO "kunne ikke åpne/initialisere lyd-enhet -> NOSOUND\n"
#define MSGTR_StartPlaying "Starter avspilling...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"         ************************************************************\n"\
"         **** Systemed ditt er for TREGT til å spille av dette!  ****\n"\
"         ************************************************************\n"\
"!!! Mulige årsaker, problemer, løsninger: \n"\
"- Vanligste problem: ødelagte _lyd_ drivere, eller lyddrivere med feil. \n"\
"  Prøv: -ao sdl eller bruk ALSA 0.5/oss emuleringen i ALSA 0.9. Les også\n"\
"  DOCS/HTML/en/audio.html for flere tips!\n"\
"- Treg video output. Prøv en annen -vo driver (for liste: -vo help) eller\n"\
"  prøv med -framedrop! Les DOCS/HTML/en/video.html for flere tips\n"\
"- Treg CPU. ikke forsøk å spille av store dvd/divx filer på en treg CPU!\n"\
"  forsøk -hardframedrop\n"\
"- Feil på filen. forsøk forskjellige kombinasjoner av disse:\n"\
"  -nobps  -ni  -mc 0  -forceidx\n"\
"Dersom dette ikke hjelper, les DOCS/HTML/en/bugreports.html!\n\n"

#define MSGTR_NoGui "MPlayer er kompilert uten GUI-støtte!\n"
#define MSGTR_GuiNeedsX "MPlayer GUI trenger X11!\n"
#define MSGTR_Playing "Spiller %s\n"
#define MSGTR_NoSound "Lyd: ingen lyd!!!\n"
#define MSGTR_FPSforced "FPS tvunget til %5.3f  (ftime: %5.3f)\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM enhet '%s' ikke funnet!\n"
#define MSGTR_ErrTrackSelect "Feil under valg av VCD spor!"
#define MSGTR_ReadSTDIN "Leser fra stdin...\n"
#define MSGTR_FileNotFound "Finner ikke filen: '%s'\n"

#define MSGTR_CantOpenDVD "Kan ikke åpne DVD enhet: %s (%s)\n"
#define MSGTR_DVDnumTitles "Det er %d titler på denne DVD.\n"
#define MSGTR_DVDinvalidTitle "Ugyldig DVD tittelnummer: %d\n"
#define MSGTR_DVDnumAngles "Det er %d vinkler i denne DVD tittelen.\n"
#define MSGTR_DVDinvalidAngle "Ugyldig DVD vinkel nummer: %d\n"
#define MSGTR_DVDnoIFO "Kan ikke åpne IFO filen for DVD tittel %d.\n"
#define MSGTR_DVDnoVOBs "Kan ikke åpne VOBS tittel (VTS_%02d_1.VOB).\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "Advarsel! lyd stream header %d redefinert!\n"
#define MSGTR_VideoStreamRedefined "Advarsel! video stream header %d redefinert!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: For mange (%d i %d bytes) lyd pakker i bufferen!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: For mange (%d i %d bytes) video pakker i bufferen!\n"
#define MSGTR_MaybeNI "(kanskje du spiller av en ikke-interleaved stream/fil eller codec'en feilet)\n"
#define MSGTR_Detected_XXX_FileFormat "Detekterte %s filformat!\n"
#define MSGTR_FormatNotRecognized "======== Beklager, dette filformatet er ikke gjenkjent/støttet ===============\n"\
                                  "=== Hvis det er en AVI, ASF eller MPEG stream, kontakt utvikleren! ===\n"
#define MSGTR_MissingVideoStream "Ingen video stream funnet!\n"
#define MSGTR_MissingAudioStream "Ingen lyd stream funnet...  ->nosound\n"
#define MSGTR_MissingVideoStreamBug "Manglende video stream!? Kontakt utvikleren, det kan være en  feil :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: filen inneholder ikke valgte lyd eller video stream\n"

#define MSGTR_NI_Forced "Tvunget"
#define MSGTR_NI_Detected "Detekterte"
#define MSGTR_NI_Message "%s IKKE-INTERLEAVED AVI filformat!\n"

#define MSGTR_UsingNINI "Bruker NON-INTERLEAVED Ødelagt AVI filformat!\n"
#define MSGTR_CouldntDetFNo "Kan ikke bestemme antall frames (for absolutt søk)  \n"
#define MSGTR_CantSeekRawAVI "Kan ikke søke i rå .AVI streams! (index behøves, prøv med -idx valget!)  \n"
#define MSGTR_CantSeekFile "Kan ikke søke i denne filen!  \n"

#define MSGTR_MOVcomprhdr "MOV: Komprimerte headere ikke støttet (enda)!\n"
#define MSGTR_MOVvariableFourCC "MOV: Advarsel! variabel FOURCC detektert!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Advarsel! for mange sport!"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "kunne ikke åpne codec\n"
#define MSGTR_CantCloseCodec "kunne ikke lukke codec\n"

#define MSGTR_MissingDLLcodec "FEIL: Kunne ikke åpne nødvendig DirectShow codec: %s\n"
#define MSGTR_ACMiniterror "Kunne ikke laste/initialisere Win32/ACM AUDIO codec (manglende DLL fil?)\n"
#define MSGTR_MissingLAVCcodec "Kan ikke finne codec '%s' i libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATALT: EOF under søking etter sekvens header\n"
#define MSGTR_CannotReadMpegSequHdr "FATALT: Kan ikke lese sekvens header!\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATALT: Kan ikke lese sekvens header tillegg!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Feil i sekvens header!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Feil i sekvens header tillegg!\n"

#define MSGTR_ShMemAllocFail "Kan ikke allokere delt minne\n"
#define MSGTR_CantAllocAudioBuf "Kan ikke allokere lyd-ut buffer\n"

#define MSGTR_UnknownAudio "Ukjent/manglende lydformat, bruker nosound\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Feil under åpning av lirc!\n"
#define MSGTR_LIRCcfgerr "Feil under lesing av lirc konfigurasjonsfil %s!\n"


// ================================ GUI ================================

#define MSGTR_GUI_AboutMPlayer "Om MPlayer"
#define MSGTR_GUI_Add "Legg til"
#define MSGTR_GUI_AudioTracks "Lyd språk"
#define MSGTR_GUI_AvailableSkins "Skins"
#define MSGTR_GUI_Cancel "Avbryt"
#define MSGTR_GUI_ChapterNN "Kapittel %2d"
#define MSGTR_GUI_Chapters "Kapittel"
#define MSGTR_GUI_DVD "DVD"
#define MSGTR_GUI_Error "fail..."
#define MSGTR_GUI_ErrorFatal "fatal feil..."
#define MSGTR_GUI_File "Spill file"
#define MSGTR_GUI_MSG_MemoryErrorWindow "Beklager, ikke nok minne til tegnebuffer."
#define MSGTR_GUI_MSG_SkinBitmapConversionError "24 bit til 32 bit konverteringsfeil (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapNotFound "finner ikke filen (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapPngReadError "PNG lesefeil (%s)\n"
#define MSGTR_GUI_MSG_SkinCfgNotFound "Skin ikke funnet (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorBitmap16Bit "16 bits eller minde bitmap ikke støttet (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorMessage "[skin] feil i skin konfigurasjonsfil linje %d: %s"
#define MSGTR_GUI_MSG_SkinFontFileNotFound "skrifttypefil ikke funnet\n"
#define MSGTR_GUI_MSG_SkinFontImageNotFound "skrifttype image fil ikke funnet\n"
#define MSGTR_GUI_MSG_SkinFontNotFound "ikke-ekstisterende skrifttype identifikasjon (%s)\n"
#define MSGTR_GUI_MSG_SkinMemoryError "ikke nok minne\n"
#define MSGTR_GUI_MSG_SkinTooManyFonts "for mange skrifttyper deklarert\n"
#define MSGTR_GUI_MSG_SkinUnknownMessage "ukjent beskjed: %s\n"
#define MSGTR_GUI_MSG_SkinUnknownParameter "ukjent parameter (%s)\n"
#define MSGTR_GUI_Next "Neste stream"
#define MSGTR_GUI__none_ "(ingen)"
#define MSGTR_GUI_Ok "Ok"
#define MSGTR_GUI_Open "Åpne..."
#define MSGTR_GUI_Pause "Pause"
#define MSGTR_GUI_Play "Spill"
#define MSGTR_GUI_Playback "Spiller"
#define MSGTR_GUI_Playlist "Spilleliste"
#define MSGTR_GUI_Preferences "Preferanser"
#define MSGTR_GUI_Previous "Forrige stream"
#define MSGTR_GUI_Quit "Avslutt"
#define MSGTR_GUI_Remove "Fjern"
#define MSGTR_GUI_SelectFile "Åpne fil..."
#define MSGTR_GUI_SelectSubtitle "Velg teksting..."
#define MSGTR_GUI_SizeDouble "Dobbel størrelse"
#define MSGTR_GUI_SizeFullscreen "Fullskjerm"
#define MSGTR_GUI_SizeNormal "Normal størrelse"
#define MSGTR_GUI_SkinBrowser "Skin velger"
#define MSGTR_GUI_Stop "Stopp"
#define MSGTR_GUI_Subtitle "Tekst"
#define MSGTR_GUI_Subtitles "Tekster"
#define MSGTR_GUI_TitleNN "Titel %2d"
#define MSGTR_GUI_Titles "Titler"
#define MSGTR_GUI_URL "Spill URL"
#define MSGTR_GUI_VCD "VCD"
#define MSGTR_GUI_Warning "advarsel..."
