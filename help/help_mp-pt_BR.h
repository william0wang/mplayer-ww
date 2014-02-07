// Translated by Fabio Pugliese Ornellas <fabio.ornellas@poli.usp.br>
// Portuguese from Brazil Translation
// GPLed code
// in sync version 1.87 from CVS 2002-02-04

// ========================= MPlayer help ===========================

#ifdef CONFIG_VCD
#define MSGTR_HelpVCD " vcd://<numtrilha> reproduz trilha de VCD (Video CD) do dispositivo em vez de um\n"\
                      "                 arquivo\n"
#else
#define MSGTR_HelpVCD
#endif

#ifdef CONFIG_DVDREAD
#define MSGTR_HelpDVD " dvd://<numtítilo> reproduz título de DVD do dispositivo em vez de um arquivo\n"\
                      " -alang/-slang   seleciona o idioma/legenda do DVD (pelo código país de duas\n"\
                      "                 letras)\n"
#else
#define MSGTR_HelpDVD
#endif

#define MSGTR_Help \
"Uso:   mplayer [opções] [url|caminho/]nome-do-arquivo\n"\
"\n"\
"Opções básicas: (lista completa na página do manual)\n"\
" -vo <drv[:dev]> seleciona o driver de saída de vídeo & dispositivo\n"\
"                 ('-vo help' para listar)\n"\
" -ao <drv[:dev]> seleciona o driver de saída de audio & dispositivo\n"\
"                 ('-vo help' para listar)\n"\
MSGTR_HelpVCD \
MSGTR_HelpDVD \
" -ss <tempopos>  busca para a posição dada (segundos ou hh:mm:ss)\n"\
" -nosound        não reproduz som\n"\
" -fs             reprodução em tela cheia (ou -vm, -zoom, detalhes na página do\n"\
"                 manual)\n"\
" -x <x> -y <y>   especifica a resolução da tela (para uso com -vm ou -zoom)\n"\
" -sub <arquivo>  especifica o arquivo de legenda a usar (veja também -subfps,\n"\
"                 -subdelay)\n"\
" -playlist <arquivo> especifica o aruqivo com a lista de reprodução\n"\
" -vid x -aid y   seleciona a trilha de vídeo (x) e audio (y) a reproduzir\n"\
" -fps x -srate y muda a taxa do vídeo (x quadros por segundo) e audio (y Hz)\n"\
" -pp <qualidade> habilita filtro de pós processamento (veja detalhes na página\n"\
"                 do manual)\n"\
" -framedrop      habilita descarte de quadros (para máquinas lentas)\n"\
"\n"\
"Teclas básicas: (lista completa na páginal do manual, cheque também input.conf)\n"\
" <-  ou  ->      retorna/avança 10 segundos\n"\
" cima ou baixo   retorna/avança 1 minuto\n"\
" pgup ou pgdown  retorna/avança 10 minutos\n"\
" < ou >          retorna/avança na lista de reprodução\n"\
" p ou ESPAÇO     pausa o filme (pressione qualquer tecla para continuar)\n"\
" q ou ESC        para a reprodução e sai do programa\n"\
" + ou -          ajusta o atraso do audio de +/- 0.1 segundo\n"\
" o               alterna modo OSD: nenhum / busca / busca+cronômetro\n"\
" * ou /          aumenta ou diminui o volume pcm\n"\
" z ou x          ajusta o atraso da legenda de +/- 0.1 segundo\n"\
" r ou t          posição da legenda para cima/baixo, veja também -vf expand\n"\
"\n"\
"* VEJA A PÁGINA DO MANUAL PARA DETALHES, FUTURAS (AVANÇADAS) OPÇÕES E TECLAS *\n"\
"\n"

