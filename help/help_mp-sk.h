// Translated by:  Daniel Beňa, benad (at) centrum.cz
// last sync on 2006-04-28 with 1.249
// but not compleated

// ========================= MPlayer help ===========================

// Preklad do slovenčiny

#ifdef CONFIG_VCD
#define MSGTR_HelpVCD " vcd://<trackno>  prehrať VCD (video cd) stopu zo zariadenia namiesto zo súboru\n"
#else
#define MSGTR_HelpVCD
#endif

#ifdef CONFIG_DVDREAD
#define MSGTR_HelpDVD " dvd://<titleno>  prehrať DVD titul/stopu zo zariadenia (mechaniky) namiesto súboru\n"\
                      " -alang/-slang   vybrať jazyk DVD zvuku/titulkov(pomocou 2-miest. kódu krajiny)\n"
#else
#define MSGTR_HelpDVD
#endif

#define MSGTR_Help \
"Použitie:   mplayer [prepínače] [url|cesta/]menosúboru\n"\
"\n"\
"Základné prepínače: (Kompletný zoznam nájdete v man stránke)\n"\
" -vo <drv[:dev]> výber výstup. video ovládača&zariadenia (-vo help pre zoznam)\n"\
" -ao <drv[:dev]> výber výstup. audio ovládača&zariadenia (-ao help pre zoznam)\n"\
MSGTR_HelpVCD \
MSGTR_HelpDVD \
" -ss <timepos>   posun na pozíciu (sekundy alebo hh:mm:ss)\n"\
" -nosound        prehrávať bez zvuku\n"\
" -fs             voľby pre celú obrazovku (alebo -vm -zoom, detaily viď. man stránku)\n"\
" -x <x> -y <y>   zväčšenie obrazu na rozmer <x>*<y> (pokiaľ to vie -vo ovládač!)\n"\
" -sub <file>     voľba súboru s titulkami (viď tiež -subfps, -subdelay)\n"\
" -playlist <file> určenie súboru so zoznamom prehrávaných súborov\n"\
" -vid x -aid y   výber čísla video (x) a audio (y) prúdu pre prehrávanie\n"\
" -fps x -srate y voľba pre zmenu video (x fps) a audio (y Hz) frekvencie\n"\
" -pp <quality>   aktivácia postprocesing filtra (0-4 pre DivX, 0-63 pre mpegy)\n"\
" -framedrop      povoliť zahadzovanie snímkov (pre pomalé stroje)\n"\
"\n"\
"Zákl. klávesy:   (pre kompl. pozrite aj man stránku a input.conf)\n"\
" <-  alebo  ->   posun vzad/vpred o 10 sekund\n"\
" hore / dole     posun vzad/vpred o  1 minútu\n"\
" pgup alebo pgdown  posun vzad/vpred o 10 minút\n"\
" < alebo >       posun vzad/vpred v zozname prehrávaných súborov\n"\
" p al. medzerník pauza (pokračovanie stlačením klávesy)\n"\
" q alebo ESC     koniec prehrávania a ukončenie programu\n"\
" + alebo -       upraviť spozdenie zvuku v krokoch +/- 0.1 sekundy\n"\
" o               cyklická zmena režimu OSD:  nič / pozícia / pozícia+čas\n"\
" * alebo /       pridať alebo ubrať hlasitosť (stlačením 'm' výber master/pcm)\n"\
" z alebo x       upraviť spozdenie titulkov v krokoch +/- 0.1 sekundy\n"\
" r alebo t       upraviť pozíciu titulkov hore/dole, pozrite tiež -vf!\n"\
"\n"\
" * * * * PREČÍTAJTE SI MAN STRÁNKU PRE DETAILY (ĎALŠIE VOĽBY A KLÁVESY)! * * * *\n"\
"\n"

static const char help_text[] = MSGTR_Help;

// ========================= MPlayer messages ===========================
// mplayer.c:

#define MSGTR_Exiting "\nKončím...\n"
#define MSGTR_ExitingHow "\nKončím... (%s)\n"
#define MSGTR_Exit_quit "Koniec"
#define MSGTR_Exit_eof "Koniec súboru"
#define MSGTR_Exit_error "Závažná chyba"
#define MSGTR_IntBySignal "\nMPlayer prerušený signálom %d v module: %s \n"
#define MSGTR_NoHomeDir "Nemôžem najsť domáci (HOME) adresár\n"
#define MSGTR_GetpathProblem "get_path(\"config\") problém\n"
#define MSGTR_CreatingCfgFile "Vytváram konfiguračný súbor: %s\n"
#define MSGTR_CantLoadFont "Nemôžem načítať font: %s\n"
#define MSGTR_CantLoadSub "Nemôžem načítať titulky: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: požadovaný prúd chýba!\n"
#define MSGTR_CantOpenDumpfile "Nejde otvoriť súbor pre dump!!!\n"
#define MSGTR_CoreDumped "jadro vypísané :)\n"
#define MSGTR_FPSnotspecified "V hlavičke súboru nie je udané (alebo je zlé) FPS! Použite voľbu -fps!\n"
#define MSGTR_TryForceAudioFmtStr "Pokúšam sa vynútiť rodinu audiokodeku %s...\n"
#define MSGTR_CantFindAudioCodec "Nemôžem nájsť kodek pre audio formát 0x%X!\n"
#define MSGTR_TryForceVideoFmtStr "Pokúšam se vnútiť rodinu videokodeku %s...\n"
#define MSGTR_CantFindVideoCodec "Nemôžem najsť kodek pre video formát 0x%X!\n"
#define MSGTR_CannotInitVO "FATAL: Nemôžem inicializovať video driver!\n"
#define MSGTR_CannotInitAO "nemôžem otvoriť/inicializovať audio driver -> TICHO\n"
#define MSGTR_StartPlaying "Začínam prehrávať...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"         ***********************************************************\n"\
"         ****  Na prehratie tohoto je váš systém príliš POMALÝ!  ****\n"\
"         ***********************************************************\n"\
"!!! Možné príčiny, problémy a riešenia:\n"\
"- Nejčastejšie: nesprávny/chybný _zvukový_ ovládač.\n"\
"  - Skúste -ao sdl alebo použite OSS emuláciu ALSA.\n"\
"  - Experimentujte s rôznymi hodnotami -autosync, 30 je dobrý začiatok.\n"\
"- Pomalý video výstup\n"\
"  - Skúste iný -vo ovládač (pre zoznam: -vo help) alebo skúste -framedrop!\n"\
"- Pomalý CPU\n"\
"  - Neskúšajte prehrávať veľké dvd/divx na pomalom cpu! Skúste lavdopts,\n"\
"    napr. -vfm ffmpeg -lavdopts lowres=1:fast:skiploopfilter=all.\n"\
"- Poškodený súbor\n"\
"  - Skúste rôzne kombinácie týchto volieb -nobps -ni -forceidx -mc 0.\n"\
"- Pomalé médium (NFS/SMB, DVD, VCD...)\n"\
"  - Skúste -cache 8192.\n"\
"- Používate -cache na prehrávanie non-interleaved súboru?\n"\
"  - Skúste -nocache.\n"\
"Prečítajte si DOCS/HTML/en/video.html sú tam tipy na vyladenie/zrýchlenie.\n"\
"Ak nič z tohto nepomohlo, prečítajte si DOCS/HTML/en/bugreports.html.\n\n"

