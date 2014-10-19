#include "deque.h"
#include "mem.h"

dequeNode* dequeAdd(volatile deque* deq)
{
	dequeNode* newTail=allocNode();
	if(newTail==0)
	{
		return(0);
	}
	newTail->next=0;

	if(deq->count)
	{
		deq->preTail=deq->tail;
		deq->tail->next=newTail;
	}
	else
	{
		deq->head=newTail;
	}

	deq->count++;
	deq->tail=newTail;
	return(newTail);
}

dequeNode* dequeGet(volatile deque* deq)
{
	if(deq->count)
	{
		dequeNode* ret=deq->head;
		deq->head=deq->head->next;
		deq->count--;
		if(deq->count<=1)
		{
			deq->preTail=0;
			if(deq->count==0)
			{
				ASSERT(deq->head==0);
			}
		}
		return(ret);
	}
	else
	{
		return(0);
	}

}

void newDeq(volatile deque* deq)
{
	deq->head=0;
	deq->tail=0;
	deq->preTail=0;
	deq->count=0;
}

void killDeq(volatile deque* deq)
{
	for(dequeNode* current=deq->head;current;current=current->next)
	{
		freeNode(current);
	}
	newDeq(deq);
}
