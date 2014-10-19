#ifndef __MEM_H__
#define __MEM_H__

#include "deque.h"

#include <stm32f30x_it.h>

#define ASSERT(n) if(!(n)) {HardFault_Handler();}

#define BUFSIZE (256)

extern dequeNode allMem[BUFSIZE];

extern dequeNode* freeBuf[BUFSIZE];

extern dequeNode** freeStackPtr;

void initMem();
void freeNode(dequeNode* nodeToFree);
dequeNode* allocNode();

#endif