static const char help_text[] = MSGTR_Help;

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\nSaindo...\n"
#define MSGTR_ExitingHow "\nSaindo... (%s)\n"
#define MSGTR_Exit_quit "Sair"
#define MSGTR_Exit_eof "Fim do arquivo"
#define MSGTR_Exit_error "Erro fatal"
#define MSGTR_IntBySignal "\nMPlayer interrompido por sinal %d no módulo %s\n"
#define MSGTR_NoHomeDir "Diretório HOME não encontrado.\n"
#define MSGTR_GetpathProblem "Problema em get_path(\"config\")\n"
#define MSGTR_CreatingCfgFile "Criando arquivo de configuração: %s\n"
#define MSGTR_CantLoadFont "Impossível carregar fonte: %s\n"
#define MSGTR_CantLoadSub "Impossível carregar legendas: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: faltando trilha selecionada!\n"
#define MSGTR_CantOpenDumpfile "Impossível abrir arquivo de dump.\n"
#define MSGTR_CoreDumped "core dumped ;)\n"
#define MSGTR_FPSnotspecified "Quadros por segundo não especificado no cabeçalho ou inválido, use a opção -fps.\n"
#define MSGTR_TryForceAudioFmtStr "Tentando forçar a família %s do driver do codec de audio...\n"
#define MSGTR_CantFindAudioCodec "Codec para o format de audio 0x%X não encontrado!\n"
#define MSGTR_TryForceVideoFmtStr "Tentando forçar a família %s do driver do codec de vídeo...\n"
#define MSGTR_CantFindVideoCodec "Impossível encontrar codec que se iguale ao -vo selecionado e ao formato de video 0x%X!\n"
#define MSGTR_VOincompCodec "O dispositivo de saída de vídeo selecionado é incompatível com este codec.\n"
#define MSGTR_CannotInitVO "FATAL: Impossível inicializar o driver de vídeo.\n"
#define MSGTR_CannotInitAO "Impossível abrir/inicializar o dispositivo de audio -> sem som\n"
#define MSGTR_StartPlaying "Iníciando reprodução...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"          ***************************************************\n"\
"          * Seu sistema é muito LENTO para reproduzir isto! *\n"\
"          ***************************************************\n\n"\
"Possíveis razões, problemas, soluções:\n"\
"- Mais comum: driver de _audio_ quebrado/falho\n"\
"  - Tente -ao sdl ou use ALSA 0.5 ou a emulação OSS do ALSA 0.9.\n"\
"  - Experimente com diferente valores para -autosync, 30 é um bom começo.\n"\
"- Saída de vídeo lenta\n"\
"  - Tente um driver diferente -vo (-vo help para listar) ou tente -framedrop!\n"\
"- CPU Lento\n"\
"  - Não tente reproduzir um DVD/DivX grande em um CPU lento! Tente\n"\
"    -hardframedrop.\n"\
"- Arquivo corrompido\n"\
"  - Tente várias combinações de -nobps -ni -forceidx -mc 0.\n"\
"- Mídia lenta (montagens NFS/SMB, DVD, VCD etc...)\n"\
"  - Tente -cache 8192.\n"\
"- Você está usando -cache para reproduzir um arquivo AVI não-entrelaçado?\n"\
"  - Tente -nocache.\n"\
"Leia DOCS/HTML/en/video.html para dicas de ajuste/velocidade.\n"\
"Se nenhum destes ajudar você, leia DOCS/HTML/en/bugreports.html.\n\n"

#define MSGTR_NoGui "MPlayer foi compilado SEM suporte a GUI (interface gráfica com o usuário)!\n"
#define MSGTR_GuiNeedsX "MPlayer GUI (interface gráfica com o usuário) requer X11!\n"
#define MSGTR_Playing "Reproduzindo %s\n"
#define MSGTR_NoSound "Audio: sem som.\n"
#define MSGTR_FPSforced "FPS (quadros por segundo) forçado a ser %5.3f (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "Drivers de saída de vídeo disponíveis:\n"
#define MSGTR_AvailableAudioOutputDrivers "Drivers de saída de audio disponíveis:\n"
#define MSGTR_AvailableAudioCodecs "Codecs de audio disponíveis:\n"
#define MSGTR_AvailableVideoCodecs "Codecs de vídeo disponíveis:\n"
#define MSGTR_AvailableAudioFm "\nFamílias/drivers de codec de audio disponíveis (compilados):\n"
#define MSGTR_AvailableVideoFm "\nFamílias/drivers de codec de vídeo disponíveis (compilados):\n"
#define MSGTR_CannotReadVideoProperties "Video: impossível ler propriedades\n"
#define MSGTR_NoStreamFound "Trilha não encontrada\n"
#define MSGTR_ErrorInitializingVODevice "Erro abrindo/inicializando o dispositivo da saída de vídeo (-vo)!\n"
#define MSGTR_ForcedVideoCodec "Codec de vídeo forçado: %s\n"
#define MSGTR_ForcedAudioCodec "Codec de audio forçado: %s\n"
#define MSGTR_Video_NoVideo "Vídeo: sem vídeo\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: Impossível inicializar os filtros de vídeo (-vf) ou a saída de vídeo (-vo)!\n"
#define MSGTR_Paused "================= PAUSADO ================="
#define MSGTR_PlaylistLoadUnable "\nIncapaz de carregar a lista de reprodução %s.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer falhou por uma 'Instrução Ilegal'.\n"\
"  Pode ser um erro no nosso novo código de detecção de CPU em tempo real...\n"\
"  Por favor leia DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer falhou por uma 'Instrução Ilegal'.\n"\
"  Isso frequentemente acontece quando você o exucuta em um CPU diferente do que\n"\
"  aquele para o qual foi compilado/otimizado.\n  Verifique isso!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer falhou por mau uso do CPU/FPU/RAM.\n"\
"  Recompile o MPlayer com --enable-debug e faça um 'gdb backtrace' e\n"\
"  'disassembly'. Para detalhes, veja DOCS/HTML/en/bugreports_what.html#bugreports_crash\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer falhou. Isso não deveria acontecer.\n"\
"  Pode ser um erro no código do MPlayer _ou_ em seus drivers _ou_ em sua versão\n"\
"  do gcc. Se você acha que é culpa do MPlayer, por favor leia\n"\
"  DOCS/HTML/en/bugreports.html e siga as instruções. Nós não podemos e não vamos ajudar\n"\
"  a não ser que você proveja esta informação quando reportar um possível erro.\n"

