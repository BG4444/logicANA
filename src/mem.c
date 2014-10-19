#include "mem.h"
#include "stm32f3_discovery.h"
void initMem()
{
	for(int i=0;i<BUFSIZE;i++)
	{
		freeBuf[i]=&allMem[i];
	}
	freeStackPtr=&freeBuf[BUFSIZE-1];
}

void freeNode(dequeNode* nodeToFree)
{
	STM_EVAL_LEDOn(LED8);
	freeStackPtr++;
	ASSERT((nodeToFree>=&allMem[0]) && (nodeToFree<=&allMem[BUFSIZE-1]))
	*freeStackPtr=nodeToFree;
}

dequeNode* allocNode()
{
	if((freeStackPtr+1)==&freeBuf[0])
	{
		STM_EVAL_LEDOff(LED8);
		return(0);
	}
	dequeNode* ret=*freeStackPtr;
	freeStackPtr--;
	return(ret);
}
