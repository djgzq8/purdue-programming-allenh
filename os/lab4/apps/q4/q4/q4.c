#include "lab4-api.h"
#include "usertraps.h"
#include "misc.h"

#include "../include/q4.h"
void producer (uint32 h_mem);
void consumer (uint32 h_mem);


void main(int argc, char *argv[]){
	sem_t s_procs_completed; // Semaphore to signal the original process that we're done
	uint32 h_mem;            // Handle to the shared memory page
	char type;

	if (argc != 4) {
		Printf("Usage: ");
		Printf(argv[0]);
		Printf(" <shared_memory_page_handle> <page_mapped_semaphore_handle> <p_or_c>\n");
		Exit();
	}
	// Convert the command-line strings into integers for use as handles
	h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
	s_procs_completed = dstrtol(argv[2], NULL, 10);
	type = dstrtol(argv[3], NULL, 10);

	Printf("argv[3] is %d\n", type);
	if (type == 'p'){
		producer (h_mem);
	}
	else if(type == 'c'){
		consumer(h_mem);
	} else{
		Printf("q2: Invalid process type to create\n");
		//invalid argument
	}
	// Signal the semaphore to tell the original process that we're done
//	Printf("q2: PID %d is complete.\n", getpid());
	if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
		Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
		Exit();
	}
}

void producer (uint32 h_mem)
{
	buffer *b;        // Used to access missile codes in shared memory page
	int t = 0;
	int i = 0;
	char h[12] = "Hello World";

	//	// Map shared memory page into this process's memory space
	if ((b = (buffer *)shmat(h_mem)) == NULL) {
		Printf("Could not map the virtual address to the memory in producer");
		Printf(", exiting...\n");
		Exit();
	}
	while (i < dstrlen(h)){
		//		while ((b->head + 1) % BUFFER_SIZE == b->tail);
		//sem_wait(b->emptyCount);
		//		} else{

		sem_wait(b->emptyCount);
		lock_acquire(b->l);
		Printf("Producer %d inserted: %c\n", getpid(), h[i]);
		b->data[b->head] = h[i];
		b->head = (b->head+1) % BUFFER_SIZE;
		i++;
		lock_release(b->l);
		sem_signal(b->fillCount);

		//		}
}

	//
	//	// Now print a message to show that everything worked
}

void consumer (uint32 h_mem){
	buffer *b;        // Used to access missile codes in shared memory page
	int i = 0;
	char c = 0;


	//	// Map shared memory page into this process's memory space
	if ((b = (buffer *)shmat(h_mem)) == NULL) {
		Printf("Could not map the virtual address to the memory in producer");
		Printf(", exiting...\n");
		Exit();
	}

	while (i < dstrlen("Hello World")){

		//		while (b->head == b->tail);
		//			sem_wait(b->fillCount);
		//		}else{

		sem_wait(b->fillCount);
		lock_acquire(b->l);
		c = b->data[b->tail];
		b->tail = (b->tail + 1) % BUFFER_SIZE;
		i++;
		Printf("Consumer %d removed:  %c\n", getpid(), c);
		lock_release(b->l);
		sem_signal(b->emptyCount);

		//		}

	}

//	Printf("q2->consumer: My PID is %d\n", getpid());
}
