// Spanish translation by
//
// Diego Biurrun
// Reynaldo H. Verdejo Pinochet
// Juan A. Javierre <jjavierre at telefonica.net>
// Original work done by:
//
// Leandro Lucarella <leandro at lucarella.com.ar>,
// Jesús Climent <jesus.climent at hispalinux.es>,
// Sefanja Ruijsenaars <sefanja at gmx.net>,
// Andoni Zubimendi <andoni at lpsat.net>
//
// In sync with r34785
// FIXME: en revisión desde 11.04.2012


// ========================= MPlayer help ===========================

#ifdef CONFIG_VCD
#define MSGTR_HelpVCD " vcd://<numpista>    Reproducir pista de (S)VCD (Super Video CD) (acceso directo al dispositivo, no montado)\n"
#else
#define MSGTR_HelpVCD
#endif

#ifdef CONFIG_DVDREAD
#define MSGTR_HelpDVD " dvd://<numtítulo>   Reproducir título de DVD desde el dispositivo en vez de un fichero regular.\n"
#else
#define MSGTR_HelpDVD
#endif

#define MSGTR_Help \
"Uso:   mplayer [opciones] [[url|ruta/]nombre de fichero\n"\
"\n"\
"Opciones básicas: (En 'man mplayer' está la lista completa)\n"\
" -vo <drv>           Elegir controlador del dispositivo de salida de vídeo ('-vo help' para obtener una lista).\n"\
" -ao <drv>           Elegir controlador y dispositivo de salida de audio ('-ao help' para obtener una lista).\n"\
MSGTR_HelpVCD \
MSGTR_HelpDVD \
" -alang/-slang       Elegir idioma de audio/subtítulos del DVD (código de país de dos caracteres. p. ej. 'es')\n"\
" -ss <tiempo>        Saltar a una posición dada (en segundos u hh:mm:ss)\n"\
" -nosound            No reproducir sonido\n"\
" -fs                 Reproducir a pantalla completa (o -vm, -zoom, ver página man)\n"\
" -x <x> -y <y>       Establecer resolución de pantalla  (para usar con -vm o -zoom)\n"\
" -sub <fichero>      Indicar el fichero de subtítulos a usar (vea también -subfps, -subdelay)\n"\
" -playlist <fichero> Indicar el fichero de lista de reproducción\n"\
" -vid x -aid y       Elegir streams de vídeo (x) y audio (y) a reproducir\n"\
" -fps x -srate y     Cambia la tasa de refresco/muestreo de vídeo (x fps) y audio (y Hz)\n"\
" -pp <calidad>       Activa filtro de postproceso (más detalles en página man)\n"\
" -framedrop          Permite descartar cuadros (para máquinas lentas)\n"\
"\n"\
"Teclas básicas: ('man mplayer' da la lista completa, véase también input.conf)\n"\
" <-  o  ->           Avanza/retrocede 10 segundos\n"\
" arriba o abajo      Avanza/retrocede  1 minuto\n"\
" RePág o AvPág       Avanza/retrocede 10 minutos\n"\
" < o >               Avanza/retrocede en la lista de reproducción\n"\
" p o ESPACIO         Pausa (pulse cualquier tecla para reanudar)\n"\
" q o ESC             Detener la reproducción y salir del programa\n"\
" + o -               Ajusta retraso de audio +/- 0,1 segundos\n"\
" o                   Cicla modo OSD: nada / búsqueda / búsqueda + tiempo\n"\
" * o /               Aumenta o disminuye el volumen PCM\n"\
" z o x               Ajusta retraso del subtítulo +/- 0,1 segundo\n"\
" r o t               Ajusta posición del subtítulo arriba/abajo, véase también -vf expand\n"\
"\n"\
" * * * HAY MÁS DETALLES EN LA PÁGINA MAN, OPCIONES (AVANZADAS) Y TECLAS DE CONTROL * * *\n"\
"\n"

static const char help_text[] = MSGTR_Help;

// ========================= MPlayer messages ===========================

// mplayer.c
#define MSGTR_Exiting "\nSaliendo...\n"
#define MSGTR_ExitingHow "\nSaliendo... (%s)\n"
#define MSGTR_Exit_quit "Salida."
#define MSGTR_Exit_eof "Fin de archivo."
#define MSGTR_Exit_error "Error fatal."
#define MSGTR_IntBySignal "\nMPlayer interrumpido por señal %d en el módulo: %s\n"
#define MSGTR_NoHomeDir "No puedo encontrar el directorio HOME.\n"
#define MSGTR_GetpathProblem "Problema en get_path(\"config\")\n"
#define MSGTR_CreatingCfgFile "Creando archivo de configuración: %s\n"
#define MSGTR_CantLoadFont "No puedo cargar tipo bitmap '%s'.\n"
#define MSGTR_CantLoadSub "No puedo cargar los subtítulos '%s'.\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: No encuentro el stream elegido.\n"
#define MSGTR_CantOpenDumpfile "No puedo abrir el archivo de volcado.\n"
#define MSGTR_CoreDumped "Volcado de núcleo ;)\n"
#define MSGTR_DumpBytesWrittenPercent "dump: %"PRIu64" bytes escritos (~%.1f%%)\r"
#define MSGTR_DumpBytesWritten "dump: %"PRIu64" bytes escritos\r"
#define MSGTR_DumpBytesWrittenTo "dump: %"PRIu64" bytes escritos en '%s'.\n"
#define MSGTR_FPSnotspecified "No se indican las FPS en la cabecera (o no son válidas). Usa la opción -fps.\n"
#define MSGTR_TryForceAudioFmtStr "Tratando de forzar la familia de códecs de audio %s...\n"
#define MSGTR_CantFindAudioCodec "No encontré un códec para el formato de audio 0x%X.\n"
#define MSGTR_TryForceVideoFmtStr "Tratando de forzar la familia de códecs de video %s...\n"
#define MSGTR_CantFindVideoCodec "No encontré un códec que coincida con -vo y el formato de vídeo 0x%X elegido.\n"
#define MSGTR_CannotInitVO "FATAL: No puedo iniciar el controlador de vídeo.\n"
#define MSGTR_CannotInitAO "No puedo abrir/iniciar el dispositivo de audio -> no hay sonido.\n"
#define MSGTR_StartPlaying "Comenzando reproducción...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"       **************************************************************\n"\
"       **** El sistema es demasiado lento para reproducir esto  ****\n"\
"       **************************************************************\n"\
"Posibles razones, problemas, soluciones:\n"\
"- Más común: controlador de _audio_ con errores o roto.\n"\
"  - Intenta con -ao sdl o usa la emulación OSS de ALSA.\n"\
"  - Prueba con diferentes valores de -autosync, 30 es un buen comienzo.\n"\
"- Salida de vídeo lenta\n"\
"  - Prueba otro controlador -vo (con -vo help verás una lista) o intenta\n"\
"    arrancar con la opción -framedrop\n"\
"- CPU lenta\n"\
"  - No intentes reproducir DVD/DivX grandes en una CPU lenta. Intenta algo\n"\
"     como -vfm ffmpeg -lavdopts lowres=1:fast:skiploopfilter=all.\n"\
"- Archivo roto\n"\
"  - Prueba combinaciones de -nobps -ni -forceidx -mc 0.\n"\
"- Medios lentos (unidad NFS/SMB, DVD, VCD, etc)\n"\
"  - Intenta con -cache 8192.\n"\
"- ¿Estás usando -cache para reproducir archivos AVI no entrelazados?\n"\
"  - Intenta -nocache.\n"\
"En DOCS/HTML/es/video.html hay consejos de ajuste/mejora de la velocidad.\n"\
"Si nada de eso sirve de ayuda, lee DOCS/HTML/es/bugreports.html\n\n"

#define MSGTR_NoGui "MPlayer se compiló SIN SOPORTE de GUI (interfaz gráfica).\n"
#define MSGTR_GuiNeedsX "La interfaz gráfica de MPlayer necesita X11.\n"
#define MSGTR_Playing "\nReproduciendo %s.\n"
#define MSGTR_NoSound "Audio: sin sonido\n"
#define MSGTR_FPSforced "FPS forzado a %5.3f  (ftime: %5.3f).\n"
#define MSGTR_AvailableVideoOutputDrivers "Controladores de salida de vídeo disponibles:\n"
#define MSGTR_AvailableAudioOutputDrivers "Controladores de salida de audio disponibles:\n"
#define MSGTR_AvailableAudioCodecs "Códecs de audio disponibles:\n"
#define MSGTR_AvailableVideoCodecs "Códecs de vídeo disponibles:\n"
#define MSGTR_AvailableAudioFm "Familias/controladores de códecs de audio (compilados en MPlayer) disponibles:\n"
#define MSGTR_AvailableVideoFm "Familias/controladores de códecs de vídeo (compilados en MPlayer) disponibles:\n"
#define MSGTR_AvailableFsType "Modos disponibles de cambio a capa de pantalla completa:\n"
#define MSGTR_CannotReadVideoProperties "Vídeo: no puedo leer las propiedades.\n"
#define MSGTR_NoStreamFound "No he encontrado el stream.\n"
#define MSGTR_ErrorInitializingVODevice "Error al abrir/iniciar el dispositivo de salida de vídeo (-vo).\n"
#define MSGTR_ForcedVideoCodec "Códec de vídeo forzado: %s\n"
#define MSGTR_ForcedAudioCodec "Códec de audio forzado: %s\n"
#define MSGTR_Video_NoVideo "Vídeo: no hay vídeo\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: No he podido iniciar los filtros (-vf) o la salida de vídeo (-vo).\n"
#define MSGTR_Paused "  =====  PAUSA  =====" // no más de 23 caracteres (línea de estado en ficheros de audio)
#define MSGTR_PlaylistLoadUnable "\nNo he podido cargar la lista de reproducción %s.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer se ha colgado por una 'Instrucción Ilegal'.\n"\
"  Puede ser debido a un defecto de código en la nueva rutina de detección de CPU...\n"\
"  Por favor lee DOCS/HTML/es/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer se ha colgado por una 'Instrucción Ilegal'.\n"\
"  Normalmente, ocurre al ejecutar el programa en una CPU diferente de\n"\
"  la usada para compilarlo o para la cual se optimizó.\n"\
"  Sería bueno que lo comprobases...\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer se ha colgado por mal uso de CPU/FPU/RAM.\n"\
"  Recompila MPlayer con la opción --enable-debug y haz un backtrace y \n"\
"  desensamblado con'gdb'. En DOCS/HTML/es/bugreports_what.html#bugreports_crash\n"\
"  hay más detalles.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer se ha colgado. Esto no debería ocurrir.\n"\
"  Puede ser un defecto en el código de MPlayer, en tus controladores\n"\
"  _o_ en tu versión de gcc. Si crees que es culpa de MPlayer, por\n"\
"  favor lee DOCS/HTML/es/bugreports.html y sigue las instrucciones que allí\n"\
"  se encuentran. No podemos ayudarte a menos que nos facilites esa\n"\
"  información al comunicarnos un posible defecto.\n"
#define MSGTR_LoadingConfig "Cargando configuración '%s'\n"
#define MSGTR_LoadingProtocolProfile "Cargando el perfil relacionado con protocolo '%s'\n"
#define MSGTR_LoadingExtensionProfile "Cargando el perfil relacionado con extensión '%s'\n"
#define MSGTR_AddedSubtitleFile "SUB: añadí el fichero de subtítulos (%d): %s\n"
#define MSGTR_RemovedSubtitleFile "SUB: eliminé el fichero de subtítulos (%d): %s\n"
#define MSGTR_RTCDeviceNotOpenable "No pude abrir %s: %s (el usuario tiene que poder leerlo)\n"
#define MSGTR_LinuxRTCInitErrorIrqpSet "Error al iniciar Linux RTC en llamada a ioctl (rtc_irqp_set %lu): %s\n"
#define MSGTR_IncreaseRTCMaxUserFreq "Intenta añadir \"echo %lu > /proc/sys/dev/rtc/max-user-freq\" a los scripts de inicio del sistema.\n"
#define MSGTR_LinuxRTCInitErrorPieOn "Error al iniciar Linux RTC en llamada a ioctl (rtc_pie_on): %s\n"
#define MSGTR_Getch2InitializedTwice "ADVERTENCIA: se ha llamado a getch2_init dos veces\n"
#define MSGTR_CantOpenLibmenuFilterWithThisRootMenu "No puedo abrir el filtro de vídeo libmenu con el menú raíz %s.\n"
#define MSGTR_AudioFilterChainPreinitError "Error de pre-inicio en la cadena de filtros de audio\n"
#define MSGTR_LinuxRTCReadError "Error de lectura en Linux RTC: %s\n"
#define MSGTR_SoftsleepUnderflow "Advertencia: underflow de softsleep\n"
#define MSGTR_MasterQuit "Opción -udp-slave: saliendo porque el maestro salió\n"
#define MSGTR_InvalidIP "Opción -udp-ip: dirección IP no válida\n"
#define MSGTR_Forking "Bifurcando...\n"
#define MSGTR_Forked "Bifurcado...\n"
#define MSGTR_CouldntStartGdb "No pude iniciar gdb\n"
#define MSGTR_CouldntFork "No pude bifurcar\n"
#define MSGTR_FilenameTooLong "Nombre de fichero muy largo, no puedo cargar ficheros de configuración específicos de archivo o directorio\n"
#define MSGTR_AudioDeviceStuck "El dispositivo de audio se ha encallado\n"
#define MSGTR_AudioOutputTruncated "Salida de audio truncada al final.\n"
#define MSGTR_ASSCannotAddVideoFilter "ASS: no puedo añadir filtro de vídeo\n"
#define MSGTR_PtsAfterFiltersMissing "PERDIDO pts tras filtros\n"
#define MSGTR_CommandLine "Línea de comando:"
#define MSGTR_MenuInitFailed "Fallo al iniciar el menú.\n"

// --- edit decision lists
#define MSGTR_EdlOutOfMem "No puedo asignar suficiente memoria para almacenar datos EDL.\n"
#define MSGTR_EdlOutOfMemFile "No puedo asignar suficiente memoria para almacenar nombres de fichero EDL [%s].\n"
#define MSGTR_EdlRecordsNo "Leídas %d acciones EDL.\n"
#define MSGTR_EdlQueueEmpty "No hay acciones EDL de las que ocuparse.\n"
#define MSGTR_EdlCantOpenForWrite "No puedo abrir el fichero EDL [%s] para escribir.\n"
#define MSGTR_EdlNOsh_video "No puedo usar EDL sin video, desactivándolo.\n"
#define MSGTR_EdlNOValidLine "La linea EDL %s no es válida\n"
#define MSGTR_EdlBadlyFormattedLine "La linea EDL [%d] está mal formateada, la descarto.\n"
#define MSGTR_EdlBadLineOverlap "La última posición de paro fue [%f]; el próximo incicio "\
"es [%f]. Las posiciones deben estar en orden cronológico y sin solaparse. Las descarto.\n"
#define MSGTR_EdlBadLineBadStop "La hora de parada debe ser posterior a la de inicio.\n"
#define MSGTR_EdloutBadStop "Salto de EDL cancelado, último comienzo > parada\n"
#define MSGTR_EdloutStartSkip "Salto de EDL iniciado, pulse 'i' otra vez para terminar el bloque.\n"
#define MSGTR_EdloutEndSkip "Fin de salto EDL, se ha escrito la línea.\n"

// mplayer.c OSD
#define MSGTR_OSDenabled "activado"
#define MSGTR_OSDdisabled "desactivado"
#define MSGTR_OSDAudio "Audio: %s"
#define MSGTR_OSDChannel "Canal: %s"
#define MSGTR_OSDSubDelay "Retraso sub: %d ms"
#define MSGTR_OSDSpeed "Velocidad: x %6.2f"
#define MSGTR_OSDosd "OSD: %s"
#define MSGTR_OSDChapter "Capítulo: (%d) %s"
#define MSGTR_OSDAngle "Ángulo: %d/%d"
#define MSGTR_OSDDeinterlace "Desentrelazado: %s"
#define MSGTR_OSDCapturing "Capturando: %s"
#define MSGTR_OSDCapturingFailure "Fallo de captura"

// property values
#define MSGTR_Enabled "activado"
#define MSGTR_EnabledEdl "activado (EDL)"
#define MSGTR_Disabled "desactivado"
#define MSGTR_HardFrameDrop "hard"
#define MSGTR_Unknown "desconocido"
#define MSGTR_Bottom "abajo"
#define MSGTR_Center "centro"
#define MSGTR_Top "arriba"
#define MSGTR_SubSourceFile "fichero"
#define MSGTR_SubSourceVobsub "vobsub"
#define MSGTR_SubSourceDemux "incrustado"

// OSD bar names
#define MSGTR_Volume "Volumen"
#define MSGTR_Panscan "Panscan"
#define MSGTR_Gamma "Gamma"
#define MSGTR_Brightness "Brillo"
#define MSGTR_Contrast "Contraste"
#define MSGTR_Saturation "Saturación"
#define MSGTR_Hue "Tono"
#define MSGTR_Balance "Balance"

// property state
#define MSGTR_LoopStatus "Bucle: %s"
#define MSGTR_MuteStatus "Silenciar: %s"
#define MSGTR_AVDelayStatus "Retraso A-V: %s"
#define MSGTR_OnTopStatus "Por encima: %s"
#define MSGTR_RootwinStatus "Rootwin: %s"
#define MSGTR_BorderStatus "Borde: %s"
#define MSGTR_FramedroppingStatus "Framedropping: %s"
#define MSGTR_VSyncStatus "VSync: %s"
#define MSGTR_SubSelectStatus "Subtítulos: %s"
#define MSGTR_SubSourceStatus "Fuente de subs: %s"
#define MSGTR_SubPosStatus "Posición de subs: %s/100"
#define MSGTR_SubAlignStatus "Alineación de subs: %s"
#define MSGTR_SubDelayStatus "Retraso de subs: %s"
#define MSGTR_SubScale "Escalado de subs: %s"
#define MSGTR_SubVisibleStatus "Subtítulos: %s"
#define MSGTR_SubForcedOnlyStatus "Sólo subs forzados: %s"