#define MSGTR_NoGui "MPlayer bol zostavený BEZ podpory GUI!\n"
#define MSGTR_GuiNeedsX "MPlayer GUI vyžaduje X11!\n"
#define MSGTR_Playing "Prehrávam %s\n"
#define MSGTR_NoSound "Audio: bez zvuku!!!\n"
#define MSGTR_FPSforced "FPS vnútené na hodnotu %5.3f  (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "Dostupné video výstupné ovládače:\n"
#define MSGTR_AvailableAudioOutputDrivers "Dostupné audio výstupné ovládače:\n"
#define MSGTR_AvailableAudioCodecs "Dostupné audio kodeky:\n"
#define MSGTR_AvailableVideoCodecs "Dostupné video kodeky:\n"
#define MSGTR_AvailableAudioFm "Dostupné (vkompilované) audio rodiny kodekov/ovládače:\n"
#define MSGTR_AvailableVideoFm "Dostupné (vkompilované) video rodiny kodekov/ovládače:\n"
#define MSGTR_AvailableFsType "Dostupné zmeny plnoobrazovkových módov:\n"
#define MSGTR_CannotReadVideoProperties "Video: nemôžem čítať vlastnosti\n"
#define MSGTR_NoStreamFound "Nenájdený prúd\n"
#define MSGTR_ErrorInitializingVODevice "Chyba pri otváraní/inicializácii vybraných video_out (-vo) zariadení!\n"
#define MSGTR_ForcedVideoCodec "Vnútený video kodek: %s\n"
#define MSGTR_ForcedAudioCodec "Vnútený video kodek: %s\n"
#define MSGTR_Video_NoVideo "Video: žiadne video!!!\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: Nemôžem inicializovať video filtre (-vf) alebo video výstup (-vo)!\n"
#define MSGTR_Paused "  =====  PAUZA  ====="
#define MSGTR_PlaylistLoadUnable "\nNemôžem načítať playlist %s\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer zhavaroval na 'Illegal Instruction'.\n"\
"  Môže to byť chyba v našom novom kóde na detekciu procesora...\n"\
"  Prosím prečítajte si DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer zhavaroval na 'Illegal Instruction'.\n"\
"  Obyčajne sa to stáva, keď ho používate na inom procesore ako pre ktorý bol\n"\
"  skompilovaný/optimalizovaný.\n"\
"  Skontrolujte si to!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer zhavaroval nesprávnym použitím CPU/FPU/RAM.\n"\
"  Prekompilujte MPlayer s --enable-debug a urobte 'gdb' backtrace a\n"\
"  disassemblujte. Pre detaily, pozrite DOCS/HTML/en/bugreports_what.html#bugreports_crash.b.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer zhavaroval. To sa nemalo stať.\n"\
"  Môže to byť chyba v MPlayer kóde _alebo_ vo Vaších ovládačoch _alebo_ gcc\n"\
"  verzii. Ak si myslíte, že je to chyba MPlayeru, prosím prečítajte si DOCS/HTML/en/bugreports.html\n"\
"  a postupujte podľa inštrukcii. Nemôžeme Vám pomôcť, pokiaľ neposkytnete\n"\
"  tieto informácie pri ohlasovaní možnej chyby.\n"
#define MSGTR_LoadingConfig "Čítam konfiguráciu '%s'\n"
#define MSGTR_AddedSubtitleFile "SUB: pridaný súbor titulkov (%d): %s\n"
#define MSGTR_RemovedSubtitleFile "SUB: odobratý súbor titulkov (%d): %s\n"
#define MSGTR_RTCDeviceNotOpenable "Nepodarilo sa otvoriť %s: %s (malo by to byť čitateľné pre používateľa.)\n"
#define MSGTR_LinuxRTCInitErrorIrqpSet "Chyba pri inicializácii Linuxových RTC v ioctl (rtc_irqp_set %lu): %s\n"
#define MSGTR_IncreaseRTCMaxUserFreq "Skúste pridať \"echo %lu > /proc/sys/dev/rtc/max-user-freq\" do štartovacích skriptov vášho systému.\n"
#define MSGTR_LinuxRTCInitErrorPieOn "Chyba pri inicializácii Linuxových RTC v ioctl (rtc_pie_on): %s\n"
#define MSGTR_Getch2InitializedTwice "VAROVANIE: getch2_init je volaná dvakrát!\n"
#define MSGTR_CantOpenLibmenuFilterWithThisRootMenu "Nemôžem otvoriť video filter libmenu s koreňovým menu %s.\n"
#define MSGTR_AudioFilterChainPreinitError "Chyba pri predinicializácii reťazca audio filtrov!\n"
#define MSGTR_LinuxRTCReadError "Chyba pri čítaní z Linuxových RTC: %s\n"
#define MSGTR_SoftsleepUnderflow "Pozor! Podtečenie softsleep!\n"

#define MSGTR_EdlOutOfMem "Nedá sa alokovať dostatok pamäte pre EDL dáta.\n"
#define MSGTR_EdlRecordsNo "Čítam %d EDL akcie.\n"
#define MSGTR_EdlQueueEmpty "Všetky EDL akcie boly už vykonané.\n"
#define MSGTR_EdlCantOpenForWrite "Nedá sa otvoriť EDL súbor [%s] pre zápis.\n"
#define MSGTR_EdlNOsh_video "EDL sa nedá použiť bez videa, vypínam.\n"
#define MSGTR_EdlNOValidLine "Chyba EDL na riadku: %s\n"
#define MSGTR_EdlBadlyFormattedLine "Zle formátovaný EDL riadok [%d] Zahadzujem.\n"
#define MSGTR_EdlBadLineOverlap "Posledná stop značka bola [%f]; ďalší štart je "\
"[%f]. Záznamy musia byť chronologicky, a nesmú sa prekrývať. Zahadzujem.\n"
#define MSGTR_EdlBadLineBadStop "Časová značka stop má byť za značkou start.\n"

// mplayer.c OSD

#define MSGTR_OSDenabled "zapnuté"
#define MSGTR_OSDdisabled "vypnuté"
#define MSGTR_OSDChannel "Kanál: %s"
#define MSGTR_OSDSubDelay "Zpozdenie tit: %d ms"
#define MSGTR_OSDSpeed "Rýchlosť: x %6.2f"
#define MSGTR_OSDosd "OSD: %s"

// property values
#define MSGTR_Enabled "zapnuté"
#define MSGTR_EnabledEdl "zapnuté (edl)"
#define MSGTR_Disabled "vypnuté"
#define MSGTR_HardFrameDrop "hard"
#define MSGTR_Unknown "neznáme"
#define MSGTR_Bottom "dole"
#define MSGTR_Center "stred"
#define MSGTR_Top "hore"

// osd bar names
#define MSGTR_Volume "Hlasitosť"
#define MSGTR_Panscan "Panscan"
#define MSGTR_Gamma "Gama"
#define MSGTR_Brightness "Jas"
#define MSGTR_Contrast "Kontrast"
#define MSGTR_Saturation "Sýtosť"
#define MSGTR_Hue "Tón"

// property state
#define MSGTR_MuteStatus "Utlmenie zvuku: %s"
#define MSGTR_AVDelayStatus "A-V odchylka: %s"
#define MSGTR_OnTopStatus "Vždy navrchu: %s"
#define MSGTR_RootwinStatus "Hlavné okno: %s"
#define MSGTR_BorderStatus "Ohraničenie: %s"
#define MSGTR_FramedroppingStatus "Zahadzovanie snímkov: %s"
#define MSGTR_VSyncStatus "VSync: %s"
#define MSGTR_SubSelectStatus "Titulky: %s"
#define MSGTR_SubPosStatus "Pozícia tit.: %s/100"
#define MSGTR_SubAlignStatus "Zarovnanie tit.: %s"
#define MSGTR_SubDelayStatus "Spozdenie tit.: %s"
#define MSGTR_SubVisibleStatus "Zobr. titulkov: %s"
#define MSGTR_SubForcedOnlyStatus "Iba vynútené tit.: %s"

