#ifndef __FILES_SHARED__
#define __FILES_SHARED__

#define FILE_SEEK_SET 1
#define FILE_SEEK_END 2
#define FILE_SEEK_CUR 3

#define FILE_MAX_FILENAME_LENGTH 12

#define FILE_MAX_READWRITE_BYTES 4096 //this seems to be determined arbitrarily

typedef struct file_descriptor {
  // STUDENT: put file descriptor info here
	char filename[FILE_MAX_FILENAME_LENGTH];
	uint32 inode;
	uint32 position;
	uint32 EOF;
	uint32 mode;//0 readonly, 1 write, 2 R/W
	uint32 owner; //opening programs PID
} file_descriptor;

#define FILE_FAIL -1
#define FILE_EOF -1
#define FILE_SUCCESS 1

#endif