// mencoder.c
#define MSGTR_UsingPass3ControlFile "Usando fichero de control pass3: %s\n"
#define MSGTR_MissingFilename "\nFichero sin nombre.\n\n"
#define MSGTR_CannotOpenFile_Device "No pude abrir el fichero/dispositivo.\n"
#define MSGTR_CannotOpenDemuxer "No pude abrir el demuxer.\n"
#define MSGTR_NoAudioEncoderSelected "\nNo has elegido un codificador de audio (-oac). Escoge uno (busca -oac en la ayuda) o usa -nosound.\n"
#define MSGTR_NoVideoEncoderSelected "\nNo has elegido un codificador de vídeo (-ovc). Escoge uno (busca -ovc en la ayuda).\n"
#define MSGTR_CannotOpenOutputFile "No puedo abrir el fichero de salida '%s'.\n"
#define MSGTR_EncoderOpenFailed "No puedo abrir el codificador.\n"
#define MSGTR_MencoderWrongFormatAVI "\nAVISO: EL FORMATO DEL FICHERO DE SALIDA ES _AVI_. Busca -of en la ayuda.\n"
#define MSGTR_MencoderWrongFormatMPG "\nAVISO: EL FORMATO DEL FICHERO DE SALIDA ES _MPEG_. Busca -of en la ayuda.\n"
#define MSGTR_MissingOutputFilename "No has indicado un fichero de salida. Busca la opción-o, por favor."
#define MSGTR_ForcingOutputFourcc "Forzando salida fourcc a %x [%.4s].\n"
#define MSGTR_ForcingOutputAudiofmtTag "Forzando etiqueta del formato de audio de la salidaa 0x%x.\n"
#define MSGTR_DuplicateFrames "\n%d cuadro(s) duplicados.\n"
#define MSGTR_SkipFrame "\nSaltando cuadro\n"
#define MSGTR_ResolutionDoesntMatch "\nEl nuevo fichero de vídeo tiene resolución o espacio de color distintos que el anterior.\n"
#define MSGTR_FrameCopyFileMismatch "\nTodos los ficheros de vídeo deben tener fps, resolución y códec identicos para la copia -ovc.\n"
#define MSGTR_AudioCopyFileMismatch "\nTodos los ficheros deben tener los mismos códecs de audio e igual formato para la copia -oac.\n"
#define MSGTR_NoAudioFileMismatch "\nNo se pueden mezclar ficheros de vídeo sólo con ficheros con video y audio. Prueba -nosound.\n"
#define MSGTR_NoSpeedWithFrameCopy "ADVERTENCIA: No puedo garantizar que -speed funcione bien con la copia -oac\n"\
"La codificación puede salir mal\n"
#define MSGTR_ErrorWritingFile "%s: error al escribir el fichero.\n"
#define MSGTR_FlushingVideoFrames "\nLimpiando cuadros de vídeo.\n"
#define MSGTR_FiltersHaveNotBeenConfiguredEmptyFile "No se han configurado los filtros. ¿Fichero vacío?\n"
#define MSGTR_RecommendedVideoBitrate "La tasa de bits recomendada para %s CD: %d\n"
#define MSGTR_VideoStreamResult "\nStream de video: %8.3f kbit/s (%d B/s), tamaño: %"PRIu64" bytes, %5.3f segundos, %d cuadros\n"
#define MSGTR_AudioStreamResult "\nStream de audio: %8.3f kbit/s (%d B/s), tamaño: %"PRIu64" bytes, %5.3f segundos\n"
#define MSGTR_EdlSkipStartEndCurrent "SALTO EDL: Inicio: %.2f  Final: %.2f   Actual: V: %.2f  A: %.2f     \r"
#define MSGTR_OpenedStream "éxito: formato: %d  datos: 0x%X - 0x%x\n"
#define MSGTR_VCodecFramecopy "Códec de vídeo: framecopy (%dx%d %dbpp fourcc=%x)\n"
#define MSGTR_ACodecFramecopy "Códec de audio: framecopy (formato=%x canales=%d tasa=%d bits=%d B/s=%d muestra-%d)\n"
#define MSGTR_MP3AudioSelected "Elegido audio MP3.\n"
#define MSGTR_SettingAudioDelay "Ajustando retraso de audio a %5.3fs.\n"
#define MSGTR_SettingVideoDelay "Ajustando retraso de vídeo a %5.3fs.\n"
#define MSGTR_LimitingAudioPreload "Limitando la precarga de audio a 0.4s.\n"
#define MSGTR_IncreasingAudioDensity "Aumentando la densidad de audio a 4.\n"
#define MSGTR_ZeroingAudioPreloadAndMaxPtsCorrection "Forzando la precarga de audio a 0, corrección pts máx a 0.\n"
#define MSGTR_LameVersion "Versión de LAME %s (%s)\n\n"
#define MSGTR_InvalidBitrateForLamePreset "Error: la tasa de bits indicada está fuera del rango válido para esta preconfiguración.\n"\
"\n"\
"Cuando uses este modo debes escribir un valor entre \"8\" y \"320\".\n"\
"\n"\
"Para mayor información prueba: \"-lameopts preset=help\"\n"
#define MSGTR_InvalidLamePresetOptions "Error: No indicaste un perfil válido y/u opciones con la preconfiguración.\n"\
"\n"\
"Los perfiles disponibles son:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (Modo ABR) - Asume el modo ABR. Para usarlo,\n"\
"                      indica sólo la tasa de bits. Por ejemplo:\n"\
"                      \"preset=185\" activa esta\n"\
"                      preconfiguración y usa 185 como kbps promedio.\n"\
"\n"\
"    Algunos ejemplos:\n"\
"\n"\
"    \"-lameopts fast:preset=standard  \"\n"\
" o  \"-lameopts  cbr:preset=192       \"\n"\
" o  \"-lameopts      preset=172       \"\n"\
" o  \"-lameopts      preset=extreme   \"\n"\
"\n"\
"Para mayor información prueba: \"-lameopts preset=help\"\n"
#define MSGTR_LamePresetsLongInfo "\n"\
"Las opciones de preconfiguración se han diseñado para dar la mayor calidad posible.\n"\
"\n"\
"En su mayoría han sido ensayadas y ajustadas por medio de rigurosas pruebas de\n"\
"doble escucha ciega (double blind listening) para verificarlas y lograr este objetivo.\n"\
"\n"\
"Se actualizan continuamente con los últimos desarrollos y, en consecuencia, \n"\
"deberían dar prácticamente la mejor calidad posible hoy día con LAME.\n"\
"\n"\
"Para activar estas preconfiguraciones:\n"\
"\n"\
"   Para los modos VBR (generalmente los de mayor calidad):\n"\
"\n"\
"     \"preset=standard\" Esta preconfiguración generalmente debería ser transparente\n"\
"                             para casi todo el mundo, en casi toda la música y ya tiene\n"\
"                             una calidad bastante buena.\n"\
"\n"\
"     \"preset=extreme\"  Si tienes un oido extremademente bueno y un equipo\n"\
"                             tan bueno como tu oído, esta preconfiguración te dará\n"\
"                             una calidad levemente superior al modo \"standard\"\n"\
"                             la mayoría de veces.\n"\
"\n"\
"   Para 320kbps CBR (la mejor calidad posible desde las preconfiguraciones):\n"\
"\n"\
"     \"preset=insane\"   Esta preconfiguración será, casi siempre, excesiva \n"\
"                             para la mayoría de la gente, pero si debes tener\n"\
"                             absolutamente la mayor calidad posible, sin importar el\n"\
"                             tamaño del fichero, ésta es tu opción.\n"\
"\n"\
"   Para los modos ABR (alta calidad para la tasa de bits dada pero no tanta como el modo VBR):\n"\
"\n"\
"  \"preset=<kbps>\"      Usando esta preconfiguración normalmente tendrás una\n"\
"                             buena calidad a la tasa de bits que indiques.\n"\
"                             Dependiendo de ésta, la preconfiguración determinará\n"\
"                             el ajuste óptimo para esa situación particular.\n"\
"                             Aunque funciona, esta configuración no es tan flexible\n"\
"                             como VBR y, normalmente, no alcanza el mismo nivel de calidad \n"\
"                             que VBR a mayores tasas de bits.\n"\
"\n"\
"Cada perfil tiene también disponibles las opciones siguentes:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (Modo ABR) - Asume el modo ABR. Para usarlo,\n"\
"                      sólo indica una tasa de bits. Por ejemplo:\n"\
"                      \"preset=185\" activa esta preconfiguración y usa \n"\
"                      185 como kbps promedio.\n"\
"\n"\
"   \"fast\" - Activa el nuevo modo rápido VBR para un perfil en particular. La\n"\
"            desventaja frente a la preconfiguración rápida es que, a menudo, la \n"\
"            tasa de bits es ligeramente más alta que en el modo normal y la calidad\n"\
"            puede llegar a ser un poco más baja también.\n"\
"Advertencia: con la versión actual las preconfiguraciones rápidas pueden provocar\n"\
"             tasas de bits demasiado altas comparadas con las normales.\n"\
"\n"\
"   \"cbr\"  - Si usas el modo ABR (ver más arriba) con una tasa de bits significativa\n"\
"            como 80, 96, 112, 128, 160, 192, 224, 256, 320,\n"\
"            puedes usar la opción \"cbr\" para forzar la codificación en modo CBR\n"\
"            en lugar del modo por omisión abr. ABR proporciona mayor calidad pero\n"\
"            CBR podría ser útil en ciertas situaciones, por ejemplo cuando puede\n"\
"            ser importante difundir un MP3 a través de internet.\n"\
"\n"\
"    Por ejemplo:\n"\
"\n"\
"    \"-lameopts fast:preset=standard  \"\n"\
" o  \"-lameopts  cbr:preset=192       \"\n"\
" o  \"-lameopts      preset=172       \"\n"\
" o  \"-lameopts      preset=extreme   \"\n"\
"\n"\
"\n"\
"Hay disponibles unos cuantos alias para el modo ABR:\n"\
"teléf => 16kbps/mono        teléfono/lw/mw-eu/sw => 24kbps/mono\n"\
"om => 40kbps/mono           voz => 56kbps/mono\n"\
"fm/radio/cinta => 112kbps   hifi => 160kbps\n"\
"cd => 192kbps               estudio => 256kbps"
#define MSGTR_LameCantInit "No pude establecer las opciones de LAME, corrige la tasa"\
" de bits o de muestreo. Algunas tasas de bit muy bajas (<32) necesitan de una tasa"\
" muestreo más baja (ej. -srate 8000). Si todo falla, prueba con una preconfiguración."
#define MSGTR_ConfigFileError "error en fichero de configuración"
#define MSGTR_ErrorParsingCommandLine "error en parámetros de la línea de comando"
#define MSGTR_VideoStreamRequired "El stream de vídeo es obligatorio\n"
#define MSGTR_ForcingInputFPS "Las fps de entrada se interpretarán como %5.3f.\n"
#define MSGTR_DemuxerDoesntSupportNosound "Este demuxer aún no admite -nosound.\n"
#define MSGTR_MemAllocFailed "La asignación de memoria ha fallado.\n"
#define MSGTR_NoMatchingFilter "No encontré un filtro o formato de salida coincidente\n"
#define MSGTR_MP3WaveFormatSizeNot30 "sizeof(MPEGLAYER3WAVEFORMAT)==%d!=30, ¿quizás esté roto el compilador de C?\n"
#define MSGTR_NoLavcAudioCodecName "LAVC Audio, falta el nombre del códec\n"
#define MSGTR_LavcAudioCodecNotFound "LAVC Audio, no encuentro el codificador para el códec %s.\n"
#define MSGTR_CouldntAllocateLavcContext "LAVC Audio, no puedo asignar contexto\n"
#define MSGTR_CouldntOpenCodec "No puedo abrir el códec %s, br=%d.\n"
#define MSGTR_CantCopyAudioFormat "El formato de audio 0x%x no es compatible con '-oac copy', por favor intenta con '-oac pcm' o usa '-fafmttag' para anularlo.\n"

// cfg-mencoder.h
#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     método de tasa de bits variable\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(default)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           tasa de bits media\n"\
"\n"\
" cbr           tasa de bits constante\n"\
"               Forzar también modo de codificación CBR en modos ABR \n"\
"               preseleccionados subsecuentemente.\n"\
"\n"\
" br=<0-1024>   especifica tasa de bits en kBit (solo CBR y ABR)\n"\
"\n"\
" q=<0-9>       calidad (0-mejor, 9-peor) (solo para VBR)\n"\
"\n"\
" aq=<0-9>      calidad del algoritmo (0-mejor/lenta, 9-peor/rápida)\n"\
"\n"\
" ratio=<1-100> razón de compresión\n"\
"\n"\
" vol=<0-10>    configura ganancia de entrada de audio\n"\
"\n"\
" mode=<0-3>    (por defecto: auto)\n"\
"                0: estéreo\n"\
"                1: estéreo-junto\n"\
"                2: canal dual\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: no\n"\
"                1: todo\n"\
"                2: ajustar\n"\
"\n"\
" fast          Activa codificación rápida en modos VBR preseleccionados\n"\
"               subsecuentes, más baja calidad y tasas de bits más altas.\n"\
"\n"\
" preset=<value> Provee configuración con la mejor calidad posible.\n"\
"                 medium: codificación VBR, buena calidad\n"\
"                 (rango de 150-180 kbps de tasa de bits)\n"\
"                 standard:  codificación VBR, alta calidad\n"\
"                 (rango de 170-210 kbps de tasa de bits)\n"\
"                 extreme: codificación VBR, muy alta calidad\n"\
"                 (rango de 200-240 kbps de tasa de bits)\n"\
"                 insane:  codificación CBR, la mejor calidad configurable\n"\
"                 (320 kbps de tasa de bits)\n"\
"                 <8-320>: codificación ABR con tasa de bits en promedio en los kbps dados.\n\n"

// codec-cfg.c
#define MSGTR_DuplicateFourcc "FourCC duplicado"
#define MSGTR_TooManyFourccs "demasiados FourCCs/formatos..."
#define MSGTR_ParseError "error en el analísis"
#define MSGTR_ParseErrorFIDNotNumber "error en el analísis (¿ID de formato no es un número?)"
#define MSGTR_ParseErrorFIDAliasNotNumber "error en el analísis (¿el alias del ID de formato no es un número?)"
#define MSGTR_DuplicateFID "ID de formato duplicado"
#define MSGTR_TooManyOut "demasiados out..."
#define MSGTR_InvalidCodecName "\n¡el nombre del codec(%s) no es valido!\n"
#define MSGTR_CodecLacksFourcc "\n¡el codec(%s) no tiene FourCC/formato!\n"
#define MSGTR_CodecLacksDriver "\ncodec(%s) does not have a driver!\n"
#define MSGTR_CodecNeedsDLL "\n¡codec(%s) necesita una 'dll'!\n"
#define MSGTR_CodecNeedsOutfmt "\n¡codec(%s) necesita un 'outfmt'!\n"
#define MSGTR_CantAllocateComment "No puedo asignar memoria para el comentario. "
#define MSGTR_GetTokenMaxNotLessThanMAX_NR_TOKEN "get_token(): max >= MAX_MR_TOKEN!"
#define MSGTR_CantGetMemoryForLine "No puedo asignar memoria para 'line': %s\n"
#define MSGTR_CantReallocCodecsp "No puedo reasignar '*codecsp': %s\n"
#define MSGTR_CodecNameNotUnique "El nombre del Codec '%s' no es único."
#define MSGTR_CantStrdupName "No puedo strdup -> 'name': %s\n"
#define MSGTR_CantStrdupInfo "No puedo strdup -> 'info': %s\n"
#define MSGTR_CantStrdupDriver "No puedo strdup -> 'driver': %s\n"
#define MSGTR_CantStrdupDLL "No puedo strdup -> 'dll': %s"
#define MSGTR_AudioVideoCodecTotals "%d codecs de audio & %d codecs de video\n"
#define MSGTR_CodecDefinitionIncorrect "Codec no esta definido correctamente."
#define MSGTR_OutdatedCodecsConf "¡El archivo codecs.conf es demasiado viejo y es incompatible con esta versión de MPlayer!"

// fifo.c

// parser-mecmd.c, parser-mpcmd.c
#define MSGTR_NoFileGivenOnCommandLine "'--' indica que no hay más opciones, pero no se ha especificado el nombre de ningún fichero en la línea de comandos\n"
#define MSGTR_TheLoopOptionMustBeAnInteger "La opción del bucle debe ser un entero: %s\n"
#define MSGTR_UnknownOptionOnCommandLine "Opción desconocida en la línea de comandos: -%s\n"
#define MSGTR_ErrorParsingOptionOnCommandLine "Error analizando la opción en la línea de comandos: -%s\n"
#define MSGTR_InvalidPlayEntry "Entrada de reproducción inválida %s\n"
#define MSGTR_NotAnMEncoderOption "-%s no es una opción de MEncoder\n"
#define MSGTR_NoFileGiven "No se ha especificado ningún fichero"

// m_config.c
#define MSGTR_SaveSlotTooOld "Encontrada casilla demasiado vieja del lvl %d: %d !!!\n"
#define MSGTR_InvalidCfgfileOption "La opción %s no puede ser usada en un archivo de configuración.\n"
#define MSGTR_InvalidCmdlineOption "La opción %s no puede ser usada desde la línea de comandos.\n"
#define MSGTR_InvalidSuboption "Error: opción '%s' no tiene la subopción '%s'.\n"
#define MSGTR_MissingSuboptionParameter "Error: ¡subopción '%s' de '%s' tiene que tener un parámetro!\n"
#define MSGTR_MissingOptionParameter "Error: ¡opción '%s' debe tener un parámetro!\n"
#define MSGTR_OptionListHeader "\n Nombre               Tipo            Min        Max      Global  LC    Cfg\n\n"
#define MSGTR_TotalOptions "\nTotal: %d opciones\n"
#define MSGTR_ProfileInclusionTooDeep "ADVERTENCIA: Inclusion de perfil demaciado profunda.\n"
#define MSGTR_NoProfileDefined "No se han definido perfiles.\n"
#define MSGTR_AvailableProfiles "Perfiles disponibles:\n"
#define MSGTR_UnknownProfile "Perfil desconocido '%s'.\n"
#define MSGTR_Profile "Perfil %s: %s\n"

// m_property.c
#define MSGTR_PropertyListHeader "\n Nombre                 Tipo            Min      Max\n\n"
#define MSGTR_TotalProperties "\nTotal: %d propiedades\n"

// loader/ldt_keeper.c
#define MSGTR_LOADER_DYLD_Warning "AVISO: Se está intentando usar los codecs DLL pero la variable de entorno\n         DYLD_BIND_AT_LAUNCH no está establecida. Probablemente falle.\n"

// ================================ GUI ================================

