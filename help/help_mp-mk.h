// translated to Macedonian by: MIsTeRIoZ "Zoran Dimovski"<zoki@email.com>
// Last sync with help_mp-en.h 1.113


// ========================= MPlayer Помош ===========================

static const char help_text[]=
"Употреба: mplayer [опции] [url|патека/]ИмеНаДатотеката\n"
"\n"
"Основни Опции: (комплетна листа на man страницата)\n"
" -vo <drv[:dev]>  избира излезен видео драјвер и уред ('-vo help' за листа)\n"
" -ao <drv[:dev]>  избира излезен аудио драјвер и уред ('-ao help' за листа)\n"
#ifdef CONFIG_VCD
" vcd://<trackno>   пушта VCD (Video CD) од уред наместо од датотека\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>   пушта DVD наслови од уред наместо од датотека\n"
" -alang/-slang    избира јазик на DVD аудио/превод (од 2-char код на државата)\n"
#endif
" -ss <timepos>    бара до дадената (секунди или hh:mm:ss) позиција\n"
" -nosound         не го пушта звукот\n"
" -fs              плејбек на цел екран (или -vm, -zoom, подетално во man страната)\n"
" -x <x> -y <y>    подесување на резолуцијата на прикажувањето (се употребува со -vm или -zoom)\n"
" -sub <file>      одредува датотека со превод за употреба (видете исто -subfps, -subdelay)\n"
" -playlist <file> одредува датотека со плејлиста\n"
" -vid x -aid y    избира видео (x) и аудио (y) проток за пуштање\n"
" -fps x -srate y  се менува видео (x fps) и аудио (y Hz) ратата\n"
" -pp <quality>    овозможува филтер за постпроцесирање (подетално во man страната)\n"
" -framedrop       овозможува отфрлање на фрејмови (за слаби машини)\n"
"\n"
"Основни копчиња: (комплетна листа во man страната, проверете го исто така и input.conf)\n"
" <-  или  ->       бара назад/напред за 10 секунди\n"
" up или down       бара назад/напред за 1 минута\n"
" pgup или pgdown   бара назад/напред за 10 минути\n"
" < или >           чекор назад/напред во плејлистата\n"
" p или SPACE       го паузира филмот (притиснете на било кое копче да продолжи)\n"
" q или ESC         го стопира пуштањето и излегува од програмата\n"
" + или -           приспосубување на аудио задоцнувањето со +/- 0.1 секунда\n"
" o                цикличен OSD мод: ниеден / барот за барање / барот за барање + тајмер\n"
" * или /           зголемување или намалување на PCM тонот\n"
" z или x           прилагодување на задоцнувањето на преводот со +/- 0.1 секунда\n"
" r или t           прилагодување на позицијата за преводот нагоре/надолу, исто така видете и -vf expand\n"
"\n"
" * * * ВИДЕТЕ ЈА MAN СТРАНАТА ЗА ДЕТАЛИ, ПОВЕЌЕ (НАПРЕДНИ) ОПЦИИ И КОПЧИЊА* * *\n"
"\n";

// ========================= MPlayer ПОРАКИ ===========================

// mplayer.c:

