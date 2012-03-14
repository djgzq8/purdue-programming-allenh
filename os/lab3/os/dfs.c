#include "ostraps.h"
#include "dlxos.h"
#include "traps.h"
#include "queue.h"
#include "disk.h"
#include "dfs.h"
#include "synch.h"
#include "misc.h"

static dfs_inode inodes[DFS_INODE_MAX_NUM]; // all inodes
static dfs_superblock sb; // superblock
static uint32 fbv[(DISK_NUMBLOCKS / DFS_BLOCKSIZE * DISK_BLOCKSIZE/32)]; // Free block vector

static uint32 negativeone = 0xFFFFFFFF;
static inline uint32 invert(uint32 n) { return n ^ negativeone; }

#define DFS_FBV_SIZE (DISK_NUMBLOCKS / DFS_BLOCKSIZE * DISK_BLOCKSIZE/32)
// You have already been told about the most likely places where you should use locks. You may use 
// additional locks if it is really necessary.

// STUDENT: put your file system level functions below.
// Some skeletons are provided. You can implement additional functions.

///////////////////////////////////////////////////////////////////
// Non-inode functions first
///////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------
// DfsModuleInit is called at boot time to initialize things and
// open the file system for use.
//-----------------------------------------------------------------

void DfsModuleInit() {

	// You essentially set the file system as invalid and then open
	// using DfsOpenFileSystem().
	DfsInvalidate();
	dbprintf('i', "Invalidated current file system\n");
	if (DfsOpenFileSystem() == DFS_FAIL){
		dbprintf('i', "Failed to open file system\n");
	}else {

		dbprintf('i', "Opened the file system\n");
	}
}

//-----------------------------------------------------------------
// DfsInavlidate marks the current version of the filesystem in
// memory as invalid.  This is really only useful when formatting
// the disk, to prevent the current memory version from overwriting
// what you already have on the disk when the OS exits.
//-----------------------------------------------------------------

void DfsInvalidate() {
	// This is just a one-line function which sets the valid bit of the
	// superblock to 0.
	sb.valid = 0;

}

//-------------------------------------------------------------------
// DfsOpenFileSystem loads the file system metadata from the disk
// into memory.  Returns DFS_SUCCESS on success, and DFS_FAIL on 
// failure.
// Reads the superblock, inodes, and free block vector from disk
// Invalidates the disk copy
// Returns DFS_FAIL on failure
// Returns DFS_SUCCESS on success
// Automatically fails if the file system is already open
//-------------------------------------------------------------------

int DfsOpenFileSystem() {
	dfs_block fs_b;
	disk_block disk_b;
	int num;
	int i;
	//Basic steps:
	// Check that filesystem is not already open
	if (sb.valid == 1){
		printf("Failed to open file system!\n");
		return DFS_FAIL;
	}
	// Read superblock from disk.  Note this is using the disk read rather
	// than the DFS read function because the DFS read requires a valid
	// filesystem in memory already, and the filesystem cannot be valid
	// until we read the superblock. Also, we don't know the block size
	// until we read the superblock, either.

	if ((num = DiskReadBlock (1, &disk_b)) == DISK_FAIL){
		printf("Failed to read the super block on disk!\n");
		return DFS_FAIL;
	}

	// Copy the data from the block we just read into the superblock in memory
	bcopy((char*)(disk_b.data), (char*)(&sb.valid), sizeof(dfs_superblock));
	printSB();

	if (sb.valid == 0){
		printf("dfs.c: DfsOpenFileSystem() - No valid file system on disk!!!\n");
		return DFS_FAIL;
	}

	// All other blocks are sized by virtual block size:
	// Read inodes
	for (i = sb.inodes; i < sb.freevector; i++){
		num = DfsReadBlock (i, &fs_b);
		bcopy((char*)&fs_b, (char*)&(inodes[(i-1)*DFS_BLOCKSIZE/sizeof(dfs_inode)]), DFS_BLOCKSIZE);
		//reads 16 inodes at a time

	}

	// ***************Read free block vector****************
	for (i = sb.freevector; i < sb.freevector + sb.fbv_numfsblock; i++){
		//printf("%d\n", (i-sb.freevector)*DFS_BLOCKSIZE/sizeof(uint32));
		DfsReadBlock(i, &fs_b);
		bcopy((char*)&fs_b, (char*)&(fbv[(i-sb.freevector)*DFS_BLOCKSIZE/sizeof(uint32)]), DFS_BLOCKSIZE);
	}

	//printFBV();

	// Change superblock to be invalid, write back to disk, then change
	// it back to be valid in memory
	sb.valid = 0;
	bzero((char *) &disk_b, sizeof(disk_block));
	bcopy((char *)(&sb.valid), (char*)&(disk_b.data), sizeof(dfs_superblock));
	if (DiskWriteBlock (1, &disk_b) == DISK_FAIL){
		printf("dfs.c: DfsOpenFileSystem() - Failed to write to the disk\n");
		return DFS_FAIL;
	}
	sb.valid = 1;

	return DFS_SUCCESS;
}


