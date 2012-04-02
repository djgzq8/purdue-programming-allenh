#include "lab4-api.h"
#include "usertraps.h"
#include "misc.h"

#include "../include/q3.h"
void wait(int n);
void eat(int *food);

void main (int argc, char *argv[])
{
	phil *mem;        // Used to access missile codes in shared memory page
	uint32 h_mem;            // Handle to the shared memory page
	sem_t s_procs_completed; // Semaphore to signal the original process that we're done
	int phil_num = -1;
	int food = 0;
	int left = -1;
	int right = -1;

	if (argc != 4) {
		Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
		Exit();
	}

	// Convert the command-line strings into integers for use as handles
	h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
	s_procs_completed = dstrtol(argv[2], NULL, 10);
	phil_num = dstrtol(argv[3], NULL, 10);
	left = phil_num;
	right = (phil_num+1)%5;
	// Map shared memory page into this process's memory space
	if ((mem = (phil *)shmat(h_mem)) == NULL) {
		Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
		Exit();
	}

	food = 100;
	while (food > 0){
		wait(100000*phil_num);
		//		Printf("Philosopher %d picking up chopstick %d on his left\n", phil_num, phil_num);
		if (mem->free[left] && mem->free[right]){
			lock_acquire(mem->chopsticks[left]);
			mem->free[left] = 0;
			lock_acquire(mem->chopsticks[right]);
			mem->free[right] = 0;
			//			Printf("Philosopher %d picking up chopstick %d on his left\n", phil_num, (phil_num+1)%5);

			wait(1000000);
			//				Printf("Philosopher %d Eating food %d\n%d Foods left\n", phil_num, food, food-1);
			eat(&food);
			Printf("Hello from philosopher %d!\n", phil_num);
			lock_acquire(mem->wait_locks[right]);
			cond_signal(mem->wait[right]);
			lock_release(mem->wait_locks[right]);
			lock_acquire(mem->wait_locks[left]);
			cond_signal(mem->wait[left]);
			lock_release(mem->wait_locks[left]);


			lock_release(mem->chopsticks[left]);
			mem->free[left] = 1;
			lock_release(mem->chopsticks[right]);
			mem->free[right] = 1;
		} else if (!mem->free[left]){
			lock_acquire(mem->wait_locks[left]);
			cond_wait(mem->wait[left]);
			lock_release(mem->wait_locks[left]);
		} else {
			lock_acquire(mem->wait_locks[right]);
			cond_wait(mem->wait[right]);
			lock_release(mem->wait_locks[right]);
		}


	}
	// Now print a message to show that everything worked


	// Signal the semaphore to tell the original process that we're done
	Printf("spawn_me: PID %d is complete.\n", getpid());
	if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
		Exit();
	}
}

void wait(int n){
	float value = 2.0;
	float inter = 0;
	int i = 0;

	for (i = 0; i < n; i++){
		value = value * value;
		inter = value + value;
		value = inter + value;
	}
}

void eat(int *food){
	(*food)--;
}
