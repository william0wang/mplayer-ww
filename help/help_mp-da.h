// Translated by:  Anders Rune Jensen <anders@gnulinux.dk>
// Særlig tak til: Tomas Groth <tomasgroth@hotmail.com>
//                 Dan Christiansen <danchr@daimi.au.dk>
// Sync'ed with help_mp-en.h 1.105


// ========================= MPlayer help ===========================

#ifdef CONFIG_VCD
#define MSGTR_HelpVCD " vcd://<spor>  afspil et VCD (Video CD) spor fra et drev i stedet for en fil\n"
#else
#define MSGTR_HelpVCD
#endif

#ifdef CONFIG_DVDREAD
#define MSGTR_HelpDVD " dvd://<titelnr> afspil DVD titel fra et drev i stedet for en fil\n"\
                      " -alang/-slang   vælg sprog til lyd og undertekster (vha. landekode på 2 tegn)\n"
#else
#define MSGTR_HelpDVD
#endif

#define MSGTR_Help \
"Benyt:   mplayer [indstillinger] [URL|sti/]filnavn\n"\
"\n"\
"Basale indstillinger (se manualen for en komplet liste):\n"\
" -vo <drv[:enhed]> vælg videodriver og enhed (detaljer, se '-vo help')\n"\
" -ao <drv[:enhed]> vælg lyddriver og enhed (detaljer, se '-ao help')\n"\
MSGTR_HelpVCD \
MSGTR_HelpDVD \
" -ss <tidspos>   søg til en given position (sekund eller hh:mm:ss)\n"\
" -nosound        slå lyd fra\n"\
" -fs             afspil i fuldskærm (el. -vm, -zoom, se manualen)\n"\
" -x <x> -y <y>   skærmopløsning til -vm eller -zoom)\n"\
" -sub <fil>      angiv fil med undertekster (se også -subfps, -subdelay)\n"\
" -playlist <fil> angiv afspilningsliste\n"\
" -vid x -aid y   vælg filmspor (x) og lydspor (y)\n"\
" -fps x -srate y sæt billedfrekvensen til x billeder pr. sekund og lydfrekvensen til y Hz\n"\
" -pp <kvalitet>  benyt efterbehandlingsfiltre (detaljer, se manualen)\n"\
" -framedrop      spring enkelte billeder over hvis nødvendigt (til langsomme maskiner)\n"\
"\n"\
"Basale taster: (se manualen for en fuldstændig liste, check også input.conf)\n"\
" <-  eller  ->   søg 10 sekunder frem eller tilbage\n"\
" up eller down   søg 1 minut frem eller tilbage \n"\
" pgup el. pgdown søg 10 minutter frem eller tilbage\n"\
" < eller >       søg frem eller tilbage i afspilningslisten\n"\
" p eller SPACE   pause filmen (starter igen ved tryk på en vilkårlig tast)\n"\
" q eller ESC     stop afspilning og afslut program\n"\
" + eller -       juster lydens forsinkelse med +/- 0.1 sekundt\n"\
" o               vælg OSD type:  ingen / søgebjælke / søgebjælke+tid\n"\
" * eller /       juster lydstyrken op og ned\n"\
" z eller x       tilpas underteksters forsinkelse med +/- 0.1 sekund\n"\
" r eller t       tilpas underteksters position op/ned, se også -vf expand\n"\
"\n"\
" * * * SE MANUALEN FOR DETALJER, FLERE (AVANCEREDE) MULIGHEDER OG TASTER * * *\n"\
"\n"

