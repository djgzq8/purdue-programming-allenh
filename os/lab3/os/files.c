#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "dfs.h"
#include "files.h"
#include "synch.h"

#define READ 0
#define WRITE 1
#define RW	2
#define CLOSED 4
file_descriptor openfiles[FILE_MAX_OPEN_FILES];

// You have already been told about the most likely places where you should use locks. You may use 
// additional locks if it is really necessary.

// STUDENT: put your file-level functions here

//typedef struct file_descriptor {
//  // STUDENT: put file descriptor info here
//	char filename[FILE_MAX_FILENAME_LENGTH];
//	uint32 handle;
//	uint32 position;
//	uint32 EOF;
//	uint32 mode;//0 readonly, 1 write, 2 R/W, 4closed file
//	uint32 owner; //opening programs PID
//} file_descriptor;

void FilesModuleInit(){
	int i = 0;
	for (i = 0; i < FILE_MAX_OPEN_FILES; i++){
		openfiles[i].filename[0] = '\0';
		openfiles[i].inode = -1;
		openfiles[i].position = -1;
		openfiles[i].EOF = 1;
		openfiles[i].mode = 4;
		openfiles[i].owner = 73;
		//		openfiles[i]. = ;
		//		openfiles[i]. = ;

	}
}

uint32 FileOpen(char *filename, char *mode) {
	uint32 handle = -1, fd = -1;
	int i;
	if (*mode == 'w' && (handle = DfsInodeFilenameExists(filename))){
		DfsInodeDelete(handle);
	}

	handle = DfsInodeOpen(filename);
	if (handle == DFS_FAIL){
		printf("Failed to open inode\n");
		return FILE_FAIL;
	}


	/* setup the file descriptor */
	fd = GetFileDescriptor();
	if (fd == FILE_FAIL){
		printf("Cannot open more than %d files\n", FILE_MAX_OPEN_FILES);
		return FILE_FAIL;
	}

	//set filename
	for (i = 0; i < FILE_MAX_FILENAME_LENGTH; i++){
		openfiles[fd].filename[i] = filename[i];
	}
	openfiles[fd].filename[i] = '\0';
	openfiles[fd].EOF = 0;
	openfiles[fd].inode = handle;
	if (mode[0] == 'r'){
		printf("setting mode to read\n");
		openfiles[fd].mode = READ;
		if (mode[1] == 'w'){
			openfiles[fd].mode = RW;
		}
	}else if (mode[0] == 'w'){
		printf("setting mode to write\n");
		openfiles[fd].mode = WRITE;
	}

	openfiles[fd].owner = GetCurrentPid();
	openfiles[fd].position = 0;

	return fd;
}

uint32 FileClose(uint32 handle) {
	if (handle >= FILE_MAX_OPEN_FILES){
		return FILE_FAIL;
	}

	openfiles[handle].filename[0] = '\0';
	openfiles[handle].inode = -1;
	openfiles[handle].position = -1;
	openfiles[handle].EOF = 1;
	openfiles[handle].mode = 4;
	openfiles[handle].owner = 73;

	return FILE_SUCCESS;
}

uint32 FileDelete(char *filename) {
	uint32 inode, fd;

	//if file is open, close it
	if ((fd = GetOpenFDbyFileName(filename)) != FILE_FAIL){
		FileClose(fd);
	}

	if (   (inode = DfsInodeFilenameExists(filename))  ){
		DfsInodeDelete(inode);
		return FILE_SUCCESS;
	}

	return FILE_FAIL;
}

uint32 FileWrite(uint32 fd, void *mem, int num_bytes) {
	uint32 w = 0;
	if (openfiles[fd].mode == CLOSED){
		printf("FileWrite: problem writing to file! mode = %d\n", openfiles[fd].mode);
		return FILE_FAIL;
	}

	w = DfsInodeWriteBytes(openfiles[fd].inode, mem, openfiles[fd].position, num_bytes);
	openfiles[fd].position += w;

	return w;
}

uint32 FileRead(uint32 fd, void *mem, int num_bytes) {
	uint32 r = 0;
	if (openfiles[fd].mode == CLOSED){
		printf("FileRead: problem reading from file! mode = %d\n", openfiles[fd].mode);
		return FILE_FAIL;
	}

	r = DfsInodeReadBytes(openfiles[fd].inode, mem, openfiles[fd].position, num_bytes);
	openfiles[fd].position += r;

	return r;
}

uint32 FileSeek(uint32 fd, int num_bytes, int from_where) {
	uint32 filesize = 0;
	uint32 new_pos = 0;
	filesize = DfsInodeFilesize(openfiles[fd].inode);

	if (from_where == FILE_SEEK_SET){
		new_pos = num_bytes;
	}else if (from_where == FILE_SEEK_CUR){
		new_pos = openfiles[fd].position + num_bytes;
	}else if (from_where == FILE_SEEK_END){
		new_pos = filesize + num_bytes;
	}else {
		return FILE_FAIL;
	}

	if (new_pos > filesize){
		openfiles[fd].position = filesize;
		openfiles[fd].EOF = 1;
	} else {
		openfiles[fd].position = new_pos;
	}

	return FILE_SUCCESS;
}

uint32 GetFileDescriptor(){
	int i;
	for (i = 0; i < FILE_MAX_OPEN_FILES; i++){
		if (openfiles[i].mode == 4){
			return i;
		}
	}
	return FILE_FAIL;
}

uint32 GetOpenFDbyFileName(char *filename){
	int i;
	for (i = 0; i < FILE_MAX_OPEN_FILES; i++){
		if (dstrncmp(openfiles[i].filename, filename, FILE_MAX_FILENAME_LENGTH) == 0){
			return i;
		}
	}
	return FILE_FAIL;
}
