#ifndef __USERPROG__
#define __USERPROG__

typedef struct phil {
	cond_t self[5];
	lock_t wait_locks[5];
	int state[5];
	int eaten[5];

} phil;

#define FILENAME_TO_RUN "q3.dlx.obj"
#define HUNGRY 0
#define EATING 1
#define THINKING 2

#endif
