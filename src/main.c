#include "main.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "deque.h"

__IO deque deq;

#define bufSize (512)

static  dequeNode mem[bufSize];
static uint8_t markers[bufSize];

__IO uint32_t overflow=0;
__IO uint32_t usedMem;

__IO uint32_t bufferOut=0;

uint32_t recpos=0;

__IO uint32_t USBConnectTimeOut = 100;
__IO uint32_t UserButtonPressed = 0;
__IO uint8_t DataReady = 0;

__IO uint32_t tickcounter=0;
__IO uint8_t allowed=0;


extern __IO uint8_t Receive_Buffer[64];
extern __IO uint32_t Receive_length ;
uint8_t Send_Buffer[64];
__IO uint32_t packet_sent=1;
uint32_t packet_receive=1;


#define NEW(t) (t*)malloc(sizeof(t))

dequeNode* allocNode()
{
	int i;
	for(i=0;i<bufSize;i++)
	{
		if(!markers[i])
		{
			markers[i]=1;
			usedMem++;
			return(&mem[i]);
		}
	}
	overflow=1;
	return(0);
}

void freeNode(dequeNode* node)
{
	markers[(node-mem)>>6]=0;
	usedMem--;
}

void initUSB (void)
{
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();

	USB_Init();

	while ((bDeviceState != CONFIGURED)&&(USBConnectTimeOut != 0))
	{}
}

uint32_t LSM303DLHC_TIMEOUT_UserCallback()
{
	return(0);
}

void dequeAdd(dequeNode* ret)
{
	deq.tail->next=ret;
	deq.tail=ret;
}

dequeNode* dequeGet(deque* deq)
{
	dequeNode* ret=deq->head;
	deq->head=deq->head->next;
	return(ret);
}

void sendDeque(deque* deq)
{
	dequeNode* current;
	uint32_t size=0;
	for(current=deq->head;current!=deq->tail;current=current->next)
	{
		size++;
	}
	CDC_Send_DATA(&size,4);
	while(packet_sent!=1);
	if(size)
	{
		for(current=dequeGet(deq);current!=deq->tail;current=dequeGet(deq))
		{

			int i=0;
		//	for(i=0;i<64;i++)
			{
				CDC_Send_DATA(&current->data[i],64);
				while(packet_sent!=1);
			}
			freeNode(current);
		}
	}
}

void init()
{
	usedMem=0;
	overflow=0;
	deq.head=allocNode();
	deq.tail=deq.head;
	int i;
	for(i=0;i<bufSize;i++)
	{
		markers[i]=0;
	}

}

void sendString(const char* str)
{
	int i;
	for(i=0;str[i];i++);
	CDC_Send_DATA(str,i);
	while(!packet_sent);
}

int main(void)
{  
	init();
	RCC_ClocksTypeDef RCC_Clocks;
	/* SysTick end of count event each 50us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency/100);
	/* Configure the USB */
	initUSB();

	while ( 1 )
	{
		CDC_Receive_DATA();
		if(Receive_length)
		{
			switch(Receive_Buffer[0])
			{
				case 'g':
				{
					if(overflow)
					{
						uint32_t buf=0xFFFFFFFF;
						CDC_Send_DATA(&buf,4);
						while(!packet_sent);
					}
					else
					{
						deque outDeq=deq;
						sendDeque(&outDeq);
						deq.head=outDeq.head;
					}
					break;
				}
				case 'r':
				{
					init();
					sendString("\r\nRESET\r\n");
				    break;
				}
				default:
				{
					CDC_Send_DATA(Receive_Buffer,Receive_length);
					while(!packet_sent);
				}
			}
			Receive_length=0;
		}
	}
}


void TimingDelay_Decrement(void)
{
	const uint32_t recpos=tickcounter&15;

	uint32_t* pos=(uint32_t*)&deq.tail->data;

	pos[recpos]=tickcounter;

    if(recpos==15)
    {
    	dequeNode* current=allocNode();
    	if(current)
    	{
    		dequeAdd(current);
    	}
    }
    tickcounter++;
}