#define MSGTR_Exiting "\nИзлегу...\n"
#define MSGTR_ExitingHow "\nИзлегува... (%s)\n"
#define MSGTR_Exit_quit "Откажи"
#define MSGTR_Exit_eof "Крај на датотеката"
#define MSGTR_Exit_error "Фатална грешка"
#define MSGTR_IntBySignal "\nMPlayer е прекинат од сигналот %d во модулот: %s\n"
#define MSGTR_NoHomeDir "Не може да го пронајде HOME директориумот.\n"
#define MSGTR_GetpathProblem "get_path(\"конфигурирај\") проблем"
#define MSGTR_CreatingCfgFile "Создавање на конфигурациона датотека: %s\n"
#define MSGTR_CantLoadFont "Не може да се вчита фонтот: %s\n"
#define MSGTR_CantLoadSub "Не може да се вчитаат преводите: %s\n"
#define MSGTR_DumpSelectedStreamMissing "отпадок: ФАТАЛНО: недостига избраниот проток!\n"
#define MSGTR_CantOpenDumpfile "Не може да се отвори датотеката за отпадоци.\n"
#define MSGTR_CoreDumped "Јадрото е отфрлено ;)\n"
#define MSGTR_FPSnotspecified "FPS(Frames Per Second) не се специфицирани во хедерот или се невалидни, користете ја -fps опцијата.\n"
#define MSGTR_TryForceAudioFmtStr "Проба да го присили драјверот на аудио кодекот %s ...\n"
#define MSGTR_CantFindAudioCodec "Не може да го пронајде кодекот за аудио форматот 0x%X.\n"
#define MSGTR_TryForceVideoFmtStr "Пробува да го присили драјверот на видео кодекот %s ...\n"
#define MSGTR_CantFindVideoCodec "Не може да го пронајде кодекот кој одговара избран со -vo и видео форматот 0x%X.\n"
#define MSGTR_VOincompCodec "Избраниот излезен видео уред не е соодветен со овој кодек.\n"
#define MSGTR_CannotInitVO "ФАТАЛНО: Не може да го иницијализира видео драјверот.\n"
#define MSGTR_CannotInitAO "Не може да го отвори/иницијализира аудио уредот -> нема звук.\n"
#define MSGTR_StartPlaying "Почнува плејбекот...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"           *********************************************************\n"\
"           ******** Вашиот систем е премногу СЛАБ да го пушта ова! *********\n"\
"           *********************************************************\n\n"\
"Можни причини, проблеми, работа:\n"\
"- Нај заедничко: скршен/лажен _аудио_ драјвер\n"\
"  - Обиди се со -ao sdl или користи ALSA 0.5 или OSS емулаторот на ALSA 0.9.\n"\
"  - Експериментирајте со различни вредности за -autosync, 30 е добар почеток.\n"\
"- СЛАБ видео излез\n"\
"  - Пробајте со друг -vo драјвер (-vo help за листа) или обидете се со -framedrop!\n"\
"- СЛАБ Процесор\n"\
"  - Не се обидувајте да пуштате голем DVD/DivX филм на СЛАБ процесор! Обидете се со -hardframedrop.\n"\
"- Расипана датотека\n"\
"  - Пробајте различни комбинации на -nobps -ni -forceidx -mc 0.\n"\
"- СЛАБ медиум (NFS/SMB монтирања, DVD, VCD итн.)\n"\
"  - Пробајте со -cache 8192.\n"\
"- Дали користите -cache за пуштање на непреклопувачка AVI датотека?\n"\
"  - Пробајте со -nocache.\n"\
"Прочитајте го DOCS/HTML/en/video.html за пораки за подобрување/забрзување.\n"\
"Ако ништо од ова не ви помага, прочитајте го DOCS/HTML/en/bugreports.html.\n\n"

#define MSGTR_NoGui "MPlayer е компајлиран БЕЗ GUI подршка.\n"
#define MSGTR_GuiNeedsX "На MPlayer GUI е потребно X11.\n"
#define MSGTR_Playing "Пуштено %s\n"
#define MSGTR_NoSound "Аудио: нема звук\n"
#define MSGTR_FPSforced "FPS присилени да бидат %5.3f  (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "Достапни излезни видео драјвери:\n"
#define MSGTR_AvailableAudioOutputDrivers "Достапни излезни аудио драјвери:\n"
#define MSGTR_AvailableAudioCodecs "Достапни аудио кодеци:\n"
#define MSGTR_AvailableVideoCodecs "Достапни видео кодеци:\n"
#define MSGTR_AvailableAudioFm "\nДостапни (внатрешно-компајлирани) фамилија/дајвери на аудио кодекот:\n"
#define MSGTR_AvailableVideoFm "\nДостапни (внатрешно-компајлирани) фамилија/дајвери на видео кодекот:\n"
#define MSGTR_AvailableFsType "Достапни модови за менување на слојот за цел екран:\n"
#define MSGTR_CannotReadVideoProperties "Видео: Не може да ги прочита својствата.\n"
#define MSGTR_NoStreamFound "Не е пронајден проток.\n"
#define MSGTR_ErrorInitializingVODevice "Грешка при отварањето/иницијализирањето на избраниот излезен видео (-vo) уред.\n"
#define MSGTR_ForcedVideoCodec "Присилен видео кодек: %s\n"
#define MSGTR_ForcedAudioCodec "Присилен аудио кодек: %s\n"
#define MSGTR_Video_NoVideo "Видео: нема слика\n"
#define MSGTR_NotInitializeVOPorVO "\nФАТАЛНО: Не може да ги иницијализира видео филтерите (-vf) или видео излезот (-vo).\n"
#define MSGTR_Paused "  =====  ПАУЗА =====" // не повеќе од 23 карактери (статус линија за аудио датотеките)
#define MSGTR_PlaylistLoadUnable "\nНе може да ја вчита плејлистата %s.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer падна од 'Нелегална Инструкција'.\n"\
"  Можеби е баг во нашиот нов код за детекција на процесорот при извршувањето..\n"\
"  Ве молиме прочитајте го DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer падна од 'Нелегална Инструкција'.\n"\
"  Ова обично се случува кога работи на процесор кој е различен од оној за кој е\n"\
"  компајлиран/оптимизиран.\n"\
"  Потврдете го ова!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer падна поради лоша употреба на CPU/FPU/RAM.\n"\
"  Рекомпалирајте го MPlayer со --enable-debug и направете 'gdb' следењеВОпозадина и\n"\
"  дисасемблирање. Подетално во DOCS/HTML/en/bugreports_what.html#bugreports_crash.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer падна. Ова не треба да се случува..\n"\
"  Може да е баг во кодот на MPlayer или_ во вашите драјвери _или_ во вашата\n"\
"  gcc верзија. Ако мислете дека тоа е грешка од MPlayer, ве молиме прочитајте\n"\
"  DOCS/HTML/en/bugreports.html и следете ги инструкциите. Не можеме и нема\n"\
"  да ви помогнеме доколку не ни ја обезбедите оваа информација кога пријавувате можен баг.\n"


