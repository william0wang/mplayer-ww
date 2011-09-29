#if defined(__MINGW32__) || defined(__CYGWIN__)

#include <windows.h>
#include "unrar.h"
#include "path.h"

char *tempsub = NULL;
int __stdcall (*RAROpenArchiveEx)(struct RAROpenArchiveDataEx *ArchiveData);
int __stdcall (*RARCloseArchive)(int hArcData);
int __stdcall (*RARReadHeaderEx)(int hArcData,struct RARHeaderDataEx *HeaderData);
int __stdcall (*RARProcessFile)(int hArcData,int Operation,char *DestPath,char *DestName);
void   __stdcall (*RARSetCallback)(int hArcData,UNRARCALLBACK Callback,long UserData);
void __stdcall (*RARSetPassword)(int hArcData,char *Password);
unsigned int unrardll=0;

BOOL unrar_init()
{
	if (unrardll = (unsigned int)LoadLibrary(get_path("unrar.dll"))) {
		RAROpenArchiveEx = GetProcAddress((HMODULE)unrardll, "RAROpenArchiveEx");
		RARCloseArchive = GetProcAddress((HMODULE)unrardll, "RARCloseArchive");
		RARReadHeaderEx = GetProcAddress((HMODULE)unrardll, "RARReadHeaderEx");
		RARProcessFile = GetProcAddress((HMODULE)unrardll, "RARProcessFile");
		RARSetCallback = (void __stdcall(*)())GetProcAddress((HMODULE)unrardll, "RARSetCallback");
		RARSetPassword = (void __stdcall(*)())GetProcAddress((HMODULE)unrardll, "RARSetPassword");

		char *path = (char *)malloc(512);
		GetTempPath(512, path);
		strcat(path, "\\mpvobsub.tmp");
		tempsub = strdup(path);
		free(path);

		return TRUE;
	}
	return FALSE;
}

void unrar_uninit()
{
	if (unrardll)
	FreeLibrary((HMODULE)unrardll);
}
#endif