// mencoder.c:

#define MSGTR_UsingPass3ControlFile "Používam pass3 ovládací súbor: %s\n"
#define MSGTR_MissingFilename "\nChýbajúce meno súboru.\n\n"
#define MSGTR_CannotOpenFile_Device "Nemôžem otvoriť súbor/zariadenie\n"
#define MSGTR_CannotOpenDemuxer "Nemôžem otvoriť demuxer\n"
#define MSGTR_NoAudioEncoderSelected "\nNevybraný encoder (-oac)! Vyberte jeden alebo -nosound. Použitie -oac help!\n"
#define MSGTR_NoVideoEncoderSelected "\nNevybraný encoder (-ovc)! Vyberte jeden, použitie -ovc help!\n"
#define MSGTR_CannotOpenOutputFile "Nemôžem otvoriť súbor '%s'\n"
#define MSGTR_EncoderOpenFailed "Zlyhalo spustenie enkóderu\n"
#define MSGTR_MencoderWrongFormatAVI "\nVAROVANIE: FORMÁT VÝSTUPNÉHO SÚBORU JE _AVI_. viz -of help.\n"
#define MSGTR_MencoderWrongFormatMPG "\nVAROVANIE: FORMÁT VÝSTUPNÉHO SÚBORU JE _MPEG_. viz -of help.\n"
#define MSGTR_MissingOutputFilename "Nebol nastavený výstupný súbor, preštudujte si volbu -o"
#define MSGTR_ForcingOutputFourcc "Vnucujem výstupný formát (fourcc) na %x [%.4s]\n"
#define MSGTR_ForcingOutputAudiofmtTag "Vynucujem značku výstupného zvukového formátu 0x%x\n"
#define MSGTR_DuplicateFrames "\nduplikujem %d snímkov!!!    \n"
#define MSGTR_SkipFrame "\npreskočiť snímok!!!    \n"
#define MSGTR_ResolutionDoesntMatch "\nNový video súbor má iné rozlišení alebo farebný priestor ako jeho predchodca.\n"
#define MSGTR_FrameCopyFileMismatch "\nVšetky video soubory musí mít shodné fps, rozlišení a kodek pro -ovc copy.\n"
#define MSGTR_AudioCopyFileMismatch "\nVšetky súbory musí používať identický audio kódek a formát pro -oac copy.\n"
#define MSGTR_NoAudioFileMismatch "\nNemôžete mixovať iba video s audio a video súbormi. Skúste -nosound.\n"
#define MSGTR_NoSpeedWithFrameCopy "VAROVANIE: -speed nemá zaručenú funkčnosť s -oac copy!\n"\
"Výsledny súbor môže byť vadný!\n"
#define MSGTR_ErrorWritingFile "%s: chyba pri zápise súboru.\n"
#define MSGTR_RecommendedVideoBitrate "Odporúčaný dátový tok videa pre CD %s: %d\n"
#define MSGTR_VideoStreamResult "\nVideo prúd: %8.3f kbit/s  (%d B/s)  velkosť: %"PRIu64" bytov  %5.3f sekund  %d snímkov\n"
#define MSGTR_AudioStreamResult "\nAudio prúd: %8.3f kbit/s  (%d B/s)  velkosť: %"PRIu64" bytov  %5.3f sekund\n"
#define MSGTR_OpenedStream "úspech: formát: %d  dáta: 0x%X - 0x%x\n"
#define MSGTR_VCodecFramecopy "videokódek: framecopy (%dx%d %dbpp fourcc=%x)\n"
#define MSGTR_ACodecFramecopy "audiokódek: framecopy (formát=%x kanálov=%d frekvencia=%d bitov=%d B/s=%d vzorka-%d)\n"
#define MSGTR_MP3AudioSelected "zvolený MP3 zvuk\n"
#define MSGTR_SettingAudioDelay "Nastavujem spozdenie zvuku na %5.3f\n"
#define MSGTR_SettingVideoDelay "Nastavujem spozděnie videa na %5.3fs\n"
#define MSGTR_LimitingAudioPreload "Obmedzujem prednačítanie zvuku na 0.4s\n"
#define MSGTR_IncreasingAudioDensity "Zvyšujem hustotu audia na 4\n"
#define MSGTR_ZeroingAudioPreloadAndMaxPtsCorrection "Vnucujem prednačítanie zvuku na 0, max korekciu pts na 0\n"
#define MSGTR_LameVersion "LAME verzia %s (%s)\n\n"
#define MSGTR_InvalidBitrateForLamePreset "Chyba: Špecifikovaný dátový tok je mimo rozsah pre tento preset.\n"\
"\n"\
"Pokiaľ používate tento režim, musíte zadat hodnotu od \"8\" do \"320\".\n"\
"\n"\
"Dalšie informácie viz: \"-lameopts preset=help\"\n"
#define MSGTR_InvalidLamePresetOptions "Chyba: Nezadali ste platný profil a/alebo voľby s presetom.\n"\
"\n"\
"Dostupné profily sú:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (ABR Mode) - Implikuje režim ABR. Pre jeho použitie,\n"\
"                      jednoduche zadejte dátový tok. Napríklad:\n"\
"                      \"preset=185\" aktivuje tento preset\n"\
"                      a použije priemerný dátový tok 185 kbps.\n"\
"\n"\
"    Niekolko príkladov:\n"\
"\n"\
"    \"-lameopts fast:preset=standard  \"\n"\
" or \"-lameopts  cbr:preset=192       \"\n"\
" or \"-lameopts      preset=172       \"\n"\
" or \"-lameopts      preset=extreme   \"\n"\
"\n"\
"Dalšie informácie viz: \"-lameopts preset=help\"\n"
#define MSGTR_LamePresetsLongInfo "\n"\
"Preset prepínače sú navrhnuté tak, aby poskytovaly čo najvyššiu možnú kvalitu.\n"\
"\n"\
"Väčšina z nich bola testovaná a vyladená pomocou dôkladných zdvojených slepých\n"\
"posluchových testov, za účelom dosiahnutia a overenia tohto ciela.\n"\
"\n"\
"Nastavenia sú neustále aktualizované v súlade s najnovším vývojom\n"\
"a mali by poskytovať prakticky najvyššiu možnú kvalitu, aká je v súčasnosti \n"\
"s kódekom LAME dosažiteľná.\n"\
"\n"\
"Aktivácia presetov:\n"\
"\n"\
"   Pre režimy VBR (vo všeobecnosti najvyššia kvalita):\n"\
"\n"\
"     \"preset=standard\" Tento preset by mal býť jasnou voľbou\n"\
"                             pre väčšinu ludí a hudobných žánrov a má\n"\
"                             už vysokú kvalitu.\n"\
"\n"\
"     \"preset=extreme\" Pokiaľ máte výnimočne dobrý sluch a zodpovedajúce\n"\
"                             vybavenie, tento preset vo všeob. poskytuje\n"\
"                             mierně vyšší kvalitu ako režim \"standard\".\n"\
"\n"\
"   Pre CBR 320kbps (najvyššia možná kvalita ze všetkých presetov):\n"\
"\n"\
"     \"preset=insane\"  Tento preset je pre väčšinu ludí a situácii\n"\
"                             predimenzovaný, ale pokiaľ vyžadujete\n"\
"                             absolutne najvyššiu kvalitu bez ohľadu na\n"\
"                             velkosť súboru, je toto vaša voľba.\n"\
"\n"\
"   Pre režimy ABR (vysoká kvalita pri danom dátovém toku, ale nie tak ako VBR):\n"\
"\n"\
"     \"preset=<kbps>\"  Použitím tohoto presetu obvykle dosiahnete dobrú\n"\
"                             kvalitu pri danom dátovém toku. V závislosti\n"\
"                             na zadanom toku tento preset odvodí optimálne\n"\
"                             nastavenie pre danú situáciu.\n"\
"                             Hoci tento prístup funguje, nie je ani zďaleka\n"\
"                             tak flexibilný ako VBR, a obvykle nedosahuje\n"\
"                             úrovne kvality ako VBR na vyšších dátových tokoch.\n"\
"\n"\
"Pre zodpovedajúce profily sú k dispozícii tiež nasledujúce voľby:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (ABR režim) - Implikuje režim ABR. Pre jeho použitie\n"\
"                      jednoducho zadajte dátový tok. Napríklad:\n"\
"                      \"preset=185\" aktivuje tento preset\n"\
"                      a použije priemerný dátový tok 185 kbps.\n"\
"\n"\
"   \"fast\" - V danom profile aktivuje novú rýchlu VBR kompresiu.\n"\
"            Nevýhodou je obvykle mierne vyšší dátový tok ako v normálnom\n"\
"            režime a tiež môže dôjsť k miernemu poklesu kvality.\n"\
"   Varovanie:v aktuálnej verzi môže nastavenie \"fast\" viesť k príliš\n"\
"            vysokému dátovému toku v porovnaní s normálnym nastavením.\n"\
"\n"\
"   \"cbr\"  - Pokiaľ použijete režim ABR (viz vyššie) s významným\n"\
"            dátovým tokom, napr. 80, 96, 112, 128, 160, 192, 224, 256, 320,\n"\
"            môžete použíť voľbu \"cbr\" k vnúteniu kódovánia v režime CBR\n"\
"            (konštantný tok) namiesto štandardního ABR režimu. ABR poskytuje\n"\
"            lepšiu kvalitu, ale CBR môže byť užitočný v situáciach ako je\n"\
"            vysielanie mp3 prúdu po internete.\n"\
"\n"\
"    Napríklad:\n"\
"\n"\
"      \"-lameopts fast:preset=standard  \"\n"\
" alebo \"-lameopts  cbr:preset=192       \"\n"\
" alebo \"-lameopts      preset=172       \"\n"\
" alebo \"-lameopts      preset=extreme   \"\n"\
"\n"\
"\n"\
"Pre ABR režim je k dispozícii niekolko skratiek:\n"\
"phone => 16kbps/mono        phon+/lw/mw-eu/sw => 24kbps/mono\n"\
"mw-us => 40kbps/mono        voice => 56kbps/mono\n"\
"fm/radio/tape => 112kbps    hifi => 160kbps\n"\
"cd => 192kbps               studio => 256kbps"
#define MSGTR_LameCantInit "Nedá sa nastaviť voľba pre LAME, overte dátový_tok/vzorkovaciu_frekv.,"\
"niektoré veľmi nízke dátové toky (<32) vyžadujú nižšiu vzorkovaciu frekv. (napr. -srate 8000)."\
"Pokud všetko ostané zlyhá, zkúste prednastavenia (presets)."
#define MSGTR_ConfigFileError "chyba konfiguračného súboru"
#define MSGTR_ErrorParsingCommandLine "chyba pri spracovávaní príkazového riadku"
#define MSGTR_VideoStreamRequired "Video prúd je povinný!\n"
#define MSGTR_ForcingInputFPS "vstupné fps bude interpretované ako %5.3f\n"
#define MSGTR_DemuxerDoesntSupportNosound "Tento demuxer zatiaľ nepodporuje -nosound.\n"
#define MSGTR_MemAllocFailed "Alokácia pamäte zlyhala\n"
#define MSGTR_NoMatchingFilter "Nemožem nájsť zodpovedajúci filter/ao formát!\n"
#define MSGTR_MP3WaveFormatSizeNot30 "sizeof(MPEGLAYER3WAVEFORMAT)==%d!=30, možno je vadný prekladač C?\n"
#define MSGTR_NoLavcAudioCodecName "Audio LAVC, chýba meno kódeku!\n"
#define MSGTR_LavcAudioCodecNotFound "Audio LAVC, nemôžem nájsť enkóder pre kódek %s\n"
#define MSGTR_CouldntAllocateLavcContext "Audio LAVC, nemôžem alokovať kontext!\n"
#define MSGTR_CouldntOpenCodec "Nedá sa otvoriť kódek %s, br=%d\n"
#define MSGTR_CantCopyAudioFormat "Audio formát 0x%x je nekompatibilný s '-oac copy', skúste prosím '-oac pcm',\n alebo použite '-fafmttag' pre jeho prepísanie.\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     metóda variabilnej bit. rýchlosti \n"\
"                0: cbr (konštantná bit.rýchlosť)\n"\
"                1: mt (Mark Taylor VBR alg.)\n"\
"                2: rh(Robert Hegemann VBR alg. - default)\n"\
"                3: abr (priemerná bit.rýchlosť)\n"\
"                4: mtrh (Mark Taylor Robert Hegemann VBR alg.)\n"\
"\n"\
" abr           priemerná bit. rýchlosť\n"\
"\n"\
" cbr           konštantná bit. rýchlosť\n"\
"               Vnúti tiež CBR mód na podsekvenciách ABR módov\n"\
"\n"\
" br=<0-1024>   špecifikovať bit. rýchlosť v kBit (platí iba pre CBR a ABR)\n"\
"\n"\
" q=<0-9>       kvalita (0-najvyššia, 9-najnižšia) (iba pre VBR)\n"\
"\n"\
" aq=<0-9>      algoritmická kvalita (0-najlep./najpomalšia, 9-najhoršia/najrýchl.)\n"\
"\n"\
" ratio=<1-100> kompresný pomer\n"\
"\n"\
" vol=<0-10>    nastavenie audio zosilnenia\n"\
"\n"\
" mode=<0-3>    (default: auto)\n"\
"                0: stereo\n"\
"                1: joint-stereo\n"\
"                2: dualchannel\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: no\n"\
"                1: all\n"\
"                2: adjust\n"\
"\n"\
" fast          prepnúť na rýchlejšie kódovanie na podsekvenciách VBR módov,\n"\
"               mierne nižšia kvalita and vyššia bit. rýchlosť.\n"\
"\n"\
" preset=<value> umožňuje najvyššie možné nastavenie kvality.\n"\
"                 medium: VBR  kódovanie,  dobrá kvalita\n"\
"                 (150-180 kbps rozpätie bit. rýchlosti)\n"\
"                 standard:  VBR kódovanie, vysoká kvalita\n"\
"                 (170-210 kbps rozpätie bit. rýchlosti)\n"\
"                 extreme: VBR kódovanie, velmi vysoká kvalita\n"\
"                 (200-240 kbps rozpätie bit. rýchlosti)\n"\
"                 insane:  CBR  kódovanie, najvyššie nastavenie kvality\n"\
"                 (320 kbps bit. rýchlosť)\n"\
"                 <8-320>: ABR kódovanie na zadanej kbps bit. rýchlosti.\n\n"