// mencoder.c:

#define MSGTR_UsingPass3ControlFile "Користи pass3 контролна датотека: %s\n"
#define MSGTR_MissingFilename "\nНедостасува името на датотеката.\n\n"
#define MSGTR_CannotOpenFile_Device "Не може да ја/го отвори датотеката/уредот.\n"
#define MSGTR_CannotOpenDemuxer "Не може да го отвори демуксерот.\n"
#define MSGTR_NoAudioEncoderSelected "\nНема избрано аудио енкодер (-oac). Изберете еден (видете -oac help) или користете -nosound.\n"
#define MSGTR_NoVideoEncoderSelected "\nНема избрано видео енкодер (-ovc). Изберете еден (видете -ovc help).\n"
#define MSGTR_CannotOpenOutputFile "Не може да ја отвори излезната датотека '%s'.\n"
#define MSGTR_EncoderOpenFailed "Не успеа да се отвори енкодерот.\n"
#define MSGTR_ForcingOutputFourcc "Присилување на излезното fourcc на %x [%.4s]\n"
#define MSGTR_DuplicateFrames "\n%d дуплирање на фрејмо(ви)т!\n"
#define MSGTR_SkipFrame "\nПрескокнување на фрејмот!\n"
#define MSGTR_ErrorWritingFile "%s: Грешка при запишување на датотеката.\n"
#define MSGTR_RecommendedVideoBitrate "Препорачана видео битрата за %s CD: %d\n"
#define MSGTR_VideoStreamResult "\nВидео проток: %8.3f kbit/s  (%d B/s)  големина: %"PRIu64" бајти %5.3f сек %d фрејма\n"
#define MSGTR_AudioStreamResult "\nАудио проток: %8.3f kbit/s (%d B/s) големина: %"PRIu64" бајти %5.3f сек\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     метод на променлива битрата\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(стандардно)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           просечна битрата\n"\
"\n"\
" cbr           константна битрата\n"\
"               Исто така го принудува CBR модот на енкодирање на последователните претходно дефинирани ABR модови\n"\
"\n"\
" br=<0-1024>   одредете битрата во kBit (само за CBR и ABR)\n"\
"\n"\
" q=<0-9>       квалитет (0-најголем, 9-најмал) (само за VBR)\n"\
"\n"\
" aq=<0-9>      алгоритамски квалитет (0-најдобар/најспор, 9-најлош/најбрз)\n"\
"\n"\
" ratio=<1-100> коефициент на компресијата\n"\
"\n"\
" vol=<0-10>    подеси го влезното аудио искористување\n"\
"\n"\
" mode=<0-3>    (стандардно: auto)\n"\
"                0: стерео\n"\
"                1: joint-стерео\n"\
"                2: дво-канално\n"\
"                3: моно\n"\
"\n"\
" padding=<0-2>\n"\
"                0: не\n"\
"                1: сите\n"\
"                2: прилагоди\n"\
"\n"\
" fast          се префрла на побрзо енкодирање на последователните предходно дефинирани VBR модови,\n"\
"               незабележително послаб квалитет и поголема битрата.\n"\
"\n"\
" preset=<value> ги обезбедува најдобрите можни подесувања за квалитет.\n"\
"                 средно(medium): VBR  енкодирање,  добар квалитет\n"\
"                 (150-180 kbps опсег на битратата)\n"\
"                 стандардно(standard):  VBR енкодирање, висок квалитет\n"\
"                 (170-210 kbps опсег на битратата)\n"\
"                 екстремно(extreme): VBR енкодирање, многу висок квалитет\n"\
"                 (200-240 kbps опсег на битратата)\n"\
"                 лудо(insane):  CBR  енкодирање, највисок предходно дефиниран квалитет\n"\
"                 (320 kbps битрата)\n"\
"                 <8-320>: ABR енкодирање на просечно дадени kbps битрата.\n\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM Уредот '%s' не е пронајден.\n"
#define MSGTR_ErrTrackSelect "Грешка во избирањето на VCD датотеката."
#define MSGTR_ReadSTDIN "Читање од stdin...\n"
#define MSGTR_UnableOpenURL "Не може да се отвори URL: %s\n"
#define MSGTR_ConnToServer "Поврзан со серверот: %s\n"
#define MSGTR_FileNotFound "Датотеката не е пронајдена: '%s'\n"

