#ifndef __DFS_SHARED__
#define __DFS_SHARED__

#define DFS_BLOCKSIZE 			1024  // Must be an integer multiple of the disk blocksize
#define DFS_MAX_FILESYSTEM_SIZE 0x2000000  // 32MB
#define DFS_INODE_MAX_NUM 		512

typedef struct dfs_superblock {
	uint32 valid;
	uint32 blocksize;
	uint32 numblocks;
	uint32 inodes;
	uint32 numinodes;
	uint32 freevector;
} dfs_superblock;

typedef struct dfs_block {
  char data[DFS_BLOCKSIZE];
} dfs_block;

typedef struct dfs_inode {
  // STUDENT: put inode structure internals here
  // IMPORTANT: sizeof(dfs_inode) MUST return 64 in order to fit in enough
  // inodes in the filesystem (and to make your life easier).  To do this, 
  // adjust the maximumm length of the filename until the size of the overall inode 
  // is 64 bytes.
	uint32 used;
	uint32 size;
	char filename[12];//file name length determined by everything else in inode
	uint32 direct[10];
	uint32 indirect;
} dfs_inode;


#define DFS_FAIL -1
#define DFS_SUCCESS 1

#endif