//-------------------------------------------------------------------
// DfsCloseFileSystem writes the current memory version of the
// filesystem metadata to the disk, and invalidates the memory's 
// version.
// Write the superblock, inodes, and free block vector back to disk
// The valid superblock is written last to ensure write was successful
// Return DFS_FAIL on failure - or file system is not open
// Return DFS_SUCCESS on success
//-------------------------------------------------------------------

int DfsCloseFileSystem() {
	dfs_block fs_b;
	disk_block disk_b;
	int num;
	int i;

	printf("dfs.c: DfsCloseFileSystem - Attempting to close the file system\n");

	if (sb.valid == 0){
		printf("No file system in memory to close\n");
		return DFS_FAIL;
	}

	//write inodes back
	for (i = sb.inodes; i < sb.freevector; i++){
		//write 16 inodes at a time
		bcopy((char*)&fs_b, (char*)&(inodes[(i-1)*DFS_BLOCKSIZE/sizeof(dfs_inode)]), DFS_BLOCKSIZE);
		num = DfsWriteBlock (i, &fs_b);
	}

	//write fbv back
	//close any open files that might still be there????

	//write superblock back
	bzero((char *) &disk_b, sizeof(disk_block));
	bcopy((char *)(&sb.valid), (char*)&(disk_b.data), sizeof(dfs_superblock));
	if (DiskWriteBlock (1, &disk_b) == DISK_FAIL){
		printf("dfs.c: DfsCloseFileSystem() - Failed to write to the disk\n");
		return DFS_FAIL;
	}

	DfsInvalidate();
	return DFS_SUCCESS;
}


//-----------------------------------------------------------------
// DfsAllocateBlock allocates a DFS block for use. Remember to use 
// locks where necessary.
// Returns the block number on success
// Returns DFS_FAIL on failure
//-----------------------------------------------------------------

