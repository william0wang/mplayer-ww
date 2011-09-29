static char big5_help_text[]=
"用法:   mplayer [選項] [URL|路徑/]文件名\n"
"\n"
"基本選項: (完整列表參見手冊頁)\n"
" -vo <drv>        選擇視頻輸出驅動 (查看驅動列表用“-vo help”)\n"
" -ao <drv>        選擇音頻輸出驅動 (查看驅動列表用“-ao help”)\n"
#ifdef CONFIG_VCD
" vcd://<trackno>  播放 (S)VCD 軌跡號 (原始設備, 無需安挂)\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>  從設備而不是普通文件上播放 DVD 標題號\n"
" -alang/-slang    選擇 DVD 音軌/字幕的語言(使用兩字符的國家代號)\n"
#endif
" -ss <position>   尋找到給定(多少秒或時分秒 hh:mm:ss 的)位置\n"
" -nosound         不播放聲音\n"
" -fs              全屏播放 (或者用 -vm, -zoom, 詳見于手冊頁)\n"
" -x <x> -y <y>    設置顯示的分辨率(提供給 -vm 或者 -zoom 使用)\n"
" -sub <file>      指定字幕文件 (參見 -subfps, -subdelay)\n"
" -playlist <file> 指定播放列表文件\n"
" -vid x -aid y    選擇用于播放的 x 視頻流和 y 音頻流通道號\n"
" -fps x -srate y  改變視頻率為 x 幀秒(fps)和音頻率為 y 赫茲(Hz)\n"
" -pp <quality>    使用後期處理過濾器/濾鏡 (詳見于手冊頁)\n"
" -framedrop       使用丟幀(用于慢機器)\n"
"\n"
"基本控製鍵: (完整的列表參見于手冊頁, 同時也請核查 input.conf)\n"
" <-  or  ->       後退/快進 10 秒\n"
" down or up       後退/快進 1 分鐘\n"
" pgdown or pgup   後退/快進 10 分鐘\n"
" < or >           跳到播放列表中的前一個/後一個\n"
" p or SPACE       暫停播放(按任意鍵繼續)\n"
" q or ESC         停止播放并退出程序\n"
" + or -           調整音頻延遲增加/減少 0.1 秒\n"
" o                循環 OSD 模式:  無/搜索條/搜索條加計時器\n"
" * or /           增加或減少 PCM 音量\n"
" x or z           調整字幕延遲增加/減少 0.1 秒\n"
" r or t           上/下調整字幕位置, 參見“-vf expand”\n"
"\n"
" * * *  詳細內容，更多的(高級)選項和控製鍵，請參見手冊頁  * * *\n"
"\n";

#define BIG5_INFOTR_MEDIAINFO "媒體信息"
#define BIG5_INFOTR_NORMAL "綜述"
#define BIG5_INFOTR_VIDEO "視頻"
#define BIG5_INFOTR_AUDIO "音頻"
#define BIG5_INFOTR_NAME "檔案"
#define BIG5_INFOTR_DEMUXER "容器"
#define BIG5_INFOTR_SIZE "大小"
#define BIG5_INFOTR_LENG "時長"
#define BIG5_INFOTR_FORMAT "格式"
#define BIG5_INFOTR_CODEC "解碼器"
#define BIG5_INFOTR_BITRATE "位元率"
#define BIG5_INFOTR_CLIPINFO "素材資訊"
#define BIG5_INFOTR_VIDEO_RESOLUTION "解析度"
#define BIG5_INFOTR_VIDEO_ASPECT "寬高比"
#define BIG5_INFOTR_VIDEO_FPS "幀速率"
#define BIG5_INFOTR_AUDIO_RATE "采樣率"
#define BIG5_INFOTR_AUDIO_NCH "聲道數"
#define BIG5_INFOTR_SUB "字幕"
#define BIG5_INFOTR_UNKNOWN "未知"
#define BIG5_INFOTR_TRACK "軌跡"
#define BIG5_INFOTR_NAME "名稱"
#define BIG5_INFOTR_AUTHOR "作者"
#define BIG5_INFOTR_COPYRIGHT "版權"
#define BIG5_INFOTR_COMMENTS "注釋"
#define BIG5_INFOTR_TITLE "標題"
#define BIG5_INFOTR_YEAR "年代"
#define BIG5_INFOTR_ALBUM "專輯"

