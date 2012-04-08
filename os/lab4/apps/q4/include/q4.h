#ifndef __USERPROG__
#define __USERPROG__

#define BUFFER_SIZE 12

typedef struct buffer {
  int head;
  int tail;
  char data[BUFFER_SIZE];
  lock_t l_pro;
  lock_t l_con;
  cond_t c_pro;
  cond_t c_con;
  lock_t l;

} buffer;

#define FILENAME_TO_RUN "q4.dlx.obj"

#endif
