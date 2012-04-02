#ifndef __USERPROG__
#define __USERPROG__

typedef struct phil {
	lock_t chopsticks[5];
	cond_t wait[5];
	lock_t wait_locks[5];
	int free[5];
} phil;

#define FILENAME_TO_RUN "q3.dlx.obj"


#endif