// mencoder.c:

#define MSGTR_UsingPass3ControlFile "Usando controle de arquivo pass3: %s\n"
#define MSGTR_MissingFilename "\nFaltando nome do arquivo!\n\n"
#define MSGTR_CannotOpenFile_Device "Impossível abrir arquivo/dispositivo\n"
#define MSGTR_CannotOpenDemuxer "Impossível abrir \"demuxer\"\n"
#define MSGTR_NoAudioEncoderSelected "\nNenhum codificador de audio (-oac) selecionado! Selecione um ou use -nosound. Use -oac help para listar!\n"
#define MSGTR_NoVideoEncoderSelected "\nNenhum codificador de vídeo (-ovc) selecionado! Selecione um, use -ovc help para listar!\n"
#define MSGTR_CannotOpenOutputFile "Impossível abrir arquivo de saída '%s'\n"
#define MSGTR_EncoderOpenFailed "Falha ao abrir o codificador\n"
#define MSGTR_ForcingOutputFourcc "Forçando saída fourcc para %x [%.4s]\n"
#define MSGTR_DuplicateFrames "\n%d quadro(s) duplicado(s)!\n"
#define MSGTR_SkipFrame "\npulando frame!!!    \n"
#define MSGTR_ErrorWritingFile "%s: erro gravando arquivo.\n"
#define MSGTR_RecommendedVideoBitrate "Bitrate do vídeo recomendado para CD de %s: %d\n"
#define MSGTR_VideoStreamResult "\nTrilha de vídeo: %8.3f kbit/s  (%d B/s)  tamanho: %"PRIu64" bytes  %5.3f segundos  %d quadros\n"
#define MSGTR_AudioStreamResult "\nTrilha de audio: %8.3f kbit/s  (%d B/s)  tamanho: %"PRIu64" bytes  %5.3f segundos\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     método da taxa de bits variável\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(padrão)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           taxa de bits média\n"\
"\n"\
" cbr           taxa de bits constante\n"\
"               Força também modo de codificação CBR nos modos ABR\n"\
"               pré-selecionados subsequentes.\n"\
"\n"\
" br=<0-1024>   especifica a taxa de bits em kBit (somente CBR e ABR)\n"\
"\n"\
" q=<0-9>       qualidade (0-melhor, 9-pior) (somente para VBR)\n"\
"\n"\
" aq=<0-9>      qualidade do algorítmo (0-melhor/mais lento, 9-pior/mais rápido)\n"\
"\n"\
" ratio=<1-100> taxa de compressão\n"\
"\n"\
" vol=<0-10>    configura ganho da entrada de audio\n"\
"\n"\
" mode=<0-3>    (padrão: auto)\n"\
"                0: estéreo\n"\
"                1: estéreo-junto\n"\
"                2: canal duplo\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: não\n"\
"                1: tudo\n"\
"                2: adaptar\n"\
"\n"\
" fast          aciona codificação rápida nos modos VBR pré-selecionados\n"\
"               subsequentes, qualidade muito baixa e altas taxas de bit.\n"\
"\n"\
" preset=<value> provê os ajustes com a mais alta qualidade.\n"\
"                 medium: codificação VBR, qualidade boa\n"\
"                 (taxa de bits entre 150-180 kbps)\n"\
"                 standard:  codificação VBR, qualidade alta\n"\
"                 (taxa de bits entre 170-210 kbps)\n"\
"                 extreme: codificação VBR, qualidade muito alta\n"\
"                 (taxa de bits entre 200-240 kbps)\n"\
"                 insane:  codificação CBR, ajuste para a mais alta qualidade\n"\
"                 (taxa de bits fixa em 320 kbps)\n"\
"                 <8-320>: codificação ABR com a taxa de bits em kbps dada.\n\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "Dispositivo de CD-ROM '%s' não encontrado!\n"
#define MSGTR_ErrTrackSelect "Erro selecionando trilha do VCD!"
#define MSGTR_ReadSTDIN "Lendo de stdin...\n"
#define MSGTR_UnableOpenURL "Impossível abrir URL: %s\n"
#define MSGTR_ConnToServer "Conecatado ao servidor: %s\n"
#define MSGTR_FileNotFound "Arquivo não encontrado: '%s'\n"

