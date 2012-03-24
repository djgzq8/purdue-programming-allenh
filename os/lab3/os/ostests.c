#include "ostraps.h"
#include "dlxos.h"
#include "traps.h"
#include "disk.h"
#include "dfs.h"

void RunOSTests() {
	char filename[12] = "file.c";
	char filename2[12] = "file2.c";
	int handle = -1;
	int i = 0;
	char mem[30] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	handle = DfsInodeOpen(filename);
	if (handle != DFS_FAIL){
		printf("%s successfully opened with inode number %d\n", filename, handle);
	}

	DfsInodeAllocateVirtualBlock(handle, 10);
	DfsInodeWriteBytes(handle, (void *)mem, 10245, 25);
	DfsInodeTranslateVirtualToFilesys(handle, 10);

//	DfsFreeBlock(38);
//	DfsInodeWriteBytes(handle, (void *) mem, 1020, 26);
//	for (i = 0; i < 30; i++){
//		mem[i] = 'C';
//	}

//	DfsInodeReadBytes(handle, (void *)mem, 1020, 15);
//
//	mem[29] = '\0';
//	printf("Mem is now %s \n", mem);

//	handle = DfsInodeOpen(filename2);
//	if (handle != DFS_FAIL){
//		printf("%s successfully opened with inode number %d\n", filename2, handle);
//	}

//	if (DfsInodeFilenameExists(filename) != DFS_FAIL){
//		printf("The file %s exists!\n", filename);
//	} else {
//		printf("The file %s doesn't exist!\n", filename);
//	}


	//    int DfsInodeOpen(char *filename): search the list of all inuse inodes for the specified filename. If the filename exists, return the handle of the inode. If it does not, allocate a new inode for this filename and return its handle. Return DFS_FAIL on failure.
	//    int DfsInodeDelete(int handle): de-allocate any data blocks used by this inode, including the indirect addressing block if necessary, then mark the inode as no longer in use. Return DFS_FAIL on failure, and DFS_SUCCESS on success.
	//    int DfsInodeReadBytes(int handle, void *mem, int start_byte, int num_bytes): read num_bytes from the file represented by the inode handle, starting at virtual byte start_byte, copying the data to the address pointed to by mem. Return DFS_FAIL on failure, and the number of bytes read on success.
	//    int DfsInodeWriteBytes(int handle, void *mem, int start_byte, int num_bytes): write num_bytes from the memory pointed to by mem to the file represented by the inode handle, starting at virtual byte start_byte. Note that if you are only writing part of a given file system block, you'll need to read that block from the disk first. Return DFS_FAIL on failure and the number of bytes written on success.
	//    int DfsInodeFilesize(int handle): return the size of an inode's file. This is defined as the maximum virtual byte number that has been written to the inode thus far. Return DFS_FAIL on failure.
	//    int DfsInodeAllocateVirtualBlock(int handle, int virtual_blocknum): allocate a new filesystem block for the given inode, storing its blocknumber at index virtual_blocknumber in the translation table. If the virtual_blocknumber resides in the indirect address space, and there is not an allocated indirect addressing table, allocate it. Return DFS_FAIL on failure, and the newly allocated file system block number on success.
	//    int DfsInodeTranslateVirtualToFilesys(int handle, int virtual_blocknum): translate the virtual_blocknum to the corresponding file system block using the inode identified by handle. Return DFS_FAIL on failure.


}

