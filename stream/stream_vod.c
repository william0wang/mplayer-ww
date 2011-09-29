#include "config.h"

#ifdef CONFIG_VOD

#include <stdio.h>
#include <stdlib.h>
#include <mbstring.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#if !HAVE_WINSOCK2_H
#include <sys/socket.h>
#define closesocket close
#else
#include <winsock2.h>
#endif

#include "tcp.h"
#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"

static struct stream_priv_s {
  char* host;
  int port;
  char* filename;
  char *comment;
} stream_priv_dflts = {
  NULL,
  1680,
  NULL,
  NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static m_option_t stream_opts_fields[] = {
  {"hostname", ST_OFF(host), CONF_TYPE_STRING, 0, 0, 0, NULL},
  {"port", ST_OFF(port), CONF_TYPE_INT, 0, 0, 65635, NULL},
  {"filename", ST_OFF(filename), CONF_TYPE_STRING, 0, 0, 0, NULL},
  {"comment", ST_OFF(comment), CONF_TYPE_STRING, 0, 0, 0, NULL},
  { NULL, NULL, 0, 0, 0, 0,  NULL }
};
static struct m_struct_st stream_opts = {
  "vod",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};

#if __GNUC__ == 2
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

typedef struct m_response_st {
  char magic[4];
  unsigned int ver;         /* always 00 02 00 00 */
  unsigned int type;        /* ok = 00 00 00 00, err = 0c 00 00 00 */
  unsigned int unknown1;    /* always 00 00 00 00 */
  unsigned int len_lo;      /* lower 32 bits of length */
  unsigned int len_hi;      /* higher 32 bits of length */
  unsigned int pos_lo;      /* lower 32 bits of position */
  unsigned int pos_hi;      /* higher 32 bits of position */
  unsigned int unknown2;    /* always 00 00 01 00 */
  unsigned int unknown3;    /* always 00 00 00 00 */
  unsigned int unknown4;    /* ok = 64 00 00 00, err = 00 00 00 00 */
  char errmsg[100];
  char name[260];
  char date[108];
} m_response_t;

#if __GNUC__ == 2
#pragma pack()
#else
#pragma pack(pop)
#endif

static char* get_comment(char* fn) {
  char* pos;
#if defined(__MINGW32__) || defined(__CYGWIN__)
  pos = _mbsrchr(fn,'\\');
#else
  pos = strrchr(fn,'\\');
#endif
  if (pos) fn = pos+1;
  pos = strrchr(fn,'/');
  if (pos) fn = pos+1;
  return fn;
  //  return "comic.sjtu.edu.cn";
}

static int send_request(int fd, char* req, int len) {
  int nbytes;
  while(len > 0) {
    nbytes = send(fd, req, len, 0);
    if(nbytes <= 0) {
      mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] send request error: %s\n", strerror(errno));
      return STREAM_ERROR;
    }
    req += nbytes;
    len -= nbytes;
  }
  return STREAM_OK;
}

static int recv_response_until(int fd, char* start, char* expect, int blocksize, char** next, int* over) {
  char* pos = start;
  int nbytes;

  while (!strstr(start, expect)) {
    nbytes = recv ( fd, pos, blocksize, 0 );
    if (nbytes <= 0) {
      mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] recv response error: %s\n", strerror(errno));
      return STREAM_ERROR;
    }
    pos += nbytes;
  }

  *next = strstr(start, expect) + strlen(expect);
  *over = pos - *next;
  return STREAM_OK;
}

static int recv_response_size(int fd, char* buffer, int len) {
  int nbytes;

  while (len > 0) {
    nbytes = recv ( fd, buffer, len, 0 );
    if (nbytes <= 0) {
      mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] recv response error: %s\n", strerror(errno));
      return STREAM_ERROR;
    }
    buffer += nbytes;
    len -= nbytes;
  }

  if (len < 0) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] recv response socket error\n");
    return STREAM_ERROR;
  }
  return STREAM_OK;
}

static int get_response(int fd, m_response_t* resp) {
  int nbytes;
  char buffer[4096];
  char* start;
  int over;

  memset(buffer,0,sizeof(buffer));

  if (recv_response_until(fd, buffer, "\r\n\r\n", sizeof(m_response_t), &start, &over) == STREAM_ERROR) {
    return STREAM_ERROR;
  }

  char c = *start;
  *start = 0;
  mp_msg(MSGT_OPEN,MSGL_V, "[vod] recv http response: \n%s\n", buffer);
  *start = c;

  if (recv_response_size(fd, start + over, sizeof(m_response_t) - over) == STREAM_ERROR) {
    return STREAM_ERROR;
  }

  memcpy(resp, start, sizeof(m_response_t));

  if (strncmp(resp->magic,"XMMS",4) != 0) {     // CHECK MAGIC
    mp_msg(MSGT_OPEN, MSGL_ERR, "[vod] magic mismatch\n");
    return STREAM_ERROR;
  }

  if ((resp->type != 0) && (resp->unknown4 == 0)) { // maybe error
    mp_msg(MSGT_OPEN, MSGL_ERR, "[vod] server error %s", resp->errmsg);
    return STREAM_ERROR;
  }

  mp_msg(MSGT_OPEN, MSGL_V, "[vod] opening %s\n", resp->name);
  mp_msg(MSGT_OPEN, MSGL_V, "[vod] date %s\n", resp->date);
  mp_msg(MSGT_OPEN, MSGL_INFO, "[vod] position %u/%u\n", resp->pos_lo, resp->len_lo);
  return STREAM_OK;
}

