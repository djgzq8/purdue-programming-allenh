#include "usertraps.h"
#include "misc.h"
#include "fdisk.h"

dfs_superblock sb;
dfs_inode inodes[DFS_INODE_MAX_NUM];
//uint32 fbv[DFS_FBV_MAX_NUM_WORDS];

int diskblocksize = 0; // These are global in order to speed things up
int disksize = 0; // (i.e. fewer traps to OS to get the same number)
int ratio = 0;

char UnsignedToCharCopy(uint32 a,  uint32 index);
int FdiskWriteBlock(uint32 blocknum, dfs_block *b); //You can use your own function. This function 
//calls disk_write_block() to write physical blocks to disk

void main(int argc, char *argv[]) {
	int num_filesystem_blocks = 0;
	int fbv_num_fsblocks, inode_num_fsblocks = 0;
	int i,j = 0;
	dfs_block fs_wdata;

//	Printf("size of super block = %d\n", sizeof(dfs_superblock));


	// You need to think of the finer details. You can use bzero() to zero out bytes in memory
//	void bzero(char *mem, int num_bytes);
//	void bcopy(char *src, char *dst, int num_bytes);
	//Initializations and argc check
	dfs_invalidate();
	for (i = 0; i < DFS_BLOCKSIZE; i++){
		fs_wdata.data[i] = 0;
	}

	disksize = disk_size();
	diskblocksize = disk_blocksize();
	ratio = DFS_BLOCKSIZE/diskblocksize;

	num_filesystem_blocks = (int)(diskblocksize / (float) DFS_BLOCKSIZE * disksize / diskblocksize);

	fbv_num_fsblocks = (int)(diskblocksize / (float) DFS_BLOCKSIZE * disksize / diskblocksize / 32 * 4 /  DFS_BLOCKSIZE  );
//	Printf("%d words %d\n", num_filesystem_blocks, fbv_num_fsblocks);

	inode_num_fsblocks = (int)((float) DFS_INODE_MAX_NUM / (DFS_BLOCKSIZE / sizeof(dfs_inode)));

	// Make sure the disk exists before doing anything else
	 if (disk_create() == DISK_FAIL){
		 Printf("Failed to open the disk file for writing!\n");
		 Printf("Make sure the file name is defined correctly.\n");
		 return;
	 }

	 //Setup super block in memory, it writes to disk later though
	 sb.valid = 1;
	 sb.blocksize = DFS_BLOCKSIZE;
	 sb.numblocks = num_filesystem_blocks;
	 sb.inodes = 1;
	 sb.numinodes = DFS_INODE_MAX_NUM;
	 sb.freevector = inode_num_fsblocks + sb.inodes;

	// Write all inodes as not in use and empty (all zeros)
	 Printf("Initializing %d inodes\n", DFS_INODE_MAX_NUM);
	 bzero((char *)(&fs_wdata), sizeof(fs_wdata));
	 j=0;
	 for (i = 0; i < sizeof(inodes); i = i + sizeof(fs_wdata)){
		 FdiskWriteBlock(j+1, &fs_wdata);
		 j++;
	 }

	// Next, setup free block vector (fbv) and write free block vector to the disk
	 Printf("Initializing %d blocks used for the freeblockvector starting at FS block %d\n", fbv_num_fsblocks, sb.freevector);
	 for (i = 0; i < DFS_BLOCKSIZE; i++){
		 fs_wdata.data[i] = 0xFF;
	 }
	 for (i = 0; i < fbv_num_fsblocks; i++){
		 FdiskWriteBlock(i+sb.freevector, &fs_wdata);
	 }

	 // Finally, setup superblock as valid filesystem and write superblock and boot record to disk:
	 // boot record is all zeros in the first physical block,
	 // and superblock structure goes into the second physical block
	 Printf("Writing MBR and superblock information...\n\n");
	 bzero((char *) &fs_wdata, sizeof(fs_wdata));
	 bcopy((char *)(&sb.valid), &(fs_wdata.data[diskblocksize]), sizeof(dfs_superblock));
	 FdiskWriteBlock(0, &fs_wdata);


	 Printf("fdisk (%d): Formatted DFS disk for %d bytes.\n", getpid(), disksize);
}


int FdiskWriteBlock(uint32 blocknum, dfs_block *b) {
	// STUDENT: put your code here
	int i,j = 0;
	char wdata[diskblocksize];
	int phys_blocknum = blocknum * ratio;

	for(i = 0; i < DFS_BLOCKSIZE/diskblocksize; i++){
//		Printf("Writing physical block %d\n", phys_blocknum + i);
		for (j = 0; j < diskblocksize; j++){
			wdata[j] = b->data[j + i*diskblocksize];
		}

		if (disk_write_block(phys_blocknum + i, wdata) == DISK_FAIL){
			return DISK_FAIL;
		}
	}

	return DFS_SUCCESS;
}

char UnsignedToCharCopy(uint32 a, uint32 index){
	return a >> (index * 8);
}