#define MSGTR_GUI_AboutMPlayer "Acerca de MPlayer"
#define MSGTR_GUI_Add "Agregar"
#define MSGTR_GUI_AspectRatio "Relación de aspecto"
#define MSGTR_GUI_Audio "Audio"
#define MSGTR_GUI_AudioDelay "Retraso de audio"
#define MSGTR_GUI_AudioDriverConfiguration "Configuración de controlador de Audio"
#define MSGTR_GUI_AudioTrack "Cargar archivo de audio externo"
#define MSGTR_GUI_AudioTracks "Pista de Audio"
#define MSGTR_GUI_AvailableDrivers "Controladores disponibles:"
#define MSGTR_GUI_AvailableSkins "Skins"
#define MSGTR_GUI_Bass "Bajo"
#define MSGTR_GUI_Blur "Blur"
#define MSGTR_GUI_Bottom "Inferior"
#define MSGTR_GUI_Brightness "Brillo"
#define MSGTR_GUI_Browse "Navegar"
#define MSGTR_GUI_Cache "Cache"
#define MSGTR_GUI_CacheSize "Tamaño de Cache"
#define MSGTR_GUI_Cancel "Cancelar"
#define MSGTR_GUI_Center "Centro"
#define MSGTR_GUI_Channel1 "Canal 1"
#define MSGTR_GUI_Channel2 "Canal 2"
#define MSGTR_GUI_Channel3 "Canal 3"
#define MSGTR_GUI_Channel4 "Canal 4"
#define MSGTR_GUI_Channel5 "Canal 5"
#define MSGTR_GUI_Channel6 "Canal 6"
#define MSGTR_GUI_ChannelAll "Todos"
#define MSGTR_GUI_ChapterN "capítulo %d"
#define MSGTR_GUI_ChapterNN "Capítulo %2d"
#define MSGTR_GUI_Chapters "Capítulos"
#define MSGTR_GUI_Clear "Limpiar"
#define MSGTR_GUI_CodecFamilyAudio "Familia de codec de audio"
#define MSGTR_GUI_CodecFamilyVideo "Familia de codec de video"
#define MSGTR_GUI_CodecsAndLibraries "Codecs y librerías de terceros"
#define MSGTR_GUI_Coefficient "Coeficiente"
#define MSGTR_GUI_Configure "Configurar"
#define MSGTR_GUI_ConfigureDriver "Configurar driver"
#define MSGTR_GUI_Contrast "Contraste"
#define MSGTR_GUI_Contributors "Contribuyentes al código y documentación"
#define MSGTR_GUI_Cp874 "Thai (CP874)"
#define MSGTR_GUI_Cp936 "Chino simplificado (CP936)"
#define MSGTR_GUI_Cp949 "Coreano (CP949)"
#define MSGTR_GUI_Cp1250 "Eslavo/Centroeuropeo (Windows) (CP1250)"
#define MSGTR_GUI_Cp1251 "Cirílico (Windows) (CP1251)"
#define MSGTR_GUI_Cp1256 "Arabic Windows (CP1256)"
#define MSGTR_GUI_CpBIG5 "Chino tradicional (BIG5)"
#define MSGTR_GUI_CpISO8859_1 "Occidental (ISO-8859-1)"
#define MSGTR_GUI_CpISO8859_2 "Eslavo/Centroeuropeo (ISO-8859-2)"
#define MSGTR_GUI_CpISO8859_3 "Esperanto, Gallego, Maltés, Turco (ISO-8859-3)"
#define MSGTR_GUI_CpISO8859_4 "Báltico (ISO-8859-4)"
#define MSGTR_GUI_CpISO8859_5 "Cirílico (ISO-8859-5)"
#define MSGTR_GUI_CpISO8859_6 "Árabe (ISO-8859-6)"
#define MSGTR_GUI_CpISO8859_7 "Griego moderno (ISO-8859-7)"
#define MSGTR_GUI_CpISO8859_8 "Hebreo (ISO-8859-8)"
#define MSGTR_GUI_CpISO8859_9 "Turco (ISO-8859-9)"
#define MSGTR_GUI_CpISO8859_13 "Báltico (ISO-8859-13)"
#define MSGTR_GUI_CpISO8859_14 "Céltico (ISO-8859-14)"
#define MSGTR_GUI_CpISO8859_15 "Occidental con euro (ISO-8859-15)"
#define MSGTR_GUI_CpKOI8_R "Ruso (KOI8-R)"
#define MSGTR_GUI_CpKOI8_U "Belaruso (KOI8-U/RU)"
#define MSGTR_GUI_CpShiftJis "Japanés(SHIFT-JIS)"
#define MSGTR_GUI_CpUnicode "Unicode"
#define MSGTR_GUI_DefaultSetting "controlador por omisión"
#define MSGTR_GUI_Delay "Retraso"
#define MSGTR_GUI_Demuxers_Codecs "Codecs y demuxer"
#define MSGTR_GUI_Device "Dispositivo"
#define MSGTR_GUI_DeviceCDROM "Dispositivo de CD-ROM"
#define MSGTR_GUI_DeviceDVD "Dispositivo de DVD"
#define MSGTR_GUI_Directory "Ubicación"
#define MSGTR_GUI_DirectoryTree "Árbol de directorios"
#define MSGTR_GUI_DropSubtitle "Cancelar subtitulos..."
#define MSGTR_GUI_DVD "DVD"
#define MSGTR_GUI_EnableAssSubtitle "SSA/ASS renderizado de subtítulos"
#define MSGTR_GUI_EnableAutomaticAVSync "AutoSync si/no"
#define MSGTR_GUI_EnableCache "Cache si/no"
#define MSGTR_GUI_EnableDirectRendering "Activar renderización directa"
#define MSGTR_GUI_EnableDoubleBuffering "Activar buffering doble"
#define MSGTR_GUI_EnableEqualizer "Activar equalizer"
#define MSGTR_GUI_EnableExtraStereo "Activar estereo extra"
#define MSGTR_GUI_EnableFrameDropping "Activar frame dropping"
#define MSGTR_GUI_EnableFrameDroppingIntense "Activar frame dropping DURO (peligroso)"
#define MSGTR_GUI_EnablePlaybar "Habilitar barra de reproducción"
#define MSGTR_GUI_EnablePostProcessing "Activar postprocesado"
#define MSGTR_GUI_EnableSoftwareMixer "Activar mezclador por software"
#define MSGTR_GUI_Encoding "Codificación"
#define MSGTR_GUI_Equalizer "Equalizador"
#define MSGTR_GUI_EqualizerConfiguration "Configurar el equalizador"
#define MSGTR_GUI_Error "Error"
#define MSGTR_GUI_ErrorFatal "Error fatal"
#define MSGTR_GUI_File "Reproducir archivo"
#define MSGTR_GUI_Files "Archivos"
#define MSGTR_GUI_Flip "Visualizar imagen al revés"
#define MSGTR_GUI_Font "Fuente"
#define MSGTR_GUI_FrameRate "FPS"
#define MSGTR_GUI_FrontLeft "Frente izquierdo"
#define MSGTR_GUI_FrontRight "Frente derecho"
#define MSGTR_GUI_HideVideoWindow "Mostrar Ventana de Video cuando este inactiva"
#define MSGTR_GUI_Hue "Hue"
#define MSGTR_GUI_Lavc "Usar LAVC (FFmpeg)"
#define MSGTR_GUI_MaximumUsageSpareCPU "Calidad automática"
#define MSGTR_GUI_Miscellaneous "Misc"
#define MSGTR_GUI_Mixer "Mezclador"
#define MSGTR_GUI_MixerChannel "Canal del Mezclador"
#define MSGTR_GUI_MSG_AddingVideoFilter "[GUI] Agregando filtro de video: %s\n"
#define MSGTR_GUI_MSG_ColorDepthTooLow "Lo lamento, la profundidad de color es demasiado baja.\n"
#define MSGTR_GUI_MSG_DragAndDropNothing "D&D: ¡No retorno nada!\n"
#define MSGTR_GUI_MSG_DXR3NeedsLavc "No puede reproducir archivos no MPEG con su DXR3/H+ sin recodificación. Activa lavc en la configuración del DXR3/H+."
#define MSGTR_GUI_MSG_LoadingSubtitle "[GUI] Carganado subtítulos: %s\n"
#define MSGTR_GUI_MSG_MemoryErrorImage "Lo lamento, no hay suficiente memoria para el buffer de dibujo.\n"
#define MSGTR_GUI_MSG_MemoryErrorWindow "No hay suficiente memoria para dibujar el búfer."
#define MSGTR_GUI_MSG_NoFileLoaded "no se ha cargado ningún archivo"
#define MSGTR_GUI_MSG_NoMediaOpened "no se abrió audio/video"
#define MSGTR_GUI_MSG_NotAFile0 "Esto no parece ser un archivo...\n"
#define MSGTR_GUI_MSG_NotAFile1 "Esto no parece ser un archivo: %s !\n"
#define MSGTR_GUI_MSG_PlaybackNeedsRestart "Algunas opciones requieren reiniciar la reproducción."
#define MSGTR_GUI_MSG_RemoteDisplay "Display remoto, desactivando XMITSHM.\n"
#define MSGTR_GUI_MSG_RemovingSubtitle "[GUI] Borrando subtítulos.\n"
#define MSGTR_GUI_MSG_SkinBitmapConversionError "Error de conversión de 24 bit a 32 bit (%s).\n"
#define MSGTR_GUI_MSG_SkinBitmapNotFound "Archivo no encontrado (%s).\n"
#define MSGTR_GUI_MSG_SkinBitmapPngReadError "Error al leer PNG (%s).\n"
#define MSGTR_GUI_MSG_SkinCfgNotFound "Skin no encontrado (%s).\n"
#define MSGTR_GUI_MSG_SkinCfgSelectedNotFound "Skin elegida ( %s ) no encontrada, probando 'default'...\n"
#define MSGTR_GUI_MSG_SkinErrorBitmap16Bit "Mapa de bits de 16 bits o menos no soportado (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorMessage "[skin] error en configuración de skin en la línea %d: %s"
#define MSGTR_GUI_MSG_SkinFileNotFound "[skin] no se encontró archivo ( %s ).\n"
#define MSGTR_GUI_MSG_SkinFileNotReadable "[skin] file no leible ( %s ).\n"
#define MSGTR_GUI_MSG_SkinFontFileNotFound "Archivo de fuentes no encontrado.\n"
#define MSGTR_GUI_MSG_SkinFontImageNotFound "Archivo de imagen de fuente no encontrado.\n"
#define MSGTR_GUI_MSG_SkinFontNotFound "identificador de fuente no existente (%s).\n"
#define MSGTR_GUI_MSG_SkinMemoryError "No hay suficiente memoria.\n"
#define MSGTR_GUI_MSG_SkinTooManyFonts "Demasiadas fuentes declaradas.\n"
#define MSGTR_GUI_MSG_SkinUnknownMessage "Mensaje desconocido: %s.\n"
#define MSGTR_GUI_MSG_SkinUnknownParameter "parámetro desconocido (%s)\n"
#define MSGTR_GUI_MSG_TooManyWindows "Hay demasiadas ventanas abiertas.\n"
#define MSGTR_GUI_MSG_UnableToSaveOption "[cfg] No se puede guardar la opción '%s'.\n"
#define MSGTR_GUI_MSG_VideoOutError "No se encuentra driver -vo compatible con la interfaz gráfica."
#define MSGTR_GUI_MSG_XShapeError "Lo lamento, su sistema no soporta la extensión XShape.\n"
#define MSGTR_GUI_MSG_XSharedMemoryError "Error en la extensión de memoria compartida\n"
#define MSGTR_GUI_MSG_XSharedMemoryUnavailable "Lo lamento, su sistema no soporta la extensión de memoria compartida X.\n"
#define MSGTR_GUI_Mute "Mudo"
#define MSGTR_GUI_NetworkStreaming "Streaming por red..."
#define MSGTR_GUI_Next "Siguiente stream"
#define MSGTR_GUI_NoChapter "sin capítulo"
#define MSGTR_GUI__none_ "(ninguno)"
#define MSGTR_GUI_NonInterleavedParser "Usar non-interleaved AVI parser"
#define MSGTR_GUI_NormalizeSound "Normalizar sonido"
#define MSGTR_GUI_Ok "OK"
#define MSGTR_GUI_Open "Abrir..."
#define MSGTR_GUI_Original "Original"
#define MSGTR_GUI_OsdLevel "Nivel OSD"
#define MSGTR_GUI_OSD_Subtitles "Subtítulos y OSD"
#define MSGTR_GUI_Outline "Outline"
#define MSGTR_GUI_PanAndScan "Panscan"
#define MSGTR_GUI_Pause "Pausa"
#define MSGTR_GUI_Play "Reproducir"
#define MSGTR_GUI_Playback "Reproduciendo"
#define MSGTR_GUI_Playlist "Lista de reproducción"
#define MSGTR_GUI_Position "Posición"
#define MSGTR_GUI_PostProcessing "Postprocesado"
#define MSGTR_GUI_Preferences "Preferencias"
#define MSGTR_GUI_Previous "Anterior stream"
#define MSGTR_GUI_Quit "Salir"
#define MSGTR_GUI_RearLeft "Fondo izquierdo"
#define MSGTR_GUI_RearRight "Fondo dercho"
#define MSGTR_GUI_Remove "Quitar"
#define MSGTR_GUI_Saturation "Saturación"
#define MSGTR_GUI_SaveWindowPositions "Guardar posición de la ventana"
#define MSGTR_GUI_ScaleMovieDiagonal "Proporcional al diagonal de película"
#define MSGTR_GUI_ScaleMovieHeight "Proporcional a la altura de película"
#define MSGTR_GUI_ScaleMovieWidth "Proporcional a la anchura de película"
#define MSGTR_GUI_ScaleNo "Sin autoescalado"
#define MSGTR_GUI_SeekingInBrokenMedia "Reconstruir tabla de índices, si se necesita"
#define MSGTR_GUI_SelectAudioFile "Seleccionar canal de audio externo..."
#define MSGTR_GUI_SelectedFiles "Archivos seleccionados"
#define MSGTR_GUI_SelectFile "Seleccionar archivo..."
#define MSGTR_GUI_SelectFont "Seleccionar fuente..."
#define MSGTR_GUI_SelectSubtitle "Seleccionar subtítulos..."
#define MSGTR_GUI_SizeDouble "Tamaño doble"
#define MSGTR_GUI_SizeFullscreen "Pantalla completa"
#define MSGTR_GUI_SizeHalf "Mitad del Tamaño"
#define MSGTR_GUI_SizeNormal "Tamaño normal"
#define MSGTR_GUI_SizeOSD "Escalado de OSD"
#define MSGTR_GUI_SizeSubtitles "Escalado de texto"
#define MSGTR_GUI_SkinBrowser "Navegador de skins"
#define MSGTR_GUI_Skins "Skins"
#define MSGTR_GUI_Sponsored " Desarrollo de GUI patrocinado por UHU Linux"
#define MSGTR_GUI_StartFullscreen "Empezar en pantalla completa"
#define MSGTR_GUI_Stop "Parar"
#define MSGTR_GUI_Subtitle "Subtítulo"
#define MSGTR_GUI_SubtitleAddMargins "Usar márgenes"
#define MSGTR_GUI_SubtitleAllowOverlap "Superposición de subtitulos"
#define MSGTR_GUI_SubtitleAutomaticLoad "Desactivar carga automática de subtítulos"
#define MSGTR_GUI_SubtitleConvertMpsub "Convertir el subtítulo dado al formato de subtítulos de MPlayer"
#define MSGTR_GUI_SubtitleConvertSrt "Convertir el subtítulo dado al formato basado en tiempo SubViewer (SRT)"
#define MSGTR_GUI_Subtitles "Subtítulos"
#define MSGTR_GUI_SyncValue "Autosync"
#define MSGTR_GUI_TitleNN "Título %2d"
#define MSGTR_GUI_Titles "Títulos"
#define MSGTR_GUI_Top "Superior"
#define MSGTR_GUI_TrackN "Pista %d"
#define MSGTR_GUI_Translations "Traducciones"
#define MSGTR_GUI_TurnOffXScreenSaver "Detener Salvador de Pantallas de X"
#define MSGTR_GUI_URL "Reproducir URL"
#define MSGTR_GUI_VCD "VCD"
#define MSGTR_GUI_Video "Video"
#define MSGTR_GUI_VideoEncoder "Codificador de video"
#define MSGTR_GUI_VideoTracks "Pista de Video"
#define MSGTR_GUI_Warning "Advertencia"

// ======================= video output drivers ========================

#define MSGTR_VOincompCodec "Disculpe, el dispositivo de salida de video es incompatible con este codec.\n"
#define MSGTR_VO_GenericError "Este error ha ocurrido"
#define MSGTR_VO_UnableToAccess "No es posible acceder"
#define MSGTR_VO_ExistsButNoDirectory "ya existe, pero no es un directorio."
#define MSGTR_VO_DirExistsButNotWritable "El directorio ya existe, pero no se puede escribir en él."
#define MSGTR_VO_CantCreateDirectory "No es posible crear el directorio de salida."
#define MSGTR_VO_CantCreateFile "No es posible crear archivo de salida."
#define MSGTR_VO_DirectoryCreateSuccess "Directorio de salida creado exitosamente."
#define MSGTR_VO_ValueOutOfRange "Valor fuera de rango"

// aspect.c
#define MSGTR_LIBVO_ASPECT_NoSuitableNewResFound "[ASPECT] Aviso: ¡No se ha encontrado ninguna resolución nueva adecuada!\n"
#define MSGTR_LIBVO_ASPECT_NoNewSizeFoundThatFitsIntoRes "[ASPECT] Error: No new size found that fits into res!\n"

// font_load_ft.c
#define MSGTR_LIBVO_FONT_LOAD_FT_NewFaceFailed "Fallo en New_Face. Quizas el font path no es correcto.\nPor favor proporcione el archivo de fuentes de texto (~/.mplayer/subfont.ttf).\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_NewMemoryFaceFailed "Fallo en New_Memory_Face ..\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_SubFaceFailed "Fuente de subtítulo: fallo en load_sub_face.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_SubFontCharsetFailed "Fuente de subtítulo: fallo en prepare_charset.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_CannotPrepareSubtitleFont "Imposible preparar la fuente para subtítulos.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_CannotPrepareOSDFont "Imposible preparar la fuente para el OSD.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_CannotGenerateTables "Imposible generar tablas.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_DoneFreeTypeFailed "Fallo en FT_Done_FreeType.\n"

// sub.c
#define MSGTR_VO_SUB_Seekbar "Barra de navegación"
#define MSGTR_VO_SUB_Play "Play"
#define MSGTR_VO_SUB_Pause "Pausa"
#define MSGTR_VO_SUB_Stop "Stop"
#define MSGTR_VO_SUB_Rewind "Rebobinar"
#define MSGTR_VO_SUB_Forward "Avanzar"
#define MSGTR_VO_SUB_Clock "Reloj"
#define MSGTR_VO_SUB_Contrast "Contraste"
#define MSGTR_VO_SUB_Saturation "Saturación"
#define MSGTR_VO_SUB_Volume "Volumen"
#define MSGTR_VO_SUB_Brightness "Brillo"
#define MSGTR_VO_SUB_Hue "Hue"
#define MSGTR_VO_SUB_Balance "Balance"

// vo_3dfx.c
#define MSGTR_LIBVO_3DFX_Only16BppSupported "[VO_3DFX] Solo 16bpp soportado!"
#define MSGTR_LIBVO_3DFX_VisualIdIs "[VO_3DFX] El id visual es  %lx.\n"
#define MSGTR_LIBVO_3DFX_UnableToOpenDevice "[VO_3DFX] No pude abrir /dev/3dfx.\n"
#define MSGTR_LIBVO_3DFX_Error "[VO_3DFX] Error: %d.\n"
#define MSGTR_LIBVO_3DFX_CouldntMapMemoryArea "[VO_3DFX] No pude mapear las areas de memoria 3dfx: %p,%p,%d.\n"
#define MSGTR_LIBVO_3DFX_DisplayInitialized "[VO_3DFX] Inicializado: %p.\n"
#define MSGTR_LIBVO_3DFX_UnknownSubdevice "[VO_3DFX] sub-dispositivo desconocido: %s.\n"

// vo_aa.c
#define MSGTR_VO_AA_HelpHeader "\n\nAquí estan las subopciones del vo_aa aalib:\n"
#define MSGTR_VO_AA_AdditionalOptions "Opciones adicionales provistas por vo_aa:\n" \
"  help        mostrar esta ayuda\n" \
"  osdcolor    elegir color de osd\n  subcolor    elegir color de subtitlos\n" \
"        los parámetros de color son:\n           0 : normal\n" \
"           1 : oscuro\n           2 : bold\n           3 : boldfont\n" \
"           4 : reverso\n           5 : especial\n\n\n"

// vo_dxr3.c
#define MSGTR_LIBVO_DXR3_UnableToLoadNewSPUPalette "[VO_DXR3] No pude cargar la nueva paleta SPU!\n"
#define MSGTR_LIBVO_DXR3_UnableToSetPlaymode "[VO_DXR3] No pude setear el playmode!\n"
#define MSGTR_LIBVO_DXR3_UnableToSetSubpictureMode "[VO_DXR3] No pude setear el modo del subpicture!\n"
#define MSGTR_LIBVO_DXR3_UnableToGetTVNorm "[VO_DXR3] No pude obtener la norma de TV!\n"
#define MSGTR_LIBVO_DXR3_AutoSelectedTVNormByFrameRate "[VO_DXR3] Norma de TV autoeleccionada a partir del frame rate: "
#define MSGTR_LIBVO_DXR3_UnableToSetTVNorm "[VO_DXR3] Imposible setear la norma de TV!\n"
#define MSGTR_LIBVO_DXR3_SettingUpForNTSC "[VO_DXR3] Configurando para NTSC.\n"
#define MSGTR_LIBVO_DXR3_SettingUpForPALSECAM "[VO_DXR3] Configurando para PAL/SECAM.\n"
#define MSGTR_LIBVO_DXR3_SettingAspectRatioTo43 "[VO_DXR3] Seteando la relación de aspecto a 4:3.\n"
#define MSGTR_LIBVO_DXR3_SettingAspectRatioTo169 "[VO_DXR3] Seteando la relación de aspecto a 16:9.\n"
#define MSGTR_LIBVO_DXR3_OutOfMemory "[VO_DXR3] Ouch! me quede sin memoria!\n"
#define MSGTR_LIBVO_DXR3_UnableToAllocateKeycolor "[VO_DXR3] No pude disponer el keycolor!\n"
#define MSGTR_LIBVO_DXR3_UnableToAllocateExactKeycolor "[VO_DXR3] No pude disponer el keycolor exacto, voy a utilizar el mas parecido (0x%lx).\n"
#define MSGTR_LIBVO_DXR3_Uninitializing "[VO_DXR3] Uninitializing.\n"
#define MSGTR_LIBVO_DXR3_FailedRestoringTVNorm "[VO_DXR3] No pude restaurar la norma de TV!\n"
#define MSGTR_LIBVO_DXR3_EnablingPrebuffering "[VO_DXR3] Habilitando prebuffering.\n"
#define MSGTR_LIBVO_DXR3_UsingNewSyncEngine "[VO_DXR3] Utilizando el nuevo motor de syncronía.\n"
#define MSGTR_LIBVO_DXR3_UsingOverlay "[VO_DXR3] Utilizando overlay.\n"
#define MSGTR_LIBVO_DXR3_ErrorYouNeedToCompileMplayerWithX11 "[VO_DXR3] Error: Necesitas compilar MPlayer con las librerias x11 y sus headers para utilizar overlay.\n"
#define MSGTR_LIBVO_DXR3_WillSetTVNormTo "[VO_DXR3] Voy a setear la norma de TV a: "
#define MSGTR_LIBVO_DXR3_AutoAdjustToMovieFrameRatePALPAL60 "Auto-adjustando al framerate del video (PAL/PAL-60)"
#define MSGTR_LIBVO_DXR3_AutoAdjustToMovieFrameRatePALNTSC "Auto-adjustando al framerate del video (PAL/NTSC)"
#define MSGTR_LIBVO_DXR3_UseCurrentNorm "Utilizar norma actual."
#define MSGTR_LIBVO_DXR3_UseUnknownNormSuppliedCurrentNorm "Norma sugerida: desconocida. Utilizando norma actual."
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingTrying "[VO_DXR3] Error abriendo %s para escribir, intentando /dev/em8300 en su lugar.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingTryingMV "[VO_DXR3] Error abriendo %s para escribir, intentando /dev/em8300_mv en su lugar.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingAsWell "[VO_DXR3] Nuevamente error abriendo /dev/em8300 para escribir!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingAsWellMV "[VO_DXR3] Nuevamente error abriendo /dev/em8300_mv para escribir!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_Opened "[VO_DXR3] Abierto: %s.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingTryingSP "[VO_DXR3] Error abriendo %s para escribir, intentando /dev/em8300_sp en su lugar.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingAsWellSP "[VO_DXR3] Nuevamente error abriendo /dev/em8300_sp para escribir!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_UnableToOpenDisplayDuringHackSetup "[VO_DXR3] Unable to open display during overlay hack setup!\n"
#define MSGTR_LIBVO_DXR3_UnableToInitX11 "[VO_DXR3] No puede inicializar x11!\n"
#define MSGTR_LIBVO_DXR3_FailedSettingOverlayAttribute "[VO_DXR3] Fallé tratando de setear el atributo overlay.\n"
#define MSGTR_LIBVO_DXR3_FailedSettingOverlayScreen "[VO_DXR3] Fallé tratando de setear el overlay screen!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedEnablingOverlay "[VO_DXR3] Fallé habilitando overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedResizingOverlayWindow "[VO_DXR3] Fallé tratando de redimiencionar la ventana overlay!\n"
#define MSGTR_LIBVO_DXR3_FailedSettingOverlayBcs "[VO_DXR3] Fallé seteando el bcs overlay!\n"
#define MSGTR_LIBVO_DXR3_FailedGettingOverlayYOffsetValues "[VO_DXR3] Fallé tratando de obtener los valores Y-offset del overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedGettingOverlayXOffsetValues "[VO_DXR3] Fallé tratando de obtener los valores X-offset del overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedGettingOverlayXScaleCorrection "[VO_DXR3] Fallé obteniendo la corrección X scale del overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_YOffset "[VO_DXR3] Yoffset: %d.\n"
#define MSGTR_LIBVO_DXR3_XOffset "[VO_DXR3] Xoffset: %d.\n"
#define MSGTR_LIBVO_DXR3_XCorrection "[VO_DXR3] Xcorrection: %d.\n"
#define MSGTR_LIBVO_DXR3_FailedSetSignalMix "[VO_DXR3] Fallé seteando el signal mix!\n"