static const char help_text[] = MSGTR_Help;

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\n Afslutter...\n"
#define MSGTR_ExitingHow "\n Afslutter... (%s)\n"
#define MSGTR_Exit_quit "Afslut"
#define MSGTR_Exit_eof "Slut på filen"
#define MSGTR_Exit_error "Fatal fejl"
#define MSGTR_IntBySignal "\nMPlayer afbrudt af signal %d i modul: %s \n"
#define MSGTR_NoHomeDir "Kunne ikke finde hjemmekatalog\n"
#define MSGTR_GetpathProblem "get_path(\"config\") problem\n"
#define MSGTR_CreatingCfgFile "Genererer konfigurationsfil: %s\n"
#define MSGTR_CantLoadFont "Kunne ikke indlæse skrifttype: %s\n"
#define MSGTR_CantLoadSub "Kunne ikke indlæse undertekstfil: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATALT: Kunne ikke finde det valgte spor!\n"
#define MSGTR_CantOpenDumpfile "Kunne ikke åbne dump filen.\n"
#define MSGTR_CoreDumped "kernen dumpede ;)\n"
#define MSGTR_FPSnotspecified "Billedfrekvensen er enten ikke angivet i filen eller ugyldig. Brug -fps!\n"
#define MSGTR_TryForceAudioFmtStr "Gennemtvinger lyd-codec driverfamilie %s...\n"
#define MSGTR_CantFindAudioCodec "Kunne ikke finde codec til lydformatet 0x%X!\n"
#define MSGTR_TryForceVideoFmtStr "Gennemtvinger video-codec driver familie %s...\n"
#define MSGTR_CantFindVideoCodec "Kunne ikke finde video-codec til den valgte -vo og formatet 0x%X!\n"
#define MSGTR_VOincompCodec "Beklager, den valgte video-driverenhed er ikke kompatibel med dette codec.\n"
#define MSGTR_CannotInitVO "FATAL: Kunne ikke initialisere videodriveren\n"
#define MSGTR_CannotInitAO "Kunne ikke åbne/initialisere lydkortet -> INGEN LYD\n"
#define MSGTR_StartPlaying "Påbegynder afspilning...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"      ************************************************************\n"\
"      **** Din computer er for LANGSOM til at afspille dette! ****\n"\
"      ************************************************************\n\n"\
"Mulige årsager, problemer, løsninger:\n"\
"- Oftest: dårlig driver til lydkort\n"\
"  - Prøv -ao sdl eller brug ALSA 0.5 eller OSS emulation af ALSA 0.9.\n"\
"  - Experimenter med forskellige værdier for -autosync, 30 er en god start.\n"\
"- Langsom video output\n"\
"  - Prøv en anden -vo driver (se -vo help) eller prøv med -framedrop!\n"\
"- Langsom processor\n"\
"  - Undlad at afspille højopløsnings DVD eller DivX på en langsom processor! Prøv -hardframedrop.\n"\
"- Ødelagt fil\n"\
"  - Prøv med forskellige sammensætninger af -nobps -ni -forceidx -mc 0.\n"\
"- Langsomt medie (NFS/SMB, DVD, VCD osv.)\n"\
"  - Prøv -cache 8192.\n"\
"- Bruger du -cache til at afspille en non-interleaved AVI fil?\n"\
"  - Prøv -nocache.\n"\
"Se DOCS/HTML/en/video.html for tuning/optimerings tips.\n"\
"Hvis intet af dette hjalp, så læs DOCS/HTML/en/bugreports.html!\n\n"

#define MSGTR_NoGui "MPlayer blev kompileret UDEN grafisk brugergrænseflade!\n"
#define MSGTR_GuiNeedsX "MPlayer grafisk brugergrænseflade kræver X11!\n"
#define MSGTR_Playing "Afspiller %s\n"
#define MSGTR_NoSound "Lyd: ingen lyd\n"
#define MSGTR_FPSforced "Billedfrekvens sat til %5.3f  (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "Tilgængelige videodrivere:\n"
#define MSGTR_AvailableAudioOutputDrivers "Tilgængelige lyddrivere:\n"
#define MSGTR_AvailableAudioCodecs "Tilgængelige lyd-codecs:\n"
#define MSGTR_AvailableVideoCodecs "Tilgængelige video-codecs:\n"
#define MSGTR_AvailableAudioFm "\nTilgængelige (prækompilerede) lyd-codec familier/drivere:\n"
#define MSGTR_AvailableVideoFm "\nTilgængelige (prækompilerede) video-codec familier/drivere:\n"
#define MSGTR_AvailableFsType "Tilgængelige fuldskærmstilstande:\n"
#define MSGTR_CannotReadVideoProperties "Video: Kunne ikke læse egenskaber\n"
#define MSGTR_NoStreamFound "Ingen spor fundet\n"
#define MSGTR_ErrorInitializingVODevice "Fejl under åbning/initialisering af den valgte videodriver (-vo)!\n"
#define MSGTR_ForcedVideoCodec "Gennemtvunget video-codec: %s\n"
#define MSGTR_ForcedAudioCodec "Gennemtvunget lyd-codec: %s\n"
#define MSGTR_Video_NoVideo "Video: ingen video\n"
#define MSGTR_NotInitializeVOPorVO "\nFATALT: Kunne ikke initialisere videofiltre (-vf) eller videodriver (-vo)!\n"
#define MSGTR_Paused "  =====  PAUSE  ====="
#define MSGTR_PlaylistLoadUnable "\nKunne ikke indlæse afspilningslisten %s\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer fik en alvorlig fejl af typen 'ulovlig instruktion'.\n"\
"  Det kan være en fejl i den nye dynamiske processoroptimeringskode...\n"\
"  Se DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer fik en alvorlig fejl af typen 'ulovlig instruktion'.\n"\
"  Dette sker oftest kun hvis du kører på en processor forskellig fra den\n"\
"  MPlayer var kompileret til.\n Check venligst dette!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer fik en alvorlig fejl pga. forkert brug af CPU/FPU/RAM.\n"\
"  Rekompiler MPlayer med --enable-debug og lav et 'gdb' backtrace og\n"\
"  disassemling. For detaljer læs venligst DOCS/HTML/en/bugreports_what.html#bugreports_crash.b.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer fik en alvorlig fejl af ukendt type. Dette burde ikke ske.\n"\
"  Det kan være en fejl i MPlayer koden _eller_ i andre drivere _ eller_ i \n"\
"  den version af gcc du kører. Hvis du tror det er en fejl i MPlayer læs da \n"\
"  venligst DOCS/HTML/en/bugreports.html og følg instruktionerne der. Vi kan ikke \n"\
"  og vil ikke hjælpe medmindre du følger instruktionerne når du rapporterer \n"\
"  en mulig fejl.\n"