#define MSGTR_SMBInitError "Не може да ја иницијализира libsmbclient библиотеката: %d\n"
#define MSGTR_SMBFileNotFound "Не може да отвори од локалната мрежа: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer не е компајлиран со подршка за SMB читање\n"

#define MSGTR_CantOpenDVD "Не можеше да се отвори DVD уредот: %s (%s)\n"
#define MSGTR_DVDnumTitles "Има %d наслови на ова DVD.\n"
#define MSGTR_DVDinvalidTitle "Невалиден DVD број на насловот: %d\n"
#define MSGTR_DVDnumChapters "Има %d поглавја на овој DVD наслов.\n"
#define MSGTR_DVDinvalidChapter "Невалиден DVD број на поглавјето: %d\n"
#define MSGTR_DVDnumAngles "Има %d агли на овој DVD наслов.\n"
#define MSGTR_DVDinvalidAngle "Невалиден DVD број на аголот: %d\n"
#define MSGTR_DVDnoIFO "Не може да ја отвори IFO датотеката за DVD насловот %d.\n"
#define MSGTR_DVDnoVOBs "Не може да ги отвори насловите на VOB датотеките (VTS_%02d_1.VOB).\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "ПРЕДУПРЕДУВАЊЕ: Предефиниран е хедерот на аудио протокот %d.\n"
#define MSGTR_VideoStreamRedefined "ПРЕДУПРЕДУВАЊЕ: Предефиниран е хедерот на видео протокот %d.\n"
#define MSGTR_TooManyAudioInBuffer "\nПремногу аудио пакети во баферот: (%d во %d бајти).\n"
#define MSGTR_TooManyVideoInBuffer "\nПремногу видео пакети во баферот: (%d во %d бајти).\n"
#define MSGTR_MaybeNI "Можеби вие пуштате непреклопувачки стрим/датотека или кодекот не работи?\n" \
                      "За AVI датотеки, обиди се да го присилиш непреклопувачкиот мод со опцијата -ni.\n"
#define MSGTR_SwitchToNi "\nОткриена е лошо преклопена AVI датотека - се префрла на модот -ni...\n"
#define MSGTR_Detected_XXX_FileFormat "Пронајден е %s формат на датотеката.\n"
#define MSGTR_DetectedAudiofile "Пронајдена е аудио датотека.\n"
#define MSGTR_InvalidMPEGES "Невалиден MPEG-ES проток??? Контактирајте со авторот, можеби е баг :(\n"
#define MSGTR_FormatNotRecognized "============ Извинете, овој формат на датотеката не е пропознат/подржан =============\n"\
                                  "=== Ако оваа датотека е AVI, ASF или MPEG проток, ве молиме контактирајте со авторот! ===\n"
#define MSGTR_MissingVideoStream "Не е пронајден видео проток.\n"
#define MSGTR_MissingAudioStream "Не е пронајден аудио проток -> нема звук.\n"
#define MSGTR_MissingVideoStreamBug "Недостига видео проток!? Контактирајте со авторот, можеби е баг :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: Датотеката не го содржи избраниот аудио или видео проток.\n"

#define MSGTR_NI_Forced "Присилено"
#define MSGTR_NI_Detected "Пронајдено"
#define MSGTR_NI_Message "%s НЕПРЕКЛОПУВАЧКИ формат на AVI датотеката.\n"

#define MSGTR_UsingNINI "Се користи НЕПРЕКЛОПУВАЧКИ неисправен формат на AVI датотеката.\n"
#define MSGTR_CouldntDetFNo "Не може да го одреди бројот на фрејмови (за абсолутно барање)\n"
#define MSGTR_CantSeekRawAVI "Не може да бара во редовите на AVI стримовите. (Потребен е индекс, обидете се со -idx префрлување.)\n"
#define MSGTR_CantSeekFile "Не може да бара во оваа датотека.\n"