#define MSGTR_SMBInitError "Impossível inicializar biblioteca libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "Impossível abrir da \"lan\": '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer não foi compilado com suporte a leitura de SMB\n"

#define MSGTR_CantOpenDVD "Impossível abrir dispositivo de DVD: %s (%s)\n"
#define MSGTR_DVDnumTitles "Existem %d títulos neste DVD.\n"
#define MSGTR_DVDinvalidTitle "Número do título do DVD inválido: %d\n"
#define MSGTR_DVDnumChapters "Existem %d capítulos neste título de DVD.\n"
#define MSGTR_DVDinvalidChapter "Número do capítulo do DVD inválido: %d\n"
#define MSGTR_DVDnumAngles "Existem %d anglos neste título de DVD.\n"
#define MSGTR_DVDinvalidAngle "Número do anglo do DVD inválido: %d\n"
#define MSGTR_DVDnoIFO "Impossível abrir o arquivo IFO para o título de DVD %d.\n"
#define MSGTR_DVDnoVOBs "Impossível abrir título VOBS (VTS_%02d_1.VOB).\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "AVISO! Cabeçalho %d da trilha de audio redefinido!\n"
#define MSGTR_VideoStreamRedefined "AVISO! Cabeçalho %d da trilha de vídeo redefinido!\n"
#define MSGTR_TooManyAudioInBuffer "\nMuitos pacotes de audio no buffer: (%d em %d bytes).\n"
#define MSGTR_TooManyVideoInBuffer "\nMuitos pacotes de audio no buffer: (%d em %d bytes).\n"
#define MSGTR_MaybeNI "Talvez você esteja reproduzindo um fluxo/arquivo não-entrelaçado ou o codec falhou?\n" \
                      "Para arquivos .AVI, tente forçar um modo não-entrelaçado com a opção -ni.\n"
#define MSGTR_SwitchToNi "\nDetectado .AVI mau entrelaçado - mudando para o modo -ni!\n"
#define MSGTR_Detected_XXX_FileFormat "Detectado formato de arquivo %s!\n"
#define MSGTR_DetectedAudiofile "Detectado arquivo de audio!\n"
#define MSGTR_InvalidMPEGES "Fluxo MPEG-ES inválido??? Contacte o autor, pode ser um bug :(\n"
#define MSGTR_FormatNotRecognized "======= Desculpe, este formato de arquivo não é reconhecido/suportado ========\n"\
                                  "== Se este arquivo é um fluxo AVI, ASF ou MPEG, por favor contacte o autor ==\n"
#define MSGTR_MissingVideoStream "Nenhuma trilha de vídeo encontrado!\n"
#define MSGTR_MissingAudioStream "Nenhuma trilha de audio encontrado -> sem som\n"
#define MSGTR_MissingVideoStreamBug "Trilha de vídeo faltando!? Contacte o autor, pode ser um bug :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: Arquivo não contém a trilha de audio ou vídeo selecionado.\n"

#define MSGTR_NI_Forced "Forçado"
#define MSGTR_NI_Detected "Detectado"
#define MSGTR_NI_Message "%s formato de arquivo AVI NÃO ENTRELAÇADO!\n"

#define MSGTR_UsingNINI "Usando formato quebrado não-entrelaçado do arquivo AVI!\n"
#define MSGTR_CouldntDetFNo "Impossível determinar o número de quadros (para busca absoluta)  \n"
#define MSGTR_CantSeekRawAVI "Impossível buscar em fluxos de .AVI brutos! (índice requerido, tente com a opção -idx!)  \n"
#define MSGTR_CantSeekFile "Impossível buscar neste arquivo!  \n"

