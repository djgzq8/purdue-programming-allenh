#ifndef __FILES_H__
#define __FILES_H__

#include "dfs.h"
#include "files_shared.h"

//#define FILE_MAX_OPEN_FILES 15

uint32 FileOpen(char *filename, char *mode);

uint32 FileClose(uint32);

uint32 FileDelete(char *filename);

uint32 FileWrite(uint32, void *mem, int);

uint32 FileRead(uint32, void *mem, int);

uint32 FileSeek(uint32, int, int);

int FileMaxFilenameLength();

#endif
