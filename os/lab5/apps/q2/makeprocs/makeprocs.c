#include "usertraps.h"
#include "misc.h"
#include "spawn.h"

#include "../include/spawn.h"

void main (int argc, char *argv[])
{
	int water,sulfate,limit;               // Used to store number of processes to create
	int i;                          // Loop index variable
	int total_proc;
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
	Printf("\n\n******************   q2   ******************\n");
	Printf("makeprocs (%d): Creating %d water molecules and %d sulfate molecules\n", getpid(), water, sulfate);

	if (water <= 1){
		Printf("\nERROR!\n\tThe number of water molecules must be greater than 2 in order for a reaction to occur!\n");
		Printf("****************** end q2 ******************\n\n");
		Exit(-1);
	}
	if (sulfate <= 0){
		Printf("\nERROR!\n\tThe number of sulfate molecules must be greater than 2 in order for a reaction to occur!\n");
		Printf("****************** end q2 ******************\n\n");
		Exit(-1);
	}

	// Put some values in the mc structure to send as a message
	h2_mbox = mbox_create();
	so4_mbox = mbox_create();
	o2_mbox = mbox_create();
	so2_mbox = mbox_create();
	h2o_mbox = mbox_create();

	if (mbox_open(h2_mbox) == MBOX_FAIL) {
		Printf("makeprocs (%d): Could not open h2 mailbox !\n", getpid());
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}
	if (mbox_open(so4_mbox) == MBOX_FAIL) {
		Printf("makeprocs (%d): Could not open so4 mailbox !\n", getpid());
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}
	if (mbox_open(o2_mbox) == MBOX_FAIL) {
		Printf("makeprocs (%d): Could not open o2 mailbox !\n", getpid());
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}
	if (mbox_open(so2_mbox) == MBOX_FAIL) {
		Printf("makeprocs (%d): Could not open so2 mailbox !\n", getpid());
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}
	if (mbox_open(h2o_mbox) == MBOX_FAIL) {
		Printf("makeprocs (%d): Could not open h20 mailbox !\n", getpid());
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}


	if(water < sulfate){
		limit = water;
	} else {
		limit = sulfate;
	}

	Printf("RX1: 2H2O -> 2H2 + O2 will happen %d times\n", water/2);
	Printf("RX2: SO4 -> SO2 + O2 will happen %d times\n", sulfate);
	Printf("RX3: H2 + SO2 + O2 -> H2SO4 will happen %d times\n", limit);

	if ((water/2 + sulfate - limit) > 10){
		Printf("makeprocs (%d): In this configuration, an excess of oxygen will cause a fire!\n", getpid());
		Printf("makeprocs (%d): (ie all the messages in the o2 mailbox will be used, Goodbye!\n", getpid());
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}

	total_proc = (water/2)*2 + sulfate + (water/2) + sulfate + limit;

	// Create semaphore to not exit this process until all other processes
	// have signaled that they are complete.  To do this, we will initialize
	// the semaphore to (-1) * (number of signals), where "number of signals"
	// should be equal to the number of processes we're spawning - 1.  Once
	// each of the processes has signaled, the semaphore should be back to
	// zero and the final sem_wait below will return.
	if ((s_procs_completed = sem_create(-(total_proc-1))) == SYNC_FAIL) {
		Printf("makeprocs (%d): Bad sem_create\n", getpid());
		Printf("****************** end q2 ******************\n\n");
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
		if (i < limit) {
			process_create(RX3,  10,0, h2_mbox_str, so2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
			Printf("makeprocs (%d): Process RX3 created\n", getpid());
		}
		if (i < sulfate) {
			process_create(RX2,  5,0, so4_mbox_str, so2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
			Printf("makeprocs (%d): Process RX2 created\n", getpid());
		}
		if (i < water/2) {
			process_create(RX1,  3,0, h2o_mbox_str, h2_mbox_str, o2_mbox_str, s_procs_completed_str, NULL);
			Printf("makeprocs (%d): Process RX1 created\n", getpid());
		}
		if (i < water/2) {
			process_create(WATER,  1, 0, h2o_mbox_str, s_procs_completed_str, NULL);
			Printf("makeprocs (%d): Process WATER created\n", getpid());

			process_create(WATER, 7, 0,  h2o_mbox_str, s_procs_completed_str, NULL);
			Printf("makeprocs (%d): Process WATER created\n", getpid());
		}
		if (i <sulfate) {
			process_create(SULFATE,  4, 0, so4_mbox_str, s_procs_completed_str, NULL);
			Printf("makeprocs (%d): Process SULFATE created\n", getpid());
		}
		//		sleep(3);
	}

	Printf("makeprocs (%d): Finished creating processes\n", getpid(), i);


	// And finally, wait until all spawned processes have finished.
	if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
		Printf("****************** end q2 ******************\n\n");
		Exit();
	}
	Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());
	Printf("****************** end q2 ******************\n\n");
}