// mencoder.c:

#define MSGTR_UsingPass3ControlFile "Benytter 3. pass kontrolfilen: %s\n"
#define MSGTR_MissingFilename "\nFilnavn mangler\n\n"
#define MSGTR_CannotOpenFile_Device "Kunne ikke åbne fil/enhed\n"
#define MSGTR_CannotOpenDemuxer "Kunne ikke åbne demuxer\n"
#define MSGTR_NoAudioEncoderSelected "\nIngen lydenkoder (-oac) valgt! Vælg en eller brug -nosound. Se også -oac help!\n"
#define MSGTR_NoVideoEncoderSelected "\nIngen videoenkoder (-ovc) valgt! Vælg en, se -ovc help!\n"
#define MSGTR_CannotOpenOutputFile "Kunne ikke åbne '%s' til skrivning\n"
#define MSGTR_EncoderOpenFailed "Kunne ikke åbne enkoderen\n"
#define MSGTR_ForcingOutputFourcc "Tvinger udgangskode (fourcc) til %x [%.4s]\n"
#define MSGTR_DuplicateFrames "\n%d ens billede(r)!!!    \n"
#define MSGTR_SkipFrame "\nSpringer over billede!!!    \n"
#define MSGTR_ErrorWritingFile "%s: Fejl ved skrivning til fil.\n"
#define MSGTR_RecommendedVideoBitrate "Anbefalet video bitrate til %s CD: %d\n"
#define MSGTR_VideoStreamResult "\nVideospor: %8.3f kbit/s  (%d B/s)  størrelse: %"PRIu64" bytes  %5.3f sek.  %d billeder\n"
#define MSGTR_AudioStreamResult "\nAudiospor: %8.3f kbit/s  (%d B/s)  størrelse: %"PRIu64" bytes  %5.3f sek.\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     bitratemetode (vbr)\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(default)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           gennemsnitlig bitrate\n"\
"\n"\
" cbr           konstant bitrate\n"\
"               Gennemtvinger også CBR på efterfølgende ABR indstilligner\n"\
"\n"\
" br=<0-1024>   specificer bitrate i kBit (kun CBR og ABR)\n"\
"\n"\
" q=<0-9>       kvalitet (0-højest, 9-lavest) (kun VBR)\n"\
"\n"\
" aq=<0-9>      algoritmisk kvalitet (0-bedst/langsomst, 9-værst/hurtigst)\n"\
"\n"\
" ratio=<1-100> komprimeringsforhold\n"\
"\n"\
" vol=<0-10>    indstil lydstyrke\n"\
"\n"\
" mode=<0-3>    (standard: auto)\n"\
"                0: stereo\n"\
"                1: joint-stereo\n"\
"                2: dobbelt mono\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: ingen\n"\
"                1: alle\n"\
"                2: justeret\n"\
"\n"\
" fast          vælg hastighed fremfor kvalitet i efterfølgende VBR indstillinger,\n"\
"               giver lavere kvalitet og højere bitrate.\n"\
"\n"\
" preset=<value> tunede indstillinger til høj kvalitet.\n"\
"                 medium: VBR,  god  kvalitet\n"\
"                 (150-180 kbps bitrate interval)\n"\
"                 standard: VBR, høj kvalitet\n"\
"                 (170-210 kbps bitrate interval)\n"\
"                 extreme: VBR, meget høj kvalitet\n"\
"                 (200-240 kbps bitrate interval)\n"\
"                 insane:  CBR, højeste præsets kvalitet\n"\
"                 (320 kbps bitrate)\n"\
"                 <8-320>: ABR med den angivne, gennemsnitlige bitrate.\n\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM drev '%s' ikke fundet!\n"
#define MSGTR_ErrTrackSelect "Fejl i valg af VCD nummer!"
#define MSGTR_ReadSTDIN "Læser fra stdin...\n"
#define MSGTR_UnableOpenURL "Kunne ikke åbne adressen: %s\n"
#define MSGTR_ConnToServer "Forbundet til serveren: %s\n"
#define MSGTR_FileNotFound "Filen blev ikke fundet: '%s'\n"