//codec-cfg.c:
#define MSGTR_DuplicateFourcc "zdvojené FourCC"
#define MSGTR_TooManyFourccs "príliš vela FourCCs/formátov..."
#define MSGTR_ParseError "chyba spracovania (parse)"
#define MSGTR_ParseErrorFIDNotNumber "chyba spracovania (parse) (ID formátu nie je číslo?)"
#define MSGTR_ParseErrorFIDAliasNotNumber "chyba spracovania (parse) (alias ID formátu nie je číslo?)"
#define MSGTR_DuplicateFID "duplikátne format ID"
#define MSGTR_TooManyOut "príliš mnoho výstupu..."
#define MSGTR_InvalidCodecName "\nmeno kódeku(%s) nie je platné!\n"
#define MSGTR_CodecLacksFourcc "\nmeno kódeku(%s) nemá FourCC/formát!\n"
#define MSGTR_CodecLacksDriver "\nmeno kódeku(%s) nemá ovládač!\n"
#define MSGTR_CodecNeedsDLL "\nkódek(%s) vyžaduje 'dll'!\n"
#define MSGTR_CodecNeedsOutfmt "\nkódek(%s) vyžaduje 'outfmt'!\n"
#define MSGTR_CantAllocateComment "Nedá sa alokovať pamäť pre poznámku. "
#define MSGTR_GetTokenMaxNotLessThanMAX_NR_TOKEN "get_token(): max >= MAX_MR_TOKEN!"
#define MSGTR_CantGetMemoryForLine "Nejde získať pamäť pre 'line': %s\n"
#define MSGTR_CantReallocCodecsp "Nedá sa realokovať '*codecsp': %s\n"
#define MSGTR_CodecNameNotUnique " Meno kódeku '%s' nie je jedinečné."
#define MSGTR_CantStrdupName "Nedá sa spraviť strdup -> 'name': %s\n"
#define MSGTR_CantStrdupInfo "Nedá sa spraviť strdup -> 'info': %s\n"
#define MSGTR_CantStrdupDriver "Nedá sa spraviť strdup -> 'driver': %s\n"
#define MSGTR_CantStrdupDLL "Nedá sa spraviť strdup -> 'dll': %s"
#define MSGTR_AudioVideoCodecTotals "%d audio & %d video codecs\n"
#define MSGTR_CodecDefinitionIncorrect "Kódek nie je definovaný korektne."
#define MSGTR_OutdatedCodecsConf "Súbor codecs.conf je príliš starý a nekompatibilný s touto verziou MPlayer-u!"

