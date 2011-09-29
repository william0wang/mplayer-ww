#if defined(__MINGW32__) || defined(__CYGWIN__)
#include "resource.h"
#include "libavformat/avformat.h"

#define INFO_MAX 512

static HANDLE hInfoThread = NULL;

extern int info_to_html;

#define MATROSKA_TRACK_VIDEO	0x01 /* rectangle-shaped pictures aka video */
#define MATROSKA_TRACK_AUDIO	0x02 /* anything you can hear */
#define MATROSKA_TRACK_SUBTITLE 0x11 /* text-subtitles */

typedef struct lavf_priv_t{
    AVInputFormat *avif;
}lavf_priv_t;

typedef struct da_priv {
  int frmt;
} da_priv_t;

typedef struct
{
  uint32_t order, type, scope;
  uint32_t comp_algo;
  uint8_t *comp_settings;
  int comp_settings_len;
} mkv_content_encoding_t;

typedef struct mkv_track
{
  int tnum;
  char *name;

  char *codec_id;
  int ms_compat;
  char *language;

  int type;

  uint32_t v_width, v_height, v_dwidth, v_dheight;
  float v_frate;

  uint32_t a_formattag;
  uint32_t a_channels, a_bps;
  float a_sfreq;

  float default_duration;

  int default_track;

  void *private_data;
  unsigned int private_size;

  /* stuff for realmedia */
  int realmedia;
  int rv_kf_base, rv_kf_pts;
  float rv_pts;  /* previous video timestamp */
  float ra_pts;  /* previous audio timestamp */

  /** realaudio descrambling */
  int sub_packet_size; ///< sub packet size, per stream
  int sub_packet_h; ///< number of coded frames per block
  int coded_framesize; ///< coded frame size, per stream
  int audiopk_size; ///< audio packet size
  unsigned char *audio_buf; ///< place to store reordered audio data
  float *audio_timestamp; ///< timestamp for each audio packet
  int sub_packet_cnt; ///< number of subpacket already received
  int audio_filepos; ///< file position of first audio packet in block

  /* stuff for quicktime */
  int fix_i_bps;
  float qt_last_a_pts;

  int subtitle_type;

  /* The timecodes of video frames might have to be reordered if they're
	 in display order (the timecodes, not the frames themselves!). In this
	 case demux packets have to be cached with the help of these variables. */
  int reorder_timecodes;
  demux_packet_t **cached_dps;
  int num_cached_dps, num_allocated_dps;
  float max_pts;

  /* generic content encoding support */
  mkv_content_encoding_t *encodings;
  int num_encodings;

  /* For VobSubs and SSA/ASS */
  sh_sub_t *sh_sub;
} mkv_track_t;

typedef struct mkv_index
{
  int tnum;
  uint64_t timecode, filepos;
} mkv_index_t;

typedef struct mkv_demuxer
{
  off_t segment_start;

  float duration, last_pts;
  uint64_t last_filepos;

  mkv_track_t **tracks;
  int num_tracks;

  uint64_t tc_scale, cluster_tc, first_tc;
  int has_first_tc;

  uint64_t cluster_size;
  uint64_t blockgroup_size;

  mkv_index_t *indexes;
  int num_indexes;

  off_t *parsed_cues;
  int parsed_cues_num;
  off_t *parsed_seekhead;
  int parsed_seekhead_num;

  uint64_t *cluster_positions;
  int num_cluster_pos;

  int64_t skip_to_timecode;
  int v_skip_to_keyframe, a_skip_to_keyframe;

  int64_t stop_timecode;

  int last_aid;
  int audio_tracks[MAX_A_STREAMS];
} mkv_demuxer_t;

static int format_use_cache()
{
	lavf_priv_t *lavf_priv;
	if(mpctx->demuxer)	{
		if(mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED ||mpctx->demuxer->type == DEMUXER_TYPE_LAVF) {
			lavf_priv = (lavf_priv_t *)mpctx->demuxer->priv;
			if(lavf_priv && (!strcasecmp(lavf_priv->avif->name, "mpegts") ||
				!strcasecmp(lavf_priv->avif->name, "matroska,webm") ||
				!strcasecmp(lavf_priv->avif->name, "mov,mp4,m4a,3gp,3g2,mj2")))
					return 1;
		} else if(mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA ||
				mpctx->demuxer->type == DEMUXER_TYPE_MPEG_TS ||
				mpctx->demuxer->type == DEMUXER_TYPE_AVI ||
				mpctx->demuxer->type == DEMUXER_TYPE_MOV ||
				mpctx->demuxer->type == DEMUXER_TYPE_ASF) {
			return 1;
		}
	}
	return 0;
}

