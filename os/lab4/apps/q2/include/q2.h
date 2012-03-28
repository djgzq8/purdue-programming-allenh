#ifndef __USERPROG__
#define __USERPROG__

#define BUFFER_SIZE 12

typedef struct buffer {
  int head;
  int tail;
  char data[BUFFER_SIZE];
  lock_t l;
  sem_t fillCount;
  sem_t emptyCount;
} buffer;

#define FILENAME_TO_RUN "q2.dlx.obj"

#endif
