static char gb_help_text[]=
"用法:   mplayer [选项] [URL|路径/]文件名\n"
"\n"
"基本选项: (完整列表参见手册页)\n"
" -vo <drv>        选择视频输出驱动 (查看驱动列表用“-vo help”)\n"
" -ao <drv>        选择音频输出驱动 (查看驱动列表用“-ao help”)\n"
#ifdef CONFIG_VCD
" vcd://<trackno>  播放 (S)VCD 轨迹号 (原始设备, 无需安挂)\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>  从设备而不是普通文件上播放 DVD 标题号\n"
" -alang/-slang    选择 DVD 音轨/字幕的语言(使用两字符的国家代号)\n"
#endif
" -ss <position>   寻找到给定(多少秒或时分秒 hh:mm:ss 的)位置\n"
" -nosound         不播放声音\n"
" -fs              全屏播放 (或者用 -vm, -zoom, 详见于手册页)\n"
" -x <x> -y <y>    设置显示的分辨率(提供给 -vm 或者 -zoom 使用)\n"
" -sub <file>      指定字幕文件 (参见 -subfps, -subdelay)\n"
" -playlist <file> 指定播放列表文件\n"
" -vid x -aid y    选择用于播放的 x 视频流和 y 音频流通道号\n"
" -fps x -srate y  改变视频率为 x 帧秒(fps)和音频率为 y 赫兹(Hz)\n"
" -pp <quality>    使用后期处理过滤器/滤镜 (详见于手册页)\n"
" -framedrop       使用丢帧(用于慢机器)\n"
"\n"
"基本控制键: (完整的列表参见于手册页, 同时也请核查 input.conf)\n"
" <-  or  ->       后退/快进 10 秒\n"
" down or up       后退/快进 1 分钟\n"
" pgdown or pgup   后退/快进 10 分钟\n"
" < or >           跳到播放列表中的前一个/后一个\n"
" p or SPACE       暂停播放(按任意键继续)\n"
" q or ESC         停止播放并退出程序\n"
" + or -           调整音频延迟增加/减少 0.1 秒\n"
" o                循环 OSD 模式:  无/搜索条/搜索条加计时器\n"
" * or /           增加或减少 PCM 音量\n"
" x or z           调整字幕延迟增加/减少 0.1 秒\n"
" r or t           上/下调整字幕位置, 参见“-vf expand”\n"
"\n"
" * * *  详细内容，更多的(高级)选项和控制键，请参见手册页  * * *\n"
"\n";

static char gbk_help_text[]=
"用法:   mplayer [選項] [URL|路徑/]文件名\n"
"\n"
"基本選項: (完整列表參見手冊頁)\n"
" -vo <drv>        選擇視頻輸出驅動 (查看驅動列表用“-vo help”)\n"
" -ao <drv>        選擇音頻輸出驅動 (查看驅動列表用“-ao help”)\n"
#ifdef HAVE_VCD
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

#define GB_INFOTR_MEDIAINFO "媒体信息"
#define GB_INFOTR_NORMAL "综述"
#define GB_INFOTR_VIDEO "视频"
#define GB_INFOTR_AUDIO "音频"
#define GB_INFOTR_NAME "文件"
#define GB_INFOTR_DEMUXER "容器"
#define GB_INFOTR_SIZE "大小"
#define GB_INFOTR_LENG "时长"
#define GB_INFOTR_FORMAT "格式"
#define GB_INFOTR_CODEC "解码器"
#define GB_INFOTR_BITRATE "比特率"
#define GB_INFOTR_CLIPINFO "剪辑信息"
#define GB_INFOTR_VIDEO_RESOLUTION "分辨率"
#define GB_INFOTR_VIDEO_ASPECT "宽高比"
#define GB_INFOTR_VIDEO_FPS "帧速率"
#define GB_INFOTR_AUDIO_RATE "采样率"
#define GB_INFOTR_AUDIO_NCH "声道数"
#define GB_INFOTR_SUB "字幕"
#define GB_INFOTR_UNKNOWN "未知"
#define GB_INFOTR_TRACK "轨道"
#define GB_INFOTR_NAME "名称"
#define GB_INFOTR_AUTHOR "作者"
#define GB_INFOTR_COPYRIGHT "版权"
#define GB_INFOTR_COMMENTS "注释"
#define GB_INFOTR_TITLE "标题"
#define GB_INFOTR_YEAR "年代"
#define GB_INFOTR_ALBUM "专辑"

