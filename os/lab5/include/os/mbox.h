#ifndef __MBOX_OS__
#define __MBOX_OS__

#define MBOX_NUM_MBOXES 16           // Maximum number of mailboxes allowed in the system
#define MBOX_NUM_BUFFERS 50          // Maximum number of message buffers allowed in the system
#define MBOX_MAX_BUFFERS_PER_MBOX 10 // Maximum number of buffer slots available to any given mailbox
#define MBOX_MAX_MESSAGE_LENGTH 50   // Buffer size of 50 for each message

#define MBOX_FAIL -1
#define MBOX_SUCCESS 1

//---------------------------------------------
// Define your mailbox structures here
//--------------------------------------------

typedef int mbox_t; // This is the "type" of mailbox handles

typedef struct mbox_message {
	int id;
	char message[MBOX_MAX_MESSAGE_LENGTH];
	int length;
} mbox_message;

typedef struct mbox {
	mbox_t handle;
	int head;
	int tail;
	lock_t lock;
	sem_t empty;
	sem_t full;
	int in_use;
	int msg_count;
	int users[MBOX_NUM_BUFFERS];
	mbox_message msgs[MBOX_MAX_BUFFERS_PER_MBOX];
} mbox;



//-------------------------------------------
// Prototypes for Mbox functions you have to write
//-------------------------------------------

void MboxModuleInit();
mbox_t MboxCreate();
int MboxOpen(mbox_t m);
int MboxClose(mbox_t m);
int MboxSend(mbox_t m, int length, void *message);
int MboxRecv(mbox_t m, int maxlength, void *message);
int MboxCloseAllByPid(int pid);
void PrintUsersByHandle(mbox_t handle);
#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#endif