#define MSGTR_MOVcomprhdr "MOV: За подршка на компресирани хедери потребно е ZLIB!\n"
#define MSGTR_MOVvariableFourCC "MOV: ПРЕДУПРЕДУВАЊЕ: Откриено е променливо FOURCC!?\n"
#define MSGTR_MOVtooManyTrk "MOV: ПРЕДУПРЕДУВАЊЕ: премногу траки"
#define MSGTR_DetectedTV "Пронајден е ТВ! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Не може да се отвори ogg демуксерот.\n"
#define MSGTR_CannotOpenAudioStream "Не може да се отвори аудио протокот: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Не може да се отвори протокот за преводи: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Не успеа да се отвори аудио демуксерот: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Не успеа да се отвори демуксерот за преводи: %s\n"
#define MSGTR_TVInputNotSeekable "Влезот за ТВ не е за барање! (Барањето веројатно е за менување на канали ;)\n"
#define MSGTR_ClipInfo "Информации за клипот:\n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: Пронајдена е NTSC содржина од 30fps, се менува фрејмратата.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: Пронајдена е прогресивна NTSC содржина од 24fps, се менува фрејмратата  \n"

// dec_video.c и dec_audio.c:
#define MSGTR_CantOpenCodec "Не може да се отвори кодекот.\n"
#define MSGTR_CantCloseCodec "Не може да се затвори кодекот.\n"

#define MSGTR_MissingDLLcodec "ГРЕШКА: Не може да се отвори потребниот DirectShow кодек %s.\n"
#define MSGTR_ACMiniterror "Не може да се вчита/иницијализира Win32/ACM AUDIO кодекот (недостасува DLL датотека?).\n"
#define MSGTR_MissingLAVCcodec "Не може да го пронајде кодекот '%s' во libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: ФАТАЛНО: EOF додека се бараше хедерот на секвенцата.\n"
#define MSGTR_CannotReadMpegSequHdr "ФАТАЛНО: Не може да се прочита хедерот на секвенцата.\n"
#define MSGTR_CannotReadMpegSequHdrEx "ФАТАЛНО: Не може да се прочита продолжетокот од хедерот на секвенцата.\n"
#define MSGTR_BadMpegSequHdr "MPEG: лош хедер на секвенцата\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: лош продолжеток од хедерот на секвенцата\n"

#define MSGTR_ShMemAllocFail "Не може да се рапореди заедничката меморија\n"
#define MSGTR_CantAllocAudioBuf "Не може да се распореди баферот на аудио излезот\n"

#define MSGTR_UnknownAudio "Непознат/недостасува аудио формат-> нема звук\n"

#define MSGTR_UsingExternalPP "[PP] Се користи надворешен постпроцесорски филтер, max q = %d.\n"
#define MSGTR_UsingCodecPP "[PP] Се користи постпроцесирањето на кодекот, max q = %d.\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Бараната фамилија на видео кодекот [%s] (vfm=%s) не е достапна.\nОвозможете ја при компајлирањето.\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Бараната фамилија на видео кодекот [%s] (afm=%s) не е достапна.\nОвозможете ја при компајлирањето.\n"
#define MSGTR_OpeningVideoDecoder "Се отвара видео декодерот: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "Се отвара аудио декодерот: [%s] %s\n"
#define MSGTR_VDecoderInitFailed "Иницијализирањето на Видео Декодерот не успеа :(\n"
#define MSGTR_ADecoderInitFailed "Иницијализирањето на Аудио Декодерот не успеа :(\n"
#define MSGTR_ADecoderPreinitFailed "Преиницијализирањето на Аудио Декодерот не успеа :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Не успеа да се отвори LIRC подршката.\n"
#define MSGTR_LIRCcfgerr "Не успеа да се прочита LIRC конфигурационата датотека %s.\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Не може да се пронајде видео филтерот '%s'\n"
#define MSGTR_CouldNotOpenVideoFilter "Не може да се отвори видео филтерот '%s'\n"
#define MSGTR_OpeningVideoFilter "Се отвара видео филтерот: "
#define MSGTR_CannotFindColorspace "Не може да се пронајде заеднички простор за боја, дури ни со внесување на 'нијанса' :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: Кодекот не ги подеси sh->disp_w и sh->disp_h, се обидува да заобиколи.\n"
#define MSGTR_CouldNotFindColorspace "Не може да се пронајде соодветен простор за боја - се обидува повторно со -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Аспектот на Филмот е %.2f:1 - се преместува на точниот аспект на филмот.\n"
#define MSGTR_MovieAspectUndefined "Аспектот на филмот не е дефиниран - не е применето преместување.\n"

// ================================ GUI ================================

