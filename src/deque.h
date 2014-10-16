#ifndef __DEQUE_H__
#define __DEQUE_H__

typedef struct _dequeNode
{
	uint8_t data[64];
	struct _dequeNode* next;
} dequeNode;


typedef struct _deque
{
	dequeNode* head;
	dequeNode* tail;
} deque;


#endif
