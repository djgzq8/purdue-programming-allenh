#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------
static mbox mbox_list[MBOX_NUM_MBOXES];
static mbox_message message_list[MBOX_NUM_BUFFERS];

void MboxModuleInit() {
	int i;
	for (i = 0; i < MBOX_NUM_MBOXES; i++) {
		mbox_list[i].handle = (mbox_t)i;
		mbox_list[i].in_use = 0;
	}
	return;
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
	int i;
	int j;
	for (i = 0; i < MBOX_NUM_MBOXES; i++) {  //modified (MBOX_NUM_BOXES)
		if (mbox_list[i].in_use == 0){
			mbox_list[i].in_use = 1;
			mbox_list[i].msg_count = 0;
			mbox_list[i].empty = SemCreate(MBOX_MAX_BUFFERS_PER_MBOX);  //modified (MBOX_MAX_BUFFERS)
			mbox_list[i].full = SemCreate(0);
			mbox_list[i].lock = LockCreate();
			for(j = 0; j < MBOX_NUM_BUFFERS; j++) {
				mbox_list[i].users[j] = 0;
			}
			return mbox_list[i].handle;
		}
	}
 	return MBOX_FAIL;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
	int i;
	if (handle >= 0 && handle < MBOX_NUM_MBOXES) {  //modified (MBOX_NUM_BOXES)
		if (mbox_list[(int)handle].in_use >= 1 &&
		    mbox_list[(int)handle].msg_count < MBOX_MAX_BUFFERS_PER_MBOX) {
			for (i = 0; i < MBOX_NUM_BUFFERS; i++) {
				if (mbox_list[(int)handle].users[i] == 0) {
					mbox_list[(int)handle].users[i] = GetCurrentPid();
					mbox_list[(int)handle].in_use++;
				}
			}
			return MBOX_SUCCESS;
		}
	}
	return MBOX_FAIL;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
	int i;
	int pid = GetCurrentPid();
	if (handle >= 0 && handle < MBOX_NUM_MBOXES) { //modified (MBOX_NUM_BOXES)
		if (mbox_list[(int)handle].in_use >= 1) {
			for (i = 0; i < MBOX_NUM_BUFFERS; i++) {
				if (mbox_list[(int)handle].users[i] == pid) {
					mbox_list[(int)handle].users[i] = 0;
					mbox_list[(int)handle].in_use--;
				}
			}
			return MBOX_SUCCESS;
		}
	}		
	return MBOX_FAIL;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
	mbox tmp = mbox_list[handle];
	LockHandleAcquire(tmp.lock);
	while (tmp.msg_count >= MBOX_MAX_BUFFERS_PER_MBOX) {
		SemWait(tmp.full);
	}

	//dstrncpy(tmp.msgs[tmp.head].message,message,length);
	tmp.msgs[tmp.head].length = length;
	tmp.head = (tmp.head + 1) % MBOX_MAX_BUFFERS_PER_MBOX;
	tmp.msg_count++;

	SemHandleSignal(tmp.empty);
	LockHandleRelease(tmp.lock);
	return MBOX_SUCCESS;
	
  return MBOX_FAIL;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
	int size;
	mbox tmp = mbox_list[handle];
	LockHandleAcquire(tmp.lock);
	while(tmp.msg_count == 0) {
		SemWait(tmp.empty);
	}

	if (tmp.msgs[tmp.tail].length < maxlength) {
		size = tmp.msgs[tmp.tail].length;
	}
	else {
		return MBOX_FAIL;
	}
	//dstrncpy(message, tmp.msgs[tmp.tail].message,size);
	tmp.tail = (tmp.tail + 1) % MBOX_MAX_BUFFERS_PER_MBOX;
	tmp.msg_count--;

	SemHandleSignal(tmp.full);
	LockHandleRelease(tmp.lock);
	return size;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
	int i;
	int j;
	for (i = 0; i < MBOX_NUM_MBOXES; i++) {
		for (j = 0; j < MBOX_NUM_BUFFERS; j++) {
				if (mbox_list[i].users[j] == pid) {       //modified (mbox_list[(int)handle].users[j] == pid)
					mbox_list[i].users[i] = 0;  //modified (mbox_list[(int)handle].users[i] = 0)
					mbox_list[i].in_use--;      //modified (mbox_list[(int)handle].in_use--)
				}
		}
	}
	return MBOX_SUCCESS;
       //return MBOX_FAIL;
}