#define MSGTR_SMBInitError "Kunne ikke initialisere libsmbclient bibliotek: %d\n"
#define MSGTR_SMBFileNotFound "Kunne ikke åbne netværksadressen '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer er ikke blevet kompileret med SMB læse-understøttelse\n"

#define MSGTR_CantOpenDVD "Kunne ikke åbne DVD drev: %s (%s)\n"
#define MSGTR_DVDnumTitles "Der er %d titler på denne DVD.\n"
#define MSGTR_DVDinvalidTitle "Ugyldig DVD-titel: %d\n"
#define MSGTR_DVDnumChapters "Der er %d kapitler i denne DVD-titel.\n"
#define MSGTR_DVDinvalidChapter "Ugyldigt DVD-kapitel: %d\n"
#define MSGTR_DVDnumAngles "Der er %d vinkler i denne DVD-titel.\n"
#define MSGTR_DVDinvalidAngle "Ugyldig DVD-vinkel: %d\n"
#define MSGTR_DVDnoIFO "Kunne ikke finde IFO filen for DVD-titel %d.\n"
#define MSGTR_DVDnoVOBs "Kunne ikke åbne titlen VOBS (VTS_%02d_1.VOB).\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "Advarsel! Lydfilens header %d er blevet omdefineret!\n"
#define MSGTR_VideoStreamRedefined "Advarsel! Videofilens header %d er blevet omdefineret!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: For mange (%d i %d bytes) lydpakker i bufferen!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: For mange (%d i %d bytes) videopakker i bufferen!\n"
#define MSGTR_MaybeNI "Måske afspiller du en 'ikke-interleaved' stream/fil ellers der kan være en fejl i afspilleren\n"\
                      "For AVI filer, prøv at påtvinge non-interleaved tilstand med -ni.\n"
#define MSGTR_SwitchToNi "\nDefekt .AVI - skifter til ikke-interleaved (-ni)...\n"
#define MSGTR_Detected_XXX_FileFormat "Filformat er %s\n"
#define MSGTR_DetectedAudiofile "Filen er en lydfil!\n"
#define MSGTR_InvalidMPEGES "Ugyldig MPEG-ES stream??? Rapporter venligst dette, det kunne være en fejl i programmet :(\n"
#define MSGTR_FormatNotRecognized "============ Desværre, dette filformat kunne ikke genkendes =================\n"\
"=== Er denne fil af typen AVI, ASF eller MPEG, så rapporter venligst dette, det kan skyldes en fejl. ==\n"
#define MSGTR_MissingVideoStream "Ingen videospor fundet.\n"
#define MSGTR_MissingAudioStream "Ingen lydspor fundet -> ingen lyd\n"
#define MSGTR_MissingVideoStreamBug "Manglende videospor!? Rapporter venligst dette, det kunne skyldes en fejl i programmet :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: filen indeholder ikke det angivne lyd- eller videospor\n"

#define MSGTR_NI_Forced "Gennemtvang"
#define MSGTR_NI_Detected "Opdagede"
#define MSGTR_NI_Message "%s IKKE-INTERLEAVED AVI fil-format!\n"