#define GBK_INFOTR_MEDIAINFO "媒體信息"
#define GBK_INFOTR_NORMAL "綜述"
#define GBK_INFOTR_VIDEO "視頻"
#define GBK_INFOTR_AUDIO "音頻"
#define GBK_INFOTR_NAME "檔案"
#define GBK_INFOTR_DEMUXER "容器"
#define GBK_INFOTR_SIZE "大小"
#define GBK_INFOTR_LENG "時長"
#define GBK_INFOTR_FORMAT "格式"
#define GBK_INFOTR_CODEC "解碼器"
#define GBK_INFOTR_BITRATE "位元率"
#define GBK_INFOTR_CLIPINFO "素材資訊"
#define GBK_INFOTR_VIDEO_RESOLUTION "解析度"
#define GBK_INFOTR_VIDEO_ASPECT "寬高比"
#define GBK_INFOTR_VIDEO_FPS "幀速率"
#define GBK_INFOTR_AUDIO_RATE "采樣率"
#define GBK_INFOTR_AUDIO_NCH "聲道數"
#define GBK_INFOTR_SUB "字幕"
#define GBK_INFOTR_UNKNOWN "未知"
#define GBK_INFOTR_TRACK "軌跡"
#define GBK_INFOTR_NAME "名稱"
#define GBK_INFOTR_AUTHOR "作者"
#define GBK_INFOTR_COPYRIGHT "版權"
#define GBK_INFOTR_COMMENTS "注釋"
#define GBK_INFOTR_TITLE "標題"
#define GBK_INFOTR_YEAR "年代"
#define GBK_INFOTR_ALBUM "專輯"

// ========================= MPlayer messages ===========================

// mplayer.c:
//#define GB_MSGTR_Exiting "\n正在退出..\n"
//#define GB_MSGTR_ExitingHow "\n正在退出...（%s）\n"
//#define GB_MSGTR_BuiltinCodecsConf "使用内建默认的 codecs.conf 文件。\n"
//#define GB_MSGTR_CantLoadSub "不能加载字幕：%s\n"
//#define GB_MSGTR_FPSnotspecified "FPS 在文件头中没有指定或者无效，请使用 -fps 选项。\n"
//#define GB_MSGTR_StartPlaying "开始播放...\n"

//#define GB_MSGTR_SystemTooSlow "\n\n"\
//"         ************************************************\n"\
//"         ****     你的系统运行太“慢”，播放不了!    ****\n"\
//"         ************************************************\n"\
//" 可能的原因、问题和变通的办法：\n"\
//"- 最常见的原因：损坏的或有漏洞的 _audio_ 驱动\n"\
//"  - 试试 -ao sdl 或使用 ALSA  的 OSS 模拟方式。\n"\
//"  - 尝试使用不同的 -autosync 的值，不妨从 30 开始。\n"\
//"- 视频输出运行慢\n"\
//"  - 试试 -vo 用不同的驱动（参见 -vo help 以获取驱动列表）或者试试 -framedrop！\n"\
//"- CPU 运行慢\n"\
//"  - 不要试图在运行慢的 CPU 上播放大容量的 DVD/DivX！试试 lavdopts 中的一些选项，\n"\
//"    例如：-vfm ffmpeg -lavdopts lowres=1:fast:skiploopfilter=all。\n"\
//"- 文件损坏\n"\
//"  - 试试组合使用 -nobps -ni -forceidx -mc 0 这些选项。\n"\
//"- 媒体读取慢（NFS/SMB 挂载、DVD、VCD 等设备）\n"\
//"  - 试试 -cache 8192 选项。\n"\
//"- 你是否在用 -cache 选项播放一个非交错合并的 AVI 文件？\n"\
//"  - 试试 -nocache 选项。\n"\
//"阅读 DOCS/zh/video.html 和 DOCS/zh/sound.html，寻找调整/加速的技巧。\n"\
//"如果这些一个都帮不了你，请阅读 DOCS/zh/bugreports.html。\n\n"