// vo_jpeg.c
#define MSGTR_VO_JPEG_ProgressiveJPEG "JPEG progresivo habilitado."
#define MSGTR_VO_JPEG_NoProgressiveJPEG "JPEG progresivo deshabilitado."
#define MSGTR_VO_JPEG_BaselineJPEG "Baseline JPEG habilitado."
#define MSGTR_VO_JPEG_NoBaselineJPEG "Baseline JPEG deshabilitado."

// vo_mga.c
#define MSGTR_LIBVO_MGA_AspectResized "[VO_MGA] aspect(): redimencionado a %dx%d.\n"
#define MSGTR_LIBVO_MGA_Uninit "[VO] uninit!\n"

// mga_template.c
#define MSGTR_LIBVO_MGA_ErrorInConfigIoctl "[MGA] Error en mga_vid_config ioctl (versión de mga_vid.o erronea?)"
#define MSGTR_LIBVO_MGA_CouldNotGetLumaValuesFromTheKernelModule "[MGA] No pude obtener los valores de luma desde el módulo del kernel!\n"
#define MSGTR_LIBVO_MGA_CouldNotSetLumaValuesFromTheKernelModule "[MGA] No pude setear los valores de luma que obtuve desde el módulo del kernel!\n"
#define MSGTR_LIBVO_MGA_ScreenWidthHeightUnknown "[MGA] Ancho/Alto de la pantalla desconocidos!\n"
#define MSGTR_LIBVO_MGA_InvalidOutputFormat "[MGA] Formáto de salida inválido %0X\n"
#define MSGTR_LIBVO_MGA_IncompatibleDriverVersion "[MGA] La versión de tu driver mga_vid no es compatible con esta versión de MPlayer!\n"
#define MSGTR_LIBVO_MGA_CouldntOpen "[MGA] No pude abrir: %s\n"
#define MSGTR_LIBVO_MGA_ResolutionTooHigh "[MGA] La resolución de la fuente es en por lo menos una dimensión mas grande que 1023x1023. Por favor escale en software o use -lavdopts lowres=1\n"
#define MSGTR_LIBVO_MGA_mgavidVersionMismatch "[MGA] Las versiones del controlador mga_vid del kernel (%u) y MPlayer (%u) no coinciden\n"

// vo_null.c
#define MSGTR_LIBVO_NULL_UnknownSubdevice "[VO_NULL] Sub-dispositivo desconocido: %s.\n"

// vo_png.c
#define MSGTR_LIBVO_PNG_Warning1 "[VO_PNG] Advertencia: nivel de compresión seteado a 0, compresión desabilitada!\n"
#define MSGTR_LIBVO_PNG_Warning2 "[VO_PNG] Info: Utiliza -vo png:z=<n> para setear el nivel de compresión desde 0 a 9.\n"
#define MSGTR_LIBVO_PNG_Warning3 "[VO_PNG] Info: (0 = sin compresión, 1 = la más rápida y baja - 9 la más lenta y alta)\n"
#define MSGTR_LIBVO_PNG_ErrorOpeningForWriting "\n[VO_PNG] Error abriendo '%s' para escribir!\n"
#define MSGTR_LIBVO_PNG_ErrorInCreatePng "[VO_PNG] Error en create_png.\n"

// vo_pnm.c
#define MSGTR_VO_PNM_ASCIIMode "modo ASCII habilitado."
#define MSGTR_VO_PNM_RawMode "Raw mode habilitado."
#define MSGTR_VO_PNM_PPMType "Escribiré archivos PPM."
#define MSGTR_VO_PNM_PGMType "Escribiré archivos PGM."
#define MSGTR_VO_PNM_PGMYUVType "Escribiré archivos PGMYUV."

// vo_sdl.c
#define MSGTR_LIBVO_SDL_CouldntGetAnyAcceptableSDLModeForOutput "[VO_SDL] No pude obtener ni un solo modo SDL aceptable para la salida.\n"
#define MSGTR_LIBVO_SDL_UnsupportedImageFormat "[VO_SDL] Formato de imagen no soportado (0x%X).\n"
#define MSGTR_LIBVO_SDL_InfoPleaseUseVmOrZoom "[VO_SDL] Info - por favor utiliza -vm ó -zoom para cambiar a la mejor resolución.\n"
#define MSGTR_LIBVO_SDL_FailedToSetVideoMode "[VO_SDL] Fallè tratando de setear el modo de video: %s.\n"
#define MSGTR_LIBVO_SDL_CouldntCreateAYUVOverlay "[VO_SDL] No pude crear un overlay YUV: %s.\n"
#define MSGTR_LIBVO_SDL_CouldntCreateARGBSurface "[VO_SDL] No pude crear una superficie RGB: %s.\n"
#define MSGTR_LIBVO_SDL_UsingDepthColorspaceConversion "[VO_SDL] Utilizando conversión de depth/colorspace, va a andar un poco más lento.. (%ibpp -> %ibpp).\n"
#define MSGTR_LIBVO_SDL_UnsupportedImageFormatInDrawslice "[VO_SDL] Formato no soportado de imagen en draw_slice, contacta a los desarrolladores de MPlayer!\n"
#define MSGTR_LIBVO_SDL_BlitFailed "[VO_SDL] Blit falló: %s.\n"
#define MSGTR_LIBVO_SDL_InitializationFailed "[VO_SDL] Fallo la inicialización de SDL: %s.\n"
#define MSGTR_LIBVO_SDL_UsingDriver "[VO_SDL] Utilizando el driver: %s.\n"

// vo_svga.c
#define MSGTR_LIBVO_SVGA_ForcedVidmodeNotAvailable "[VO_SVGA] El vid_mode forzado %d (%s) no esta disponible.\n"
#define MSGTR_LIBVO_SVGA_ForcedVidmodeTooSmall "[VO_SVGA] El vid_mode forzado %d (%s) es muy pequeño.\n"
#define MSGTR_LIBVO_SVGA_Vidmode "[VO_SVGA] Vid_mode: %d, %dx%d %dbpp.\n"
#define MSGTR_LIBVO_SVGA_VgasetmodeFailed "[VO_SVGA] Vga_setmode(%d) fallido.\n"
#define MSGTR_LIBVO_SVGA_VideoModeIsLinearAndMemcpyCouldBeUsed "[VO_SVGA] El modo de vídeo es lineal y memcpy se puede usar para la transferencia de imágenes.\n"
#define MSGTR_LIBVO_SVGA_VideoModeHasHardwareAcceleration "[VO_SVGA] El modo de video dispone de aceleración por hardware y put_image puede ser utilizada.\n"
#define MSGTR_LIBVO_SVGA_IfItWorksForYouIWouldLikeToKnow "[VO_SVGA] Si le ha funcionado nos gustaría saberlo.\n[VO_SVGA] (mande un registro con `mplayer test.avi -v -v -v -v &> svga.log`). ¡Gracias!\n"
#define MSGTR_LIBVO_SVGA_VideoModeHas "[VO_SVGA] El modo de video tiene %d pagina(s).\n"
#define MSGTR_LIBVO_SVGA_CenteringImageStartAt "[VO_SVGA] Centrando imagen, comenzando en (%d,%d)\n"
#define MSGTR_LIBVO_SVGA_UsingVidix "[VO_SVGA] Utilizando VIDIX. w=%i h=%i  mw=%i mh=%i\n"

// vo_tdfx_vid.c
#define MSGTR_LIBVO_TDFXVID_Move "[VO_TDXVID] Move %d(%d) x %d => %d.\n"
#define MSGTR_LIBVO_TDFXVID_AGPMoveFailedToClearTheScreen "[VO_TDFXVID] El AGP move falló al tratar de limpiar la pantalla.\n"
#define MSGTR_LIBVO_TDFXVID_BlitFailed "[VO_TDFXVID] Fallo el Blit.\n"
#define MSGTR_LIBVO_TDFXVID_NonNativeOverlayFormatNeedConversion "[VO_TDFXVID] El formato overlay no-nativo requiere conversión.\n"
#define MSGTR_LIBVO_TDFXVID_UnsupportedInputFormat "[VO_TDFXVID] Formato de entrada no soportado 0x%x.\n"
#define MSGTR_LIBVO_TDFXVID_OverlaySetupFailed "[VO_TDFXVID] Fallo en la configuración del overlay.\n"
#define MSGTR_LIBVO_TDFXVID_OverlayOnFailed "[VO_TDFXVID] Falló el Overlay on .\n"
#define MSGTR_LIBVO_TDFXVID_OverlayReady "[VO_TDFXVID] Overlay listo: %d(%d) x %d @ %d => %d(%d) x %d @ %d.\n"
#define MSGTR_LIBVO_TDFXVID_TextureBlitReady "[VO_TDFXVID] Blit de textura listo: %d(%d) x %d @ %d => %d(%d) x %d @ %d.\n"
#define MSGTR_LIBVO_TDFXVID_OverlayOffFailed "[VO_TDFXVID] Falló el Overlay off\n"
#define MSGTR_LIBVO_TDFXVID_CantOpen "[VO_TDFXVID] No pude abrir %s: %s.\n"
#define MSGTR_LIBVO_TDFXVID_CantGetCurrentCfg "[VO_TDFXVID] No pude obtener la cfg actual: %s.\n"
#define MSGTR_LIBVO_TDFXVID_MemmapFailed "[VO_TDFXVID] Falló el Memmap!!!!!\n"
#define MSGTR_LIBVO_TDFXVID_GetImageTodo "Get image todo.\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailed "[VO_TDFXVID] Fallo el AGP move.\n"
#define MSGTR_LIBVO_TDFXVID_SetYuvFailed "[VO_TDFXVID] Falló set yuv\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailedOnYPlane "[VO_TDFXVID] El AGP move falló en el plano Y\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailedOnUPlane "[VO_TDFXVID] El AGP move falló en el plano U\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailedOnVPlane "[VO_TDFXVID] El AGP move fallo en el plano V\n"
#define MSGTR_LIBVO_TDFXVID_UnknownFormat "[VO_TDFXVID] Qué es esto como formato 0x%x?\n"

// vo_tdfxfb.c
#define MSGTR_LIBVO_TDFXFB_CantOpen "[VO_TDFXFB] No pude abrir %s: %s.\n"
#define MSGTR_LIBVO_TDFXFB_ProblemWithFbitgetFscreenInfo "[VO_TDFXFB] Problema con el ioctl FBITGET_FSCREENINFO: %s.\n"
#define MSGTR_LIBVO_TDFXFB_ProblemWithFbitgetVscreenInfo "[VO_TDFXFB] Problema con el ioctl FBITGET_VSCREENINFO: %s.\n"
#define MSGTR_LIBVO_TDFXFB_ThisDriverOnlySupports "[VO_TDFXFB] Este driver solo soporta las 3Dfx Banshee, Voodoo3 y Voodoo 5.\n"
#define MSGTR_LIBVO_TDFXFB_OutputIsNotSupported "[VO_TDFXFB] La salida %d bpp no esta soporatda.\n"
#define MSGTR_LIBVO_TDFXFB_CouldntMapMemoryAreas "[VO_TDFXFB] No pude mapear las áreas de memoria: %s.\n"
#define MSGTR_LIBVO_TDFXFB_BppOutputIsNotSupported "[VO_TDFXFB] La salida %d bpp no esta soportada (esto de debería haber pasado).\n"
#define MSGTR_LIBVO_TDFXFB_SomethingIsWrongWithControl "[VO_TDFXFB] Uh! algo anda mal con control().\n"
#define MSGTR_LIBVO_TDFXFB_NotEnoughVideoMemoryToPlay "[VO_TDFXFB] No hay suficiente memoria de video para reproducir esta pelicula. Intenta con una resolución más baja.\n"
#define MSGTR_LIBVO_TDFXFB_ScreenIs "[VO_TDFXFB] La pantalla es %dx%d a %d bpp, en %dx%d a %d bpp, la norma es %dx%d.\n"

// vo_tga.c
#define MSGTR_LIBVO_TGA_UnknownSubdevice "[VO_TGA] Sub-dispositivo desconocido: %s\n"

// vo_vesa.c
#define MSGTR_LIBVO_VESA_FatalErrorOccurred "[VO_VESA] Error fatal! no puedo continuar.\n"
#define MSGTR_LIBVO_VESA_UnknownSubdevice "[VO_VESA] Sub-dispositivo desconocido: '%s'.\n"
#define MSGTR_LIBVO_VESA_YouHaveTooLittleVideoMemory "[VO_VESA] Tienes muy poca memoria de video para este modo:\n[VO_VESA] Requiere: %08lX tienes: %08lX.\n"
#define MSGTR_LIBVO_VESA_YouHaveToSpecifyTheCapabilitiesOfTheMonitor "[VO_VESA] Tienes que especificar las capacidades del monitor. No voy a cambiar la tasa de refresco.\n"
#define MSGTR_LIBVO_VESA_UnableToFitTheMode "[VO_VESA] No pude encajar este modo en las limitaciones del  monitor. No voy a cambiar la tasa de refresco.\n"
#define MSGTR_LIBVO_VESA_DetectedInternalFatalError "[VO_VESA] Error fatal interno detectado: init se llamado despues de preinit.\n"
#define MSGTR_LIBVO_VESA_SwitchFlipIsNotSupported "[VO_VESA] Switch -flip no esta soportado.\n"
#define MSGTR_LIBVO_VESA_PossibleReasonNoVbe2BiosFound "[VO_VESA] Razón posible: No se encontró una BIOS VBE2.\n"
#define MSGTR_LIBVO_VESA_FoundVesaVbeBiosVersion "[VO_VESA] Enontré una versión de BIOS VESA VBE %x.%x Revisión: %x.\n"
#define MSGTR_LIBVO_VESA_VideoMemory "[VO_VESA] Memoria de video: %u Kb.\n"
#define MSGTR_LIBVO_VESA_Capabilites "[VO_VESA] Capacidades VESA: %s %s %s %s %s.\n"
#define MSGTR_LIBVO_VESA_BelowWillBePrintedOemInfo "[VO_VESA] !!! abajo se imprimira información OEM. !!!\n"
#define MSGTR_LIBVO_VESA_YouShouldSee5OemRelatedLines "[VO_VESA] Deberias ver 5 lineas con respecto a OEM abajo; sino, has arruinado vm86.\n"
#define MSGTR_LIBVO_VESA_OemInfo "[VO_VESA] Información OEM: %s.\n"
#define MSGTR_LIBVO_VESA_OemRevision "[VO_VESA] OEM Revisión: %x.\n"
#define MSGTR_LIBVO_VESA_OemVendor "[VO_VESA] OEM vendor: %s.\n"
#define MSGTR_LIBVO_VESA_OemProductName "[VO_VESA] OEM Product Name: %s.\n"
#define MSGTR_LIBVO_VESA_OemProductRev "[VO_VESA] OEM Product Rev: %s.\n"
#define MSGTR_LIBVO_VESA_Hint "[VO_VESA] Consejo: Para que el TV_Out te funcione tienes que conectarla\n"\
"[VO_VESA] antes de que el pc bootee por que la BIOS VESA solo se inicializa a si misma durante el POST.\n"
#define MSGTR_LIBVO_VESA_UsingVesaMode "[VO_VESA] Utilizando el modo VESA (%u) = %x [%ux%u@%u]\n"
#define MSGTR_LIBVO_VESA_CantInitializeSwscaler "[VO_VESA] No pude inicializar el SwScaler.\n"
#define MSGTR_LIBVO_VESA_CantUseDga "[VO_VESA] No puedo utilizar DGA. fuersa el modo bank switching. :(\n"
#define MSGTR_LIBVO_VESA_UsingDga "[VO_VESA] Utilizand DGA (recursos físicos: %08lXh, %08lXh)"
#define MSGTR_LIBVO_VESA_CantUseDoubleBuffering "[VO_VESA] No puedo utilizar double buffering: No hay suficiente memoria de video.\n"
#define MSGTR_LIBVO_VESA_CantFindNeitherDga "[VO_VESA] No pude encontrar DGA o un cuadro de ventana relocatable.\n"
#define MSGTR_LIBVO_VESA_YouveForcedDga "[VO_VESA] Tenemos DGA forzado. Saliendo.\n"
#define MSGTR_LIBVO_VESA_CantFindValidWindowAddress "[VO_VESA] No pude encontrar una dirección de ventana válida.\n"
#define MSGTR_LIBVO_VESA_UsingBankSwitchingMode "[VO_VESA] Utilizando modo bank switching (recursos físicos: %08lXh, %08lXh).\n"
#define MSGTR_LIBVO_VESA_CantAllocateTemporaryBuffer "[VO_VESA] No pude disponer de un buffer temporal.\n"
#define MSGTR_LIBVO_VESA_SorryUnsupportedMode "[VO_VESA] Sorry, formato no soportado -- trata con -x 640 -zoom.\n"
#define MSGTR_LIBVO_VESA_OhYouReallyHavePictureOnTv "[VO_VESA] Oh! mira! realmente tienes una imagen en la TV!\n"
#define MSGTR_LIBVO_VESA_CantInitialozeLinuxVideoOverlay "[VO_VESA] No pude inicializar el Video Overlay Linux.\n"
#define MSGTR_LIBVO_VESA_UsingVideoOverlay "[VO_VESA] Utilizando overlay de video: %s.\n"
#define MSGTR_LIBVO_VESA_CantInitializeVidixDriver "[VO_VESA] No pude inicializar el driver VIDIX.\n"
#define MSGTR_LIBVO_VESA_UsingVidix "[VO_VESA] Utilizando VIDIX.\n"
#define MSGTR_LIBVO_VESA_CantFindModeFor "[VO_VESA] No pude encontrar un modo para: %ux%u@%u.\n"
#define MSGTR_LIBVO_VESA_InitializationComplete "[VO_VESA] Inicialización VESA completa.\n"

// vesa_lvo.c
#define MSGTR_LIBVO_VESA_ThisBranchIsNoLongerSupported "[VESA_LVO] Este branch ya no esta soportado.\n[VESA_LVO] Por favor utiliza -vo vesa:vidix en su lugar.\n"
#define MSGTR_LIBVO_VESA_CouldntOpen "[VESA_LVO] No pude abrir: '%s'\n"
#define MSGTR_LIBVO_VESA_InvalidOutputFormat "[VESA_LVI] Formato de salida inválido: %s(%0X)\n"
#define MSGTR_LIBVO_VESA_IncompatibleDriverVersion "[VESA_LVO] La version de tu driver fb_vid no es compatible con esta versión de MPlayer!\n"

// vo_x11.c

