#ifndef __USERPROG__
#define __USERPROG__

//Original code
/*typedef struct missile_code {
  int numprocs;
  char really_important_char;
} missile_code;*/

#define BUFFER_SIZE 12

typedef struct buffer {
  int head;
  int tail;
  char data[BUFFER_SIZE];
  lock_t l;
  sem_t fillCount;
  sem_t emptyCount;
} buffer;

#define FILENAME_TO_RUN "q4.dlx.obj"

#endif
