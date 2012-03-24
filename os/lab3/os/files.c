#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "dfs.h"
#include "files.h"
#include "synch.h"

// You have already been told about the most likely places where you should use locks. You may use 
// additional locks if it is really necessary.

// STUDENT: put your file-level functions here

uint32 FileOpen(char *filename, char *mode){
	return FILE_FAIL;
}

uint32 FileClose(uint32 handle){
	return FILE_FAIL;
}

uint32 FileDelete(char *filename){
	return FILE_FAIL;
}

uint32 FileWrite(uint32 handle, void *mem, int num_bytes){
	return FILE_FAIL;
}

uint32 FileRead(uint32 handle, void *mem, int num_bytes){
	return FILE_FAIL;
}

uint32 FileSeek(uint32 handle, int num_bytes, int from_where){
	return FILE_FAIL;
}