// vo_xv.c
#define MSGTR_LIBVO_XV_SharedMemoryNotSupported "[VO_XV] Memoria compartida no soportada\nVolviendo al Xv normal.\n"
#define MSGTR_LIBVO_XV_XvNotSupportedByX11 "[VO_XV] Perdone, Xv no está soportado por esta versión/driver de X11\n[VO_XV] ******** Pruebe con  -vo x11  o  -vo sdl  *********\n"
#define MSGTR_LIBVO_XV_XvQueryAdaptorsFailed  "[VO_XV] XvQueryAdaptors ha fallado.\n"
#define MSGTR_LIBVO_XV_InvalidPortParameter "[VO_XV] Parámetro de puerto inválido, invalidándolo con el puerto 0.\n"
#define MSGTR_LIBVO_XV_CouldNotGrabPort "[VO_XV] No se ha podido fijar el puerto %i.\n"
#define MSGTR_LIBVO_XV_CouldNotFindFreePort "[VO_XV] No se ha podido encontrar ningún puerto Xvideo libre - quizás otro proceso ya lo está usando\n"\
"[VO_XV] usándolo. Cierre todas las aplicaciones de vídeo, y pruebe otra vez\n"\
"[VO_XV] Si eso no ayuda, vea 'mplayer -vo help' para otros (no-xv) drivers de salida de vídeo.\n"
#define MSGTR_LIBVO_XV_NoXvideoSupport "[VO_XV] Parece que no hay soporte Xvideo para su tarjeta gráfica.\n"\
"[VO_XV] Ejecute 'xvinfo' para verificar el soporte Xv y lea\n"\
"[VO_XV] DOCS/HTML/en/video.html#xv!\n"\
"[VO_XV] Revise otros drivers de salida de video (no-xv) con 'mplayer -vo help'.\n"\
"[VO_XV] Intente -vo x11.\n"
#define MSGTR_VO_XV_ImagedimTooHigh "Las dimensiones de la imagen de origen son demasiado grandes: %ux%u (el máximo es %ux%u)\n"

// vo_yuv4mpeg.c
#define MSGTR_VO_YUV4MPEG_InterlacedHeightDivisibleBy4 "Modo interlaceado requiere que la altura de la imagen sea divisible por 4."
#define MSGTR_VO_YUV4MPEG_WidthDivisibleBy2 "El ancho de la imagen debe ser divisible por 2."
#define MSGTR_VO_YUV4MPEG_OutFileOpenError "Imposible obtener memoria o descriptor de archivos para escribir \"%s\"!"
#define MSGTR_VO_YUV4MPEG_OutFileWriteError "Error escribiendo imagen a la salida!"
#define MSGTR_VO_YUV4MPEG_UnknownSubDev "Se desconoce el subdevice: %s"
#define MSGTR_VO_YUV4MPEG_InterlacedTFFMode "Usando modo de salida interlaceado, top-field primero."
#define MSGTR_VO_YUV4MPEG_InterlacedBFFMode "Usando modo de salida interlaceado, bottom-field primero."
#define MSGTR_VO_YUV4MPEG_ProgressiveMode "Usando (por defecto) modo de cuadro progresivo."

// vobsub_vidix.c
#define MSGTR_LIBVO_SUB_VIDIX_CantStartPlayback "[VO_SUB_VIDIX] No puedo iniciar la reproducción: %s\n"
#define MSGTR_LIBVO_SUB_VIDIX_CantStopPlayback "[VO_SUB_VIDIX] No puedo parar la reproducción: %s\n"
#define MSGTR_LIBVO_SUB_VIDIX_InterleavedUvForYuv410pNotSupported "[VO_SUB_VIDIX] Interleaved uv para yuv410p no soportado.\n"
#define MSGTR_LIBVO_SUB_VIDIX_DummyVidixdrawsliceWasCalled "[VO_SUB_VIDIX] Dummy vidix_draw_slice() fue llamada.\n"
#define MSGTR_LIBVO_SUB_VIDIX_UnsupportedFourccForThisVidixDriver "[VO_SUB_VIDIX] fourcc no soportado para este driver vidix: %x (%s).\n"
#define MSGTR_LIBVO_SUB_VIDIX_VideoServerHasUnsupportedResolution "[VO_SUB_VIDIX] El servidor de video server tiene una resolución no soportada (%dx%d), soportadas: %dx%d-%dx%d.\n"
#define MSGTR_LIBVO_SUB_VIDIX_VideoServerHasUnsupportedColorDepth "[VO_SUB_VIDIX] Video server has unsupported color depth by vidix (%d).\n"
#define MSGTR_LIBVO_SUB_VIDIX_DriverCantUpscaleImage "[VO_SUB_VIDIX] El driver Vidix no puede ampliar la imagen (%d%d -> %d%d).\n"
#define MSGTR_LIBVO_SUB_VIDIX_DriverCantDownscaleImage "[VO_SUB_VIDIX] El driver Vidix no puede reducir la imagen (%d%d -> %d%d).\n"
#define MSGTR_LIBVO_SUB_VIDIX_CantConfigurePlayback "[VO_SUB_VIDIX] No pude configurar la reproducción: %s.\n"
#define MSGTR_LIBVO_SUB_VIDIX_CouldntFindWorkingVidixDriver "[VO_SUB_VIDIX] No pude encontrar un driver VIDIX que funcione.\n"
#define MSGTR_LIBVO_SUB_VIDIX_CouldntGetCapability "[VO_SUB_VIDIX] No pude obtener la capacidad: %s.\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: ¡No se pudo enviar evento de pantalla completa EWMH!\n"
#define MSGTR_SelectedVideoMode "XF86VM: Modo de video seleccionado %dx%d para tamaño de imagen %dx%d.\n"
#define MSGTR_InsertingAfVolume "[Mixer] No se ecnontró mezclador de volumen por hardware, insertando filtro de volumen.\n"
#define MSGTR_NoVolume "[Mixer] No hay un control de volumen disponible.\n"
#define MSGTR_NoBalance "[Mixer] No hay un control de balance disponible.\n"

// Controladores vo viejos que han sido reemplazados
#define MSGTR_VO_PGM_HasBeenReplaced "El controlador de salida pgm ha sido reemplazado por -vo pnm:pgmyuv.\n"
#define MSGTR_VO_MD5_HasBeenReplaced "El controlador de salida md5 ha sido reemplazado por -vo md5sum.\n"


// ======================= audio output drivers ========================

// audio_out.c
#define MSGTR_AO_ALSA9_1x_Removed "audio_out: los módulos alsa9 y alsa1x fueron eliminados, usa -ao alsa.\n"
#define MSGTR_AO_NoSuchDriver "No existe ese controlador de audio '%.*s'\n"
#define MSGTR_AO_FailedInit "Fallo al inicializar controlador de audio '%s'\n"

// ao_oss.c
#define MSGTR_AO_OSS_CantOpenMixer "[AO OSS] audio_setup: Imposible abrir dispositivo mezclador %s: %s\n"
#define MSGTR_AO_OSS_ChanNotFound "[AO OSS] audio_setup: El mezclador de la tarjeta de audio no tiene el canal '%s' usando valor por omisión.\n"
#define MSGTR_AO_OSS_CantOpenDev "[AO OSS] audio_setup: Imposible abrir dispositivo de audio %s: %s\n"
#define MSGTR_AO_OSS_CantMakeFd "[AO OSS] audio_setup: Imposible crear descriptor de archivo, bloqueando: %s\n"
#define MSGTR_AO_OSS_CantSet "[AO OSS] No puedo configurar el dispositivo de audio %s para salida %s, tratando %s...\n"
#define MSGTR_AO_OSS_CantSetChans "[AO OSS] audio_setup: Imposible configurar dispositivo de audio a %d channels.\n"
#define MSGTR_AO_OSS_CantUseGetospace "[AO OSS] audio_setup: El controlador no soporta SNDCTL_DSP_GETOSPACE :-(\n"
#define MSGTR_AO_OSS_CantUseSelect "[AO OSS]\n   ***  Su controlador de audio no soporta select()  ***\n Recompile MPlayer con #undef HAVE_AUDIO_SELECT en config.h !\n\n"
#define MSGTR_AO_OSS_CantReopen "[AO OSS]\n Error fatal: *** Imposible RE-ABRIR / RESETEAR DISPOSITIVO DE AUDIO *** %s\n"
#define MSGTR_AO_OSS_UnknownUnsupportedFormat "[AO OSS] Formato OSS Desconocido/No-soportado: %x.\n"

// ao_arts.c
#define MSGTR_AO_ARTS_CantInit "[AO ARTS] %s\n"
#define MSGTR_AO_ARTS_ServerConnect "[AO ARTS] Conectado al servidor de sonido.\n"
#define MSGTR_AO_ARTS_CantOpenStream "[AO ARTS] Imposible abrir stream.\n"
#define MSGTR_AO_ARTS_StreamOpen "[AO ARTS] Stream Abierto.\n"
#define MSGTR_AO_ARTS_BufferSize "[AO ARTS] Tamaño del buffer: %d\n"

// ao_dxr2.c
#define MSGTR_AO_DXR2_SetVolFailed "[AO DXR2] Fallo Seteando volumen en %d.\n"
#define MSGTR_AO_DXR2_UnsupSamplerate "[AO DXR2] dxr2: %d Hz no soportado, trate \"-aop list=resample\"\n"

// ao_esd.c
#define MSGTR_AO_ESD_CantOpenSound "[AO ESD] Fallo en esd_open_sound: %s\n"
#define MSGTR_AO_ESD_LatencyInfo "[AO ESD] latencia: [servidor: %0.2fs, red: %0.2fs] (ajuste %0.2fs)\n"
#define MSGTR_AO_ESD_CantOpenPBStream "[AO ESD] Fallo abriendo playback stream esd: %s\n"

// ao_mpegpes.c
#define MSGTR_AO_MPEGPES_CantSetMixer "[AO MPEGPES] Fallo configurando mezclador de audio DVB:%s\n"
#define MSGTR_AO_MPEGPES_UnsupSamplerate "[AO MPEGPES] %d Hz no soportado, trate de resamplear...\n"

// ao_pcm.c
#define MSGTR_AO_PCM_FileInfo "[AO PCM] Archivo: %s (%s)\nPCM: Samplerate: %iHz Canales: %s Formato %s\n"
#define MSGTR_AO_PCM_HintInfo "[AO PCM] Info: El volcado más rápido se logra con -vc null -vo null\nPCM: Info: Para escribir archivos de onda (WAVE) use -ao pcm:waveheader (valor por omisión).\n"
#define MSGTR_AO_PCM_CantOpenOutputFile "[AO PCM] Imposible abrir %s para escribir!\n"

// ao_sdl.c
#define MSGTR_AO_SDL_INFO "[AO SDL] Samplerate: %iHz Canales: %s Formato %s\n"
#define MSGTR_AO_SDL_DriverInfo "[AO SDL] usando controlador de audio: %s .\n"
#define MSGTR_AO_SDL_UnsupportedAudioFmt "[AO SDL] Formato de audio no soportado: 0x%x.\n"
#define MSGTR_AO_SDL_CantInit "[AO SDL] Error inicializando audio SDL: %s\n"
#define MSGTR_AO_SDL_CantOpenAudio "[AO SDL] Imposible abrir audio: %s\n"

// ao_sgi.c
#define MSGTR_AO_SGI_INFO "[AO SGI] control.\n"
#define MSGTR_AO_SGI_InitInfo "[AO SGI] init: Samplerate: %iHz Canales: %s Formato %s\n"
#define MSGTR_AO_SGI_InvalidDevice "[AO SGI] play: dispositivo inválido.\n"
#define MSGTR_AO_SGI_CantSetParms_Samplerate "[AO SGI] init: fallo en setparams: %s\nImposble configurar samplerate deseado.\n"
#define MSGTR_AO_SGI_CantSetAlRate "[AO SGI] init: AL_RATE No fue aceptado en el recurso dado.\n"
#define MSGTR_AO_SGI_CantGetParms "[AO SGI] init: fallo en getparams: %s\n"
#define MSGTR_AO_SGI_SampleRateInfo "[AO SGI] init: samplerate es ahora %f (el deseado es %f)\n"
#define MSGTR_AO_SGI_InitConfigError "[AO SGI] init: %s\n"
#define MSGTR_AO_SGI_InitOpenAudioFailed "[AO SGI] init: Imposible abrir canal de audio: %s\n"
#define MSGTR_AO_SGI_Uninit "[AO SGI] uninit: ...\n"
#define MSGTR_AO_SGI_Reset "[AO SGI] reseteando: ...\n"
#define MSGTR_AO_SGI_PauseInfo "[AO SGI] audio_pausa: ...\n"
#define MSGTR_AO_SGI_ResumeInfo "[AO SGI] audio_continuar: ...\n"

// ao_sun.c
#define MSGTR_AO_SUN_RtscSetinfoFailed "[AO SUN] rtsc: Fallo en SETINFO.\n"
#define MSGTR_AO_SUN_RtscWriteFailed "[AO SUN] rtsc: Fallo escribiendo."
#define MSGTR_AO_SUN_CantOpenAudioDev "[AO SUN] Imposible abrir dispositivo de audio %s, %s -> nosound.\n"
#define MSGTR_AO_SUN_UnsupSampleRate "[AO SUN] audio_setup: Su tarjeta no soporta el canal %d, %s, %d Hz samplerate.\n"

// ao_alsa.c
#define MSGTR_AO_ALSA_InvalidMixerIndexDefaultingToZero "[AO_ALSA] Índice del mezclador inválido. Usando 0.\n"
#define MSGTR_AO_ALSA_MixerOpenError "[AO_ALSA] Mezclador, error abriendo: %s\n"
#define MSGTR_AO_ALSA_MixerAttachError "[AO_ALSA] Mezclador, adjunto %s error: %s\n"
#define MSGTR_AO_ALSA_MixerRegisterError "[AO_ALSA] Mezclador, error de registro: %s\n"
#define MSGTR_AO_ALSA_MixerLoadError "[AO_ALSA] Mezclador, error de carga: %s\n"
#define MSGTR_AO_ALSA_UnableToFindSimpleControl "[AO_ALSA] Incapaz de encontrar un control simple '%s',%i.\n"
#define MSGTR_AO_ALSA_ErrorSettingLeftChannel "[AO_ALSA] Error estableciendo el canal izquierdo, %s\n"
#define MSGTR_AO_ALSA_ErrorSettingRightChannel "[AO_ALSA] Error estableciendo el canal derecho, %s\n"
#define MSGTR_AO_ALSA_CommandlineHelp "\n[AO_ALSA] -ao alsa ayuda línea de comandos:\n"\
"[AO_ALSA] Ejemplo: mplayer -ao alsa:dispositivo=hw=0.3\n"\
"[AO_ALSA]   Establece como primera tarjeta el cuarto dispositivo de hardware.\n\n"\
"[AO_ALSA] Opciones:\n"\
"[AO_ALSA]   noblock\n"\
"[AO_ALSA]     Abre el dispositivo en modo sin bloqueo.\n"\
"[AO_ALSA]   device=<nombre-dispositivo>\n"\
"[AO_ALSA]     Establece el dispositivo (cambiar , por . y : por =)\n"
#define MSGTR_AO_ALSA_ChannelsNotSupported "[AO_ALSA] %d canales no están soportados.\n"
#define MSGTR_AO_ALSA_OpenInNonblockModeFailed "[AO_ALSA] La apertura en modo sin bloqueo ha fallado, intentando abrir en modo bloqueo.\n"
#define MSGTR_AO_ALSA_PlaybackOpenError "[AO_ALSA] Error de apertura en la reproducción: %s\n"
#define MSGTR_AO_ALSA_ErrorSetBlockMode "[AL_ALSA] Error estableciendo el modo bloqueo %s.\n"
#define MSGTR_AO_ALSA_UnableToGetInitialParameters "[AO_ALSA] Incapaz de obtener los parámetros iniciales: %s\n"
#define MSGTR_AO_ALSA_UnableToSetAccessType "[AO_ALSA] Incapaz de establecer el tipo de acceso: %s\n"
#define MSGTR_AO_ALSA_FormatNotSupportedByHardware "[AO_ALSA] Formato %s no soportado por el hardware, intentando la opción por defecto.\n"
#define MSGTR_AO_ALSA_UnableToSetFormat "[AO_ALSA] Incapaz de establecer el formato: %s\n"
#define MSGTR_AO_ALSA_UnableToSetChannels "[AO_ALSA] Incapaz de establecer los canales: %s\n"
#define MSGTR_AO_ALSA_UnableToDisableResampling "[AO_ALSA] Incapaz de deshabilitar el resampling:  %s\n"
#define MSGTR_AO_ALSA_UnableToSetSamplerate2 "[AO_ALSA] Incapaz de establecer el samplerate-2: %s\n"
#define MSGTR_AO_ALSA_UnableToSetBufferTimeNear "[AO_ALSA] Incapaz de establecer el tiempo del buffer: %s\n"
#define MSGTR_AO_ALSA_UnableToGetPeriodSize "[AO ALSA] Incapaz de obtener el tamaño del período: %s\n"
#define MSGTR_AO_ALSA_UnableToSetPeriods "[AO_ALSA] Incapaz de establecer períodos: %s\n"
#define MSGTR_AO_ALSA_UnableToSetHwParameters "[AO_ALSA] Incapaz de establecer parámatros de hw: %s\n"
#define MSGTR_AO_ALSA_UnableToGetBufferSize "[AO_ALSA] Incapaz de obtener el tamaño del buffer: %s\n"
#define MSGTR_AO_ALSA_UnableToGetSwParameters "[AO_ALSA] Incapaz de obtener parámatros de sw: %s\n"
#define MSGTR_AO_ALSA_UnableToGetBoundary "[AO_ALSA] Incapaz de obtener el límite: %s\n"
#define MSGTR_AO_ALSA_UnableToSetStartThreshold "[AO_ALSA] Incapaz de establecer el umbral de comienzo: %s\n"
#define MSGTR_AO_ALSA_UnableToSetStopThreshold "[AO_ALSA] Incapaz de establecer el umbral de parada: %s\n"
#define MSGTR_AO_ALSA_UnableToSetSilenceSize "[AO_ALSA] Incapaz de establecer el tamaño del silencio: %s\n"
#define MSGTR_AO_ALSA_PcmCloseError "[AO_ALSA] pcm error de clausura: %s\n"
#define MSGTR_AO_ALSA_NoHandlerDefined "[AO_ALSA] ¡Ningún manejador definido!\n"
#define MSGTR_AO_ALSA_PcmPrepareError "[AO_ALSA] pcm error de preparación: %s\n"
#define MSGTR_AO_ALSA_PcmPauseError "[AO_ALSA] pcm error de pausa: %s\n"
#define MSGTR_AO_ALSA_PcmDropError "[AO_ALSA] pcm error de pérdida: %s\n"
#define MSGTR_AO_ALSA_PcmResumeError "[AO_ALSA] pcm error volviendo al estado normal: %s\n"
#define MSGTR_AO_ALSA_DeviceConfigurationError "[AO_ALSA] Error de configuración del dispositivo."
#define MSGTR_AO_ALSA_PcmInSuspendModeTryingResume "[AO_ALSA] Pcm en modo suspendido, intentando volver al estado normal.\n"
#define MSGTR_AO_ALSA_WriteError "[AO_ALSA] Error de escritura: %s\n"
#define MSGTR_AO_ALSA_TryingToResetSoundcard "[AO_ALSA] Intentando resetear la tarjeta de sonido.\n"
#define MSGTR_AO_ALSA_CannotGetPcmStatus "[AO_ALSA] No se puede obtener el estado pcm: %s\n"

// ao_plugin.c


// ======================= audio filters ================================

// af_scaletempo.c
#define MSGTR_AF_ValueOutOfRange MSGTR_VO_ValueOutOfRange

// af_ladspa.c
#define MSGTR_AF_LADSPA_AvailableLabels "Etiquetas disponibles en"
#define MSGTR_AF_LADSPA_WarnNoInputs "ADVERTENCIA! Este plugin LADSPA no tiene entradas de audio.\n La señal de entrada de audio se perderá."
#define MSGTR_AF_LADSPA_ErrNoOutputs "Este plugín LADSPA no tiene salidas de audio."
#define MSGTR_AF_LADSPA_ErrInOutDiff "El número de entradas y de salidas  de audio de este plugin LADSPA son distintas."
#define MSGTR_AF_LADSPA_ErrFailedToLoad "Fallo la carga."
#define MSGTR_AF_LADSPA_ErrNoDescriptor "No se pudo encontrar la función ladspa_descriptor() en el archivo de biblioteca especificado."
#define MSGTR_AF_LADSPA_ErrLabelNotFound "No se puede encontrar la etiqueta en la biblioteca del plugin."
#define MSGTR_AF_LADSPA_ErrNoSuboptions "No se especificaron subopciones"
#define MSGTR_AF_LADSPA_ErrNoLibFile "No se especificó archivo de biblioteca"
#define MSGTR_AF_LADSPA_ErrNoLabel "No se especificó etiqueta del filtro"
#define MSGTR_AF_LADSPA_ErrNotEnoughControls "No se especificaron suficientes controles en la línea de comando"
#define MSGTR_AF_LADSPA_ErrControlBelow "%s: Control de entrada #%d esta abajo del límite inferior que es %0.4f.\n"
#define MSGTR_AF_LADSPA_ErrControlAbove "%s: Control de entrada #%d esta por encima del límite superior que es %0.4f.\n"

