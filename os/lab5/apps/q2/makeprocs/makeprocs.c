#include "usertraps.h"
#include "misc.h"

#include "../include/spawn.h"

void main (int argc, char *argv[])
{
  int water,sulfate,limit;               // Used to store number of processes to create
  int i;                          // Loop index variable
  missile_code mc;                // Used as message for mailbox
  mbox_t h2_mbox;
  mbox_t so2_mbox;
  mbox_t o2_mbox;
  mbox_t so4_mbox;
  mbox_t h2o_mbox;
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  char h2o_mbox_str[10];            // Used as command-line argument to pass mem_handle to new processes
  char h2_mbox_str[10];
  char o2_mbox_str[10];
  char so2_mbox_str[10];
  char so4_mbox_str[10];
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes

  if (argc != 3) {
    Printf("Usage: %s <number of h20 molecules> <number of so4 molecules>\n", argv[0]);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  water = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  sulfate = dstrtol(argv[2], NULL, 10); // the "10" means base 10
  Printf("makeprocs (%d): Creating processes\n", getpid());

  // Allocate space for a mailbox
//  if ((h_mbox = mbox_create()) == MBOX_FAIL) {
//    Printf("makeprocs (%d): ERROR: could not allocate mailbox!\n", getpid());
//    Exit();
//  }

  // Open mailbox to prevent deallocation


  // Put some values in the mc structure to send as a message
  h2_mbox = mbox_create();
  so4_mbox = mbox_create();
  o2_mbox = mbox_create();
  so2_mbox = mbox_create();
  h2o_mbox = mbox_create();

  if (mbox_open(h2_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox !\n", getpid());
    Exit();
  }
  if (mbox_open(so4_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox !\n", getpid());
    Exit();
  }
  if (mbox_open(o2_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox !\n", getpid());
    Exit();
  }
  if (mbox_open(so2_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox !\n", getpid());
    Exit();
  }
  if (mbox_open(h2o_mbox) == MBOX_FAIL) {
    Printf("makeprocs (%d): Could not open mailbox !\n", getpid());
    Exit();
  }

	if(water < sulfate){
		limit = water;
	} else {
		limit = sulfate;
	}


  // Create semaphore to not exit this process until all other processes 
  // have signaled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_procs_completed = sem_create(-(limit*6-1))) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }


  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h2o_mbox, h2o_mbox_str);
  ditoa(h2_mbox, h2_mbox_str);
  ditoa(o2_mbox, o2_mbox_str);
  ditoa(so2_mbox, so2_mbox_str);
  ditoa(so4_mbox, so4_mbox_str);
  ditoa(s_procs_completed, s_procs_completed_str);
  for (i = 0; i < limit; i++) {

      process_create(WATER, 0, 0, h2o_mbox_str, s_procs_completed_str, NULL);
      Printf("makeprocs (%d): Process %d created\n", getpid(), i);

      process_create(WATER, 0, 0, h2o_mbox_str, s_procs_completed_str, NULL);
      Printf("makeprocs (%d): Process %d created\n", getpid(), i);

      process_create(SULFATE, 0, 0, so4_mbox_str, s_procs_completed_str, NULL);
      Printf("makeprocs (%d): Process %d created\n", getpid(), i);

      process_create(RX1, 0, 0, h2o_mbox_str, h2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
      Printf("makeprocs (%d): Process %d created\n", getpid(), i);

      process_create(RX2, 0, 0, so4_mbox_str, so2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
      Printf("makeprocs (%d): Process %d created\n", getpid(), i);

      process_create(RX3, 0, 0, h2_mbox_str, so2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
      Printf("makeprocs (%d): Process %d created\n", getpid(), i);
  }

  Printf("makeprocs (%d): Finished creating processes\n", getpid(), i);


  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
}