uint32 DfsAllocateBlock() {
	int i = 0;
	int j = 0;
	int bitpos = 0;
	// Check that file system has been validly loaded into memory
	if (sb.valid == 0){
		printf("No valid FS loaded!\n");
		return DFS_FAIL;
	}

	for (i = 0; i < DFS_FBV_SIZE; i++) {
		if (fbv[i] != 0) { //not zero means there's at least 1 free page
			for (j = 1; j > 0; j = j << 1) {//j is a bit mask, starts as 1, 2, 4 blah blah, when the bit falls off the end it'll be zero
				if ((j & fbv[i]) > 0) { //the bit in question is 1 so it's the first free page
					//flip the bit

					fbv[i] = fbv[i] ^ j;

					return i * 32 + bitpos;
				}
				bitpos++;
			}
		}
	}


	// Find the first free block using the free block vector (FBV), mark it in use
	// Return handle to block
	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsFreeBlock deallocates a DFS block.
// Free a file system block
// Return DFS_FAIL on failure - or if the filesystem is not open
//-----------------------------------------------------------------

int DfsFreeBlock(uint32 blocknum) {

	int bitpos = blocknum % 32;
	int index = blocknum / 32;

 //Check that the filesystem is open
	if (sb.valid != 1){
		printf("The file system is not open!\n");
		return DFS_FAIL;
	}
	if (blocknum > 512) {
		return -1;
	}
	if ((fbv[index] & (0x1 << bitpos) ) == 0){
		fbv[index] = fbv[index] ^ (0x1 << bitpos);
	}

	//STUDENT

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsReadBlock reads an allocated DFS block from the disk
// (which could span multiple physical disk blocks).  The block
// must be allocated in order to read from it.  Returns DFS_FAIL
// on failure, and the number of bytes read on success.
// Fails if the file system is not open
//-----------------------------------------------------------------

int DfsReadBlock(uint32 blocknum, dfs_block *b) {
	//	int DiskReadBlock (uint32 blocknum, disk_block *b)
	int i,j = 0;
	disk_block rdata;
	int phys_blocknum = blocknum * DFS_BLOCKSIZE/DISK_BLOCKSIZE;

	// Check that the filesystem is open
	if (sb.valid != 1){
		printf("The file system is not open!\n");
		return DFS_FAIL;
	}

	printf("********TODO: Check to see if the FS block is allocated before reading from it*****\n");

	for(i = 0; i < DFS_BLOCKSIZE/DISK_BLOCKSIZE; i++){
		//printf("Reading physical block %d\n", phys_blocknum + i);
		if (DiskReadBlock(phys_blocknum + i, &rdata) == DISK_FAIL){
			printf("failed to read\n");
			return DISK_FAIL;
		}

		for (j = 0; j < DISK_BLOCKSIZE; j++){
			 b->data[j + i*DISK_BLOCKSIZE] = rdata.data[j];
		}
	}
	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsWriteBlock writes to an allocated DFS block on the disk
// (which could span multiple physical disk blocks).  The block
// must be allocated in order to write to it.  Returns DFS_FAIL
// on failure, and the number of bytes written on success.  
//-----------------------------------------------------------------

int DfsWriteBlock(uint32 blocknum, dfs_block *b){
	// STUDENT: put your code here
	int i,j = 0;
	disk_block wdata;
	int phys_blocknum = blocknum * DFS_BLOCKSIZE/DISK_BLOCKSIZE;

	printf("********TODO: Check to see if the FS block is allocated before writing to it*****\n");

	for(i = 0; i < DFS_BLOCKSIZE/DISK_BLOCKSIZE; i++){
		//		Printf("Writing physical block %d\n", phys_blocknum + i);
		for (j = 0; j < DISK_BLOCKSIZE; j++){
			wdata.data[j] = b->data[j + i*DISK_BLOCKSIZE];
		}

		if (DiskWriteBlock(phys_blocknum + i, &wdata) == DISK_FAIL){
			return DISK_FAIL;
		}
	}
	return DFS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// Inode-based functions
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------
// DfsInodeFilenameExists looks through all the inuse inodes for 
// the given filename.
// If the filename is found, return the handle
// of the inode. If it is not found, return DFS_FAIL.
//-----------------------------------------------------------------

uint32 DfsInodeFilenameExists(char *filename) {

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeOpen: search the list of all inuse inodes for the 
// specified filename.
// If the filename exists, return the handle
// of the inode.
// If it does not, allocate a new inode for this
// filename and return its handle.
// Return DFS_FAIL on failure.
// Remember to use locks whenever you allocate a new inode.
//-----------------------------------------------------------------

uint32 DfsInodeOpen(char *filename) {

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeDelete de-allocates any data blocks used by this inode, 
// including the indirect addressing block if necessary,
// then mark the inode as no longer in use.
// Use locks when modifying the "inuse" flag in an inode.
// Return DFS_FAIL on failure, and
// DFS_SUCCESS on success.
//-----------------------------------------------------------------

int DfsInodeDelete(uint32 handle) {

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeReadBytes reads num_bytes from the file represented by 
// the inode handle, starting at virtual byte start_byte, copying 
// the data to the address pointed to by mem. Return DFS_FAIL on 
// failure, and the number of bytes read on success.
//-----------------------------------------------------------------

int DfsInodeReadBytes(uint32 handle, void *mem, int start_byte, int num_bytes) {

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeWriteBytes writes num_bytes from the memory pointed to 
// by mem to the file represented by the inode handle, starting at 
// virtual byte start_byte. Note that if you are only writing part 
// of a given file system block, you'll need to read that block 
// from the disk first. Return DFS_FAIL on failure and the number 
// of bytes written on success.
//-----------------------------------------------------------------

int DfsInodeWriteBytes(uint32 handle, void *mem, int start_byte, int num_bytes) {

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeFilesize simply returns the size of an inode's file. 
// This is defined as the maximum virtual byte number that has 
// been written to the inode thus far. Return DFS_FAIL on failure.
//-----------------------------------------------------------------

uint32 DfsInodeFilesize(uint32 handle) {

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeAllocateVirtualBlock allocates a new filesystem block 
// for the given inode, storing its blocknumber at index 
// virtual_blocknumber in the translation table. If the 
// virtual_blocknumber resides in the indirect address space, and 
// there is not an allocated indirect addressing table, allocate it. 
// Return DFS_FAIL on failure, and the newly allocated file system 
// block number on success.
//-----------------------------------------------------------------

uint32 DfsInodeAllocateVirtualBlock(uint32 handle, uint32 virtual_blocknum) {

	return DFS_FAIL;
}



//-----------------------------------------------------------------
// DfsInodeTranslateVirtualToFilesys translates the 
// virtual_blocknum to the corresponding file system block using 
// the inode identified by handle. Return DFS_FAIL on failure.
//-----------------------------------------------------------------

uint32 DfsInodeTranslateVirtualToFilesys(uint32 handle, uint32 virtual_blocknum) {
	return DFS_FAIL;
}

void printSB(){
	printf("\nPrinting SUPER BLOCK *****\n");
	printf("valid = %d\n", sb.valid);
	printf("blocksize = %d\n", sb.blocksize);
	printf("number of FS blocks = %d\n", sb.numblocks);
	printf("inodes are located at FS block = %d\n", sb.inodes);
	printf("there are this many inodes = %d\n", sb.numinodes);
	printf("freevector is located at FS block = %d\n", sb.freevector);
	printf("freevector has %d file system blocks\n", sb.fbv_numfsblock);
	printf("freevector is %d words long\n\n", sb.fbv_numwords);
}

void printFBV(){
	int i = 0;
	int j = 1;
	printf("0:\t");
	for (i = 0; i < (DISK_NUMBLOCKS / DFS_BLOCKSIZE * DISK_BLOCKSIZE/32); i++){
		printf("%d ", fbv[i]);
		if (i%4==3){
			printf("\n%d:\t", j++);
		}
	}

	printf("\n\n");
}