#define MSGTR_MOVcomprhdr "MOV: Cabeçalhos comprimidos não suportados (ainda)!\n"
#define MSGTR_MOVvariableFourCC "MOV: Advertência! Variável FOURCC detectada!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Advertência! Trilhas demais!"
#define MSGTR_DetectedTV "TV detectada! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Impossível abrir o demuxer ogg\n"
#define MSGTR_CannotOpenAudioStream "Impossível abrir trilha de audio: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Impossível abrir trilha de legendas: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Falha ao abrir demuxer de audio: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Falha ao abrir demuxer de legendas: %s\n"
#define MSGTR_TVInputNotSeekable "Entrada de TV não aceita busca! (Provavelmente a busca será pra mudar de canal ;)\n"
#define MSGTR_ClipInfo "Informações do clip:\n"


// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "Impossível abrir codec\n"
#define MSGTR_CantCloseCodec "Impossível fechar codec\n"

#define MSGTR_MissingDLLcodec "ERRO: Impossível abrir o codec DirectShow %s requerido.\n"
#define MSGTR_ACMiniterror "Impossível carregar/inicializar o codec Win32/ACM AUDIO (arquivo DLL faltando?).\n"
#define MSGTR_MissingLAVCcodec "Impossível encontrar codec '%s' em libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF enquanto procurando pelo cabeçalho da sequência\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: Impossível ler cabeçalho da sequência!\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: Impossível ler extenção do cabeçalho da sequência!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Cabeçalho da sequência mau!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Extensão do cabeçalho da sequência mau!\n"

#define MSGTR_ShMemAllocFail "Impossível alocar memória compartilhada\n"
#define MSGTR_CantAllocAudioBuf "Impossível alocate buffer da saída de audio\n"

#define MSGTR_UnknownAudio "Desconhecido/faltando formato de audio -> sem som\n"

#define MSGTR_UsingExternalPP "[PP] Usando filtro de pós processamento externo, máximo q = %d\n"
#define MSGTR_UsingCodecPP "[PP] Usando pós processamento do codec, máximo q = = %d\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Família [%s] (vfm=%s) do codec de video não disponível (habilite na hora da compilação!)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Família [%s] (afm=%s) do codec de audio não disponível (habilite na hora da compilação!)\n"
#define MSGTR_OpeningVideoDecoder "Abrindo decodificador de vídeo: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "Abrindo decodificador de audio: [%s] %s\n"
#define MSGTR_VDecoderInitFailed "Falha na incialização do VDecoder :(\n"
#define MSGTR_ADecoderInitFailed "Falha na incialização do ADecoder :(\n"
#define MSGTR_ADecoderPreinitFailed "Falha na pré-inicialização do ADecoder :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Falha na abertura do suporte a lirc!\n"
#define MSGTR_LIRCcfgerr "Falha ao ler o arquivo de configuração do LIRC %s.\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Filtro de vídeo '%s' não encontrado\n"
#define MSGTR_CouldNotOpenVideoFilter "Impossível abrir o filtro de vídeo '%s'\n"
#define MSGTR_OpeningVideoFilter "Abrindo filtro de vídeo: "
#define MSGTR_CannotFindColorspace "Impossível encontrar um \"colorspace\" comum, mesmo inserindo \"scale\" :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: codec não configurou sh->disp_w e sh->disp_h, tentando solução alternativa!\n"
#define MSGTR_CouldNotFindColorspace "Impossível encotrar \"colorspace\" similar - retentando com -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Aspecto do filme é  %.2f:1 - pré-redimensionando para corrigir o aspecto do filme.\n"
#define MSGTR_MovieAspectUndefined "Aspecto do filme é indefinido - nenhum pré-redimensionamento aplicado.\n"

// ================================ GUI ================================