static void write_text_table_head(FILE *fd , const char *str)
{
	if(!fd)
		return;

	fprintf(fd ,"%s,_, ,_, \n", str);
}

static void write_text_table(FILE *fd , const char *name, const char *format, ... )
{
	if(!fd)
		return;
	char tmp[INFO_MAX];

	if(format) {
		va_list va;
		va_start(va, format);
		vsnprintf(tmp, INFO_MAX, format, va);
		va_end(va);
	}
	
	fprintf(fd ," ,_,%s,_,%s\n", name, tmp);
}

static void write_html_head(FILE *fd)
{
	if(!fd)
		return;
	fprintf(fd ,"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	if(sys_Language == 4)
		fprintf(fd ,"<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=big5\"/>\n");
	else
		fprintf(fd ,"<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gbk\"/>\n");
	fprintf(fd ,"<title>MediaInfo</title>\n<style type=\"text/css\"><!--\n");
	fprintf(fd ,"body {font: 12px/100%% Verdana, Arial, Helvetica, sans-serif; margin: 0;padding: 0;text-align: left;color: #000000;}\n");
	fprintf(fd ,"table {font: 12px/100%% Verdana, Arial, Helvetica, sans-serif;}\n");
	fprintf(fd ,".oneColElsCtr #container {text-align: center;}\n");
	fprintf(fd ,".oneColElsCtr #mainContent { padding: 0 10px; text-align: left;}\n.bule {color: #3366FF}\n");
	fprintf(fd ,".info{position:relative;left:10px;height:20px;}\n");
	fprintf(fd ,".info_name{position:absolute;left:10px;width:150px}\n");
	fprintf(fd ,".info_val{position:absolute;left:160px;width:950px}\n");
	fprintf(fd ,"--></style>\n</head>\n<body class=\"oneColElsCtr\"><div id=\"container\">\n<h2 class=\"bule\">");
	if(sys_Language == 1)
		fprintf(fd ,GB_INFOTR_MEDIAINFO);
	else if(sys_Language == 3)
		fprintf(fd ,GBK_INFOTR_MEDIAINFO);
	else if(sys_Language == 4)
		fprintf(fd ,BIG5_INFOTR_MEDIAINFO);
	else
		fprintf(fd ,INFOTR_MEDIAINFO);
	fprintf(fd ,"</h2><div id=\"mainContent\">\n");
}

static void write_html_table_head(FILE *fd , const char *str)
{
	if(!fd)
		return;

	fprintf(fd ,"<div style=\"padding-top:5px;padding-bottom:5px;\"><font color=\"#3366ff\"><b>%s</b></font></div>\n", str);
}

static void write_html_table(FILE *fd , const char *name, const char *format, ... )
{
	if(!fd)
		return;
	char tmp[INFO_MAX];
	if(format) {
		va_list va;
		va_start(va, format);
		vsnprintf(tmp, INFO_MAX, format, va);
		va_end(va);
	}
	
	fprintf(fd ,"<div class=\"info\"><div class=\"info_name\">%s</div>\n", name);
	fprintf(fd ,"<div class=\"info_val\">%s</div></div>\n", tmp);
}

static char str_name[16] ,str_video[16] ,str_audio[16] ,str_len[16] ,str_track[16];
static char str_clipinfo[16] ,str_demuxer[16] ,str_codec[16], str_unknown[16];
static char str_format[16] ,str_bitrate[16] ,str_fps[16] ,str_aspect[16] ,str_sub[16];
static char str_normal[16]  ,str_rate[16] ,str_resolution[16] ,str_nch[16] ,str_size[16];
static void initlang()
{
	if(sys_Language == 1) {
		snprintf(str_size, 16 ,GB_INFOTR_SIZE);
		snprintf(str_len, 16 ,GB_INFOTR_LENG);
		snprintf(str_name, 16 ,GB_INFOTR_NAME);
		snprintf(str_codec, 16 ,GB_INFOTR_CODEC);
		snprintf(str_video, 16 ,GB_INFOTR_VIDEO);
		snprintf(str_audio, 16 ,GB_INFOTR_AUDIO);
		snprintf(str_normal, 16 ,GB_INFOTR_NORMAL);
		snprintf(str_format, 16 ,GB_INFOTR_FORMAT);
		snprintf(str_bitrate, 16 ,GB_INFOTR_BITRATE);
		snprintf(str_demuxer, 16 ,GB_INFOTR_DEMUXER);
		snprintf(str_clipinfo, 16 ,GB_INFOTR_CLIPINFO);
		snprintf(str_aspect, 16 ,GB_INFOTR_VIDEO_ASPECT);
		snprintf(str_resolution, 16 ,GB_INFOTR_VIDEO_RESOLUTION);
		snprintf(str_fps, 16 ,GB_INFOTR_VIDEO_FPS);
		snprintf(str_rate, 16 ,GB_INFOTR_AUDIO_RATE);
		snprintf(str_nch, 16 ,GB_INFOTR_AUDIO_NCH);
		snprintf(str_sub, 16 ,GB_INFOTR_SUB);
		snprintf(str_track, 16 ,GB_INFOTR_TRACK);
		snprintf(str_unknown, 16 ,GB_INFOTR_UNKNOWN);
	}
	else if(sys_Language == 3) {
		snprintf(str_size, 16 ,GBK_INFOTR_SIZE);
		snprintf(str_len, 16 ,GBK_INFOTR_LENG);
		snprintf(str_name, 16 ,GBK_INFOTR_NAME);
		snprintf(str_codec, 16 ,GBK_INFOTR_CODEC);
		snprintf(str_video, 16 ,GBK_INFOTR_VIDEO);
		snprintf(str_audio, 16 ,GBK_INFOTR_AUDIO);
		snprintf(str_normal, 16 ,GBK_INFOTR_NORMAL);
		snprintf(str_format, 16 ,GBK_INFOTR_FORMAT);
		snprintf(str_bitrate, 16 ,GBK_INFOTR_BITRATE);
		snprintf(str_demuxer, 16 ,GBK_INFOTR_DEMUXER);
		snprintf(str_clipinfo, 16 ,GBK_INFOTR_CLIPINFO);
		snprintf(str_aspect, 16 ,GBK_INFOTR_VIDEO_ASPECT);
		snprintf(str_resolution, 16 ,GBK_INFOTR_VIDEO_RESOLUTION);
		snprintf(str_fps, 16 ,GBK_INFOTR_VIDEO_FPS);
		snprintf(str_rate, 16 ,GBK_INFOTR_AUDIO_RATE);
		snprintf(str_nch, 16 ,GBK_INFOTR_AUDIO_NCH);
		snprintf(str_sub, 16 ,GBK_INFOTR_SUB);
		snprintf(str_track, 16 ,GBK_INFOTR_TRACK);
		snprintf(str_unknown, 16 ,GBK_INFOTR_UNKNOWN);
	}
	else if(sys_Language == 4) {
		snprintf(str_size, 16 ,BIG5_INFOTR_SIZE);
		snprintf(str_len, 16 ,BIG5_INFOTR_LENG);
		snprintf(str_name, 16 ,BIG5_INFOTR_NAME);
		snprintf(str_codec, 16 ,BIG5_INFOTR_CODEC);
		snprintf(str_video, 16 ,BIG5_INFOTR_VIDEO);
		snprintf(str_audio, 16 ,BIG5_INFOTR_AUDIO);
		snprintf(str_normal, 16 ,BIG5_INFOTR_NORMAL);
		snprintf(str_format, 16 ,BIG5_INFOTR_FORMAT);
		snprintf(str_bitrate, 16 ,BIG5_INFOTR_BITRATE);
		snprintf(str_demuxer, 16 ,BIG5_INFOTR_DEMUXER);
		snprintf(str_clipinfo, 16 ,BIG5_INFOTR_CLIPINFO);
		snprintf(str_aspect, 16 ,BIG5_INFOTR_VIDEO_ASPECT);
		snprintf(str_resolution, 16 ,BIG5_INFOTR_VIDEO_RESOLUTION);
		snprintf(str_fps, 16 ,BIG5_INFOTR_VIDEO_FPS);
		snprintf(str_rate, 16 ,BIG5_INFOTR_AUDIO_RATE);
		snprintf(str_nch, 16 ,BIG5_INFOTR_AUDIO_NCH);
		snprintf(str_sub, 16 ,BIG5_INFOTR_SUB);
		snprintf(str_track, 16 ,BIG5_INFOTR_TRACK);
		snprintf(str_unknown, 16 ,BIG5_INFOTR_UNKNOWN);
	}
	else {
		snprintf(str_size, 16 ,INFOTR_SIZE);
		snprintf(str_len, 16 ,INFOTR_LENG);
		snprintf(str_name, 16 ,INFOTR_NAME);
		snprintf(str_codec, 16 ,INFOTR_CODEC);
		snprintf(str_video, 16 ,INFOTR_VIDEO);
		snprintf(str_audio, 16 ,INFOTR_AUDIO);
		snprintf(str_normal, 16 ,INFOTR_NORMAL);
		snprintf(str_format, 16 ,INFOTR_FORMAT);
		snprintf(str_bitrate, 16 ,INFOTR_BITRATE);
		snprintf(str_demuxer, 16 ,INFOTR_DEMUXER);
		snprintf(str_clipinfo, 16 ,INFOTR_CLIPINFO);
		snprintf(str_aspect, 16 ,INFOTR_VIDEO_ASPECT);
		snprintf(str_resolution, 16 ,INFOTR_VIDEO_RESOLUTION);
		snprintf(str_fps, 16 ,INFOTR_VIDEO_FPS);
		snprintf(str_rate, 16 ,INFOTR_AUDIO_RATE);
		snprintf(str_nch, 16 ,INFOTR_AUDIO_NCH);
		snprintf(str_sub, 16 ,INFOTR_SUB);
		snprintf(str_track, 16 ,INFOTR_TRACK);
		snprintf(str_unknown, 16 ,INFOTR_UNKNOWN);
	}
}

static off_t get_video_bps(double ibps)
{
	off_t v_bps = 0, mediasize, len, a_bps;
	if(ibps > 1)
		return ((off_t)ibps);
	mediasize = mpctx->demuxer->movi_end - mpctx->demuxer->movi_start;
	len = demuxer_get_time_length(mpctx->demuxer);
	if(mediasize > 0 && len > 0) {
		v_bps = mediasize*8 / 1000 / len;
		if (mpctx->sh_audio) {
			a_bps = mpctx->sh_audio->i_bps*8 / 1000;
			if(a_bps > 0)
				v_bps -= a_bps;
			if(v_bps < 0)
				return 0;
		}
	}
	return v_bps;
}

static double get_aspect(double aspect)
{
	if(aspect < 0.1)
		aspect = (double) mpctx->sh_video->disp_w / (double)mpctx->sh_video->disp_h;
	return aspect;
}

static void format_metadata(char *out_str, const char *in_str)
{
	snprintf(out_str, INFO_MAX ,in_str);
	if(stricmp(in_str, "name") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_NAME);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_NAME);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_NAME);
	} else if(stricmp(in_str, "title") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_TITLE);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_TITLE);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_TITLE);
	} else if(stricmp(in_str, "album") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_ALBUM);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_ALBUM);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_ALBUM);
	} else if(stricmp(in_str, "track") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_TRACK);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_TRACK);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_TRACK);
	} else if(stricmp(in_str, "year") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_YEAR);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_YEAR);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_YEAR);
	} else if(stricmp(in_str, "copyright") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_COPYRIGHT);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_COPYRIGHT);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_COPYRIGHT);
	} else if(stricmp(in_str, "author") == 0 || stricmp(in_str, "artist") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_AUTHOR);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_AUTHOR);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_AUTHOR);
	} else if(stricmp(in_str, "comments") == 0 || stricmp(in_str, "comment") == 0) {
		if(sys_Language == 1)
			snprintf(out_str, INFO_MAX ,GB_INFOTR_COMMENTS);
		else if(sys_Language == 3)
			snprintf(out_str, INFO_MAX ,GBK_INFOTR_COMMENTS);
		else if(sys_Language == 4)
			snprintf(out_str, INFO_MAX ,BIG5_INFOTR_COMMENTS);
	}
}