// format.c
#define MSGTR_AF_FORMAT_UnknownFormat "formato desconocido "


// ========================== INPUT =========================================

// joystick.c
#define MSGTR_INPUT_JOYSTICK_CantOpen "Imposible abrir joystick %s : %s\n"
#define MSGTR_INPUT_JOYSTICK_ErrReading "Error leyendo dispositivo joystick : %s\n"
#define MSGTR_INPUT_JOYSTICK_LoosingBytes "Joystick : perdimos %d bytes de datos\n"
#define MSGTR_INPUT_JOYSTICK_WarnLostSync "Joystick : Advertencia, init event, perdimos sync con el driver\n"
#define MSGTR_INPUT_JOYSTICK_WarnUnknownEvent "Joystick : Advertencia, tipo de evento desconocido %d\n"

// appleir.c
#define MSGTR_INPUT_APPLE_IR_CantOpen "Imposible abrir dispositivo Apple IR: %s\n"

// input.c
#define MSGTR_INPUT_INPUT_ErrCantRegister2ManyCmdFds "Demaciados fds de comandos, imposible registrar fd %d.\n"
#define MSGTR_INPUT_INPUT_ErrCantRegister2ManyKeyFds "Demaciados fds de teclas, imposible registrar fd %d.\n"
#define MSGTR_INPUT_INPUT_ErrArgMustBeInt "Comando %s: argumento %d no es un entero.\n"
#define MSGTR_INPUT_INPUT_ErrArgMustBeFloat "Comando %s: argumento %d no es punto flotante.\n"
#define MSGTR_INPUT_INPUT_ErrUnterminatedArg "Commando %s: argumento %d no está terminado.\n"
#define MSGTR_INPUT_INPUT_ErrUnknownArg "Argumento desconocido %d\n"
#define MSGTR_INPUT_INPUT_Err2FewArgs "Comando %s requiere a lo menos %d argumentos, solo encontramos %d hasta ahora.\n"
#define MSGTR_INPUT_INPUT_ErrReadingCmdFd "Error leyendo fd de comandos %d: %s\n"
#define MSGTR_INPUT_INPUT_ErrCmdBufferFullDroppingContent "Buffer de comandos de fd %d lleno: botando contenido\n"
#define MSGTR_INPUT_INPUT_ErrInvalidCommandForKey "Comando inválido asignado a la tecla %s"
#define MSGTR_INPUT_INPUT_ErrSelect "Error en Select: %s\n"
#define MSGTR_INPUT_INPUT_ErrOnKeyInFd "Error en fd %d de entrada de teclas\n"
#define MSGTR_INPUT_INPUT_ErrDeadKeyOnFd "Ingreso de tecla muerta en fd %d\n"
#define MSGTR_INPUT_INPUT_Err2ManyKeyDowns "Demaciados eventos de keydown al mismo tiempo\n"
#define MSGTR_INPUT_INPUT_ErrOnCmdFd "Error en fd de comandos %d\n"
#define MSGTR_INPUT_INPUT_ErrReadingInputConfig "Error leyendo archivo de configuración de input %s: %s\n"
#define MSGTR_INPUT_INPUT_ErrUnknownKey "Tecla desconocida '%s'\n"
#define MSGTR_INPUT_INPUT_ErrBuffer2SmallForKeyName "El buffer es demaciado pequeño para este nombre de tecla: %s\n"
#define MSGTR_INPUT_INPUT_ErrNoCmdForKey "No se econtró un comando para la tecla %s"
#define MSGTR_INPUT_INPUT_ErrBuffer2SmallForCmd "Buffer demaciado pequeño para el comando %s\n"
#define MSGTR_INPUT_INPUT_ErrCantInitJoystick "No se puede inicializar la entrada del joystick\n"
#define MSGTR_INPUT_INPUT_ErrCantOpenFile "No se puede abrir %s: %s\n"
#define MSGTR_INPUT_INPUT_ErrCantInitAppleRemote "No se puede inicializar la entrada del Apple Remote.\n"

// lirc.c
#define MSGTR_LIRCopenfailed "Fallo al abrir el soporte para LIRC.\n"
#define MSGTR_LIRCcfgerr "Fallo al leer archivo de configuración de LIRC %s.\n"


// ========================== LIBMPDEMUX ===================================

// muxer.c, muxer_*.c
#define MSGTR_TooManyStreams "¡Demasiados streams!"
#define MSGTR_RawMuxerOnlyOneStream "El muxer rawaudio soporta sólo un stream de audio!\n"
#define MSGTR_IgnoringVideoStream "Ignorando stream de video!\n"
#define MSGTR_UnknownStreamType "Advertencia! tipo de stream desconocido: %d\n"
#define MSGTR_WarningLenIsntDivisible "¡Advertencia! ¡La longitud no es divisible por el tamaño del muestreo!\n"
#define MSGTR_MuxbufMallocErr "No se puede asignar memoria para el frame buffer del Muxer!\n"
#define MSGTR_MuxbufReallocErr "No se puede reasignar memoria para el frame buffer de del Muxer!\n"
#define MSGTR_WritingHeader "Escribiendo la cabecera...\n"
#define MSGTR_WritingTrailer "Escribiendo el índice...\n"

// demuxer.c, demux_*.c
#define MSGTR_AudioStreamRedefined "Advertencia! Cabecera de stream de audio %d redefinida!\n"
#define MSGTR_VideoStreamRedefined "Advertencia! Cabecera de stream de video %d redefinida!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: Demasiados (%d en %d bytes) paquetes de audio en el buffer!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: Demasiados (%d en %d bytes) paquetes de video en el buffer!\n"
#define MSGTR_MaybeNI "¿Estás reproduciendo un stream o archivo 'non-interleaved' o falló el codec?\n " \
                "Para archivos .AVI, intente forzar el modo 'non-interleaved' con la opción -ni.\n"
#define MSGTR_WorkAroundBlockAlignHeaderBug "AVI: Rodeo CBR-MP3 nBlockAlign"
#define MSGTR_SwitchToNi "\nDetectado .AVI mal interleaveado - cambiando al modo -ni!\n"
#define MSGTR_InvalidAudioStreamNosound "AVI: flujo de audio inválido ID: %d - ignorado (sin sonido)\n"
#define MSGTR_InvalidAudioStreamUsingDefault "AVI: flujo de audio inválido ID: %d - ignorado (usando default)\n"
#define MSGTR_ON2AviFormat "Formato ON2 AVI"
#define MSGTR_Detected_XXX_FileFormat "Detectado formato de archivo %s.\n"
#define MSGTR_FormatNotRecognized "Este formato no está soportado o reconocido. Si este archivo es un AVI, ASF o MPEG, por favor contacte con el autor.\n"
#define MSGTR_SettingProcessPriority "Estableciendo la prioridad del proceso: %s\n"
#define MSGTR_FilefmtFourccSizeFpsFtime "[V] filefmt:%d  fourcc:0x%X  tamaño:%dx%d  fps:%5.3f  ftime:=%6.4f\n"
#define MSGTR_CannotInitializeMuxer "No se puede inicializar el muxer."
#define MSGTR_MissingVideoStream "¡No se encontró stream de video!\n"
#define MSGTR_MissingAudioStream "No se encontró el stream de audio, no se reproducirá sonido.\n"
#define MSGTR_MissingVideoStreamBug "¡¿Stream de video perdido!? Contacta con el autor, podría ser un fallo.\n"

#define MSGTR_DoesntContainSelectedStream "demux: El archivo no contiene el stream de audio o video seleccionado.\n"

#define MSGTR_NI_Forced "Forzado"
#define MSGTR_NI_Detected "Detectado"
#define MSGTR_NI_Message "%s formato de AVI 'NON-INTERLEAVED'.\n"

#define MSGTR_UsingNINI "Usando formato de AVI arruinado 'NON-INTERLEAVED'.\n"
#define MSGTR_CouldntDetFNo "No se puede determinar el número de cuadros (para una búsqueda absoluta).\n"
#define MSGTR_CantSeekRawAVI "No se puede avanzar o retroceder en un stream crudo .AVI (se requiere índice, prueba con -idx).\n"
#define MSGTR_CantSeekFile "No se puede avanzar o retroceder en este archivo.\n"
#define MSGTR_MOVcomprhdr "MOV: ¡Soporte de Cabecera comprimida requiere ZLIB!.\n"
#define MSGTR_MOVvariableFourCC "MOV: Advertencia. ¡Variable FOURCC detectada!\n"
#define MSGTR_MOVtooManyTrk "MOV: Advertencia. ¡Demasiadas pistas!"
#define MSGTR_ErrorOpeningOGGDemuxer "No se puede abrir el demuxer ogg.\n"
#define MSGTR_CannotOpenAudioStream "No se puede abrir stream de audio: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "No se puede abrir stream de subtítulos: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "No se pudo abrir el demuxer de audio: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "No se pudo abrir demuxer de subtítulos: %s\n"
#define MSGTR_TVInputNotSeekable "No se puede buscar en la entrada de TV.\n"
#define MSGTR_DemuxerInfoChanged "Demuxer la información %s ha cambiado a %s\n"
#define MSGTR_ClipInfo "Información de clip: \n"
#define MSGTR_LeaveTelecineMode "\ndemux_mpg: contenido NTSC de 30000/1001cps detectado, cambiando cuadros por segundo.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: contenido NTSC progresivo de 24000/1001cps detectado, cambiando cuadros por segundo.\n"
#define MSGTR_CacheFill "\rLlenando cache: %5.2f%% (%"PRId64" bytes)   "
#define MSGTR_NoBindFound "No se econtró una asignación para la tecla '%s'\n"
#define MSGTR_FailedToOpen "No se pudo abrir %s\n"
#define MSGTR_VideoID "[%s] Stream de video encontrado, -vid %d\n"
#define MSGTR_AudioID "[%s] Stream de audio encontrado, -aid %d\n"
#define MSGTR_SubtitleID "[%s] Stream de subtítulos encontrado, -sid %d\n"

// asfheader.c
#define MSGTR_MPDEMUX_ASFHDR_HeaderSizeOver1MB "FATAL: más de 1 MB de tamaño del header (%d)!\nPor favor contacte a los autores de MPlayer y suba/envie este archivo.\n"
#define MSGTR_MPDEMUX_ASFHDR_HeaderMallocFailed "Imposible obtener %d bytes para la cabecera\n"
#define MSGTR_MPDEMUX_ASFHDR_EOFWhileReadingHeader "EOF Mientras leia la cabecera ASF, archivo dañado o incompleto?\n"
#define MSGTR_MPDEMUX_ASFHDR_DVRWantsLibavformat "DVR quizas solo funcione con libavformat, intente con -demuxer 35 si tiene problemas\n"
#define MSGTR_MPDEMUX_ASFHDR_NoDataChunkAfterHeader "No hay un chunk de datos despues de la cabecera!\n"
#define MSGTR_MPDEMUX_ASFHDR_AudioVideoHeaderNotFound "ASF: No encuentro cabeceras de audio o video, archivo dañado?\n"
#define MSGTR_MPDEMUX_ASFHDR_InvalidLengthInASFHeader "Largo inválido en la cabecera ASF!\n"
#define MSGTR_MPDEMUX_ASFHDR_DRMLicenseURL "URL de la licencia DRM: %s\n"
#define MSGTR_MPDEMUX_ASFHDR_DRMProtected "Este archivo tiene DRM encription, MPlayer no lo va a reproducir!\n"

// aviheader.c
#define MSGTR_MPDEMUX_AVIHDR_EmptyList "** Lista vacia?!\n"
#define MSGTR_MPDEMUX_AVIHDR_WarnNotExtendedAVIHdr "Advertencia: esta no es una cabecera AVI extendida..\n"
#define MSGTR_MPDEMUX_AVIHDR_BuildingODMLidx "AVI: ODML: Construyendo el índice odml (%d superindexchunks).\n"
#define MSGTR_MPDEMUX_AVIHDR_BrokenODMLfile "AVI: ODML: Archivo arruinado (incompleto?) detectado. Utilizaré el índice tradicional.\n"
#define MSGTR_MPDEMUX_AVIHDR_CantReadIdxFile "No pude leer el archivo de índice %s: %s\n"
#define MSGTR_MPDEMUX_AVIHDR_NotValidMPidxFile "%s No es un archivo de índice MPlayer válido.\n"
#define MSGTR_MPDEMUX_AVIHDR_FailedMallocForIdxFile "Imposible disponer de memoria suficiente para los datos de índice de %s\n"
#define MSGTR_MPDEMUX_AVIHDR_PrematureEOF "El archivo de índice termina prematuramente %s\n"
#define MSGTR_MPDEMUX_AVIHDR_IdxFileLoaded "El archivo de índice: %s fue cargado.\n"
#define MSGTR_MPDEMUX_AVIHDR_GeneratingIdx "Generando Indice: %3lu %s     \r"
#define MSGTR_MPDEMUX_AVIHDR_IdxGeneratedForHowManyChunks "AVI: tabla de índices generada para %d chunks!\n"
#define MSGTR_MPDEMUX_AVIHDR_Failed2WriteIdxFile "Imposible escribir el archivo de índice %s: %s\n"
#define MSGTR_MPDEMUX_AVIHDR_IdxFileSaved "Archivo de índice guardado: %s\n"

// demux_audio.c
#define MSGTR_MPDEMUX_AUDIO_UnknownFormat "Audio demuxer: Formato desconocido %d.\n"

// demux_demuxers.c
#define MSGTR_MPDEMUX_DEMUXERS_FillBufferError "fill_buffer error: demuxer erroneo: no vd, ad o sd.\n"

// demux_mkv.c
#define MSGTR_MPDEMUX_MKV_ZlibInitializationFailed "[mkv] zlib la inicialización ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_ZlibDecompressionFailed "[mkv] zlib la descompresión ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_LzoDecompressionFailed "[mkv] lzo la descompresión ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_TrackEncrypted "[mkv] La pista número %u ha sido encriptada y la desencriptación no ha sido\n[mkv] todavía implementada. Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_UnknownContentEncoding "[mkv] Tipo de codificación desconocida para la pista %u. Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_UnknownCompression "[mkv] La pista %u ha sido comprimida con un algoritmo de compresión\n[mkv] desconocido o no soportado algoritmo (%u). Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_ZlibCompressionUnsupported "[mkv] La pista %u ha sido comprimida con zlib pero mplayer no ha sido\n[mkv] compilado con soporte para compresión con zlib. Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_TrackIDName "[mkv] Pista ID %u: %s (%s) \"%s\", %s\n"
#define MSGTR_MPDEMUX_MKV_TrackID "[mkv] Pista ID %u: %s (%s), %s\n"
#define MSGTR_MPDEMUX_MKV_UnknownCodecID "[mkv] CodecID (%s) desconocido/no soportado/erróneo/faltante Codec privado\n[mkv] datos (pista %u).\n"
#define MSGTR_MPDEMUX_MKV_FlacTrackDoesNotContainValidHeaders "[mkv] La pista FLAC no contiene cabeceras válidas.\n"
#define MSGTR_MPDEMUX_MKV_UnknownAudioCodec "[mkv] Codec de audio ID '%s' desconocido/no soportado para la pista %u\n[mkv] o erróneo/falta Codec privado.\n"
#define MSGTR_MPDEMUX_MKV_SubtitleTypeNotSupported "[mkv] El tipo de subtítulos '%s' no está soportado.\n"
#define MSGTR_MPDEMUX_MKV_WillPlayVideoTrack "[mkv] Reproducirá la pista de vídeo %u.\n"
#define MSGTR_MPDEMUX_MKV_NoVideoTrackFound "[mkv] Ninguna pista de vídeo encontrada/deseada.\n"
#define MSGTR_MPDEMUX_MKV_NoAudioTrackFound "[mkv] Ninguna pista de sonido encontrada/deseada.\n"
#define MSGTR_MPDEMUX_MKV_NoBlockDurationForSubtitleTrackFound "[mkv] Aviso: No se ha encontrado la duración del bloque para los subtítulos de la pista.\n"

// demux_nuv.c

// demux_xmms.c
#define MSGTR_MPDEMUX_XMMS_FoundPlugin "Plugin encontrado: %s (%s).\n"
#define MSGTR_MPDEMUX_XMMS_ClosingPlugin "Cerrando plugin: %s.\n"
#define MSGTR_MPDEMUX_XMMS_WaitForStart "Esperando a que el plugin XMMS comience la reprodución de '%s'...\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "Dispositivo de CD-ROM '%s' no encontrado.\n"
#define MSGTR_ErrTrackSelect "Error seleccionando la pista de VCD!"
#define MSGTR_ReadSTDIN "Leyendo desde la entrada estándar (stdin)...\n"
#define MSGTR_FileNotFound "Archivo no encontrado: '%s'\n"

#define MSGTR_SMBInitError "No se puede inicializar la librería libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "No se puede abrir desde la RED: '%s'\n"

#define MSGTR_CantOpenDVD "No se puede abrir el dispositivo de DVD: %s (%s)\n"

// stream_dvd.c
#define MSGTR_DVDspeedCantOpen "No se ha podido abrir el dispositivo de DVD para escritura, cambiar la velocidad del DVD requiere acceso de escritura\n"
#define MSGTR_DVDrestoreSpeed "Restableciendo la velocidad del DVD"
#define MSGTR_DVDlimitSpeed "Limitando la velocidad del DVD a %dKB/s... "
#define MSGTR_DVDlimitFail "La limitación de la velocidad del DVD ha fallado.\n"
#define MSGTR_DVDlimitOk "Se ha limitado la velocidad del DVD con éxito.\n"
#define MSGTR_NoDVDSupport "MPlayer fue compilado sin soporte para DVD, saliendo.\n"
#define MSGTR_DVDnumTitles "Hay %d títulos en este DVD.\n"
#define MSGTR_DVDinvalidTitle "Número de título de DVD inválido: %d\n"
#define MSGTR_DVDinvalidChapterRange "Especificación inválida de rango de capítulos %s\n"
#define MSGTR_DVDnumAngles "Hay %d ángulos en este título de DVD.\n"
#define MSGTR_DVDinvalidAngle "Número de ángulo de DVD inválido: %d\n"
#define MSGTR_DVDnoIFO "No se pudo abrir archivo IFO para el título de DVD %d.\n"
#define MSGTR_DVDnoVMG "No se pudo abrir la información VMG!\n"
#define MSGTR_DVDnoVOBs "No se pudo abrir VOBS del título (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDnoMatchingAudio "DVD, no se encontró un idioma coincidente!\n"
#define MSGTR_DVDaudioChannel "DVD, canal de audio seleccionado: %d idioma: %c%c\n"
#define MSGTR_DVDaudioStreamInfo "stream de audio: %d formato: %s (%s) idioma: %s aid: %d.\n"
#define MSGTR_DVDnumAudioChannels "Número de canales de audio en el disco: %d.\n"
#define MSGTR_DVDnoMatchingSubtitle "DVD, no se encontró un idioma de subtitulo coincidente!\n"
#define MSGTR_DVDsubtitleChannel "DVD, canal de subtitulos seleccionado: %d idioma: %c%c\n"
#define MSGTR_DVDsubtitleLanguage "subtítulo ( sid ): %d idioma: %s\n"
#define MSGTR_DVDnumSubtitles "Número de subtítulos en el disco: %d\n"


// dec_video.c & dec_audio.c
#define MSGTR_CantOpenCodec "No se pudo abrir codec.\n"
#define MSGTR_CantCloseCodec "No se pudo cerrar codec.\n"

#define MSGTR_MissingDLLcodec "ERROR: No se pudo abrir el codec DirectShow requerido: %s\n"
#define MSGTR_ACMiniterror "No se puede cargar/inicializar codecs de audio Win32/ACM (falta archivo DLL?)\n"
#define MSGTR_MissingLAVCcodec "No se encuentra codec '%s' en libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF mientras buscaba la cabecera de secuencia.\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: No se puede leer cabecera de secuencia.\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: No se puede leer la extensión de la cabecera de secuencia.\n"
#define MSGTR_BadMpegSequHdr "MPEG: Mala cabecera de secuencia.\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Mala extensión de la cabecera de secuencia.\n"