#define MSGTR_GUI_AboutMPlayer "Sobre o MPlayer"
#define MSGTR_GUI_Add "Adicionar"
#define MSGTR_GUI_AspectRatio "Aspecto"
#define MSGTR_GUI_Audio "Audio"
#define MSGTR_GUI_AudioDelay "Atraso do audio"
#define MSGTR_GUI_AudioTrack "Carregar arquivo de audio externo"
#define MSGTR_GUI_AudioTracks "Trilha de audio"
#define MSGTR_GUI_AvailableDrivers "Drivers disponíveis:"
#define MSGTR_GUI_AvailableSkins "Skins"
#define MSGTR_GUI_Bass "Grave"
#define MSGTR_GUI_Blur "Embassar"
#define MSGTR_GUI_Brightness "Brilho"
#define MSGTR_GUI_Browse "Procurar"
#define MSGTR_GUI_Cache "Cache"
#define MSGTR_GUI_CacheSize "Tamaho do cache"
#define MSGTR_GUI_Cancel "Cancelar"
#define MSGTR_GUI_Center "Centro"
#define MSGTR_GUI_Channel1 "Canal 1"
#define MSGTR_GUI_Channel2 "Canal 2"
#define MSGTR_GUI_Channel3 "Canal 3"
#define MSGTR_GUI_Channel4 "Canal 4"
#define MSGTR_GUI_Channel5 "Canal 5"
#define MSGTR_GUI_Channel6 "Canal 6"
#define MSGTR_GUI_ChannelAll "Todos"
#define MSGTR_GUI_ChapterN "Capítulo %d"
#define MSGTR_GUI_ChapterNN "Capítulo %2d"
#define MSGTR_GUI_Chapters "Capítulos"
#define MSGTR_GUI_Clear "Limpar"
#define MSGTR_GUI_CodecFamilyAudio "Família do codec de audio"
#define MSGTR_GUI_CodecFamilyVideo "Família do codec de vídeo"
#define MSGTR_GUI_Coefficient "Coeficiente"
#define MSGTR_GUI_Configure "Configurações"
#define MSGTR_GUI_ConfigureDriver "Configurações do driver"
#define MSGTR_GUI_Contrast "Contraste"
#define MSGTR_GUI_Cp874 "Caracteres Tailandeses (CP874)"
#define MSGTR_GUI_Cp936 "Caracteres Chineses Simplificados (CP936)"
#define MSGTR_GUI_Cp949 "Caracteres Coreanos (CP949)"
#define MSGTR_GUI_Cp1250 "Européias Eslávicas/Centrais Windows (CP1250)"
#define MSGTR_GUI_Cp1251 "Windows Cirílico (CP1251)"
#define MSGTR_GUI_CpBIG5 "Caracteres Chineses Tradicionais (BIG5)"
#define MSGTR_GUI_CpISO8859_1 "Línguas Européias Ocidentais (ISO-8859-1)"
#define MSGTR_GUI_CpISO8859_2 "Linguas Européias Eslávicas/Centrais (ISO-8859-2)"
#define MSGTR_GUI_CpISO8859_3 "Esperanto, Galego, Maltês, Turco (ISO-8859-3)"
#define MSGTR_GUI_CpISO8859_4 "Caracteres Bálticos Antigos (ISO-8859-4)"
#define MSGTR_GUI_CpISO8859_5 "Cirílico (ISO-8859-5)"
#define MSGTR_GUI_CpISO8859_6 "Árabe (ISO-8859-6)"
#define MSGTR_GUI_CpISO8859_7 "Grego Moderno (ISO-8859-7)"
#define MSGTR_GUI_CpISO8859_8 "Caracteres Hebraicos (ISO-8859-8)"
#define MSGTR_GUI_CpISO8859_9 "Turco (ISO-8859-9)"
#define MSGTR_GUI_CpISO8859_13 "Báltico (ISO-8859-13)"
#define MSGTR_GUI_CpISO8859_14 "Celta (ISO-8859-14)"
#define MSGTR_GUI_CpISO8859_15 "Línguas Européias Ocidentais com Euro (ISO-8859-15)"
#define MSGTR_GUI_CpKOI8_R "Russo (KOI8-R)"
#define MSGTR_GUI_CpKOI8_U "Ucraniano, Bielo-Russo (KOI8-U/RU)"
#define MSGTR_GUI_CpShiftJis "Caracteres Japoneses (SHIFT-JIS)"
#define MSGTR_GUI_CpUnicode "Unicode"
#define MSGTR_GUI_Delay "Atrtaso"
#define MSGTR_GUI_Demuxers_Codecs "Codecs & demuxer"
#define MSGTR_GUI_DeviceCDROM "Dispositivo de CD-ROM"
#define MSGTR_GUI_DeviceDVD "Dispositivo de DVD"
#define MSGTR_GUI_Directory "Caminho"
#define MSGTR_GUI_DirectoryTree "Árvore de diretórios"
#define MSGTR_GUI_DropSubtitle "Descartar legenda..."
#define MSGTR_GUI_DVD "DVD"
#define MSGTR_GUI_EnableAutomaticAVSync "AutoSync ligado/desligado"
#define MSGTR_GUI_EnableCache "Cache ligado/desligado"
#define MSGTR_GUI_EnableDirectRendering "Habilitar direct rendering"
#define MSGTR_GUI_EnableDoubleBuffering "Habilitar duplo buffer"
#define MSGTR_GUI_EnableEqualizer "Habilitar equalizador"
#define MSGTR_GUI_EnableExtraStereo "Habilitar extra estéreo"
#define MSGTR_GUI_EnableFrameDropping "Habilitar descarte de quadros"
#define MSGTR_GUI_EnableFrameDroppingIntense "Habilitar descarte de quadros SEVERO (perigoso)"
#define MSGTR_GUI_EnablePlaybar "Habilitar barra de reprodução"
#define MSGTR_GUI_EnablePostProcessing "Habilitar pós-processamento"
#define MSGTR_GUI_Encoding "Codificação"
#define MSGTR_GUI_Equalizer "Equalizador"
#define MSGTR_GUI_Error "Erro!"
#define MSGTR_GUI_ErrorFatal "Erro fatal!"
#define MSGTR_GUI_File "Reproduzir arquivo"
#define MSGTR_GUI_Files "Arquivos"
#define MSGTR_GUI_Flip "Inverter imagem verticalmente"
#define MSGTR_GUI_Font "Fonte"
#define MSGTR_GUI_FrameRate "FPS"
#define MSGTR_GUI_FrontLeft "Frente Esquerda"
#define MSGTR_GUI_FrontRight "Frente Direita"
#define MSGTR_GUI_HideVideoWindow "Mostrar janela do vídeo quando inativo"
#define MSGTR_GUI_Hue "Cor"
#define MSGTR_GUI_Lavc "Usar LAVC (FFmpeg)"
#define MSGTR_GUI_MaximumUsageSpareCPU "Qualidade do audio"
#define MSGTR_GUI_Miscellaneous "Misc"
#define MSGTR_GUI_MSG_DXR3NeedsLavc "Desculpe, você não pode reproduzir arquivos não-MPEG com o seu dispositivo DXR3/H+ sem recodificar.\nPor favor habilite lavc na configuração do DXR3/H+."
#define MSGTR_GUI_MSG_MemoryErrorWindow "Desculpe, sem memória suficiente para desenhar o buffer."
#define MSGTR_GUI_MSG_NoFileLoaded "Nenhum arquivo carregado"
#define MSGTR_GUI_MSG_NoMediaOpened "Nenhuma mídia aberta."
#define MSGTR_GUI_MSG_PlaybackNeedsRestart "Por favor lembre que você precisa reiniciar a reprodução para algumas opções fazerem efeito!"
#define MSGTR_GUI_MSG_SkinBitmapConversionError "erro na conversão 24 bit para 32 bit (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapNotFound "arquivo não encontrado (%s)\n"
#define MSGTR_GUI_MSG_SkinBitmapPngReadError "erro na leitura do PNG (%s)\n"
#define MSGTR_GUI_MSG_SkinCfgNotFound "Skin não encontrado (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorBitmap16Bit "16 bit ou menor profundidade de cores não suportado (%s).\n"
#define MSGTR_GUI_MSG_SkinErrorMessage "[skin] erro no arquivo de configuração do skin na linha %d: %s"
#define MSGTR_GUI_MSG_SkinFontFileNotFound "arquivo da fonte não encontrado\n"
#define MSGTR_GUI_MSG_SkinFontImageNotFound "arquivo de imagem da fonte não encontrado\n"
#define MSGTR_GUI_MSG_SkinFontNotFound "indentificador de fonte não existente (%s)\n"
#define MSGTR_GUI_MSG_SkinMemoryError "memória insuficiente\n"
#define MSGTR_GUI_MSG_SkinTooManyFonts "muitas fontes declaradas\n"
#define MSGTR_GUI_MSG_SkinUnknownMessage "mensagem desconhecida: %s\n"
#define MSGTR_GUI_MSG_SkinUnknownParameter "parâmetro desconhecido (%s)\n"
#define MSGTR_GUI_MSG_VideoOutError "Desculpe, eu não encontrei um driver saída de vídeo compatível com GUI."
#define MSGTR_GUI_Mute "Mudo"
#define MSGTR_GUI_NetworkStreaming "Rede..."
#define MSGTR_GUI_Next "Próxima faixa"
#define MSGTR_GUI_NoChapter "Nenhum capítulo"
#define MSGTR_GUI__none_ "(nenhum)"
#define MSGTR_GUI_NonInterleavedParser "Usar leitor de AVI não-entrelaçado"
#define MSGTR_GUI_NormalizeSound "Normalizar som"
#define MSGTR_GUI_Ok "OK"
#define MSGTR_GUI_Open "Abrir..."
#define MSGTR_GUI_Original "Original"
#define MSGTR_GUI_OsdLevel "Nível do OSD"
#define MSGTR_GUI_OSD_Subtitles "Legenda & OSD"
#define MSGTR_GUI_Outline "Contorno"
#define MSGTR_GUI_PanAndScan "Panscan"
#define MSGTR_GUI_Pause "Pausar"
#define MSGTR_GUI_Play "Reproduzir"
#define MSGTR_GUI_Playback "Reprodução"
#define MSGTR_GUI_Playlist "Lista de reprodução"
#define MSGTR_GUI_Position "Posição"
#define MSGTR_GUI_PostProcessing "Pós-processamento"
#define MSGTR_GUI_Preferences "Preferências"
#define MSGTR_GUI_Previous "Faixa anterior"
#define MSGTR_GUI_Quit "Sair"
#define MSGTR_GUI_RearLeft "Fundo Esquerda"
#define MSGTR_GUI_RearRight "Fundo Direita"
#define MSGTR_GUI_Remove "Remover"
#define MSGTR_GUI_Saturation "Saturação"
#define MSGTR_GUI_ScaleMovieDiagonal "Proporcional a diagonal do filme"
#define MSGTR_GUI_ScaleMovieHeight "Proporcional a altura do filme"
#define MSGTR_GUI_ScaleMovieWidth "Proporcional a largura do filme"
#define MSGTR_GUI_ScaleNo "Sem auto redimensionamento"
#define MSGTR_GUI_SeekingInBrokenMedia "Reconstruir tabela índice, se necessário"
#define MSGTR_GUI_SelectAudioFile "Selecionar canal de audio externo..."
#define MSGTR_GUI_SelectedFiles "Arquivos selecionados"
#define MSGTR_GUI_SelectFile "Selecionar arquivo..."
#define MSGTR_GUI_SelectFont "Selecionar fonte..."
#define MSGTR_GUI_SelectSubtitle "Selecionar legenda..."
#define MSGTR_GUI_SizeDouble "Tamanho dobrado"
#define MSGTR_GUI_SizeFullscreen "Tela cheia"
#define MSGTR_GUI_SizeNormal "Tamanho normal"
#define MSGTR_GUI_SizeOSD "Tamanho do OSD"
#define MSGTR_GUI_SizeSubtitles "Tamanho do texto"
#define MSGTR_GUI_SkinBrowser "Skins"
#define MSGTR_GUI_Sponsored "Desenvolvimento do GUI patrocinado por UHU Linux"
#define MSGTR_GUI_StartFullscreen "Iniciar em tela cheia"
#define MSGTR_GUI_Stop "Parar"
#define MSGTR_GUI_Subtitle "Legenda"
#define MSGTR_GUI_SubtitleAllowOverlap "Sobreposição da legenda"
#define MSGTR_GUI_SubtitleAutomaticLoad "Desabilitar auto carregamento de legendas"
#define MSGTR_GUI_SubtitleConvertMpsub "Converter a legenda dada para o formato de legenda do MPlayer"
#define MSGTR_GUI_SubtitleConvertSrt "Converter a legenda dada para o formato baseado em tempo SubViewer (SRT)"
#define MSGTR_GUI_Subtitles "Legendas"
#define MSGTR_GUI_SyncValue "Autosync"
#define MSGTR_GUI_TitleNN "Título %2d"
#define MSGTR_GUI_Titles "Títulos"
#define MSGTR_GUI_TrackN "Trilha %d"
#define MSGTR_GUI_TurnOffXScreenSaver "Parar XScreenSaver"
#define MSGTR_GUI_URL "Reproduzir URL"
#define MSGTR_GUI_VCD "VCD"
#define MSGTR_GUI_Video "Vídeo"
#define MSGTR_GUI_VideoEncoder "Codificador de video"
#define MSGTR_GUI_VideoTracks "Trilha de vídeo"
#define MSGTR_GUI_Warning "Atenção!"
