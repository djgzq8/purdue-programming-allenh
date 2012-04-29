/*****************************************************************
 *
 * Various APIs for Lab 2 of OS469
 * Copyright: Uday Savagaonkar, Jan 2002
 *
 *****************************************************************/

#ifndef _LAB2_API_H_
#define _LAB2_API_H_

#define NULL (void *)0

#define SYNC_SUCCESS 1
#define SYNC_FAIL    -1

#define BUFFERSIZE       5

typedef unsigned int uint32;



//Related to shared memory
uint32 shmget();			//trap 0x440
void *shmat(uint32 handle);		//trap 0x441



#endif _LAB2_API_H_