static void get_videocodec(unsigned int format, char *vcodec)
{
	switch(format)
	{
	case 0x10000001:
		snprintf(vcodec, 64, "MPEG-1");
		break;
	case 0x10000002:
		snprintf(vcodec, 64, "MPEG-2");
		break;
	case 0x10000004:
		snprintf(vcodec, 64, "MPEG-4");
		break;
	case 0x10000005:
		snprintf(vcodec, 64, "H264");
		break;
	default:
		if (mpctx->sh_video->format >= 0x20202020)
			snprintf(vcodec, 64, "%.4s", (char *)&mpctx->sh_video->format);
		else
			snprintf(vcodec, 64, "0x%08X", mpctx->sh_video->format);
		break;
	}
}

static void get_audiocodec(unsigned int format, char *acodec)
{
	switch(format)
	{
	case 0x0:
	case 0x1:
		snprintf(acodec, 64, "Uncompressed PCM");
		break;
	case 0x3:
		snprintf(acodec, 64, "Uncompressed PCM IEEE float");
		break;
	case 0xfffe:
		snprintf(acodec, 64, "Uncompressed PCM Extended");
		break;
	case 0x50:
		snprintf(acodec, 64, "MP2");
		break;
	case 0x55:
		snprintf(acodec, 64, "MP3");
		break;
	case 0xff:
	case 0x706D:
		snprintf(acodec, 64, "AAC");
		break;
	case 0x160:
		snprintf(acodec, 64, "WMA1");
		break;
	case 0x161:
		snprintf(acodec, 64, "WMA2");
		break;
	case 0x162:
	case 0x163:
		snprintf(acodec, 64, "WMA3");
		break;
	case 0x566F:
		snprintf(acodec, 64, "Vorbis");
		break;
	case 0x2000:
		snprintf(acodec, 64, "AC3");
		break;
	case 0x2001:
		snprintf(acodec, 64, "DTS");
		break;
	case 0xF1AC:
		snprintf(acodec, 64, "FLAC");
		break;
	default:
		if (mpctx->sh_audio->format >= 0x20202020)
			snprintf(acodec, 64, "%.4s", (char *)&mpctx->sh_audio->format);
		else
			snprintf(acodec, 64, "%X", mpctx->sh_audio->format);
	}
}

