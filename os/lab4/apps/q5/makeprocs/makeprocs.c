#include "lab4-api.h"
#include "usertraps.h"
#include "misc.h"

#include "../include/q5.h"

void main (int argc, char *argv[])
{
	int water = 0;               // Used to store number of processes to create
	int sulfate = 0;
	int i;                          // Loop index variable
	atmosphere *atmos;               // Used to get address of shared memory page
	uint32 h_mem;                   // Used to hold handle to shared memory page
	sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
	char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
	char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
	char rx_type[2];

	if (argc != 3) {
		Printf("Usage: ");
		Printf(argv[0]);
		Printf(" <number of H2O molecules to inject>");
		Printf(" <number of SO4 molecules to inject>\n");
		Exit();
	}

	// Convert string from ascii command line argument to integer number
	water = dstrtol(argv[1], NULL, 10); // the "10" means base 10
	sulfate = dstrtol(argv[2], NULL, 10); // the "10" means base 10
	//  Printf("Injecting %d water molecules\n", water);
	//  Printf("Injecting %d sulfate molecules\n", sulfate);

	// Allocate space for a shared memory page, which is exactly 64KB
	// Note that it doesn't matter how much memory we actually need: we
	// always get 64KB
	if ((h_mem = shmget()) == 0) {
		Printf("ERROR: could not allocate shared memory page in ");
		Printf(argv[0]);
		Printf(", exiting...\n");
		Exit();
	}

	// Map shared memory page into this process's memory space
	if ((atmos = (atmosphere *)shmat(h_mem)) == NULL) {
		Printf("Could not map the shared page to virtual address in ");
		Printf(argv[0]);
		Printf(", exiting..\n");
		Exit();
	}

	//if semphaphore is negative, then it will wait
	atmos->water = sem_create(0);
	atmos->oxygen = sem_create(0);
	atmos->hydrogen = sem_create(0);
	atmos->sulfate = sem_create(0);
	atmos->sulfer_dioxide = sem_create(0);
	atmos->original_sulfate_num = sulfate;
	atmos->original_water_num = water;

	// Create semaphore to not exit this process until all other processes
	// have signalled that they are complete.  To do this, we will initialize
	// the semaphore to (-1) * (number of signals), where "number of signals"
	// should be equal to the number of processes we're spawning - 1.  Once
	// each of the processes has signaled, the semaphore should be back to
	// zero and the final sem_wait below will return.
	if ((s_procs_completed = sem_create(-(5-1))) == SYNC_FAIL) {
		Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
		Exit();
	}

	// Setup the command-line arguments for the new process.  We're going to
	// pass the handles to the shared memory page and the semaphore as strings
	// on the command line, so we must first convert them from ints to strings.
	ditoa(h_mem, h_mem_str);
	ditoa(s_procs_completed, s_procs_completed_str);

	// Now we can create the processes.  Note that you MUST end your call to
	// process_create with a NULL argument so that the operating system
	// knows how many arguments you are sending.

	ditoa(RX1, rx_type);
	process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, rx_type, NULL);
//	Printf("Reaction 1 process created\n");

	ditoa(RX2, rx_type);
	process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, rx_type, NULL);
//	Printf("Reaction 2 process created\n");

	ditoa(RX3, rx_type);
	process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, rx_type, NULL);
//	Printf("Reaction 3 process created\n");

	ditoa(WATER_RX, rx_type);
	process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, rx_type, NULL);
//	Printf("Water injection process created\n");

	ditoa(SULFATE_RX, rx_type);
	process_create(FILENAME_TO_RUN, h_mem_str, s_procs_completed_str, rx_type, NULL);
//	Printf("Sulfate injection process created\n");

	// And finally, wait until all spawned processes have finished.
	if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
		Exit();
	}
	Printf("The final O2 has been created\n");
	Printf("The final H2SO4 has been created\n");
	Printf("All other processes completed, exiting main process.\n");
}
