#ifndef _UNRAR_DLL_
#define _UNRAR_DLL_

#define ERAR_END_ARCHIVE        10
#define ERAR_NO_MEMORY          11
#define ERAR_BAD_DATA           12
#define ERAR_BAD_ARCHIVE        13
#define ERAR_UNKNOWN_FORMAT     14
#define ERAR_EOPEN              15
#define ERAR_ECREATE            16
#define ERAR_ECLOSE             17
#define ERAR_EREAD              18
#define ERAR_EWRITE             19
#define ERAR_SMALL_BUF          20
#define ERAR_UNKNOWN            21
#define ERAR_MISSING_PASSWORD   22

#define RAR_OM_LIST              0
#define RAR_OM_EXTRACT           1
#define RAR_OM_LIST_INCSPLIT     2

#define RAR_SKIP              0
#define RAR_TEST              1
#define RAR_EXTRACT           2

#define RAR_VOL_ASK           0
#define RAR_VOL_NOTIFY        1

#define RAR_DLL_VERSION       4

#ifdef _UNIX
#define CALLBACK
#define PASCAL
#define LONG long
#define HANDLE void *
#define UINT unsigned int
#endif

struct RARHeaderData
{
  char         ArcName[260];
  char         FileName[260];
  unsigned int Flags;
  unsigned int PackSize;
  unsigned int UnpSize;
  unsigned int HostOS;
  unsigned int FileCRC;
  unsigned int FileTime;
  unsigned int UnpVer;
  unsigned int Method;
  unsigned int FileAttr;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
};


struct RARHeaderDataEx
{
  char         ArcName[1024];
  wchar_t      ArcNameW[1024];
  char         FileName[1024];
  wchar_t      FileNameW[1024];
  unsigned int Flags;
  unsigned int PackSize;
  unsigned int PackSizeHigh;
  unsigned int UnpSize;
  unsigned int UnpSizeHigh;
  unsigned int HostOS;
  unsigned int FileCRC;
  unsigned int FileTime;
  unsigned int UnpVer;
  unsigned int Method;
  unsigned int FileAttr;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
  unsigned int Reserved[1024];
};


struct RAROpenArchiveData
{
  char         *ArcName;
  unsigned int OpenMode;
  unsigned int OpenResult;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
};

struct RAROpenArchiveDataEx
{
  char         *ArcName;
  wchar_t      *ArcNameW;
  unsigned int OpenMode;
  unsigned int OpenResult;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
  unsigned int Flags;
  unsigned int Reserved[32];
};

enum UNRARCALLBACK_MESSAGES {
  UCM_CHANGEVOLUME,UCM_PROCESSDATA,UCM_NEEDPASSWORD
};

typedef int (__stdcall *UNRARCALLBACK)(unsigned int msg,long UserData,long P1,long P2);

typedef int (__stdcall *CHANGEVOLPROC)(char *ArcName,int Mode);
typedef int (__stdcall *PROCESSDATAPROC)(unsigned char *Addr,int Size);

#ifdef __cplusplus
extern "C" {
#endif

//extern int __stdcall (*RAROpenArchive)(struct RAROpenArchiveData *ArchiveData);
extern int __stdcall (*RAROpenArchiveEx)(struct RAROpenArchiveDataEx *ArchiveData);
extern int __stdcall (*RARCloseArchive)(int hArcData);
//extern int __stdcall (*RARReadHeader)(int hArcData,struct RARHeaderData *HeaderData);
extern int __stdcall (*RARReadHeaderEx)(int hArcData,struct RARHeaderDataEx *HeaderData);
extern int __stdcall (*RARProcessFile)(int hArcData,int Operation,char *DestPath,char *DestName);
//extern int __stdcall (*RARProcessFileW)(int hArcData,int Operation,wchar_t *DestPath,wchar_t *DestName);
extern void   __stdcall (*RARSetCallback)(int hArcData,UNRARCALLBACK Callback,long UserData);
//extern void __stdcall (*RARSetChangeVolProc)(int hArcData,CHANGEVOLPROC ChangeVolProc);
//extern void __stdcall (*RARSetProcessDataProc)(int hArcData,PROCESSDATAPROC ProcessDataProc);
extern void __stdcall (*RARSetPassword)(int hArcData,char *Password);
//extern int __stdcall (*RARGetDllVersion)();

extern unsigned int unrardll;

#ifdef __cplusplus
}
#endif

#endif