// ========================= MPlayer messages ===========================

// mplayer.c:

//#define BIG5_MSGTR_Exiting "\n正在退出..\n"
//#define BIG5_MSGTR_ExitingHow "\n正在退出... (%s)\n"
//#define BIG5_MSGTR_BuiltinCodecsConf "使用內建默認的 codecs.conf 文件。\n"
//#define BIG5_MSGTR_CantLoadSub "不能加載字幕: %s\n"
//#define BIG5_MSGTR_FPSnotspecified "FPS 在文件頭中沒有指定或者無效，用 -fps 選項。\n"
//#define BIG5_MSGTR_StartPlaying "開始播放...\n"

//#define BIG5_MSGTR_SystemTooSlow "\n\n"\
//"         ************************************************\n"\
//"         ****      你的係統太“慢”了, 播放不了!     ****\n"\
//"         ************************************************\n"\
//" 可能的原因, 問題, 和解決辦法：\n"\
//"- 最常見的原因：損壞的或有錯誤的 _音頻_ 驅動\n"\
//"  - 試試 -ao sdl 或使用 ALSA  的 OSS 模擬。\n"\
//"  - 試驗不同的 -autosync 的值，不妨從 30 開始。\n"\
//"- 視頻輸出太慢\n"\
//"  - (參考 -vo help)試試 -vo 用不同的驅動或者試試 -framedrop！\n"\
//"- CPU 太慢\n"\
//"  - 不要試圖在慢速 CPU 上播放大的 DVD/DivX! 試試一些 lavdopts 選項,\n"\
//"    例如: -vfm ffmpeg -lavdopts lowres=1:fast:skiploopfilter=all。\n"\
//"- 損壞的文件\n"\
//"  - 試試下列選項的各種組合: -nobps -ni -forceidx -mc 0。\n"\
//"- 太慢的媒體(如 NFS/SMB 安挂點, DVD, VCD 等)\n"\
//"  - 試試 -cache 8192。\n"\
//"- 你在用 -cache 選項播放一個非交錯的 AVI 文件？\n"\
//"  - 試試 -nocache。\n"\
//"閱讀 DOCS/zh/video.html 和 DOCS/zh/sound.html 來尋找調整/加速的技巧。\n"\
//"如果這些一個都用不上, 閱讀 DOCS/zh/bugreports.html！\n\n"

//#define BIG5_MSGTR_Playing "\n正在播放 %s。\n"
//#define BIG5_MSGTR_NoSound "音頻: 沒聲音\n"
//#define BIG5_MSGTR_FPSforced "FPS 鎖定為 %5.3f  (ftime: %5.3f)。\n"
//#define BIG5_MSGTR_CompiledWithRuntimeDetection "編譯時用了實時 CPU 檢測。\n"
//#define BIG5_MSGTR_CannotReadVideoProperties "視頻: 無法讀取屬性\n"
//#define BIG5_MSGTR_NoStreamFound "找不到流媒體。\n"
//#define BIG5_MSGTR_ErrorInitializingVODevice "打開/初始化 (-vo) 所選的視頻輸出設備出錯。\n"
//#define BIG5_MSGTR_Video_NoVideo "視頻: 沒視頻\n"
//#define BIG5_MSGTR_LoadingConfig "正在裝載配置文件 '%s'\n"
//#define BIG5_MSGTR_LoadingProtocolProfile "加載協議相關的配置集‘%s’\n"
//#define BIG5_MSGTR_LoadingExtensionProfile "加載擴展組件相關的配置集‘%s’\n"
//#define BIG5_MSGTR_AddedSubtitleFile "字幕: 添加字幕文件 (%d): %s\n"