static int create_html(const char *htmlfile)
{
	int len, n;
	float flen;
	double mediasize;
	mkv_demuxer_t *mkv_d;
	lavf_priv_t *lavf_priv;
	da_priv_t *audio_priv;
	char **info, vcodec[64], acodec[64], muxer[64];
	FILE *fd = NULL;
	if ( ( fd = fopen(htmlfile,"w") ) == NULL )
		return 0;
	
	initlang();
	write_text_table_head(fd,str_normal);
	write_text_table(fd,str_name,filename);
	if(mpctx->demuxer)
	{
		mediasize = mpctx->demuxer->movi_end - mpctx->demuxer->movi_start;
		if(mediasize > 1024 * 1024)
			write_text_table(fd, str_size,"%.02f MB",mediasize/(1024 * 1024));
		else
			write_text_table(fd, str_size,"%.02f KB",mediasize/1024);
		len = flen = demuxer_get_time_length(mpctx->demuxer);
		flen = (flen - len) * 1000;
		write_text_table(fd,str_len,"%02d:%02d:%02d.%03d",len/3600,(len/60)%60,len%60,(int)flen);

		if(mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED) {
			lavf_priv = (lavf_priv_t *)mpctx->demuxer->priv;
			snprintf(muxer, 64, "[lavfpref] %s", lavf_priv->avif->name);
		} else if (mpctx->demuxer->type == DEMUXER_TYPE_LAVF) {
			lavf_priv = (lavf_priv_t *)mpctx->demuxer->priv;
			snprintf(muxer, 64, "[lavf] %s", lavf_priv->avif->name);
		} else if (mpctx->demuxer->type == DEMUXER_TYPE_AUDIO) {
			audio_priv = (da_priv_t *)mpctx->demuxer->priv;
			if(audio_priv->frmt == 1)
				snprintf(muxer, 64, "[audio] mp3");
			else if(audio_priv->frmt == 2)
				snprintf(muxer, 64, "[audio] wav");
			else if(audio_priv->frmt == 3)
				snprintf(muxer, 64, "[audio] flac");
			else
				snprintf(muxer, 64, "[audio] unknow");
		} else {
			snprintf(muxer, 64, "%s", mpctx->demuxer->desc->name);
		}
		write_text_table(fd, str_demuxer, muxer);
	
		if(mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA)
		{
			mkv_d = (mkv_demuxer_t *)mpctx->demuxer->priv;
			int i, vid=0, aid=0, sid=0;
			for (i=0; i<mkv_d->num_tracks; i++)
			{
				char *type = str_unknown, str[32];
				*str = '\0';
				switch (mkv_d->tracks[i]->type)
				{
				case MATROSKA_TRACK_VIDEO:
				  type = str_video;
				  sprintf (str, "-vid %u", vid++);
				  break;
				case MATROSKA_TRACK_AUDIO:
				  type = str_audio;
				  sprintf (str, "-aid %u, -alang %.5s",aid++,mkv_d->tracks[i]->language);
				  break;
				case MATROSKA_TRACK_SUBTITLE:
				  type = str_sub;
				  sprintf (str, "-sid %u, -slang %.5s",sid++,mkv_d->tracks[i]->language);
				  break;
				}
				char trackid[32];
				snprintf(trackid, 32, "%s %u", str_track, mkv_d->tracks[i]->tnum);
				if (mkv_d->tracks[i]->name)
					write_text_table(fd, trackid, "%s (%s) \"%s\", %s",type , mkv_d->tracks[i]->codec_id
						,string_recode(mkv_d->tracks[i]->name), str);
				else
					write_text_table(fd, trackid, "%s (%s), %s",type
						, mkv_d->tracks[i]->codec_id , str);
			}
		}
	}
	if (mpctx->sh_video && !fake_video)
	{
		get_videocodec(mpctx->sh_video->format, vcodec);
		write_text_table_head(fd,str_video);
		write_text_table(fd,str_format,vcodec);
		write_text_table(fd,str_resolution,"%d x %d",mpctx->sh_video->disp_w , mpctx->sh_video->disp_h);
		write_text_table(fd,str_aspect,"%1.4f",get_aspect(mpctx->sh_video->aspect));
		write_text_table(fd,str_bitrate,"%d Kbps",get_video_bps(mpctx->sh_video->i_bps*8 / 1024));
		write_text_table(fd,str_fps,"%5.3f",mpctx->sh_video->fps);
		write_text_table(fd,str_codec,"[%s] %s",mpctx->sh_video->codec->name ,mpctx->sh_video->codec->info);
	}
	if (mpctx->sh_audio)
	{
		get_audiocodec(mpctx->sh_audio->format, acodec);
		write_text_table_head(fd,str_audio);
		write_text_table(fd,str_format,acodec);
		write_text_table(fd,str_bitrate,"%d Kbps",mpctx->sh_audio->i_bps*8 / 1000);
		write_text_table(fd,str_rate,"%d Hz",mpctx->sh_audio->samplerate);
		write_text_table(fd,str_nch,"%d",mpctx->sh_audio->channels);
		write_text_table(fd,str_codec,"[%s] %s",mpctx->sh_audio->codec->name ,mpctx->sh_audio->codec->info);
	}
	if(mpctx->demuxer && mpctx->demuxer->info) {
		info = mpctx->demuxer->info;
		write_text_table_head(fd,str_clipinfo);
		for (n = 0; info[2 * n] != NULL; n++)
		{
			char info_name[INFO_MAX];
			format_metadata(info_name, info[2 * n]);
			write_text_table(fd,info_name,string_recode(info[2 * n + 1]));
		}
	}
	fclose(fd);
	return 1;
}