//#define GB_MSGTR_Playing "\n正在播放 %s。\n"
//#define GB_MSGTR_NoSound "音频：没有音轨\n"
//#define GB_MSGTR_FPSforced "FPS 强制设为 %5.3f（ftime：%5.3f）。\n"
//#define GB_MSGTR_CompiledWithRuntimeDetection "编译时包含了实时 CPU 类型检测。\n"
//#define GB_MSGTR_CannotReadVideoProperties "视频：无法读取视频属性\n"
//#define GB_MSGTR_NoStreamFound "未找到媒体流。\n"
//#define GB_MSGTR_ErrorInitializingVODevice "打开/初始化所选的（-vo）视频输出设备出错。\n"
//#define GB_MSGTR_Video_NoVideo "视频：没有视频流\n"
//#define GB_MSGTR_LoadingConfig "正在加载配置 '%s'\n"
//#define GB_MSGTR_LoadingProtocolProfile "加载协议相关的配置集‘%s’\n"
//#define GB_MSGTR_LoadingExtensionProfile "加载扩展组件相关的配置集‘%s’\n"
//#define GB_MSGTR_AddedSubtitleFile "字幕：添加字幕文件（%d）：%s\n"


// ========================= MPlayer messages ===========================

// mplayer.c:

//#define GBK_MSGTR_Exiting "\n正在退出..\n"
//#define GBK_MSGTR_ExitingHow "\n正在退出... (%s)\n"
//#define GBK_MSGTR_BuiltinCodecsConf "使用內建默認的 codecs.conf 文件。\n"
//#define GBK_MSGTR_CantLoadSub "不能加載字幕: %s\n"
//#define GBK_MSGTR_FPSnotspecified "FPS 在文件頭中沒有指定或者無效，用 -fps 選項。\n"
//#define GBK_MSGTR_StartPlaying "開始播放...\n"

//#define GBK_MSGTR_SystemTooSlow "\n\n"\
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

//#define GBK_MSGTR_Playing "\n正在播放 %s。\n"
//#define GBK_MSGTR_NoSound "音頻: 沒聲音\n"
//#define GBK_MSGTR_FPSforced "FPS 鎖定為 %5.3f  (ftime: %5.3f)。\n"
//#define GBK_MSGTR_CompiledWithRuntimeDetection "編譯時用了實時 CPU 檢測。\n"
//#define GBK_MSGTR_CannotReadVideoProperties "視頻: 無法讀取屬性\n"
//#define GBK_MSGTR_NoStreamFound "找不到流媒體。\n"
//#define GBK_MSGTR_ErrorInitializingVODevice "打開/初始化 (-vo) 所選的視頻輸出設備出錯。\n"
//#define GBK_MSGTR_Video_NoVideo "視頻: 沒視頻\n"
//#define GBK_MSGTR_LoadingConfig "正在裝載配置文件 '%s'\n"
//#define GBK_MSGTR_LoadingProtocolProfile "加載協議相關的配置集‘%s’\n"
//#define GBK_MSGTR_LoadingExtensionProfile "加載擴展組件相關的配置集‘%s’\n"
//#define GBK_MSGTR_AddedSubtitleFile "字幕: 添加字幕文件 (%d): %s\n"

