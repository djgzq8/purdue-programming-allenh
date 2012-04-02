#ifndef __USERPROG__
#define __USERPROG__

typedef struct atmosphere {
	sem_t water;
	sem_t oxygen;
	sem_t hydrogen;
	sem_t sulfate;
	sem_t sulfer_dioxide;
	sem_t sulfric_acid;
	int original_water_num;
	int original_sulfate_num;
} atmosphere;

#define FILENAME_TO_RUN "q5.dlx.obj"
#define RX1 0
#define RX2 1
#define RX3 2
#define WATER_RX 3
#define SULFATE_RX 4
#endif
