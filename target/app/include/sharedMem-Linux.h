#ifndef H_SHAREDMEM_LINUX
#define H_SHAREDMEM_LINUX
#include "../../pru-project/sharedMem.h"

extern volatile sharedMemStruct_t* globalSharedStruct;

void sharedMemInit(void);
void sharedMemCleanUp(void);


#endif
