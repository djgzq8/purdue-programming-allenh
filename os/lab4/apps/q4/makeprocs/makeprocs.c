#include "lab4-api.h"
#include "usertraps.h"
#include "misc.h"

#include "../include/q4.h"

void main (int argc, char *argv[])
{
  int numprocs = 0;               // Used to store number of processes to create
  int i;                          // Loop index variable
  buffer *circle;               // Used to get address of shared memory page
  uint32 h_mem;                   // Used to hold handle to shared memory page
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char s_p_type[2];
  char s_c_type[2];


  if (argc != 2) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of processes to create>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  numprocs = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  Printf("Creating %d processes\n", numprocs);

  // Allocate space for a shared memory page, which is exactly 64KB
  // Note that it doesn't matter how much memory we actually need: we 
  // always get 64KB
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Map shared memory page into this process's memory space
  if ((circle = (buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the shared page to virtual address in ");
    Printf(argv[0]);
    Printf(", exiting..\n");
    Exit();
  }

  // Put some values in the shared memory, to be read by other processes
  circle->head = 0;
  circle->tail = 0;
  circle->l_pro = lock_create();
  circle->l_con = lock_create();

  circle->c_pro = cond_create(circle->l_pro);
  circle->c_con = cond_create(circle->l_con);

  Printf("return of first lock_acquire = %d\n", lock_acquire(circle->l));
  Printf("Lock handle is %d\n", circle->l);


  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_procs_completed = sem_create(-(numprocs*2-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in ");
    Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h_mem, h_mem_str);
  ditoa(s_procs_completed, s_procs_completed_str);

  ditoa('p', s_p_type);
  ditoa('c', s_c_type);

  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
  for(i=0; i<numprocs; i++) {
    process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, s_p_type, NULL);
    Printf("Producer %d created\n", i);
    process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, s_c_type, NULL);
    Printf("Consumer %d created\n", i);
  }
  lock_release(circle->l);

  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("All other processes completed, exiting main process.\n");
}
