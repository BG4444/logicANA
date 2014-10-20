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


int32_t getReadyCount(ringBuffer* rb)
{
	int32_t count=(rb->tail-rb->head) % rb->size;
	if(count < 0)
	{
		count+=rb->size;
	}
	return(count);

}

void commitCurrent(ringBuffer* rb)
{
	int32_t count=getReadyCount(rb);
	if ( count <(rb->size-1))
	{
		rb->tail=(rb->tail+1)%rb->size;
		count++;
	}
	return;
}