#define MSGTR_GUI_AboutMPlayer "За MPlayer"
#define MSGTR_GUI_Add "Додади"
#define MSGTR_GUI_AspectRatio "Пропорционалност"
#define MSGTR_GUI_Audio "Аудио"
#define MSGTR_GUI_AudioDelay "Аудио задоцнување"
#define MSGTR_GUI_AudioTrack "Вчитај надворешена аудио датотека "
#define MSGTR_GUI_AudioTracks "Аудио трака"
#define MSGTR_GUI_AvailableDrivers "Достапни драјвери:"
#define MSGTR_GUI_AvailableSkins "Скинови"
#define MSGTR_GUI_Bass "Бас"
#define MSGTR_GUI_Blur "Замаглување"
#define MSGTR_GUI_Brightness "Светлина"
#define MSGTR_GUI_Browse "Разгледај"
#define MSGTR_GUI_Cache "Кеш"
#define MSGTR_GUI_CacheSize "Големина на кешот"
#define MSGTR_GUI_Cancel "Откажи"
#define MSGTR_GUI_Center "Центар"
#define MSGTR_GUI_Channel1 "Канал 1"
#define MSGTR_GUI_Channel2 "Канал 2"
#define MSGTR_GUI_Channel3 "Канал 3"
#define MSGTR_GUI_Channel4 "Канал 4"
#define MSGTR_GUI_Channel5 "Канал 5"
#define MSGTR_GUI_Channel6 "Канал 6"
#define MSGTR_GUI_ChannelAll "Сите"
#define MSGTR_GUI_ChapterN "Поглавје %d"
#define MSGTR_GUI_ChapterNN "Поглавје %2d"
#define MSGTR_GUI_Chapters "Поглавја"
#define MSGTR_GUI_Clear "Исчисти"
#define MSGTR_GUI_CodecFamilyAudio "Фамилија на аудио кодекот"
#define MSGTR_GUI_CodecFamilyVideo "Фамилија на видео кодекот"
#define MSGTR_GUI_Coefficient "Коефициент"
#define MSGTR_GUI_Configure "Конфигурирај"
#define MSGTR_GUI_ConfigureDriver "Конфигурирај Драјвер"
#define MSGTR_GUI_Contrast "Контраст"
#define MSGTR_GUI_Cp874 "Thai charset (CP874)"
#define MSGTR_GUI_Cp936 "Поедноставена Кинеска кодна страна (CP936)"
#define MSGTR_GUI_Cp949 "Корејска кодна страна (CP949)"
#define MSGTR_GUI_Cp1250 "Словенечки/Централно европски Windows (CP1250)"
#define MSGTR_GUI_Cp1251 "Кириличен Windows (CP1251)"
#define MSGTR_GUI_CpBIG5 "Традиционална Кинеска кодна страна (BIG5)"
#define MSGTR_GUI_CpISO8859_1 "Западно-европски јазици (ISO-8859-1)"
#define MSGTR_GUI_CpISO8859_2 "Словенечки/Централно европски јазици (ISO-8859-2)"
#define MSGTR_GUI_CpISO8859_3 "Есперанто, Galician, Maltese, Турски (ISO-8859-3)"
#define MSGTR_GUI_CpISO8859_4 "Стара Балтичка кодна страна (ISO-8859-4)"
#define MSGTR_GUI_CpISO8859_5 "Кириличен (ISO-8859-5)"
#define MSGTR_GUI_CpISO8859_6 "Арапски (ISO-8859-6)"
#define MSGTR_GUI_CpISO8859_7 "Модерен Грчки (ISO-8859-7)"
#define MSGTR_GUI_CpISO8859_8 "Еврејска кодна страна (ISO-8859-8)"
#define MSGTR_GUI_CpISO8859_9 "Турски (ISO-8859-9)"
#define MSGTR_GUI_CpISO8859_13 "Балтички (ISO-8859-13)"
#define MSGTR_GUI_CpISO8859_14 "Celtic (ISO-8859-14)"
#define MSGTR_GUI_CpISO8859_15 "Западно-европски јазици со Евро (ISO-8859-15)"
#define MSGTR_GUI_CpKOI8_R "Руски (KOI8-R)"
#define MSGTR_GUI_CpKOI8_U "Украински, Белоруски (KOI8-U/RU)"
#define MSGTR_GUI_CpShiftJis "Јапонска кодна страна (SHIFT-JIS)"
#define MSGTR_GUI_CpUnicode "Повеќе-коден"
#define MSGTR_GUI_Delay "Задоцнување"
#define MSGTR_GUI_Demuxers_Codecs "Кодеци и демуксер"
#define MSGTR_GUI_DeviceCDROM "CD-ROM уред"
#define MSGTR_GUI_DeviceDVD "DVD уред"
#define MSGTR_GUI_Directory "Патека"
#define MSGTR_GUI_DirectoryTree "Дрво на директориумите"
#define MSGTR_GUI_DropSubtitle "Исклучи го преводот ..."
#define MSGTR_GUI_DVD "DVD"
#define MSGTR_GUI_EnableAutomaticAVSync "Авто-синхронизација вклучи/исклучи"
#define MSGTR_GUI_EnableCache "Кеш вклучи/исклучи"
#define MSGTR_GUI_EnableDirectRendering "Овозможи директно бафирање"
#define MSGTR_GUI_EnableDoubleBuffering "Овозможи дупло бафирање"
#define MSGTR_GUI_EnableEqualizer "Овозможи еквилајзер"
#define MSGTR_GUI_EnableExtraStereo "Овозможи екстра стерео"
#define MSGTR_GUI_EnableFrameDropping "Овозможи изоставување на фрејмови"
#define MSGTR_GUI_EnableFrameDroppingIntense "Овозможи ТЕШКО изоставување на фрејмови (опасно)"
#define MSGTR_GUI_EnablePlaybar "Овозможи го плејбарот"
#define MSGTR_GUI_EnablePostProcessing "Овозможи ПостПроцесирање"
#define MSGTR_GUI_Encoding "Енкодирање"
#define MSGTR_GUI_Equalizer "Еквилајзер"
#define MSGTR_GUI_Error "Грешка!"
#define MSGTR_GUI_ErrorFatal "Фатална Грешка!"
#define MSGTR_GUI_File "Пушти датотека "
#define MSGTR_GUI_Files "Датотеки"
#define MSGTR_GUI_Flip "Преврти ја сликата наопаку"
#define MSGTR_GUI_Font "Фонт"
#define MSGTR_GUI_FrameRate "FPS (Фрејмови Во Секунда)"
#define MSGTR_GUI_FrontLeft "Преден Лев"
#define MSGTR_GUI_FrontRight "Преден Десен"
#define MSGTR_GUI_HideVideoWindow "Прикажи го видео прозорецот кога не е активен"
#define MSGTR_GUI_Hue "Нијанси"
#define MSGTR_GUI_Lavc "Користи LAVC (ffmpeg)"
#define MSGTR_GUI_MaximumUsageSpareCPU "Авто квалитет"
#define MSGTR_GUI_Miscellaneous "Разно"
#define MSGTR_GUI_MSG_DXR3NeedsLavc "Се извинуваме, не можете да пуштате датотеки кои што не се MPEG со вашиот DXR3/H+ уред без реенкодирање.\nВе молиме овозможете lavc во DXR3/H+ кутијата за конфигурирање."
#define MSGTR_GUI_MSG_MemoryErrorWindow "Се извинуваме, немате доволно меморија за графичкиот бафер."
#define MSGTR_GUI_MSG_NoFileLoaded "Не е вчитана датотека."
#define MSGTR_GUI_MSG_NoMediaOpened "Не е отворен медиум."
#define MSGTR_GUI_MSG_PlaybackNeedsRestart "Ве молиме запомтете дека треба да го рестартирате плејбекот за да можат некои опции да проработат!"
#define MSGTR_GUI_MSG_SkinBitmapConversionError "грешка при конвертирање од 24 бита во 32 бита ( %s )\n"
#define MSGTR_GUI_MSG_SkinBitmapNotFound "датотеката не е пронајдена ( %s )\n"
#define MSGTR_GUI_MSG_SkinBitmapPngReadError "PNG грешка во читањето ( %s )\n"
#define MSGTR_GUI_MSG_SkinCfgNotFound "Не е пронајден скин ( %s ).\n"
#define MSGTR_GUI_MSG_SkinErrorBitmap16Bit "Не е подржана 16 битна или помала длабочина на битмапата ( %s ).\n"
#define MSGTR_GUI_MSG_SkinErrorMessage "[скин] грешка во конфигурационата датотека за скинови, линија %d: %s"
#define MSGTR_GUI_MSG_SkinFontFileNotFound "фонт датотеката не е пронајдена\n"
#define MSGTR_GUI_MSG_SkinFontImageNotFound "датотеката со фонт сликата не е пронајдена\n"
#define MSGTR_GUI_MSG_SkinFontNotFound "непостоечки фонт идентификувач ( %s )\n"
#define MSGTR_GUI_MSG_SkinMemoryError "нема доволно меморија\n"
#define MSGTR_GUI_MSG_SkinTooManyFonts "премногу означени фонтови\n"
#define MSGTR_GUI_MSG_SkinUnknownMessage "непозната порака: %s\n"
#define MSGTR_GUI_MSG_SkinUnknownParameter "непознат параметар ( %s )\n"
#define MSGTR_GUI_MSG_VideoOutError "Се извинуваме, не е пронајден компитабилен драјвер на излезното видео за графички интерфејс."
#define MSGTR_GUI_Mute "Мутирај"
#define MSGTR_GUI_NetworkStreaming "Мрежен проток ..."
#define MSGTR_GUI_Next "Нареден стрим"
#define MSGTR_GUI_NoChapter "Нема поглавје"
#define MSGTR_GUI__none_ "(ниеден)"
#define MSGTR_GUI_NonInterleavedParser "Користи не-преклопувачки AVI расчленувач"
#define MSGTR_GUI_NormalizeSound "Нормализирај го звукот"
#define MSGTR_GUI_Ok "Во Ред"
#define MSGTR_GUI_Open "Отвори ..."
#define MSGTR_GUI_Original "Оригинал"
#define MSGTR_GUI_OsdLevel "Ниво на OSD"
#define MSGTR_GUI_OSD_Subtitles "Преводи и OSD"
#define MSGTR_GUI_Outline "Надворешна линија"
#define MSGTR_GUI_PanAndScan "Panscan"
#define MSGTR_GUI_Pause "Пауза"
#define MSGTR_GUI_Play "Плеј"
#define MSGTR_GUI_Playback "Пуштено"
#define MSGTR_GUI_Playlist "Плејлиста"
#define MSGTR_GUI_Position "Позиција"
#define MSGTR_GUI_PostProcessing "ПостПроцесирање"
#define MSGTR_GUI_Preferences "Подесувања"
#define MSGTR_GUI_Previous "Претходен стрим"
#define MSGTR_GUI_Quit "Излези "
#define MSGTR_GUI_RearLeft "Заден Лев"
#define MSGTR_GUI_RearRight "Заден Денес"
#define MSGTR_GUI_Remove "Отстрани"
#define MSGTR_GUI_Saturation "Заситување"
#define MSGTR_GUI_SaveWindowPositions "Зачувај ја позицијата на прозорецот"
#define MSGTR_GUI_ScaleMovieDiagonal "Пропорционално со дијагоналата на филмот"
#define MSGTR_GUI_ScaleMovieHeight "Пропорционално со висината на филмот"
#define MSGTR_GUI_ScaleMovieWidth "Пропорционално со ширината на филмот"
#define MSGTR_GUI_ScaleNo "Без авто-големина"
#define MSGTR_GUI_SeekingInBrokenMedia "Повторно ја направи индекс табелата, ако е потребно"
#define MSGTR_GUI_SelectAudioFile "Изберете надворешен аудио канал ..."
#define MSGTR_GUI_SelectedFiles "Избрани датотеки"
#define MSGTR_GUI_SelectFile "Изберете датотека ..."
#define MSGTR_GUI_SelectFont "Изберете фонт ..."
#define MSGTR_GUI_SelectSubtitle "Изберете превод ..."
#define MSGTR_GUI_SizeDouble "Двојна големина"
#define MSGTR_GUI_SizeFullscreen "На Цел Екран"
#define MSGTR_GUI_SizeNormal "Нормална големина"
#define MSGTR_GUI_SizeOSD "Големина на OSD"
#define MSGTR_GUI_SizeSubtitles "Големина на текстот"
#define MSGTR_GUI_SkinBrowser "Разгледувач на скинови"
#define MSGTR_GUI_Sponsored "GUI развивањето спонзорирано од UHU Linux"
#define MSGTR_GUI_StartFullscreen "Почни со цел екран"
#define MSGTR_GUI_Stop "Стоп"
#define MSGTR_GUI_Subtitle "Превод"
#define MSGTR_GUI_SubtitleAllowOverlap "Преместувај го преклопувањето на преводот"
#define MSGTR_GUI_SubtitleAutomaticLoad "Оневозможи го автоматското вчитување на преводот"
#define MSGTR_GUI_SubtitleConvertMpsub "Конвертирај ги дадените преводи во форматот за преводи на MPlayer"
#define MSGTR_GUI_SubtitleConvertSrt "Конвертирај ги дадените преводи во форматот базиран на време SubViewer (SRT)"
#define MSGTR_GUI_Subtitles "Јазици на преводите"
#define MSGTR_GUI_SyncValue "Авто-синхронизација"
#define MSGTR_GUI_TitleNN "Наслов %2d"
#define MSGTR_GUI_Titles "Наслови"
#define MSGTR_GUI_TrackN "Трака %d"
#define MSGTR_GUI_TurnOffXScreenSaver "Стопирај ја заштитата на екранот"
#define MSGTR_GUI_URL "Пушти URL "
#define MSGTR_GUI_VCD "VCD"
#define MSGTR_GUI_Video "Видео"
#define MSGTR_GUI_VideoEncoder "Видео енкодер"
#define MSGTR_GUI_VideoTracks "Видео трака"
#define MSGTR_GUI_Warning "Предупредување!"