#define MSGTR_UsingNINI "Bruger IKKE-INTERLEAVED (-ni), <F8>delagt AVI fil-format!\n"
#define MSGTR_CouldntDetFNo "Kunne ikke beregne antallet af billeder (til søgning)  \n"
#define MSGTR_CantSeekRawAVI "Søgning i rå AVI-filer ikke mulig. (Index kræves, prøv -idx.)  \n"
#define MSGTR_CantSeekFile "Kan ikke søge i denne fil.\n"

#define MSGTR_MOVcomprhdr "MOV: Komprimerede headers (endnu) ikke understøttet!\n"
#define MSGTR_MOVvariableFourCC "MOV: Advarsel! variabel FOURCC!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Advarsel! For mange spor"
#define MSGTR_DetectedTV "TV genkendt! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Kan ikke åbne ogg demuxe.r\n"
#define MSGTR_CannotOpenAudioStream "Kan ikke åbne lydsspor: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Kan ikke åbne spor %s af underteksterne\n"
#define MSGTR_OpeningAudioDemuxerFailed "Kan ikke åbne lyddemuxer: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Kunne ikke åbne undertekstsdemuxer: %s\n"
#define MSGTR_TVInputNotSeekable "TV input er ikke søgbart! (Kunne være du skulle skifte kanal ;)\n"
#define MSGTR_ClipInfo "Klip info: \n"


// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "kunne ikke åbne codec\n"
#define MSGTR_CantCloseCodec "kunne ikke afslutte codec\n"

#define MSGTR_MissingDLLcodec "FEJL: Kunne ikke åbne DirectShow codec: %s\n"
#define MSGTR_ACMiniterror "Kunne ikke loade/initialisere Win32/ACM lyd-codec (manglende DLL fil?)\n"
#define MSGTR_MissingLAVCcodec "Kunne ikke finde codec '%s' i libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF under søgning efter sekvensheader\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: Kunne ikke læse sekvensheader!\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: Kunne ikke læse sekvensheaderudvidelse!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Ugyldig sekvensheader!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Ugyldig sekvensheaderudvidelse!\n"

#define MSGTR_ShMemAllocFail "Kunne ikke allokere delt ram\n"
#define MSGTR_CantAllocAudioBuf "Kunne ikke allokere lyd buffer\n"

#define MSGTR_UnknownAudio "Ukendt/manglende lyd format, slår over til ingen lyd\n"

#define MSGTR_UsingExternalPP "[PP] Benytter ekstern efterprocesseringsfilter, max q = %d\n"
#define MSGTR_UsingCodecPP "[PP] Benytter codec's efterprocessering, max q = %d\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Anmodede video-codec familie [%s] (vfm=%s) ikke tilgængelig (aktiver før kompilering!)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Anmodede lyd-codec familie [%s] (afm=%s) ikke tilgængelig (aktiver før kompilering!)\n"
#define MSGTR_OpeningVideoDecoder "Åbner videodekoder: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "Åbner audiodekoder: [%s] %s\n"
#define MSGTR_VDecoderInitFailed "Videodekoder initialisering fejlede :(\n"
#define MSGTR_ADecoderInitFailed "Lyddekoder initialisering fejlede :(\n"
#define MSGTR_ADecoderPreinitFailed "Lyddekoder præinitialisering fejlede :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Ingen lirc understøttelse fundet!\n"
#define MSGTR_LIRCcfgerr "Kunne ikke læse LIRC konfigurationsfil %s!\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Kunne ikke finde videofilter '%s'\n"
#define MSGTR_CouldNotOpenVideoFilter "Kunne ikke åbne videofilter '%s'\n"
#define MSGTR_OpeningVideoFilter "Åbner videofilter: "
#define MSGTR_CannotFindColorspace "Kunne ikke finde fælles colorspace, selv med 'scale' :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDek: codec satte ikke sh->disp_w og sh->disp_h, prøver en anden løsning!\n"
#define MSGTR_CouldNotFindColorspace "Kunne ikke finde colorspace som matcher - prøver med -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Størrelsesforhold er %.2f:1 - præskalerer for at rette størrelsesforholdet.\n"
#define MSGTR_MovieAspectUndefined "Størrelsesforholdet er ikke defineret - ingen præskalering benyttet.\n"

// ================================ GUI ================================