// fifo.c

// m_config.c
#define MSGTR_SaveSlotTooOld "Príliš starý save slot nájdený z lvl %d: %d !!!\n"
#define MSGTR_InvalidCfgfileOption "Voľba %s sa nedá použiť v konfiguračnom súbore.\n"
#define MSGTR_InvalidCmdlineOption "Voľba %s sa nedá použiť z príkazového riadku.\n"
#define MSGTR_InvalidSuboption "Chyba: voľba '%s' nemá žiadnu podvoľbu '%s'.\n"
#define MSGTR_MissingSuboptionParameter "Chyba: podvoľba '%s' voľby '%s' musí mať parameter!\n"
#define MSGTR_MissingOptionParameter "Chyba: voľba '%s' musí mať parameter!\n"
#define MSGTR_OptionListHeader "\n Názov                Typ             Min        Max      Globál  CL    Konfig\n\n"
#define MSGTR_TotalOptions "\nCelkovo: %d volieb\n"
#define MSGTR_ProfileInclusionTooDeep "VAROVANIE: Príliš hlboké vnorovanie profilov.\n"
#define MSGTR_NoProfileDefined "Žiadny profil nebol definovaný.\n"
#define MSGTR_AvailableProfiles "Dostupné profily:\n"
#define MSGTR_UnknownProfile "Neznámy profil '%s'.\n"
#define MSGTR_Profile "Profil %s: %s\n"

// m_property.c
#define MSGTR_PropertyListHeader "\n Meno                 Typ             Min        Max\n\n"
#define MSGTR_TotalProperties "\nCelkovo: %d vlastností\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM zariadenie '%s' nenájdené!\n"
#define MSGTR_ErrTrackSelect "Chyba pri výbere VCD stopy!"
#define MSGTR_ReadSTDIN "Čítam z stdin...\n"
#define MSGTR_FileNotFound "Súbor nenájdený: '%s'\n"

#define MSGTR_SMBInitError "Nemôžem inicializovať knižnicu libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "Nemôžem otvoriť z LAN: '%s'\n"

#define MSGTR_CantOpenDVD "Nejde otvoriť DVD zariadenie: %s (%s)\n"
#define MSGTR_NoDVDSupport "MPlayer bol skompilovaný bez podpory DVD, koniec\n"
#define MSGTR_DVDnumTitles "Na tomto DVD je %d titulov.\n"
#define MSGTR_DVDinvalidTitle "Neplatné číslo DVD titulu: %d\n"
#define MSGTR_DVDinvalidChapterRange "Nesprávně nastavený rozsah kapitol %s\n"
#define MSGTR_DVDnumAngles "Na tomto DVD je %d uhlov pohľadov.\n"
#define MSGTR_DVDinvalidAngle "Neplatné číslo uhlu pohľadu DVD: %d\n"
#define MSGTR_DVDnoIFO "Nemôžem otvoriť súbor IFO pre DVD titul %d.\n"
#define MSGTR_DVDnoVMG "Nedá sa otvoriť VMG info!\n"
#define MSGTR_DVDnoVOBs "Nemôžem otvoriť VOB súbor (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDnoMatchingAudio "DVD zvuk v požadovanom jazyku nebyl nájdený!\n"
#define MSGTR_DVDaudioChannel "Zvolený DVD zvukový kanál: %d jazyk: %c%c\n"
#define MSGTR_DVDnoMatchingSubtitle "DVD titulky v požadovanom jazyku neboli nájdené!\n"
#define MSGTR_DVDsubtitleChannel "Zvolený DVD titulkový kanál: %d jazyk: %c%c\n"

// muxer.c, muxer_*.c:
#define MSGTR_TooManyStreams "Príliš veľa prúdov!"
#define MSGTR_RawMuxerOnlyOneStream "Rawaudio muxer podporuje iba jeden audio prúd!\n"
#define MSGTR_IgnoringVideoStream "Ignorujem video prúd!\n"
#define MSGTR_UnknownStreamType "Varovanie! neznámy typ prúdu: %d\n"
#define MSGTR_WarningLenIsntDivisible "Varovanie! dĺžka nie je deliteľná velkosťou vzorky!\n"
#define MSGTR_MuxbufMallocErr "Nedá sa alokovať pamäť pre frame buffer muxeru!\n"
#define MSGTR_MuxbufReallocErr "Nedá sa realokovať pamäť pre frame buffer muxeru!\n"
#define MSGTR_WritingHeader "Zapisujem header...\n"
#define MSGTR_WritingTrailer "Zapisujem index...\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "Upozornenie! Hlavička audio prúdu %d predefinovaná!\n"
#define MSGTR_VideoStreamRedefined "Upozornenie! Hlavička video prúdu %d predefinovaná!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: Príliš mnoho (%d v %d bajtoch) audio paketov v bufferi!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: Príliš mnoho (%d v %d bajtoch) video paketov v bufferi!\n"
#define MSGTR_MaybeNI "(možno prehrávate neprekladaný prúd/súbor alebo kodek zlyhal)\n" \
                      "Pre .AVI súbory skúste vynútiť neprekladaný mód voľbou -ni\n"
