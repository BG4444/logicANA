#include "ringBuffer.h"
void initRingBuffer(ringBuffer* rb,uint32_t size,block* first)
{
	rb->size=size;
	rb->head=0;
	rb->tail=0;
	rb->first=first;
}

block* consume(ringBuffer* rb)
{
	if(rb->head<rb->tail)
	{
		block* ret=&rb->first[rb->head];
		rb->head=(rb->head+1)%(rb->size);
		return(ret);
	}
	return(0);
}

block* getCurrent(ringBuffer* rb)
{
	return(&rb->first[rb->tail%rb->size]);
}

void commitCurrent(ringBuffer* rb)
{
	if( ( (rb->tail-rb->head) % rb->size) <(rb->size-1))
	{
		rb->tail=(rb->tail+1)%rb->size;
	}
}