#define MSGTR_ShMemAllocFail "No se puede alocar memoria compartida.\n"
#define MSGTR_CantAllocAudioBuf "No se puede alocar buffer de la salida de audio.\n"
#define MSGTR_UnknownAudio "Formato de audio desconocido/faltante, no se reproducirá sonido.\n"


#define MSGTR_UsingExternalPP "[PP] Usando filtro de postprocesado externo, max q = %d.\n"
#define MSGTR_UsingCodecPP "[PP] Usando postprocesado del codec, max q = %d.\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Familia de codec de video solicitada [%s] (vfm=%s) no está disponible (actívalo al compilar).\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Familia de codec de audio solicitada [%s] (afm=%s) no está disponible (actívalo al compilar).\n"
#define MSGTR_OpeningVideoDecoder "Abriendo decodificador de video: [%s] %s.\n"
#define MSGTR_SelectedVideoCodec "Video codec seleccionado: [%s] vfm: %s (%s)\n"
#define MSGTR_OpeningAudioDecoder "Abriendo decodificador de audio: [%s] %s.\n"
#define MSGTR_SelectedAudioCodec "Audio codec seleccionado: [%s] afm: %s (%s)\n"
#define MSGTR_VDecoderInitFailed "Inicialización del VDecoder ha fallado.\n"
#define MSGTR_ADecoderInitFailed "Inicialización del ADecoder ha fallado.\n"
#define MSGTR_ADecoderPreinitFailed "Preinicialización del ADecoder ha fallado.\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "No se pudo encontrar el filtro de video '%s'.\n"
#define MSGTR_CouldNotOpenVideoFilter "No se pudo abrir el filtro de video '%s'.\n"
#define MSGTR_OpeningVideoFilter "Abriendo filtro de video: "
#define MSGTR_CannotFindColorspace "No se pudo encontrar espacio de color concordante, ni siquiera insertando 'scale' :(.\n"

// vd.c
#define MSGTR_CouldNotFindColorspace "No se pudo encontrar colorspace concordante - reintentando escalado -vf...\n"
#define MSGTR_MovieAspectIsSet "Aspecto es %.2f:1 - prescalando a aspecto correcto.\n"
#define MSGTR_MovieAspectUndefined "Aspecto de película no es definido - no se ha aplicado prescalado.\n"

// vd_dshow.c, vd_dmo.c
#define MSGTR_DownloadCodecPackage "Necesita actualizar/instalar el paquete binario con codecs.\n Dirijase a http://www.mplayerhq.hu/dload.html\n"


// url.c
#define MSGTR_MPDEMUX_URL_StringAlreadyEscaped "Al parecer el string ya ha sido escapado en url_scape %c%c%c\n"

// ai_alsa.c
#define MSGTR_MPDEMUX_AIALSA_CannotSetSamplerate "No puedo setear el samplerate.\n"
#define MSGTR_MPDEMUX_AIALSA_CannotSetBufferTime "No puedo setear el tiempo del buffer.\n"
#define MSGTR_MPDEMUX_AIALSA_CannotSetPeriodTime "No puedo setear el tiempo del periodo.\n"

// ai_alsa.c
#define MSGTR_MPDEMUX_AIALSA_PcmBrokenConfig "Configuración erronea para este PCM: no hay configuraciones disponibles.\n"
#define MSGTR_MPDEMUX_AIALSA_UnavailableAccessType "Tipo de acceso no disponible.\n"
#define MSGTR_MPDEMUX_AIALSA_UnavailableSampleFmt "Formato de muestreo no disponible.\n"
#define MSGTR_MPDEMUX_AIALSA_UnavailableChanCount "Conteo de canales no disponible, usando el valor por omisión: %d\n"
#define MSGTR_MPDEMUX_AIALSA_CannotInstallHWParams "Imposible instalar los parametros de hardware: %s"
#define MSGTR_MPDEMUX_AIALSA_PeriodEqualsBufferSize "Imposible usar un periodo igual al tamaño del buffer (%u == %lu).\n"
#define MSGTR_MPDEMUX_AIALSA_CannotInstallSWParams "Imposible instalar los parametros de software:\n"
#define MSGTR_MPDEMUX_AIALSA_ErrorOpeningAudio "Error tratando de abrir el sonido: %s\n"
#define MSGTR_MPDEMUX_AIALSA_AlsaXRUN "ALSA xrun!!! (por lo menos %.3f ms de largo)\n"
#define MSGTR_MPDEMUX_AIALSA_AlsaXRUNPrepareError "ALSA xrun: error de preparación: %s"
#define MSGTR_MPDEMUX_AIALSA_AlsaReadWriteError "ALSA Error de lectura/escritura"

// ai_oss.c

#define MSGTR_MPDEMUX_AIOSS_Unable2SetChanCount "Imposible setear el conteo de canales: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2SetStereo "Imposible setear stereo: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2Open "Imposible abrir '%s': %s\n"
#define MSGTR_MPDEMUX_AIOSS_UnsupportedFmt "Formato no soportado\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2SetAudioFmt "Imposible setear formato de audio."
#define MSGTR_MPDEMUX_AIOSS_Unable2SetSamplerate "Imposible setear el samplerate: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2SetTrigger "Imposible setear el trigger: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2GetBlockSize "No pude obtener el tamaño del bloque!\n"
#define MSGTR_MPDEMUX_AIOSS_AudioBlockSizeZero "El tamaño del bloque de audio es cero, utilizando %d!\n"
#define MSGTR_MPDEMUX_AIOSS_AudioBlockSize2Low "Tamaño del bloque de audio muy bajo, utilizando %d!\n"

// asf_mmst_streaming.c

#define MSGTR_MPDEMUX_MMST_WriteError "Error de escritura\n"
#define MSGTR_MPDEMUX_MMST_EOFAlert "\nAlerta! EOF\n"
#define MSGTR_MPDEMUX_MMST_PreHeaderReadFailed "Falló la lectura pre-header\n"
#define MSGTR_MPDEMUX_MMST_InvalidHeaderSize "Tamaño de cabecera inválido, me rindo!\n"
#define MSGTR_MPDEMUX_MMST_HeaderDataReadFailed "Falló la lectura de los datos de la cabecera\n"
#define MSGTR_MPDEMUX_MMST_packet_lenReadFailed "Falló la lectura del packet_len\n"
#define MSGTR_MPDEMUX_MMST_InvalidRTSPPacketSize "Tamaño inválido de paquete RTSP, me rindo!\n"
#define MSGTR_MPDEMUX_MMST_CmdDataReadFailed "Fallo el comando 'leer datos'.\n"
#define MSGTR_MPDEMUX_MMST_HeaderObject "Objeto de cabecera.\n"
#define MSGTR_MPDEMUX_MMST_DataObject "Objeto de datos.\n"
#define MSGTR_MPDEMUX_MMST_FileObjectPacketLen "Objeto de archivo, largo del paquete = %d (%d)\n"
#define MSGTR_MPDEMUX_MMST_StreamObjectStreamID "Objeto de stream, id: %d\n"
#define MSGTR_MPDEMUX_MMST_2ManyStreamID "Demaciados id, ignorando stream"
#define MSGTR_MPDEMUX_MMST_UnknownObject "Objeto desconocido\n"
#define MSGTR_MPDEMUX_MMST_MediaDataReadFailed "Falló la lectura de los datos desde el medio\n"
#define MSGTR_MPDEMUX_MMST_MissingSignature "Firma no encontrada.\n"
#define MSGTR_MPDEMUX_MMST_PatentedTechnologyJoke "Todo listo, gracias por bajar un archivo de medios que contiene tecnología patentada y propietaria ;)\n"
#define MSGTR_MPDEMUX_MMST_UnknownCmd "Comando desconocido %02x\n"
#define MSGTR_MPDEMUX_MMST_GetMediaPacketErr "Error en get_media_packet: %s\n"
#define MSGTR_MPDEMUX_MMST_Connected "Conectado.\n"

// asf_streaming.c

#define MSGTR_MPDEMUX_ASF_StreamChunkSize2Small "Ahhhh, el tamaño del stream_chunck es muy pequeño: %d\n"
#define MSGTR_MPDEMUX_ASF_SizeConfirmMismatch "size_confirm erroneo!: %d %d\n"
#define MSGTR_MPDEMUX_ASF_WarnDropHeader "Advertencia : botar la cabecera ????\n"
#define MSGTR_MPDEMUX_ASF_ErrorParsingChunkHeader "Error mientras se parseaba la cabecera del chunk.\n"
#define MSGTR_MPDEMUX_ASF_NoHeaderAtFirstChunk "No me llego la cabecera como primer chunk !!!!\n"
#define MSGTR_MPDEMUX_ASF_BufferMallocFailed "Error imposible obtener un buffer de %d bytes\n"
#define MSGTR_MPDEMUX_ASF_ErrReadingNetworkStream "Error leyendo el network stream\n"
#define MSGTR_MPDEMUX_ASF_ErrChunk2Small "Error, el chunk es muy pequeño\n"
#define MSGTR_MPDEMUX_ASF_Bandwidth2SmallCannotPlay "Imposible mostrarte este archivo con tu bandwidth!\n"
#define MSGTR_MPDEMUX_ASF_Bandwidth2SmallDeselectedAudio "Bandwidth muy pequeño, deselecionando este stream de audio.\n"
#define MSGTR_MPDEMUX_ASF_Bandwidth2SmallDeselectedVideo "Bandwidth muy pequeño, deselecionando este stream de video.\n"
#define MSGTR_MPDEMUX_ASF_InvalidLenInHeader "Largo inválido den cabecera ASF!\n"
#define MSGTR_MPDEMUX_ASF_ErrChunkBiggerThanPacket "Error chunk_size > packet_size.\n"
#define MSGTR_MPDEMUX_ASF_ASFRedirector "=====> Redireccionador ASF\n"
#define MSGTR_MPDEMUX_ASF_InvalidProxyURL "URL del proxy inválida.\n"
#define MSGTR_MPDEMUX_ASF_UnknownASFStreamType "Tipo de ASF stream desconocido.\n"
#define MSGTR_MPDEMUX_ASF_Failed2ParseHTTPResponse "No pude procesar la respuesta HTTP\n"
#define MSGTR_MPDEMUX_ASF_ServerReturn "El servidor retornó %d:%s\n"
#define MSGTR_MPDEMUX_ASF_ASFHTTPParseWarnCuttedPragma "ASF HTTP PARSE WARNING : Pragma %s cortado desde %zu bytes a %zu\n"
#define MSGTR_MPDEMUX_ASF_SocketWriteError "Error escribiendo en el socket : %s\n"
#define MSGTR_MPDEMUX_ASF_HeaderParseFailed "Imposible procesar la cabecera.\n"
#define MSGTR_MPDEMUX_ASF_NoStreamFound "No encontre ningún stream.\n"
#define MSGTR_MPDEMUX_ASF_UnknownASFStreamingType "Desconozco este tipo de streaming ASF\n"
#define MSGTR_MPDEMUX_ASF_InfoStreamASFURL "STREAM_ASF, URL: %s\n"
#define MSGTR_MPDEMUX_ASF_StreamingFailed "Fallo, saliendo..\n"

// audio_in.c

#define MSGTR_MPDEMUX_AUDIOIN_ErrReadingAudio "\nError leyendo el audio: %s\n"
#define MSGTR_MPDEMUX_AUDIOIN_XRUNSomeFramesMayBeLeftOut "Recuperandome de un cross-run, puede que hayamos perdido algunos frames!\n"
#define MSGTR_MPDEMUX_AUDIOIN_ErrFatalCannotRecover "Error fatal, imposible reponerme!\n"
#define MSGTR_MPDEMUX_AUDIOIN_NotEnoughSamples "\nNo hay suficientes samples de audio!\n"

// cache2.c


// cdda.c

#define MSGTR_MPDEMUX_CDDA_CantOpenCDDADevice "No puede abrir el dispositivo CDA.\n"
#define MSGTR_MPDEMUX_CDDA_CantOpenDisc "No pude abrir el disco.\n"
#define MSGTR_MPDEMUX_CDDA_AudioCDFoundWithNTracks "Encontre un disco de audio con %d pistas.\n"

// cddb.c

#define MSGTR_MPDEMUX_CDDB_FailedToReadTOC "Fallé al tratar de leer el TOC.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToOpenDevice "Falle al tratar de abrir el dispositivo %s\n"
#define MSGTR_MPDEMUX_CDDB_NotAValidURL "No es un URL válido\n"
#define MSGTR_MPDEMUX_CDDB_FailedToSendHTTPRequest "Fallé al tratar de enviar la solicitud HTTP.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToReadHTTPResponse "Fallé al tratar de leer la respuesta HTTP.\n"
#define MSGTR_MPDEMUX_CDDB_HTTPErrorNOTFOUND "No encontrado.\n"
#define MSGTR_MPDEMUX_CDDB_HTTPErrorUnknown "Código de error desconocido.\n"
#define MSGTR_MPDEMUX_CDDB_NoCacheFound "No encontre cache.\n"
#define MSGTR_MPDEMUX_CDDB_NotAllXMCDFileHasBeenRead "No todo el archivo xmcd ha sido leido.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToCreateDirectory "No pude crear el directorio %s.\n"
#define MSGTR_MPDEMUX_CDDB_NotAllXMCDFileHasBeenWritten "No todo el archivo xmcd ha sido escrito.\n"
#define MSGTR_MPDEMUX_CDDB_InvalidXMCDDatabaseReturned "Archivo de base de datos xmcd inválido retornado.\n"
#define MSGTR_MPDEMUX_CDDB_UnexpectedFIXME "FIXME Inesperado.\n"
#define MSGTR_MPDEMUX_CDDB_UnhandledCode "Unhandled code.\n"
#define MSGTR_MPDEMUX_CDDB_UnableToFindEOL "No encontré el EOL\n"
#define MSGTR_MPDEMUX_CDDB_ParseOKFoundAlbumTitle "Procesado OK, encontrado: %s\n"
#define MSGTR_MPDEMUX_CDDB_AlbumNotFound "No encontré el album.\n"
#define MSGTR_MPDEMUX_CDDB_ServerReturnsCommandSyntaxErr "El servidor retornó: Error en la sintaxis del comando.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToGetProtocolLevel "Fallé tratando de obtener el nivel del protocolo.\n"
#define MSGTR_MPDEMUX_CDDB_NoCDInDrive "No hay un CD en la unidad.\n"

// cue_read.c

#define MSGTR_MPDEMUX_CUEREAD_UnexpectedCuefileLine "[bincue] Línea cuefile inesperada: %s\n"
#define MSGTR_MPDEMUX_CUEREAD_BinFilenameTested "[bincue] Nombre de archivo bin testeado: %s\n"
#define MSGTR_MPDEMUX_CUEREAD_CannotFindBinFile "[bincue] No enccuentro el archivo bin - me rindo.\n"
#define MSGTR_MPDEMUX_CUEREAD_UsingBinFile "[bincue] Utilizando el archivo bin %s\n"
#define MSGTR_MPDEMUX_CUEREAD_UnknownModeForBinfile "[bincue] Modo desconocido para el archivo bin, esto no debería pasar. Abortando.\n"
#define MSGTR_MPDEMUX_CUEREAD_CannotOpenCueFile "[bincue] No puedo abrir %s\n"
#define MSGTR_MPDEMUX_CUEREAD_ErrReadingFromCueFile "[bincue] Error leyendo desde  %s\n"
#define MSGTR_MPDEMUX_CUEREAD_ErrGettingBinFileSize "[bincue] Error obteniendo el tamaño del archivo bin.\n"
#define MSGTR_MPDEMUX_CUEREAD_InfoTrackFormat "pista %02d:  formato=%d  %02d:%02d:%02d\n"
#define MSGTR_MPDEMUX_CUEREAD_UnexpectedBinFileEOF "[bincue] Final inesperado en el archivo bin.\n"
#define MSGTR_MPDEMUX_CUEREAD_CannotReadNBytesOfPayload "[bincue] No pude leer %d bytes de payload.\n"
#define MSGTR_MPDEMUX_CUEREAD_CueStreamInfo_FilenameTrackTracksavail "CUE stream_open, archivo=%s, pista=%d, pistas disponibles: %d -> %d\n"

// network.c

#define MSGTR_MPDEMUX_NW_UnknownAF "Familia de direcciones desconocida %d\n"
#define MSGTR_MPDEMUX_NW_ResolvingHostForAF "Resoliendo %s para %s...\n"
#define MSGTR_MPDEMUX_NW_CantResolv "No pude resolver el nombre para %s: %s\n"
#define MSGTR_MPDEMUX_NW_ConnectingToServer "Connectando con el servidor %s[%s]: %d...\n"
#define MSGTR_MPDEMUX_NW_CantConnect2Server "No pude conectarme con el servidor con %s\n"
#define MSGTR_MPDEMUX_NW_SelectFailed "Select fallido.\n"
#define MSGTR_MPDEMUX_NW_ConnTimeout "La conección expiró.\n"
#define MSGTR_MPDEMUX_NW_GetSockOptFailed "getsockopt fallido: %s\n"
#define MSGTR_MPDEMUX_NW_ConnectError "Error de conección: %s\n"
#define MSGTR_MPDEMUX_NW_InvalidProxySettingTryingWithout "Configuración del proxy inválida... probando sin proxy.\n"
#define MSGTR_MPDEMUX_NW_CantResolvTryingWithoutProxy "No pude resolver el nombre del host para AF_INET. probando sin proxy.\n"
#define MSGTR_MPDEMUX_NW_ErrSendingHTTPRequest "Error enviando la solicitud HTTP: no alcancé a enviarla completamente.\n"
#define MSGTR_MPDEMUX_NW_ReadFailed "Falló la lectura.\n"
#define MSGTR_MPDEMUX_NW_Read0CouldBeEOF "http_read_response leí 0 (ej. EOF)\n"
#define MSGTR_MPDEMUX_NW_AuthFailed "Fallo la autentificación, por favor usa las opciones -user y -passwd con tus respectivos nombre de usuario y contraseña\n"\
"para una lista de URLs o construye unu URL con la siguiente forma:\n"\
"http://usuario:contraseña@servidor/archivo\n"
#define MSGTR_MPDEMUX_NW_AuthRequiredFor "Se requiere autentificación para %s\n"
#define MSGTR_MPDEMUX_NW_AuthRequired "Se requiere autentificación.\n"
#define MSGTR_MPDEMUX_NW_NoPasswdProvidedTryingBlank "No especificaste una contraseña, voy a utilizar una contraseña en blanco.\n"
#define MSGTR_MPDEMUX_NW_ErrServerReturned "El servidor retornó %d: %s\n"
#define MSGTR_MPDEMUX_NW_CacheSizeSetTo "Se seteo el tamaño del caché a %d KBytes.\n"


// ========================== LIBMENU ===================================

// common
#define MSGTR_LIBMENU_NoEntryFoundInTheMenuDefinition "[MENU] Noencontre una entrada e n la definición del menú.\n"

// libmenu/menu.c
#define MSGTR_LIBMENU_SyntaxErrorAtLine "[MENU] Error de sintáxis en la línea: %d\n"
#define MSGTR_LIBMENU_MenuDefinitionsNeedANameAttrib "[MENU] Las definiciones de menú necesitan un nombre de atributo (linea %d)\n"
#define MSGTR_LIBMENU_BadAttrib "[MENU] Atributo erroneo %s=%s en el menú '%s' en la línea %d\n"
#define MSGTR_LIBMENU_UnknownMenuType "[MENU] Tipo de menú desconocido '%s' en la línea %d\n"
#define MSGTR_LIBMENU_CantOpenConfigFile "[MENU] No puedo abrir el archivo de configuración del menú: %s\n"
#define MSGTR_LIBMENU_ConfigFileIsTooBig "[MENU] El archivo de configuración es muy grande (> %d KB).\n"
#define MSGTR_LIBMENU_ConfigFileIsEmpty "[MENU] El archivo de configuración esta vacío\n"
#define MSGTR_LIBMENU_MenuNotFound "[MENU] No encontré el menú %s\n"
#define MSGTR_LIBMENU_MenuInitFailed "[MENU] Fallo en la inicialización del menú '%s'.\n"
#define MSGTR_LIBMENU_UnsupportedOutformat "[MENU] Formato de salida no soportado!!!!\n"

// libmenu/menu_cmdlist.c
#define MSGTR_LIBMENU_ListMenuEntryDefinitionsNeedAName "[MENU] Las definiciones de Lista del menu necesitan un nombre (line %d).\n"
#define MSGTR_LIBMENU_ListMenuNeedsAnArgument "[MENU] El menú de lista necesita un argumento.\n"