#define MSGTR_SwitchToNi "\nDetekovaný zle prekladaný .AVI - prepnite -ni mód!\n"
#define MSGTR_Detected_XXX_FileFormat "Detekovaný %s formát súboru!\n"
#define MSGTR_FormatNotRecognized "========== Žiaľ, tento formát súboru nie je rozpoznaný/podporovaný =======\n"\
                                  "==== Pokiaľ je tento súbor AVI, ASF alebo MPEG prúd, kontaktujte autora! ====\n"
#define MSGTR_MissingVideoStream "Žiadny video prúd nenájdený!\n"
#define MSGTR_MissingAudioStream "Žiadny audio prúd nenájdený...  -> bez zvuku\n"
#define MSGTR_MissingVideoStreamBug "Chýbajúci video prúd!? Kontaktujte autora, možno to je chyba (bug) :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: súbor neobsahuje vybraný audio alebo video prúd\n"

#define MSGTR_NI_Forced "Vnútený"
#define MSGTR_NI_Detected "Detekovaný"
#define MSGTR_NI_Message "%s NEPREKLADANÝ formát súboru AVI!\n"

#define MSGTR_UsingNINI "Používam NEPREKLADANÝ poškodený formát súboru AVI!\n"
#define MSGTR_CouldntDetFNo "Nemôžem určiť počet snímkov (pre absolútny posun)  \n"
#define MSGTR_CantSeekRawAVI "Nemôžem sa posúvať v surových (raw) .AVI prúdoch! (Potrebujem index, zkuste použíť voľbu -idx!)  \n"
#define MSGTR_CantSeekFile "Nemôžem sa posúvať v tomto súbore!  \n"

#define MSGTR_MOVcomprhdr "MOV: Komprimované hlavičky nie sú (ešte) podporované!\n"
#define MSGTR_MOVvariableFourCC "MOV: Upozornenie! premenná FOURCC detekovaná!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Upozornenie! Príliš veľa stôp!"
#define MSGTR_ErrorOpeningOGGDemuxer "Nemôžem otvoriť ogg demuxer\n"
#define MSGTR_CannotOpenAudioStream "Nemôžem otvoriť audio prúd: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Nemôžem otvoriť prúd titulkov: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Nemôžem otvoriť audio demuxer: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Nemôžem otvoriť demuxer titulkov: %s\n"
#define MSGTR_TVInputNotSeekable "v TV vstupe nie je možné sa pohybovať! (možno posun bude na zmenu kanálov ;)\n"
#define MSGTR_ClipInfo "Informácie o klipe: \n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: detekovaný 30000/1001 fps NTSC, prepínam frekvenciu snímkov.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: detekovaný 24000/1001 fps progresívny NTSC, prepínam frekvenciu snímkov.\n"

#define MSGTR_CacheFill "\rNaplnenie cache: %5.2f%% (%"PRId64" bajtov)   "
#define MSGTR_NoBindFound "Tlačidlo '%s' nemá priradenú žiadnu funkciu.\n"
#define MSGTR_FailedToOpen "Zlyhalo otvorenie %s\n"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "nemôžem otvoriť kodek\n"
#define MSGTR_CantCloseCodec "nemôžem uzavieť kodek\n"

#define MSGTR_MissingDLLcodec "CHYBA: Nemôžem otvoriť potrebný DirectShow kodek: %s\n"
#define MSGTR_ACMiniterror "Nemôžem načítať/inicializovať Win32/ACM AUDIO kodek (chýbajúci súbor DLL?)\n"
#define MSGTR_MissingLAVCcodec "Nemôžem najsť kodek '%s' v libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF - koniec súboru v priebehu vyhľadávania hlavičky sekvencie\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: Nemôžem prečítať hlavičku sekvencie!\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: Nemôžem prečítať rozšírenie hlavičky sekvencie!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Zlá hlavička sekvencie!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Zlé rozšírenie hlavičky sekvencie!\n"

#define MSGTR_ShMemAllocFail "Nemôžem alokovať zdieľanú pamäť\n"
#define MSGTR_CantAllocAudioBuf "Nemôžem alokovať pamäť pre výstupný audio buffer\n"

#define MSGTR_UnknownAudio "Neznámy/chýbajúci audio formát -> bez zvuku\n"

#define MSGTR_UsingExternalPP "[PP] Používam externý postprocessing filter, max q = %d\n"
#define MSGTR_UsingCodecPP "[PP] Požívam postprocessing z kodeku, max q = %d\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Požadovaná rodina video kodekov [%s] (vfm=%s) nie je dostupná (zapnite ju pri kompilácii!)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Požadovaná rodina audio kodekov [%s] (afm=%s) nie je dostupná (zapnite ju pri kompilácii!)\n"
#define MSGTR_OpeningVideoDecoder "Otváram video dekóder: [%s] %s\n"
#define MSGTR_SelectedVideoCodec "Zvolený video kódek: [%s] vfm: %s (%s)\n"
#define MSGTR_OpeningAudioDecoder "Otváram audio dekóder: [%s] %s\n"
#define MSGTR_SelectedAudioCodec "Zvolený audio kódek: [%s] afm: %s (%s)\n"
#define MSGTR_VDecoderInitFailed "VDecoder init zlyhal :(\n"
#define MSGTR_ADecoderInitFailed "ADecoder init zlyhal :(\n"
#define MSGTR_ADecoderPreinitFailed "ADecoder preinit zlyhal :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Zlyhal pokus o otvorenie podpory LIRC!\n"
#define MSGTR_LIRCcfgerr "Zlyhalo čítanie konfiguračného súboru LIRC %s!\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Nemôžem nájsť video filter '%s'\n"
#define MSGTR_CouldNotOpenVideoFilter "Nemôžem otvoriť video filter '%s'\n"
#define MSGTR_OpeningVideoFilter "Otváram video filter: "
#define MSGTR_CannotFindColorspace "Nemôžem nájsť bežný priestor farieb, ani vložením 'scale' :(\n"

// vd.c
#define MSGTR_CouldNotFindColorspace "Nemôžem nájsť zhodný priestor farieb - skúšam znova s -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Movie-Aspect je %.2f:1 - mením rozmery na správne.\n"
#define MSGTR_MovieAspectUndefined "Movie-Aspect je nedefinovný - nemenia sa rozmery.\n"

// vd_dshow.c, vd_dmo.c
#define MSGTR_DownloadCodecPackage "Potrebujete aktualizovať alebo nainštalovať binárne kódeky.\nChodte na http://www.mplayerhq.hu/dload.html\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: Nemôžem poslať udalosť EWMH fullscreen!\n"
#define MSGTR_SelectedVideoMode "XF86VM: Zvolený videorežim %dx%d pre obraz velkosti %dx%d.\n"

#define MSGTR_InsertingAfVolume "[Mixer] Hardvérový mixér nie je k dispozicí, vkladám filter pre hlasitosť.\n"
#define MSGTR_NoVolume "[Mixer] Ovládanie hlasitosti nie je dostupné.\n"

// ================================ GUI ================================