static unsigned int get_ip_from_fd(int fd) {
  struct sockaddr_in saddr;
  int len = sizeof(saddr);
  getsockname(fd,(struct sockaddr*)(&saddr), &len);
  return saddr.sin_addr.s_addr;
}

static int get_stream(struct stream_priv_s* p, stream_t *s, off_t start) {
  int fd;
  char buffer[4096];
  int pos = 0;
  int size = sizeof(buffer);
  m_response_t resp;
  if (s->fd > 0)
    closesocket(s->fd);

  fd = connect2Server(p->host,p->port,1);
  if(fd < 0) {
    return STREAM_ERROR;
  }
  s->fd = fd;

#define myprintf(fmt, args...) do { int len = snprintf(buffer+pos, size, fmt, ## args); pos+=len; size-=len; } while (0)

  myprintf("POST /XMMS/MPLAYER%d.xms HTTP/1.0\r\n", time(NULL) % 10000 + 10000);
  myprintf("Host: %s\r\n",   "mplayer");
  myprintf("Accept: %s\r\n", "*/*");
  myprintf("User-Agent: %s\r\n", "XMPLAYER");
  myprintf("UGT:%s\r\n",     "XMPLAYER");
  myprintf("UIP:%u\r\n",     get_ip_from_fd(fd));
  myprintf("USN:%d\r\n",     12345678);
  myprintf("PRI:%d\r\n",     0);
  myprintf("UPR:%d\r\n",     0);
  myprintf("SPL:%d\r\n",     start);
  myprintf("SPH:%d\r\n",     0);
  myprintf("URN:%s\r\n",     "");
  myprintf("PWD:%s\r\n",     "");
  myprintf("FPT:%d\r\n",     (start==0)?1:0);
  myprintf("FIL:%s\r\n",     p->filename);
  myprintf("ULG:%s\r\n",     "CN");
  myprintf("STT:%s\r\n",     "YYYY/MM/DD HH:MM:SS");
  myprintf("HST:%s\r\n",     "MPLAYER");
  myprintf("PNM:%s\r\n",     get_comment(p->filename));
  myprintf("PID:%d\r\n",     0);
  myprintf("IDX:%d\r\n",     0);
  myprintf("\r\n");

  mp_msg(MSGT_OPEN, MSGL_V, "[vod] sending request: \n%s\n", buffer);

  if (send_request(fd, buffer, strlen(buffer)) == STREAM_ERROR) {
    closesocket(fd);
    return STREAM_ERROR;
  }

  if (get_response(fd, &resp) == STREAM_ERROR) {
    closesocket(fd);
    return STREAM_ERROR;
  }

  if (resp.len_lo == 0) {
    closesocket(fd);
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] Open failed, may be too many users.\n");
    return STREAM_ERROR;
  }

  s->pos = resp.pos_lo;
  s->end_pos = resp.len_lo;
  if (resp.pos_lo >= resp.len_lo)
    s->eof = 1;
  return STREAM_OK;
}

static int fill_buffer(stream_t *s, char* buffer, int max_len) {
  fd_set fds;
  struct timeval tv;
  int r;

  FD_ZERO(&fds);
  FD_SET(s->fd,&fds);
  tv.tv_sec = 15;
  tv.tv_usec = 0;

  if(select(s->fd+1, &fds, NULL, NULL, &tv) < 1) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] read timed out\n");
    return -1;
  }

  r = recv(s->fd,buffer,max_len,0);
  return (r <= 0) ? -1 : r;
}

static int seek(stream_t *s, off_t newpos) {
  return get_stream(s->priv, s, newpos);
}

static void close_f(stream_t *s) {
  struct stream_priv_s* p = s->priv;

  if(!p) return;

  if(s->fd > 0) {
    closesocket(s->fd);
    s->fd = 0;
  }
  m_struct_free(&stream_opts,p);
}

static int open_f(stream_t *stream,int mode, void* opts, int* file_format) {

  struct stream_priv_s* p = (struct stream_priv_s*)opts;

  if (sizeof(m_response_t) != 512) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] Compilation error, sizeof(m_response_t) = %d\n",
           sizeof(m_response_t));
    m_struct_free(&stream_opts,opts);
    return STREAM_UNSUPPORTED;
  }

  if(mode != STREAM_READ) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] Unknown open mode %d\n",mode);
    m_struct_free(&stream_opts,opts);
    return STREAM_UNSUPPORTED;
  }

  if(!p->filename || !p->host) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] Bad url\n");
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }

  stream->fd = -1;
  if (get_stream(p, stream, 0) == STREAM_ERROR) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[vod] Open failed\n");
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }

#ifdef _LARGEFILE_SOURCE
  mp_msg(MSGT_OPEN,MSGL_V,"[vod] File size is %lld bytes\n", (long long)(stream->end_pos));
#else
  mp_msg(MSGT_OPEN,MSGL_V,"[vod] File size is %u bytes\n", (unsigned int)(stream->end_pos));
#endif
  stream->priv = p;
  stream->type = STREAMTYPE_FILE;
  stream->fill_buffer = fill_buffer;
  stream->seek = seek;
  stream->close = close_f;
  return STREAM_OK;
}

stream_info_t stream_info_vod = {
  "Truran VOD Protocol",
  "vod",
  "pigworlds",
  "Play files on Truran VOD System",
  open_f,
  { "vod", NULL },
  &stream_opts,
  1 // Urls are an option string
};

#endif //CONFIG_VOD

