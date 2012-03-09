#ifndef __DISK_H__
#define __DISK_H__

// Name of file which represents the "hard disk"
#define DISK_FILENAME "/tmp/ee469g12.img"

// Number of bytes in one physical disk block
#define DISK_BLOCKSIZE 512 

typedef struct disk_block {
  char data[DISK_BLOCKSIZE]; // This assumes that DISK_BLOCKSIZE is a multiple of 4 (for byte alignment)
} disk_block;


// Total size of this disk, in units of 512-byte blocks
//Assume disk size of 32MB. 32MB/512byte/block = 65536 physical blocks
#define DISK_NUMBLOCKS 0x10000 //STUDENT: Calculate the correct no of blocks and write

#define DISK_SUCCESS 1
#define DISK_FAIL -1

int DiskBytesPerBlock();
int DiskSize();
int DiskCreate();
int DiskWriteBlock (uint32 blocknum, disk_block *b);
int DiskReadBlock (uint32 blocknum, disk_block *b);

#endif