#define MSGTR_GUI_AboutMPlayer "O aplikácii MPlayer"
#define MSGTR_GUI_Add "Pridať"
#define MSGTR_GUI_AspectRatio "Pomer strán obrazu"
#define MSGTR_GUI_Audio "Audio"
#define MSGTR_GUI_AudioDelay "Audio oneskorenie"
#define MSGTR_GUI_AudioDriverConfiguration "Konfiguracia ovladača zvuku"
#define MSGTR_GUI_AudioTrack "Načítať externý audio súbor"
#define MSGTR_GUI_AudioTracks "Audio stopa"
#define MSGTR_GUI_AvailableDrivers "Dostupné ovládače:"
#define MSGTR_GUI_AvailableSkins "Témy"
#define MSGTR_GUI_Bass "Basový"
#define MSGTR_GUI_Blur "Rozmazanie"
#define MSGTR_GUI_Brightness "Jas"
#define MSGTR_GUI_Browse "Prehliadať"
#define MSGTR_GUI_Cache "Vyrovnávacia pamäť"
#define MSGTR_GUI_CacheSize "Veľkosť vyr. pamäte"
#define MSGTR_GUI_Cancel "Zrušiť"
#define MSGTR_GUI_Center "Stredný"
#define MSGTR_GUI_Channel1 "Kanál 1"
#define MSGTR_GUI_Channel2 "Kanál 2"
#define MSGTR_GUI_Channel3 "Kanál 3"
#define MSGTR_GUI_Channel4 "Kanál 4"
#define MSGTR_GUI_Channel5 "Kanál 5"
#define MSGTR_GUI_Channel6 "Kanál 6"
#define MSGTR_GUI_ChannelAll "Všetko"
#define MSGTR_GUI_ChapterN "Kapitola %d"
#define MSGTR_GUI_ChapterNN "Kapitola %2d"
#define MSGTR_GUI_Chapters "Kapitoly"
#define MSGTR_GUI_Clear "Vyčistiť"
#define MSGTR_GUI_CodecFamilyAudio "Rodina audeo kodekov"
#define MSGTR_GUI_CodecFamilyVideo "Rodina video kodekov"
#define MSGTR_GUI_CodecsAndLibraries "Kódeky a knižnice tretích strán"
#define MSGTR_GUI_Coefficient "Koeficient"
#define MSGTR_GUI_Configure "Konfigurácia"
#define MSGTR_GUI_ConfigureDriver "Konfigurovať ovládač"
#define MSGTR_GUI_Contrast "Kontrast"
#define MSGTR_GUI_Contributors "Přispievatelia kódu a dokumentacie"
#define MSGTR_GUI_Cp874 "Thai charset (CP874)"
#define MSGTR_GUI_Cp936 "Simplified Chinese charset (CP936)"
#define MSGTR_GUI_Cp949 "Korean charset (CP949)"
#define MSGTR_GUI_Cp1250 "Slavic/Central European Windows (CP1250)"
#define MSGTR_GUI_Cp1251 "Cyrillic Windows (CP1251)"
#define MSGTR_GUI_CpBIG5 "Traditional Chinese charset (BIG5)"
#define MSGTR_GUI_CpISO8859_1 "Western European Languages (ISO-8859-1)"
#define MSGTR_GUI_CpISO8859_2 "Slavic/Central European Languages (ISO-8859-2)"
#define MSGTR_GUI_CpISO8859_3 "Esperanto, Galician, Maltese, Turkish (ISO-8859-3)"
#define MSGTR_GUI_CpISO8859_4 "Old Baltic charset (ISO-8859-4)"
#define MSGTR_GUI_CpISO8859_5 "Cyrillic (ISO-8859-5)"
#define MSGTR_GUI_CpISO8859_6 "Arabic (ISO-8859-6)"
#define MSGTR_GUI_CpISO8859_7 "Modern Greek (ISO-8859-7)"
#define MSGTR_GUI_CpISO8859_8 "Hebrew charsets (ISO-8859-8)"
#define MSGTR_GUI_CpISO8859_9 "Turkish (ISO-8859-9)"
#define MSGTR_GUI_CpISO8859_13 "Baltic (ISO-8859-13)"
#define MSGTR_GUI_CpISO8859_14 "Celtic (ISO-8859-14)"
#define MSGTR_GUI_CpISO8859_15 "Western European Languages with Euro (ISO-8859-15)"
#define MSGTR_GUI_CpKOI8_R "Russian (KOI8-R)"
#define MSGTR_GUI_CpKOI8_U "Ukrainian, Belarusian (KOI8-U/RU)"
#define MSGTR_GUI_CpShiftJis "Japanese charsets (SHIFT-JIS)"
#define MSGTR_GUI_CpUnicode "Unicode"
#define MSGTR_GUI_DefaultSetting "východzie nastavenie"
#define MSGTR_GUI_Delay "Oneskorenie"
#define MSGTR_GUI_Demuxers_Codecs "Kódeky a demuxer"
#define MSGTR_GUI_Device "Zariadenie"
#define MSGTR_GUI_DeviceCDROM "CD-ROM zariadenie"
#define MSGTR_GUI_DeviceDVD "DVD zariadenie"
#define MSGTR_GUI_Directory "Cesta"
#define MSGTR_GUI_DirectoryTree "Adresárový strom"
#define MSGTR_GUI_DropSubtitle "Zahodiť titulky..."
#define MSGTR_GUI_DVD "DVD"
#define MSGTR_GUI_EnableAutomaticAVSync "Automatická synchronizácia zap./vyp."
#define MSGTR_GUI_EnableCache "Vyrovnávacia pamäť zap./vyp."
#define MSGTR_GUI_EnableDirectRendering "Zapnúť direct rendering"
#define MSGTR_GUI_EnableDoubleBuffering "Zapnúť dvojtý buffering"
#define MSGTR_GUI_EnableEqualizer "Zapnúť equalizer"
#define MSGTR_GUI_EnableExtraStereo "Zapnúť extra stereo"
#define MSGTR_GUI_EnableFrameDropping "Povoliť zahadzovanie rámcov"
#define MSGTR_GUI_EnableFrameDroppingIntense "Povoliť TVRDÉ zahadzovanie rámcov (nebezpečné)"
#define MSGTR_GUI_EnablePlaybar "Zapnúť playbar"
#define MSGTR_GUI_EnablePostProcessing "Zapnúť postprocess"
#define MSGTR_GUI_EnableSoftwareMixer "Aktivovať softvérový mixér"
#define MSGTR_GUI_Encoding "Kódovanie"
#define MSGTR_GUI_Equalizer "Equalizer"
#define MSGTR_GUI_EqualizerConfiguration "Konfigurovať Equalizer"
#define MSGTR_GUI_Error "Chyba!"
#define MSGTR_GUI_ErrorFatal "Fatálna chyba!"
#define MSGTR_GUI_File "Prehrať súbor"
#define MSGTR_GUI_Files "Súbory"
#define MSGTR_GUI_Flip "prehodiť obraz horná strana-dole"
#define MSGTR_GUI_Font "Font"
#define MSGTR_GUI_FrameRate "FPS"
#define MSGTR_GUI_FrontLeft "Predný Ľavý"
#define MSGTR_GUI_FrontRight "Predný Pravý"
#define MSGTR_GUI_HideVideoWindow "Ukázať video okno pri neaktivite"
#define MSGTR_GUI_Hue "Odtieň"
#define MSGTR_GUI_Lavc "Použiť LAVC (FFmpeg)"
#define MSGTR_GUI_MaximumUsageSpareCPU "Automatická qualita"
#define MSGTR_GUI_Miscellaneous "Rôzne"
#define MSGTR_GUI_Mixer "Mixér"
#define MSGTR_GUI_MixerChannel "Kanál mixéru"
#define MSGTR_GUI_MSG_DXR3NeedsLavc "Žiaľ, nemôžete prehrávať nie mpeg súbory s DXR3/H+ zariadením bez prekódovania.\nProsím zapnite lavc v DXR3/H+ konfig. okne."
#define MSGTR_GUI_MSG_MemoryErrorWindow "Žiaľ, nedostatok pamäte pre buffer na kreslenie."
#define MSGTR_GUI_MSG_NoFileLoaded "Nenahraný žiaden súbor"
#define MSGTR_GUI_MSG_NoMediaOpened "Nič nie je otvorené"
#define MSGTR_GUI_MSG_PlaybackNeedsRestart "Prosím pamätajte, nietoré voľby potrebujú reštart prehrávania!"
#define MSGTR_GUI_MSG_SkinBitmapConversionError "chyba konverzie z 24 bit do 32 bit (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapNotFound "súbor nenájdený (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapPngReadError "chyba čítania PNG (%s)\n"
#define MSGTR_GUI_MSG_SkinCfgNotFound "Téma nenájdená (%s).\n"
#define MSGTR_GUI_MSG_SkinCfgSelectedNotFound "Vybraná téma ( %s ) nenájdená, skúšam 'prednastavenú'...\n"
#define MSGTR_GUI_MSG_SkinErrorBitmap16Bit "bitmapa s hĺbkou 16 bit a menej je nepodporovaná (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorMessage "[témy] chyba v konfig. súbore tém %d: %s"
#define MSGTR_GUI_MSG_SkinFileNotFound "[skin] súbor ( %s ) nenájdený.\n"
#define MSGTR_GUI_MSG_SkinFileNotReadable "[skin] súbor ( %s ) sa nedá prečítať.\n"
#define MSGTR_GUI_MSG_SkinFontFileNotFound "súbor fontov nenájdený\n"
#define MSGTR_GUI_MSG_SkinFontImageNotFound "súbor obrazov fontu nenájdený\n"
#define MSGTR_GUI_MSG_SkinFontNotFound "neexistujúci identifikátor fontu (%s)\n"
#define MSGTR_GUI_MSG_SkinMemoryError "nedostatok pamäte\n"
#define MSGTR_GUI_MSG_SkinTooManyFonts "príliš mnoho fontov deklarovaných\n"
#define MSGTR_GUI_MSG_SkinUnknownMessage "neznáma správa: %s\n"
#define MSGTR_GUI_MSG_SkinUnknownParameter "neznámy parameter (%s)\n"
#define MSGTR_GUI_MSG_VideoOutError "Žiaľ, nemôžem nájsť gui kompatibilný ovládač video výstupu."
#define MSGTR_GUI_Mute "Stlmiť zvuk"
#define MSGTR_GUI_NetworkStreaming "Sieťové prehrávanie (streaming)..."
#define MSGTR_GUI_Next "Ďalší prúd"
#define MSGTR_GUI_NoChapter "Žiadna kapitola"
#define MSGTR_GUI__none_ "(nič)"
#define MSGTR_GUI_NonInterleavedParser "Použiť neprekladaný AVI parser"
#define MSGTR_GUI_NormalizeSound "Normalizovať zvuk"
#define MSGTR_GUI_Ok "Ok"
#define MSGTR_GUI_Open "Otvoriť..."
#define MSGTR_GUI_Original "Originál"
#define MSGTR_GUI_OsdLevel "OSD úroveň"
#define MSGTR_GUI_OSD_Subtitles "Titulky a OSD"
#define MSGTR_GUI_Outline "Obrys"
#define MSGTR_GUI_PanAndScan "Panscan"
#define MSGTR_GUI_Pause "Pauza"
#define MSGTR_GUI_Play "Prehrať"
#define MSGTR_GUI_Playback "Prehrávam"
#define MSGTR_GUI_Playlist "Playlist"
#define MSGTR_GUI_Position "Pozícia"
#define MSGTR_GUI_PostProcessing "Postprocess"
#define MSGTR_GUI_Preferences "Preferencie"
#define MSGTR_GUI_Previous "Predchádzajúci prúd"
#define MSGTR_GUI_Quit "Koniec"
#define MSGTR_GUI_RearLeft "Zadný Ľavý"
#define MSGTR_GUI_RearRight "Zadný Pravý"
#define MSGTR_GUI_Remove "Odobrať"
#define MSGTR_GUI_Saturation "Nasýtenie"
#define MSGTR_GUI_SaveWindowPositions "Uložiť pozíciu okna"
#define MSGTR_GUI_ScaleMovieDiagonal "Proporcionálne k diagonále obrazu"
#define MSGTR_GUI_ScaleMovieHeight "Proporcionálne k výške obrazu"
#define MSGTR_GUI_ScaleMovieWidth "Proporcionálne k šírke obrazu"
#define MSGTR_GUI_ScaleNo "Nemeniť rozmery"
#define MSGTR_GUI_SeekingInBrokenMedia "Obnoviť index tabulku, ak je potrebné"
#define MSGTR_GUI_SelectAudioFile "Vybrať externý audio kanál..."
#define MSGTR_GUI_SelectedFiles "Vybrané súbory"
#define MSGTR_GUI_SelectFile "Vybrať súbor..."
#define MSGTR_GUI_SelectFont "Vybrať font..."
#define MSGTR_GUI_SelectSubtitle "Vybrať titulky..."
#define MSGTR_GUI_SizeDouble "Dvojnásobná veľkosť"
#define MSGTR_GUI_SizeFullscreen "Celá obrazovka"
#define MSGTR_GUI_SizeHalf "Polovičná velikosť"
#define MSGTR_GUI_SizeNormal "Normálna veľkosť"
#define MSGTR_GUI_SizeOSD "OSD mierka"
#define MSGTR_GUI_SizeSubtitles "Mierka textu"
#define MSGTR_GUI_SkinBrowser "Prehliadač tém"
#define MSGTR_GUI_Skins "Témy"
#define MSGTR_GUI_Sponsored "vývoj GUI sponzoroval UHU Linux"
#define MSGTR_GUI_StartFullscreen "Naštartovať v režime celej obrazovky"
#define MSGTR_GUI_Stop "Zastaviť"
#define MSGTR_GUI_Subtitle "Titulky"
#define MSGTR_GUI_SubtitleAllowOverlap "Zapnúť prekrývanie titulkov"
#define MSGTR_GUI_SubtitleAutomaticLoad "Zakázať automatické nahrávanie titulkov"
#define MSGTR_GUI_SubtitleConvertMpsub "Konvertovať dané titulky do MPlayer formátu"
#define MSGTR_GUI_SubtitleConvertSrt "Konvertovať dané titulky do časovo-určeného SubViewer (SRT) formátu"
#define MSGTR_GUI_Subtitles "Titulky"
#define MSGTR_GUI_SyncValue "Automatická synchronizácia"
#define MSGTR_GUI_TitleNN "Titul %2d"
#define MSGTR_GUI_Titles "Tituly"
#define MSGTR_GUI_TrackN "Stopa %d"
#define MSGTR_GUI_Translations "Preklady"
#define MSGTR_GUI_TurnOffXScreenSaver "Zastaviť XScreenSaver"
#define MSGTR_GUI_URL "Prehrať URL"
#define MSGTR_GUI_VCD "VCD"
#define MSGTR_GUI_Video "Video"
#define MSGTR_GUI_VideoEncoder "Video kóder"
#define MSGTR_GUI_VideoTracks "Video stopa"
#define MSGTR_GUI_Warning "Upozornenie!"

// ========================== LIBMPCODECS ===================================

#define MSGTR_SamplesWanted "Potrebujeme vzorky tohto formátu, aby sme zlepšili podporu. Prosím kontaktujte vývojárov.\n"