// libmenu/menu_console.c
#define MSGTR_LIBMENU_WaitPidError "[MENU] Error Waitpid: %s.\n"
#define MSGTR_LIBMENU_SelectError "[MENU] Error en Select.\n"
#define MSGTR_LIBMENU_ReadErrorOnChildFD "[MENU] Error de lectura en el descriptor de archivo hijo: %s.\n"
#define MSGTR_LIBMENU_ConsoleRun "[MENU] Consola corriendo: %s ...\n"
#define MSGTR_LIBMENU_AChildIsAlreadyRunning "[MENU] Ya hay un hijo/child corriendo.\n"
#define MSGTR_LIBMENU_ForkFailed "[MENU] Falló el Fork!!!\n"
#define MSGTR_LIBMENU_WriteError "[MENU] Error de escritura.\n"

// libmenu/menu_filesel.c
#define MSGTR_LIBMENU_OpendirError "[MENU] Error en Opendir: %s.\n"
#define MSGTR_LIBMENU_ReallocError "[MENU] Error en Realloc: %s.\n"
#define MSGTR_LIBMENU_MallocError "[MENU] Error tratando de disponer de memoria: %s.\n"
#define MSGTR_LIBMENU_ReaddirError "[MENU] Error en Readdir: %s.\n"
#define MSGTR_LIBMENU_CantOpenDirectory "[MENU] No pude abrir el directorio %s\n"

// libmenu/menu_param.c
#define MSGTR_LIBMENU_SubmenuDefinitionNeedAMenuAttribut "[MENU] Submenu definition needs a 'menu' attribute.\n"
#define MSGTR_LIBMENU_InvalidProperty "[MENU] '%s', Propiedad inválida en entrada de menu de preferencias. (linea %d).\n"
#define MSGTR_LIBMENU_PrefMenuEntryDefinitionsNeed "[MENU] Las definiciones de las entradas de 'Preferencia' en el menu necesitan un atributo 'property' válido (linea %d).\n"
#define MSGTR_LIBMENU_PrefMenuNeedsAnArgument "[MENU] El menu 'Pref' necesita un argumento.\n"

// libmenu/menu_pt.c
#define MSGTR_LIBMENU_CantfindTheTargetItem "[MENU] Imposible encontrar el item objetivo ????\n"
#define MSGTR_LIBMENU_FailedToBuildCommand "[MENU] Fallé tratando de construir el comando: %s.\n"

// libmenu/menu_txt.c
#define MSGTR_LIBMENU_MenuTxtNeedATxtFileName "[MENU] El menu 'Text' necesita un nombre de archivo txt (param file).\n"
#define MSGTR_LIBMENU_MenuTxtCantOpen "[MENU] No pude abrir: %s.\n"
#define MSGTR_LIBMENU_WarningTooLongLineSplitting "[MENU] Advertencia, Línea muy larga, cortandola...\n"
#define MSGTR_LIBMENU_ParsedLines "[MENU] %d líneas procesadas.\n"

// libmenu/vf_menu.c
#define MSGTR_LIBMENU_UnknownMenuCommand "[MENU] Comando desconocido: '%s'.\n"
#define MSGTR_LIBMENU_FailedToOpenMenu "[MENU] No pude abrir el menú: '%s'.\n"

// ========================== LIBMPCODECS ===================================

// ad_dvdpcm.c:
#define MSGTR_SamplesWanted "Se necesitan muestras de este formato para mejorar el soporte. Por favor contacte a los desarrolladores.\n"

// libmpcodecs/ad_libdv.c
#define MSGTR_MPCODECS_AudioFramesizeDiffers "[AD_LIBDV] Advertencia! El framezise de audio difiere! leidos=%d  hdr=%d.\n"

// libmpcodecs/vd_dmo.c vd_dshow.c vd_vfw.c
#define MSGTR_MPCODECS_CouldntAllocateImageForCinepakCodec "[VD_DMO] No pude disponer imagen para el códec cinepak.\n"

// libmpcodecs/vd_ffmpeg.c
#define MSGTR_MPCODECS_ArithmeticMeanOfQP "[VD_FFMPEG] Significado aritmético de QP: %2.4f, significado armónico de QP: %2.4f\n"
#define MSGTR_MPCODECS_DRIFailure "[VD_FFMPEG] Falla DRI.\n"
#define MSGTR_MPCODECS_CouldntAllocateImageForCodec "[VD_FFMPEG] No pude disponer imagen para el códec.\n"
#define MSGTR_MPCODECS_XVMCAcceleratedMPEG2 "[VD_FFMPEG] MPEG-2 acelerado con XVMC.\n"
#define MSGTR_MPCODECS_TryingPixfmt "[VD_FFMPEG] Intentando pixfmt=%d.\n"
#define MSGTR_MPCODECS_McGetBufferShouldWorkOnlyWithXVMC "[VD_FFMPEG] El mc_get_buffer funcionará solo con aceleración XVMC!!"
#define MSGTR_MPCODECS_OnlyBuffersAllocatedByVoXvmcAllowed "[VD_FFMPEG] Solo buffers dispuestos por vo_xvmc estan permitidos.\n"

// libmpcodecs/ve_lavc.c
#define MSGTR_MPCODECS_HighQualityEncodingSelected "[VE_LAVC] Encoding de alta calidad seleccionado (no real-time)!\n"
#define MSGTR_MPCODECS_UsingConstantQscale "[VE_LAVC] Utilizando qscale = %f constante (VBR).\n"

// libmpcodecs/ve_raw.c
#define MSGTR_MPCODECS_OutputWithFourccNotSupported "[VE_RAW] La salida en bruto (raw) con fourcc [%x] no está soportada!\n"
#define MSGTR_MPCODECS_NoVfwCodecSpecified "[VE_RAW] No se especificó el codec VfW requerido!!\n"

// libmpcodecs/vf_crop.c
#define MSGTR_MPCODECS_CropBadPositionWidthHeight "[CROP] posición/ancho/alto inválido(s) - el área a cortar esta fuera del original!\n"

// libmpcodecs/vf_cropdetect.c
#define MSGTR_MPCODECS_CropArea "[CROP] Area de corte: X: %d..%d  Y: %d..%d  (-vf crop=%d:%d:%d:%d).\n"

// libmpcodecs/vf_format.c, vf_palette.c, vf_noformat.c
#define MSGTR_MPCODECS_UnknownFormatName "[VF_FORMAT] Nombre de formato desconocido: '%s'.\n"

// libmpcodecs/vf_framestep.c vf_noformat.c vf_palette.c vf_tile.c
#define MSGTR_MPCODECS_ErrorParsingArgument "[VF_FRAMESTEP] Error procesando argumento.\n"

// libmpcodecs/ve_vfw.c
#define MSGTR_MPCODECS_CompressorType "Tipo de compresor: %.4lx\n"
#define MSGTR_MPCODECS_CompressorSubtype "Subtipo de compresor: %.4lx\n"
#define MSGTR_MPCODECS_CompressorFlags "Compressor flags: %lu, versión %lu, versión ICM: %lu\n"
#define MSGTR_MPCODECS_Flags "Flags:"
#define MSGTR_MPCODECS_Quality " Calidad"

// libmpcodecs/vf_expand.c
#define MSGTR_MPCODECS_FullDRNotPossible "DR completo imposible, tratando con SLICES en su lugar!\n"
#define MSGTR_MPCODECS_FunWhydowegetNULL "Por qué obtenemos NULL??\n"


// libmpcodecs/vf_test.c, vf_yuy2.c, vf_yvu9.c
#define MSGTR_MPCODECS_WarnNextFilterDoesntSupport "%s No soportado por el próximo filtro/vo :(\n"

// ================================== LIBMPVO ====================================

// stream/stream_radio.c

#define MSGTR_RADIO_ChannelNamesDetected "[radio] Nombre de canales de radio detectados.\n"
#define MSGTR_RADIO_WrongFreqForChannel "[radio] Frecuencia erronea para canal %s\n"
#define MSGTR_RADIO_WrongChannelNumberFloat "[radio] Número de canal erroneo: %.2f\n"
#define MSGTR_RADIO_WrongChannelNumberInt "[radio] Número de canal erroneo: %d\n"
#define MSGTR_RADIO_WrongChannelName "[radio] Nombre de canal erroneo: %s\n"
#define MSGTR_RADIO_FreqParameterDetected "[radio] Parametro de frequencia de radio detectado.\n"
#define MSGTR_RADIO_GetTunerFailed "[radio] Advertencia: ioctl get tuner failed: %s. Setting frac to %d.\n"
#define MSGTR_RADIO_NotRadioDevice "[radio] %s no es un dispositivo de radio!\n"
#define MSGTR_RADIO_SetFreqFailed "[radio] ioctl set frequency 0x%x (%.2f) fallido: %s\n"
#define MSGTR_RADIO_GetFreqFailed "[radio] ioctl get frequency fallido: %s\n"
#define MSGTR_RADIO_SetMuteFailed "[radio] ioctl set mute fallido: %s\n"
#define MSGTR_RADIO_QueryControlFailed "[radio] ioctl query control fallido: %s\n"
#define MSGTR_RADIO_GetVolumeFailed "[radio] ioctl get volume fallido: %s\n"
#define MSGTR_RADIO_SetVolumeFailed "[radio] ioctl set volume fallido: %s\n"
#define MSGTR_RADIO_AllocateBufferFailed "[radio] Imposible reservar buffer de audio (bloque=%d,buf=%d): %s\n"
#define MSGTR_RADIO_CurrentFreq "[radio] Frecuencia actual: %.2f\n"
#define MSGTR_RADIO_SelectedChannel "[radio] Canal seleccionado: %d - %s (freq: %.2f)\n"
#define MSGTR_RADIO_ChangeChannelNoChannelList "[radio] No puedo cambiar canal, no se ha entregado una lista de canales.\n"
#define MSGTR_RADIO_UnableOpenDevice "[radio] Imposible abrir '%s': %s\n"
#define MSGTR_RADIO_WrongFreq "[radio] Frecuencia erronea: %.2f\n"
#define MSGTR_RADIO_UsingFreq "[radio] Utilizando frecuencia: %.2f.\n"
#define MSGTR_RADIO_AudioInInitFailed "[radio] audio_in_init fallido.\n"
#define MSGTR_RADIO_AudioInSetupFailed "[radio] audio_in_setup llamada fallida: %s\n"
#define MSGTR_RADIO_ClearBufferFailed "[radio] Fallo al limpiar el buffer: %s\n"
#define MSGTR_RADIO_StreamEnableCacheFailed "[radio] Llamada fallida a stream_enable_cache: %s\n"
#define MSGTR_RADIO_DriverUnknownStr "[radio] Nombre de driver desconocido: %s\n"
#define MSGTR_RADIO_DriverV4L2 "[radio] Utilizando interfaz de radio V4Lv2.\n"
#define MSGTR_RADIO_DriverV4L "[radio] Utilizando interfaz de radio V4Lv1.\n"
#define MSGTR_RADIO_DriverBSDBT848 "[radio] Usando la interfaz de radio *BSD BT848.\n"

//tv.c
#define MSGTR_TV_BogusNormParameter "tv.c: norm_from_string(%s): Parametro de normal inválido al configurar %s.\n"
#define MSGTR_TV_NoVideoInputPresent "Error: Entrada de video no encontrada!\n"
#define MSGTR_TV_UnknownImageFormat ""\
"==================================================================\n"\
" ADVERTENCIA: HA SELECCIONADO UN FORMATO DE SALIDA DE IMAGEN NO  \n"\
" CONOCIDO Y/O NO PROBADO (0x%x)                                  \n"\
" Esto podría causarle problemas de reproducción o que el programa\n"\
" se cayera! Los Bug reports serán ignorados! Intentelo nuevamente\n"\
" con YV12 (el colorspace por omisión) y leer la documentación.   \n"\
"==================================================================\n"
#define MSGTR_TV_CannotSetNorm "Error: No puedo configurar la norma!\n"
#define MSGTR_TV_MJP_WidthHeight "  MJP: ancho %d alto %d\n"
#define MSGTR_TV_UnableToSetWidth "Imposible configurar ancho requerido: %d\n"
#define MSGTR_TV_UnableToSetHeight "Imposible configurar alto requerido: %d\n"
#define MSGTR_TV_NoTuner "No hay un sintonizador en la entrada seleccionada!\n"
#define MSGTR_TV_UnableFindChanlist "Imposible encontrar lista de canales seleccionada! (%s)\n"
#define MSGTR_TV_ChannelFreqParamConflict "No puede configurar simultaneamente frecuencia y canal!\n"
#define MSGTR_TV_ChannelNamesDetected "Nombres de canales de TV detectados.\n"
#define MSGTR_TV_NoFreqForChannel "Imposible encontrar frecuencia para el canal %s (%s)\n"
#define MSGTR_TV_SelectedChannel3 "Canal seleccionado: %s - %s (freq: %.3f)\n"
#define MSGTR_TV_SelectedChannel2 "Canal seleccionado: %s (freq: %.3f)\n"
#define MSGTR_TV_UnsupportedAudioType "Tipo de audio '%s (%x)' no soportado!\n"
#define MSGTR_TV_AvailableDrivers "Drivers disponibles:\n"
#define MSGTR_TV_DriverInfo "Driver seleccionado: %s\n nombre: %s\n autor: %s\n comentario: %s\n"
#define MSGTR_TV_NoSuchDriver "Ese driver no existe: %s\n"
#define MSGTR_TV_DriverAutoDetectionFailed "Falló la utodetección del driver de TV.\n"
#define MSGTR_TV_UnknownColorOption "Se especificó una opción de color desconodida (%d)!\n"
#define MSGTR_TV_NoTeletext "Sin teletexto"
#define MSGTR_TV_Bt848IoctlFailed "tvi_bsdbt848: llamada a %s ioctl fallida. Error: %s\n"
#define MSGTR_TV_Bt848InvalidAudioRate "tvi_bsdbt848: Audio rate inválido. Error: %s\n"
#define MSGTR_TV_Bt848ErrorOpeningBktrDev "tvi_bsdbt848: Imposible abrir dispositivo bktr. Error: %s\n"
#define MSGTR_TV_Bt848ErrorOpeningTunerDev "tvi_bsdbt848: Imposible abrir dispositivo sintonizador. Error: %s\n"
#define MSGTR_TV_Bt848ErrorOpeningDspDev "tvi_bsdbt848: Imposible abrir dispositivo dsp. Error: %s\n"
#define MSGTR_TV_Bt848ErrorConfiguringDsp "tvi_bsdbt848: Falló la configuración del dsp. Error: %s\n"
#define MSGTR_TV_Bt848ErrorReadingAudio "tvi_bsdbt848: Error leyendo datos de audio. Error: %s\n"
#define MSGTR_TV_Bt848MmapFailed "tvi_bsdbt848: mmap fallido. Error: %s\n"
#define MSGTR_TV_Bt848FrameBufAllocFailed "tvi_bsdbt848: Fallo al reservar frame buffer (allocation failed). Error: %s\n"
#define MSGTR_TV_Bt848ErrorSettingWidth "tvi_bsdbt848: Error configurando el ancho del picture. Error: %s\n"
#define MSGTR_TV_Bt848UnableToStopCapture "tvi_bsdbt848: Imposible detener la captura. Error: %s\n"
#define MSGTR_TV_TTSupportedLanguages "Idiomas de teletexto soportados:\n"
#define MSGTR_TV_TTSelectedLanguage "Se seleccionó el idioma de teletexto por omisión: %s\n"
#define MSGTR_TV_ScannerNotAvailableWithoutTuner "No se puede utilizar el scanner de canales sin un sintonizador\n"

//tvi_dshow.c
#define MSGTR_TVI_DS_UnableConnectInputVideoDecoder  "Imposible conectar la entrada seleccionada con el decodificador de video. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableConnectInputAudioDecoder  "Imposible conectar la entrada seleccionada con el decodificador de audio. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableSelectVideoFormat "tvi_dshow: Imposible seleccioar formato de video. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableSelectAudioFormat "tvi_dshow: Imposible seleccionar formato de audio. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableGetMediaControlInterface "tvi_dshow: Imposible obtener interfaz IMediaControl. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableStartGraph "tvi_dshow: Imposible comenzar graph! Error:0x%x\n"
#define MSGTR_TVI_DS_DeviceNotFound "tvi_dshow: No se encontró el dispositivo #%d\n"
#define MSGTR_TVI_DS_UnableGetDeviceName "tvi_dshow: Imposible obtener nombre para dispositivo #%d\n"
#define MSGTR_TVI_DS_UsingDevice "tvi_dshow: Utilizando dispositivo #%d: %s\n"
#define MSGTR_TVI_DS_DirectGetFreqFailed "tvi_dshow: Imposible obtener la frecuencia directamente. Se utilizará la tabla de canales incluída con el OS.\n"
//following phrase will be printed near the selected audio/video input
#define MSGTR_TVI_DS_UnableExtractFreqTable "tvi_dshow: Imposible cargar tabla de frecuencias desde kstvtune.ax\n"
#define MSGTR_TVI_DS_WrongDeviceParam "tvi_dshow: Parametro de dispositivo inválido: %s\n"
#define MSGTR_TVI_DS_WrongDeviceIndex "tvi_dshow: Indice de dispositivo inválido: %d\n"
#define MSGTR_TVI_DS_WrongADeviceParam "tvi_dshow: Parametro de adevice inválido: %s\n"
#define MSGTR_TVI_DS_WrongADeviceIndex "tvi_dshow: Indice de adevice inválido: %d\n"
#define MSGTR_TVI_DS_SamplerateNotsupported "tvi_dshow: El dispositivo no soporta el Samplerate %d. Probando con el primero disponible.\n"
#define MSGTR_TVI_DS_VideoAdjustigNotSupported "tvi_dshow: El dispositivo no soporta el ajuste de brillo/hue/saturación/contraste\n"
#define MSGTR_TVI_DS_ChangingWidthHeightNotSupported "tvi_dshow: El dispositivo no soporta cambios de ancho/largo.\n"
#define MSGTR_TVI_DS_SelectingInputNotSupported  "tvi_dshow: El dispositivo no soporta la selección de la fuente de captura\n"
#define MSGTR_TVI_DS_ErrorParsingAudioFormatStruct "tvi_dshow: Imposible procesar la estructura del formato de audio.\n"
#define MSGTR_TVI_DS_ErrorParsingVideoFormatStruct "tvi_dshow: Imposible procesar la estructura del formato de video.\n"
#define MSGTR_TVI_DS_UnableSetAudioMode "tvi_dshow: Imposible seleccionar modo de audio %d. Error:0x%x\n"
#define MSGTR_TVI_DS_UnsupportedMediaType "tvi_dshow: A %s se le ha pasado un tipo de medios no soportado.\n"
#define MSGTR_TVI_DS_UnableFindNearestChannel "tvi_dshow: Imposible encontrar el canal más cercano en la tabla de frecuencias del sistema\n"
#define MSGTR_TVI_DS_UnableToSetChannel "tvi_dshow: Imposible cambiar al canal más cercano en la tabla de frecuencias del sistema. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableTerminateVPPin "tvi_dshow: Imposible terminar pin VideoPort con cualquiera de los filtros en el graph. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableBuildVideoSubGraph "tvi_dshow: Imposible construir cadena de video del graph de captura. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableBuildAudioSubGraph "tvi_dshow: Imposible construir cadena de audio del graph de captura. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableBuildVBISubGraph "tvi_dshow: Imposible construir cadena VBI del graph de captura. Error:0x%x\n"
#define MSGTR_TVI_DS_GraphInitFailure "tvi_dshow: Falló la inicialización del graph Directshow.\n"
#define MSGTR_TVI_DS_NoVideoCaptureDevice "tvi_dshow: No se pudo encontrar un dispositivo de captura de video\n"
#define MSGTR_TVI_DS_NoAudioCaptureDevice "tvi_dshow: No se pudo encontrar un dispositivo de captura de audio\n"
#define MSGTR_TVI_DS_GetActualMediatypeFailed "tvi_dshow: Imposible obtener el tipo de medios actual (Error:0x%x). Se asumirá que es igual al requerido.\n"


// ================================== LIBASS ====================================

// ass_bitmap.c


// ass.c
#define MSGTR_LIBASS_FopenFailed "[ass] ass_read_file(%s): fopen ha fallado\n"
#define MSGTR_LIBASS_RefusingToLoadSubtitlesLargerThan100M "[ass] ass_read_file(%s): No se pueden cargar ficheros de subtítulos mayores de 100M\n"

// ass_cache.c

// ass_fontconfig.c

// ass_render.c

// ass_font.c