#define MSGTR_GUI_AboutMPlayer "Om MPlayer"
#define MSGTR_GUI_Add "Tilføj"
#define MSGTR_GUI_AspectRatio "Størrelsesforhold"
#define MSGTR_GUI_Audio "Lyd"
#define MSGTR_GUI_AudioDelay "Lydforsinkelse"
#define MSGTR_GUI_AudioTrack "Indlæs extern lyd fil"
#define MSGTR_GUI_AudioTracks "Lydspor"
#define MSGTR_GUI_AvailableDrivers "Tilgængelige drivere:"
#define MSGTR_GUI_AvailableSkins "Temaer"
#define MSGTR_GUI_Bass "Bass"
#define MSGTR_GUI_Blur "Uskarp"
#define MSGTR_GUI_Brightness "Lysstyrke"
#define MSGTR_GUI_Browse "Gennemse"
#define MSGTR_GUI_Cache "Cache"
#define MSGTR_GUI_CacheSize "Cache størrelse"
#define MSGTR_GUI_Cancel "Annuller"
#define MSGTR_GUI_Center "Center"
#define MSGTR_GUI_Channel1 "Kanal 1"
#define MSGTR_GUI_Channel2 "Kanal 2"
#define MSGTR_GUI_Channel3 "Kanal 3"
#define MSGTR_GUI_Channel4 "Kanal 4"
#define MSGTR_GUI_Channel5 "Kanal 5"
#define MSGTR_GUI_Channel6 "Kanal 6"
#define MSGTR_GUI_ChannelAll "Alle"
#define MSGTR_GUI_ChapterN "kapitel %d"
#define MSGTR_GUI_ChapterNN "Kapitel %2d"
#define MSGTR_GUI_Chapters "Kapitler"
#define MSGTR_GUI_Clear "Nulstil"
#define MSGTR_GUI_CodecFamilyAudio "Lyd-codec familie"
#define MSGTR_GUI_CodecFamilyVideo "Video-codec familie"
#define MSGTR_GUI_Coefficient "Koefficient"
#define MSGTR_GUI_Configure "Konfigurer"
#define MSGTR_GUI_ConfigureDriver "Konfigurer driver"
#define MSGTR_GUI_Contrast "Kontrast"
#define MSGTR_GUI_Cp874 "Thai tegnsæt (CP874)"
#define MSGTR_GUI_Cp936 "Simplificeret kinesisk tegnsæt (CP936)"
#define MSGTR_GUI_Cp949 "Koreansk tegnsæt (CP949)"
#define MSGTR_GUI_Cp1250 "Slavisk/Central Europæisk Windows (CP1250)"
#define MSGTR_GUI_Cp1251 "Cyrillic Windows (CP1251)"
#define MSGTR_GUI_CpBIG5 "Traditionel kinesisk tegnsæt (BIG5)"
#define MSGTR_GUI_CpISO8859_1 "Vesteuropæriske sprog (ISO-8859-1)"
#define MSGTR_GUI_CpISO8859_2 "Øst-/Centraleuropæriske sprog (ISO-8859-2)"
#define MSGTR_GUI_CpISO8859_3 "Esperanto, Galician, Maltisk, Tyrkisk (ISO-8859-3)"
#define MSGTR_GUI_CpISO8859_4 "Gammel Baltisk tegnsæt (ISO-8859-4)"
#define MSGTR_GUI_CpISO8859_5 "Cyrillisk (ISO-8859-5)"
#define MSGTR_GUI_CpISO8859_6 "Arabisk (ISO-8859-6)"
#define MSGTR_GUI_CpISO8859_7 "Moderne Græsk (ISO-8859-7)"
#define MSGTR_GUI_CpISO8859_8 "Hebræisk tegnsæt (ISO-8859-8)"
#define MSGTR_GUI_CpISO8859_9 "Tyrkisk (ISO-8859-9)"
#define MSGTR_GUI_CpISO8859_13 "Baltisk (ISO-8859-13)"
#define MSGTR_GUI_CpISO8859_14 "Keltisk (ISO-8859-14)"
#define MSGTR_GUI_CpISO8859_15 "Vesteuropæriske sprog med euro (ISO-8859-15)"
#define MSGTR_GUI_CpKOI8_R "Russisk (KOI8-R)"
#define MSGTR_GUI_CpKOI8_U "Ukrainsk, Belarusian (KOI8-U/RU)"
#define MSGTR_GUI_CpShiftJis "Japansk tegnsæt (SHIFT-JIS)"
#define MSGTR_GUI_CpUnicode "Unicode"
#define MSGTR_GUI_Delay "Forsinkelse"
#define MSGTR_GUI_Demuxers_Codecs "Codecs & demuxer"
#define MSGTR_GUI_DeviceCDROM "CD-ROM drev"
#define MSGTR_GUI_DeviceDVD "DVD drev"
#define MSGTR_GUI_Directory "Sti"
#define MSGTR_GUI_DirectoryTree "Katalog træ"
#define MSGTR_GUI_DropSubtitle "Drop undertekst..."
#define MSGTR_GUI_DVD "DVD"
#define MSGTR_GUI_EnableAutomaticAVSync "AutoSynk. til/fra"
#define MSGTR_GUI_EnableCache "Cache til/fra"
#define MSGTR_GUI_EnableDirectRendering "Anvend 'direct rendering'"
#define MSGTR_GUI_EnableDoubleBuffering "Anvend double buffering"
#define MSGTR_GUI_EnableEqualizer "Anvend equalizer"
#define MSGTR_GUI_EnableExtraStereo "Anvend extra stereo"
#define MSGTR_GUI_EnableFrameDropping "Anvend billed-skip"
#define MSGTR_GUI_EnableFrameDroppingIntense "Anvend meget billed-skip (farlig)"
#define MSGTR_GUI_EnablePlaybar "Anvend afspilningsbar"
#define MSGTR_GUI_EnablePostProcessing "Anvend efterprocesseringsfilter"
#define MSGTR_GUI_Encoding "Enkodning"
#define MSGTR_GUI_Equalizer "Equalizer"
#define MSGTR_GUI_Error "Fejl!"
#define MSGTR_GUI_ErrorFatal "Fatal fejl!"
#define MSGTR_GUI_File "Afspil fil"
#define MSGTR_GUI_Files "Filer"
#define MSGTR_GUI_Flip "Flip billede"
#define MSGTR_GUI_Font "Skriftype"
#define MSGTR_GUI_FrameRate "Billedfrekvens"
#define MSGTR_GUI_FrontLeft "Venstre Front"
#define MSGTR_GUI_FrontRight "Højre Front"
#define MSGTR_GUI_HideVideoWindow "Vis video vindue når inaktiv"
#define MSGTR_GUI_Hue "Farve"
#define MSGTR_GUI_Lavc "Brug LAVC (FFmpeg)"
#define MSGTR_GUI_MaximumUsageSpareCPU "Auto kvalitet"
#define MSGTR_GUI_Miscellaneous "Forskelligt"
#define MSGTR_GUI_MSG_DXR3NeedsLavc "For at afspille ikke-mpeg filer med dit DXR3/H+ skal du kode filmen igen.\nVenligst aktiver lavc i DXR3/H+ configboxen."
#define MSGTR_GUI_MSG_MemoryErrorWindow "Desværre, ikke nok ram til at vise bufferen."
#define MSGTR_GUI_MSG_NoFileLoaded "Ingen fil indlæst"
#define MSGTR_GUI_MSG_NoMediaOpened "Medie ikke åbnet"
#define MSGTR_GUI_MSG_PlaybackNeedsRestart "Husk, nogle funktioner kræver at MPlayer bliver genstartet for at de virker."
#define MSGTR_GUI_MSG_SkinBitmapConversionError "Fejl i 24 bit to 32 bit convertering (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapNotFound "filen ikke fundet (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapPngReadError "PNG læse fejl (%s)\n"
#define MSGTR_GUI_MSG_SkinCfgNotFound "Tema blev ikke fundet (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorBitmap16Bit "16 bits eller mindre ikke understøttet (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorMessage "[tema] fejl i konfigurationsfilen til temaet på linje %d: %s"
#define MSGTR_GUI_MSG_SkinFontFileNotFound "skriftypefilen ikke fundet\n"
#define MSGTR_GUI_MSG_SkinFontImageNotFound "skrifttypebilled ikke fundet\n"
#define MSGTR_GUI_MSG_SkinFontNotFound "ikke eksisterende font (%s)\n"
#define MSGTR_GUI_MSG_SkinMemoryError "ikke nok ram\n"
#define MSGTR_GUI_MSG_SkinTooManyFonts "for mange skrifttyper specificeret\n"
#define MSGTR_GUI_MSG_SkinUnknownMessage "ukendt besked: %s\n"
#define MSGTR_GUI_MSG_SkinUnknownParameter "ukendt parameter (%s)\n"
#define MSGTR_GUI_MSG_VideoOutError "Desværre, kunne ikke finde GUI kompabitel video driver."
#define MSGTR_GUI_Mute "Mute"
#define MSGTR_GUI_NetworkStreaming "Netværksstreaming..."
#define MSGTR_GUI_Next "Næste stream"
#define MSGTR_GUI_NoChapter "Ingen kapitel"
#define MSGTR_GUI__none_ "(ingen)"
#define MSGTR_GUI_NonInterleavedParser "Benyt non-interleaved AVI parser"
#define MSGTR_GUI_NormalizeSound "Normaliser lydstyrke"
#define MSGTR_GUI_Ok "Ok"
#define MSGTR_GUI_Open "Åben..."
#define MSGTR_GUI_Original "Original"
#define MSGTR_GUI_OsdLevel "OSD level"
#define MSGTR_GUI_OSD_Subtitles "undertekster og OSD"
#define MSGTR_GUI_Outline "Omrids"
#define MSGTR_GUI_PanAndScan "Panscan"
#define MSGTR_GUI_Pause "Pause"
#define MSGTR_GUI_Play "Afspil"
#define MSGTR_GUI_Playback "Afspilning"
#define MSGTR_GUI_Playlist "Afspilningsliste"
#define MSGTR_GUI_Position "Position"
#define MSGTR_GUI_PostProcessing "Efterprocesseringsfilter"
#define MSGTR_GUI_Preferences "Indstillinger"
#define MSGTR_GUI_Previous "Forrige stream"
#define MSGTR_GUI_Quit "Forlad"
#define MSGTR_GUI_RearLeft "Venstre Baghøjtaler"
#define MSGTR_GUI_RearRight "Højre Baghøjtaler"
#define MSGTR_GUI_Remove "Fjern"
#define MSGTR_GUI_Saturation "Mætning"
#define MSGTR_GUI_SaveWindowPositions "Gem vinduets position"
#define MSGTR_GUI_ScaleMovieDiagonal "Proportional med film diagonalt"
#define MSGTR_GUI_ScaleMovieHeight "Proportional med film højde"
#define MSGTR_GUI_ScaleMovieWidth "Proportional med film bredde"
#define MSGTR_GUI_ScaleNo "Ingen autoskalering"
#define MSGTR_GUI_SeekingInBrokenMedia "Genopbyg index tabel, hvis nødvendig"
#define MSGTR_GUI_SelectAudioFile "Vælg ekstern lydkanal..."
#define MSGTR_GUI_SelectedFiles "Valgte filer"
#define MSGTR_GUI_SelectFile "Vælg fil..."
#define MSGTR_GUI_SelectFont "Vælg font..."
#define MSGTR_GUI_SelectSubtitle "Vælg undertekst-fil..."
#define MSGTR_GUI_SizeDouble "Dobbelt størrelse"
#define MSGTR_GUI_SizeFullscreen "Fuld skærm"
#define MSGTR_GUI_SizeNormal "Normal størrelse"
#define MSGTR_GUI_SizeOSD "OSD skalering"
#define MSGTR_GUI_SizeSubtitles "Text skalering"
#define MSGTR_GUI_SkinBrowser "Vælg udseende"
#define MSGTR_GUI_Sponsored "GUI udvikling sponsereret af UHU Linux"
#define MSGTR_GUI_StartFullscreen "Start i fullskærm"
#define MSGTR_GUI_Stop "Stop"
#define MSGTR_GUI_Subtitle "Undertekst"
#define MSGTR_GUI_SubtitleAllowOverlap "slå (til/fra) undertekst overlapning"
#define MSGTR_GUI_SubtitleAutomaticLoad "Deaktiver automatisk undertekster"
#define MSGTR_GUI_SubtitleConvertMpsub "Konverter en given undertekst til MPlayer's undertekst format"
#define MSGTR_GUI_SubtitleConvertSrt "Konverter den angivne undertekst til et tidsbaseret SubViewer (SRT) format"
#define MSGTR_GUI_Subtitles "Undertekster"
#define MSGTR_GUI_SyncValue "Autosynk."
#define MSGTR_GUI_TitleNN "Titel %2d"
#define MSGTR_GUI_Titles "Titler"
#define MSGTR_GUI_TrackN "Spor %d"
#define MSGTR_GUI_TurnOffXScreenSaver "Stop XScreenSaver"
#define MSGTR_GUI_URL "Afspil URL"
#define MSGTR_GUI_VCD "VCD"
#define MSGTR_GUI_Video "Video"
#define MSGTR_GUI_VideoEncoder "Video enkoder"
#define MSGTR_GUI_VideoTracks "Videospor"
#define MSGTR_GUI_Warning "Advarsel!"