static void addinfo(HWND hDlg, int is_title, const char *name, const char *format, ... )
{
	char str[STRING_MAX];
	char tmp[INFO_MAX];
	int len;
	if(format)
	{
		va_list va;
		va_start(va, format);
		vsnprintf(tmp, INFO_MAX, format, va);
		va_end(va);
	}

	if (is_title) {
		if(is_title > 1) SendDlgItemMessage(hDlg, IDC_LIST_MEDIAINFO, LB_ADDSTRING, 0, "");
		snprintf(str,STRING_MAX,"%s", name);
	} else {
		char names[STRING_MAX];
		snprintf(names , STRING_MAX ,"%s: ",name);
		snprintf(str,STRING_MAX,"   %-15s%s", names, tmp);
	}
	SendDlgItemMessage(hDlg, IDC_LIST_MEDIAINFO, LB_ADDSTRING, 0, (LPARAM)str);
	len = lstrlen(str) * 6;
	if(len > (LONG)SendDlgItemMessage(hDlg, IDC_LIST_MEDIAINFO, LB_GETHORIZONTALEXTENT, 0, 0))
		SendDlgItemMessage(hDlg, IDC_LIST_MEDIAINFO, LB_SETHORIZONTALEXTENT, len, 0);
}

static void initlist(HWND hDlg)
{
	int len, n;
	float flen;
	double mediasize;
	mkv_demuxer_t *mkv_d;
	lavf_priv_t *lavf_priv;
	da_priv_t *audio_priv;
	char **info, vcodec[64], acodec[64], muxer[64];
	SendDlgItemMessage(hDlg, IDC_LIST_MEDIAINFO, LB_RESETCONTENT, 0, 0);
	initlang();
	addinfo(hDlg, 1,  str_normal, "");
	addinfo(hDlg, 0,  str_name,filename);
	if(mpctx->demuxer)
	{
		mediasize = mpctx->demuxer->movi_end - mpctx->demuxer->movi_start;
		if(mediasize > 1024 * 1024)
			addinfo(hDlg, 0,  str_size,"%.02f MB",mediasize/(1024 * 1024));
		else
			addinfo(hDlg, 0,  str_size,"%.02f KB",mediasize/1024);
		len = flen = demuxer_get_time_length(mpctx->demuxer);
		flen = (flen - len) * 1000;
		if(len < 0) {
			len = 0;
			flen = 0;
		}
		addinfo(hDlg, 0,  str_len,"%02d:%02d:%02d.%03d",len/3600,(len/60)%60,len%60,(int)flen);

		if(mpctx->demuxer->type == DEMUXER_TYPE_LAVF_PREFERRED) {
			lavf_priv = (lavf_priv_t *)mpctx->demuxer->priv;
			snprintf(muxer, 64, "[lavfpref] %s", lavf_priv->avif->name);
		} else if (mpctx->demuxer->type == DEMUXER_TYPE_LAVF) {
			lavf_priv = (lavf_priv_t *)mpctx->demuxer->priv;
			snprintf(muxer, 64, "[lavf] %s", lavf_priv->avif->name);
		} else if (mpctx->demuxer->type == DEMUXER_TYPE_AUDIO) {
			audio_priv = (da_priv_t *)mpctx->demuxer->priv;
			if(audio_priv->frmt == 1)
				snprintf(muxer, 64, "[audio] mp3");
			else if(audio_priv->frmt == 2)
				snprintf(muxer, 64, "[audio] wav");
			else if(audio_priv->frmt == 3)
				snprintf(muxer, 64, "[audio] flac");
			else
				snprintf(muxer, 64, "[audio] unknow");
		} else {
			snprintf(muxer, 64, "%s", mpctx->demuxer->desc->name);
		}
		addinfo(hDlg, 0, str_demuxer, muxer);

		if(mpctx->demuxer->type == DEMUXER_TYPE_MATROSKA)
		{
			mkv_d = (mkv_demuxer_t *)mpctx->demuxer->priv;
			int i, vid=0, aid=0, sid=0;
			for (i=0; i<mkv_d->num_tracks; i++)
			{
				char *type = str_unknown, str[32];
				*str = '\0';
				switch (mkv_d->tracks[i]->type)
				{
				case MATROSKA_TRACK_VIDEO:
				  type = str_video;
				  sprintf (str, "-vid %u", vid++);
				  break;
				case MATROSKA_TRACK_AUDIO:
				  type = str_audio;
				  sprintf (str, "-aid %u, -alang %.5s",aid++,mkv_d->tracks[i]->language);
				  break;
				case MATROSKA_TRACK_SUBTITLE:
				  type = str_sub;
				  sprintf (str, "-sid %u, -slang %.5s",sid++,mkv_d->tracks[i]->language);
				  break;
				}
				char trackid[32];
				snprintf(trackid, 32, "%s %u", str_track, mkv_d->tracks[i]->tnum);
				if (mkv_d->tracks[i]->name)
					addinfo(hDlg, 0,  trackid, "%s (%s) \"%s\", %s",type , mkv_d->tracks[i]->codec_id
						,string_recode(mkv_d->tracks[i]->name), str);
				else
					addinfo(hDlg, 0,  trackid, "%s (%s), %s",type
						, mkv_d->tracks[i]->codec_id , str);
			}
		}
	}
	if (mpctx->sh_video && !fake_video)
	{
		get_videocodec(mpctx->sh_video->format, vcodec);
		addinfo(hDlg, 2,  str_video, "");
		addinfo(hDlg, 0,  str_format, vcodec);
		addinfo(hDlg, 0,  str_resolution,"%d x %d",mpctx->sh_video->disp_w , mpctx->sh_video->disp_h);
		addinfo(hDlg, 0,  str_aspect,"%1.4f",get_aspect(mpctx->sh_video->aspect));
		addinfo(hDlg, 0,  str_bitrate,"%d Kbps",get_video_bps(mpctx->sh_video->i_bps*8 / 1024));
		addinfo(hDlg, 0,  str_fps,"%5.3f",mpctx->sh_video->fps);
		addinfo(hDlg, 0,  str_codec,"[%s] %s",mpctx->sh_video->codec->name ,mpctx->sh_video->codec->info);
	}
	if (mpctx->sh_audio)
	{
		get_audiocodec(mpctx->sh_audio->format, acodec);
		addinfo(hDlg, 2,  str_audio,"");
		addinfo(hDlg, 0,  str_format, acodec);
		addinfo(hDlg, 0,  str_bitrate,"%d Kbps",mpctx->sh_audio->i_bps*8 / 1000);
		addinfo(hDlg, 0,  str_rate,"%d Hz",mpctx->sh_audio->samplerate);
		addinfo(hDlg, 0,  str_nch,"%d",mpctx->sh_audio->channels);
		addinfo(hDlg, 0,  str_codec,"[%s] %s",mpctx->sh_audio->codec->name ,mpctx->sh_audio->codec->info);
	}
	if(mpctx->demuxer && mpctx->demuxer->info) {
		info = mpctx->demuxer->info;
		addinfo(hDlg, 2,  str_clipinfo, "");
		for (n = 0; info[2 * n] != NULL; n++)
		{
			char info_name[INFO_MAX];
			format_metadata(info_name, info[2 * n]);
			addinfo(hDlg, 0, info_name, string_recode(info[2 * n + 1]));
		}
	}
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG :
		{
			initlist(hDlg);
			SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
			return TRUE ;
		}
		case WM_CLOSE:
			EndDialog( hDlg, 0);
			break;
		case WM_COMMAND :
			switch (LOWORD (wParam))
			{
				case IDOK :
				case IDCANCEL :
					EndDialog (hDlg, wParam) ;
					return TRUE ;
			}
			break ;
	}
	return FALSE ;
}

static DWORD WINAPI threadProc(LPVOID lpParam){
	SetThreadPriority(hInfoThread, THREAD_PRIORITY_LOWEST);
	if(!filename)
		return 1;
	
	if(info_to_html)
	{
		char *infofile = get_path("\\tools\\media_info.txt");
		if(create_html(infofile))
		{
			char *minfo = get_path("\\tools\\minfo.exe");
			if (GetFileAttributes(minfo) != 0xFFFFFFFF) {
				char file[MAX_PATH * 2];
				snprintf(file, MAX_PATH * 2, "\"%s\"", filename);
				ShellExecute(0,NULL , minfo, file, NULL, SW_SHOW);
			} else
				ShellExecute(0, NULL, infofile, NULL, NULL, SW_NORMAL);
		}
		free(infofile);
	}
	else
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_MEDIAINFO), NULL, DlgProc);

	return 0;
}

static void StartInfoThread(void){
	hInfoThread = CreateThread(NULL, 0, threadProc, NULL, 0, NULL);
}
#endif

void show_media_info(void)
{
#if defined(__MINGW32__) || defined(__CYGWIN__)
	StartInfoThread();
#endif
}
