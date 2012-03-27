#include "ostraps.h"
#include "dlxos.h"
#include "traps.h"
#include "queue.h"
#include "disk.h"
#include "dfs.h"
#include "synch.h"
#include "misc.h"
#include "files.h"

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
	printf("\n\n\n\n");
	// You essentially set the file system as invalid and then open
	// using DfsOpenFileSystem().
	DfsInvalidate();
	FilesModuleInit();
	dbprintf('y', "DfsModuleInit: Invalidated current file system\n");
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

	dbprintf ('y', "DfsOpenFileSystem: Attempting to open filesystem\n");
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

	printf("dfs.c: DfsCloseFileSystem - Attempting to close the file system\n\n\n\n");

	if (sb.valid == 0){
		printf("DfsWriteBlock: No file system in memory to close\n");
		return DFS_FAIL;
	}

	bzero((char *) &fs_b, sizeof(dfs_block));
	//	for (i = 0; i < sizeof(dfs_block); i++){
	//		fs_b.data[i] = 0xFF;
	//	}
	//write inodes back
	for (i = sb.inodes; i < sb.freevector; i++){
		//write 16 inodes at a time
		bcopy((char*)&(inodes[(i-1)*DFS_BLOCKSIZE/sizeof(dfs_inode)]), (char*)&fs_b, DFS_BLOCKSIZE);
		num = DfsWriteBlock (i, &fs_b);
		bzero((char *) &fs_b, sizeof(dfs_block));
	}


	// ***************Write free block vector****************
	bzero((char *) &fs_b, sizeof(dfs_block));
	for (i = sb.freevector; i < sb.freevector + sb.fbv_numfsblock; i++){
		//printf("%d\n", (i-sb.freevector)*DFS_BLOCKSIZE/sizeof(uint32));
		bcopy((char*)&(fbv[(i-sb.freevector)*DFS_BLOCKSIZE/sizeof(uint32)]), (char*)&fs_b,  DFS_BLOCKSIZE);
		DfsWriteBlock(i, &fs_b);
		bzero((char *) &fs_b, sizeof(dfs_block));
	}
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
	uint32 j = 0;
	int bitpos = 0;
	dbprintf ('y', "DfsAllocateBlock: starting fbv_numwords %d\n", sb.fbv_numwords);
	// Check that file system has been validly loaded into memory
	if (sb.valid == 0){
		printf("No valid FS loaded!\n");
		return DFS_FAIL;
	}

	for (i = 0; i < sb.fbv_numwords; i++) {
		if (fbv[i] != 0) { //not zero means there's at least 1 free page
			for (j = 0x80000000; j > 0; j = j >> 1) {//j is a bit mask, starts as 1, 2, 4 blah blah, when the bit falls off the end it'll be zero
				if ((j & fbv[i]) > 0) { //the bit in question is 1 so it's the first free page
					//flip the bit

					fbv[i] = fbv[i] ^ j;
					dbprintf ('y', "DfsAllocateBlock: allocated FS block %d\n", i * 32 + bitpos);
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
	dbprintf ('y', "DfsFreeBlock: Begin freeing block %d\n", blocknum);
	//Check that the filesystem is open
	if (sb.valid != 1){
		printf("The file system is not open!\n");
		return DFS_FAIL;
	}
	if (blocknum > sb.numfsblocks) {
		printf("DfsFreeBlock: block requested for "
				"freeing is beyond the maximum number of blocks\n!");
		return DFS_FAIL;
	}
	if ((fbv[index] & (0x80000000 >> bitpos) ) == 0){//only changes value if it's already zero
		fbv[index] = fbv[index] ^ (0x80000000 >> bitpos);
	}

	//STUDENT

	return DFS_SUCCESS;
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
	int bytes_read = 0;
	disk_block rdata;
	int phys_blocknum = blocknum * DFS_BLOCKSIZE/DISK_BLOCKSIZE;

	// Check that the filesystem is open
	if (sb.valid != 1){
		printf("The file system is not open!\n");
		return DFS_FAIL;
	}
	//Ensure blocknum is allocated
	if (!DfsIsAllocated(blocknum) && blocknum > (sb.freevector+sb.fbv_numfsblock)){
		printf("DfsReadBlock: Can only read from an allocated block\n");
		return DFS_FAIL;
	}
	for(i = 0; i < DFS_BLOCKSIZE/DISK_BLOCKSIZE; i++){
		//
		if (DiskReadBlock(phys_blocknum + i, &rdata) == DISK_FAIL){
			printf("failed to read\n");
			return DISK_FAIL;
		}

		for (j = 0; j < DISK_BLOCKSIZE; j++){
			bytes_read++;
			b->data[j + i*DISK_BLOCKSIZE] = rdata.data[j];
		}
	}

	//Return # of Bytes read
	return bytes_read;
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
	int bytes_written = 0;
	disk_block wdata;
	int phys_blocknum = blocknum * DFS_BLOCKSIZE/DISK_BLOCKSIZE;

	// Check that the filesystem is open
	if (sb.valid != 1){
		printf("DfsWriteBlock: The file system is not open!\n");
		return DFS_FAIL;
	}

	//make sure blocknum is allocated
	if (!DfsIsAllocated(blocknum)&& blocknum > (sb.freevector+sb.fbv_numfsblock)){
		printf("DfsWriteBlock: Can only write to an allocated block\n");
	}
	for(i = 0; i < DFS_BLOCKSIZE/DISK_BLOCKSIZE; i++){
		//		Printf("Writing physical block %d\n", phys_blocknum + i);
		for (j = 0; j < DISK_BLOCKSIZE; j++){
			wdata.data[j] = b->data[j + i*DISK_BLOCKSIZE];
		}

		if (DiskWriteBlock(phys_blocknum + i, &wdata) == DISK_FAIL){
			return DISK_FAIL;
		}
		bytes_written += DISK_BLOCKSIZE;
	}
	return bytes_written;
}

//-----------------------------------------------------------------
// DfsIsAllocated returns a 1 if the blocknum is allocated and
// a 0 if it is not
//-----------------------------------------------------------------
int DfsIsAllocated(uint32 blocknum){
	int bitpos = blocknum % 32;
	int index = blocknum / 32;

	if (blocknum < sb.freevector+sb.fbv_numfsblock){
		return 0;
	}
	//Check that the filesystem is open
	if (sb.valid != 1){
		printf("DfsIsAllocated: The file system is not open!\n");
		return 0;
	}
	if (blocknum > sb.numfsblocks) {
		printf("DfsIsAllocated: block in question"
				" is beyond the maximum number of fs blocks\n!");
		return 0;
	}
	if ((fbv[index] & (0x80000000 >> bitpos) ) == 0){
		//fbv[index] = fbv[index] ^ (0x1 << bitpos);
		return DFS_SUCCESS;
	}

	//STUDENT


	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Inode-based functions
////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------
// DfsInodeFilenameExists looks through all the in use inodes for
// the given filename.
// If the filename is found, return the handle
// of the inode. If it is not found, return DFS_FAIL.
//-----------------------------------------------------------------

uint32 DfsInodeFilenameExists(char *filename) {
	int i = 0;
	//Check to see if the file system is valid
	if (sb.valid == 0){
		printf("The File System in memory is not valid\n");
		return DFS_FAIL;
	}

	//Check through every inode
	for(i=0; i< sb.numinodes; i++)
	{
		if(dstrncmp(inodes[i].filename, filename, FILE_MAX_FILENAME_LENGTH) == 0){
			//return inode handle
			return i;
		}
	}

	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeOpen: search the list of all in use inodes for the
// specified filename.
// If the filename exists, return the handle
// of the inode.
// If it does not, allocate a new inode for this
// filename and return its handle.
// Return DFS_FAIL on failure.
// Remember to use locks whenever you allocate a new inode.
//-----------------------------------------------------------------

uint32 DfsInodeOpen(char *filename) {

	int i = 0;
	int freeinode = 0;
	uint32 handle = DFS_FAIL;
	dbprintf ('y', "DfsInodeOpen: Starting...\n");
	//Check to see if the file system is valid
	if (sb.valid == 0){
		printf("The File System in memory is not valid\n");
		return DFS_FAIL;
	}

	//if filename already exists return the handle
	if((handle = DfsInodeFilenameExists(filename)) != DFS_FAIL){
		dbprintf ('y', "DfsInodeOpen: filename already exists and has handle = %d\n", handle);
		return handle;
	}

	//find the first free inode
	for(i=0; i< sb.numinodes; i++)
	{
		if(inodes[i].used == 0){
			//return inode handle
			freeinode = i;
			break;
		}
	}

	//allocate that free inode
	for (i = 0; i < FILE_MAX_FILENAME_LENGTH; i++){
		inodes[freeinode].filename[i] = filename[i];
	}
	printINODE();
	inodes[freeinode].filename[11] = '\0';
	inodes[freeinode].size = 0;
	inodes[freeinode].direct[0] = DfsAllocateBlock();
	for (i = 1; i < 10; i++){
		inodes[freeinode].direct[i] = 0;
	}
	inodes[freeinode].indirect = 0;
	inodes[freeinode].used = 1;

	//return new inode handle
	return freeinode;
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
	int i = 0;

	dfs_block indirect;
	uint32* data;


	//SOMETHING ABOUT LOCKS?

	//Check to see if the file system is valid
	if (sb.valid == 0) {
		printf("The File System in memory is not valid\n");
		return DFS_FAIL;
	}

	//TODO: De-allocate data blocks used by the inode
	//		Possibly go through the direct inodes & indirect nodes and
	//		set them all to 0

	//Clear the direct inodes
	for (i = 0; i < 10; i++) {
		DfsFreeBlock(inodes[handle].direct[i]);
	}

	//clear the indirect inodes

	if (DfsInodeHasIndirect(handle)){//if indirect field is valid address
		DfsReadBlock(inodes[handle].indirect, &indirect);//read the block pointed to by indirect field
		for (i = 0;i < DFS_BLOCKSIZE/4; i++){//Free every fs block pointed to by the data in the indirect block
			DfsFreeBlock(data[i]);
		}
	}

	inodes[handle].filename[0] = '\0';
	inodes[handle].size = 0;
	for (i = 0; i < 10; i++){
		inodes[handle].direct[i] = 0;
	}
	inodes[handle].indirect = 0;

	//inode is no longer in use
	inodes[handle].used = 0;

	return DFS_SUCCESS;
}


//-----------------------------------------------------------------
// DfsInodeReadBytes reads num_bytes from the file represented by 
// the inode handle, starting at virtual byte start_byte, copying 
// the data to the address pointed to by mem. Return DFS_FAIL on 
// failure, and the number of bytes read on success.
//-----------------------------------------------------------------

int DfsInodeReadBytes(uint32 handle, void *mem, int start_byte, int num_bytes) {

	uint32 start_block = 0, end_block = 0;
	dfs_block fs_b;
	int i = 0, bytes_read= 0, k = 0;
	int end_byte = 0;
	uint32 fs_blocknum = 0;
	char *write;
	write = (char *)mem;

	printf("dfs.c: DfsInodeReadBytes - Reading %d bytes from file i[%d]\n", num_bytes, handle);

	//Check for a valid file system
	if (sb.valid == 0){
		printf("The File System in memory is not valid\n");
		return DFS_FAIL;
	}
	if (inodes[handle].used == 0){
		printf("Not an actual file\n");
		return DFS_FAIL;
	}

	if (start_byte > inodes[handle].size){
		printf("DfsInodeReadBytes: Requested data not allocated\n");
		return DFS_FAIL;
	}


	end_byte = start_byte + num_bytes - 1;
	end_block = end_byte / DFS_BLOCKSIZE;
	start_block = start_byte / DFS_BLOCKSIZE;


	//read the first block in
	fs_blocknum = DfsInodeTranslateVirtualToFilesys(handle, start_block);
	DfsReadBlock(fs_blocknum, &fs_b);

	for (i = start_byte%DFS_BLOCKSIZE; i < DFS_BLOCKSIZE; i++){
		bcopy ((&fs_b.data[i]),	&(write[bytes_read]), 1);
		bytes_read++;
		if (bytes_read == num_bytes){
			return bytes_read;
		}
	}

	for (k = start_block+1; k <= end_block; k++){
		if (DfsIsAllocated((fs_blocknum = DfsInodeTranslateVirtualToFilesys(handle, k)))){
			DfsReadBlock(fs_blocknum, &fs_b);
		}else {
			break;
		}

		for (i = 0; i < DFS_BLOCKSIZE; i++){
			bcopy (fs_b.data, (char *)mem, 1);
			bytes_read++;
			if (bytes_read == num_bytes){
				return bytes_read;
			}
		}
	}

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

	uint32 start_block = 0, end_block = 0;
	dfs_block fs_b;
	int i = 0, bytes_written= 0, k = 0;
	int end_byte = 0;
	uint32 fs_blocknum = 0;
	char *write;
	write = (char *)mem;

	dbprintf ('y', "DfsInodeWriteBytes: writing file i[%d], starting size is %d\n", handle, inodes[handle].size);
	//Check for a valid file system
	if (sb.valid == 0){
		printf("The File System in memory is not valid\n");
		return DFS_FAIL;
	}
	if (inodes[handle].used == 0){
		printf("Not an actual file\n");
		return DFS_FAIL;
	}

	end_byte = start_byte + num_bytes - 1;
	end_block = end_byte / DFS_BLOCKSIZE;
	start_block = start_byte / DFS_BLOCKSIZE;

	dbprintf ('y', "DfsInodeWriteBytes: start_block %d end_block = %d end_byte = %d DFS_BLOCKSIZE = %d\n",
			start_block, end_block, end_byte, DFS_BLOCKSIZE);
	//is start block allocated? if so read it in.
	//	if not allocate as many as we need


	//at the end, write back data

	if (DfsIsAllocated((fs_blocknum = DfsInodeTranslateVirtualToFilesys(handle, start_block)))){
		//read the first block in
		dbprintf ('y', "DfsInodeWriteBytes: start_block %d is already allocated to fs block %d\n", start_block, fs_blocknum);
		DfsReadBlock(fs_blocknum, &fs_b);

	} else {
		//allocate an appropriate number of them
		fs_blocknum = DfsInodeAllocateVirtualBlock(handle, start_block);
		bzero(fs_b.data, DFS_BLOCKSIZE);
	}

	for (i = start_byte%DFS_BLOCKSIZE; i < DFS_BLOCKSIZE; i++){
		bcopy (&(write[bytes_written]), (&fs_b.data[i]),	1);

		bytes_written++;
		if (bytes_written == num_bytes){
			if (inodes[handle].size < (start_block * DFS_BLOCKSIZE) + i){
				inodes[handle].size = (start_block * DFS_BLOCKSIZE) + i;
			}

			DfsWriteBlock(fs_blocknum, &fs_b);
			return bytes_written;
		}
	}

	DfsWriteBlock(fs_blocknum, &fs_b);

	for (k = start_block+1; k <= end_block; k++){
		dbprintf ('y', "DfsInodeWriteBytes: writing back block %d\n", k);
		if (DfsIsAllocated((fs_blocknum = DfsInodeTranslateVirtualToFilesys(handle, k)))){
			DfsReadBlock(fs_blocknum, &fs_b);
		}else {
			fs_blocknum = DfsInodeAllocateVirtualBlock(handle, k);
			bzero(fs_b.data, DFS_BLOCKSIZE);
		}

		for (i = 0; i < DFS_BLOCKSIZE; i++){
			bcopy ((char *)mem, fs_b.data, 1);
			bytes_written++;

			if (bytes_written == num_bytes){
				if (inodes[handle].size < (k * DFS_BLOCKSIZE) + i){
					inodes[handle].size = (k * DFS_BLOCKSIZE) + i;
				}
				DfsWriteBlock(fs_blocknum, &fs_b);
				return bytes_written;
			}
		}
		DfsWriteBlock(fs_blocknum, &fs_b);
	}

	return DFS_FAIL;
	//	//determine the number of blocks we are writing to
	//	int numBlocks = 0;
	//
	//	while (num_bytes > 0 ){
	//		numBlocks++;
	//		num_bytes -= <sizeof diskBlock>;
	//	}
	//
	//	//Have number of blocks to write to
	//	//Find the number of bytes to write to the last block
	//
	//
	//
	//	return DFS_FAIL;
}


//-----------------------------------------------------------------
// DfsInodeFilesize simply returns the size of an inode's file. 
// This is defined as the maximum virtual byte number that has 
// been written to the inode thus far. Return DFS_FAIL on failure.
//-----------------------------------------------------------------

uint32 DfsInodeFilesize(uint32 handle) {

	//Check if file system is valid
	if (sb.valid == 0 || inodes[handle].used == 0){
		printf("The File System in memory is not valid or something else\n");
		return DFS_FAIL;
	}


	return inodes[handle].size;
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
	dfs_block indirect;
	uint32* data;

	uint32 fs_blocknum = 0;
	data = (uint32*)indirect.data;

	dbprintf ('y', "DfsInodeAllocateVirtualBlock: allocating vblock %d\n", virtual_blocknum);
	if (virtual_blocknum < 10){
		if (DfsIsAllocated(inodes[handle].direct[virtual_blocknum])){
			return inodes[handle].direct[virtual_blocknum];
		} else {
			inodes[handle].direct[virtual_blocknum] = DfsAllocateBlock();
			return inodes[handle].direct[virtual_blocknum];
		}
	}

	if (DfsInodeHasIndirect(handle)){
		DfsReadBlock(inodes[handle].indirect, &indirect);//read the block pointed to by indirect field
		if (DfsIsAllocated((fs_blocknum = data[virtual_blocknum-10]))){
			return fs_blocknum;
		} else {
			data[virtual_blocknum-10] = DfsAllocateBlock();
			DfsWriteBlock(inodes[handle].indirect, &indirect);
			return data[virtual_blocknum-10];
		}

	} else {
		bzero(indirect.data, DFS_BLOCKSIZE);
		inodes[handle].indirect = DfsAllocateBlock();
		data[virtual_blocknum - 10] = DfsAllocateBlock();
		DfsWriteBlock(inodes[handle].indirect, &indirect);
		return data[virtual_blocknum-10];
	}

	return DFS_FAIL;
}



//-----------------------------------------------------------------
// DfsInodeTranslateVirtualToFilesys translates the 
// virtual_blocknum to the corresponding file system block using 
// the inode identified by handle. Return DFS_FAIL on failure.
//-----------------------------------------------------------------

uint32 DfsInodeTranslateVirtualToFilesys(uint32 handle, uint32 virtual_blocknum) {
	dfs_block indirect;
	uint32* data;

	uint32 fs_blocknum = 0;
	data = (uint32*)indirect.data;

	dbprintf ('y', "DfsInodeTranslateVirtualToFilesys: translating vblock %d\n", virtual_blocknum);
	if (virtual_blocknum < 10){
		if (DfsIsAllocated(inodes[handle].direct[virtual_blocknum])){
			dbprintf ('y', "DfsInodeTranslateVirtualToFilesys: vblock %d is already allocated and translates to %d"
					"\n", virtual_blocknum, inodes[handle].direct[virtual_blocknum] );
			return inodes[handle].direct[virtual_blocknum];
		} else {
			dbprintf ('y', "DfsInodeTranslateVirtualToFilesys: translation failed in first fail\n");
			return DFS_FAIL;
		}
	}

	if (DfsInodeHasIndirect(handle)){
		dbprintf ('y', "DfsInodeTranslateVirtualToFilesys: performing indirect translation\n");
		DfsReadBlock(inodes[handle].indirect, &indirect);//read the block pointed to by indirect field
		if (DfsIsAllocated((fs_blocknum = data[virtual_blocknum-10]))){
			dbprintf ('y', "DfsInodeTranslateVirtualToFilesys: indirect trans of %d to FS block %d\n", virtual_blocknum, fs_blocknum);
			return fs_blocknum;
		}
	}

	return DFS_FAIL;
}

uint32 DfsInodeHasIndirect(uint32 handle){
	if ((inodes[handle].indirect < sb.freevector+sb.fbv_numfsblock) ||
			inodes[handle].indirect >= sb.numfsblocks){
		return 0;
	}else{
		return 1;
	}
}

void printSB(){
	printf("\nPrinting SUPER BLOCK *****\n");
	printf("valid = %d\n", sb.valid);
	printf("blocksize = %d\n", sb.blocksize);
	printf("number of FS blocks = %d\n", sb.numfsblocks);
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

void printINODE(){
	printf("Inode 0 filename is %s\n", inodes[0].filename);
}
