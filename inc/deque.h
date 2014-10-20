#ifndef __DEQUE_H__
#define __DEQUE_H__

#include <inttypes.h>
#include <usb_desc.h>

typedef struct _dequeNode
{
	uint8_t data[VIRTUAL_COM_PORT_DATA_SIZE-1];
	struct _dequeNode* next;
} dequeNode;


typedef struct _deque
{
	dequeNode* head;
	dequeNode* tail;
} deque;


dequeNode* newDeq(volatile deque* deq);
void killDeq(volatile deque* deq);
void connect(volatile deque* deq,dequeNode* newTail);

#endif
