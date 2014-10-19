#include "main.h"

#include "deque.h"
#include "mem.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f30x.h"
#include<stm32f30x_rcc.h>



__IO uint32_t overflow=0;


__IO uint32_t USBConnectTimeOut = 100;


__IO uint32_t tickcounter=0;

dequeNode** freeStackPtr;

dequeNode allMem[BUFSIZE];
dequeNode* freeBuf[BUFSIZE];
dequeNode* currentBuf;

volatile deque dataBuf;
volatile deque sendBuf;
volatile bool lock_data;

void resetRecv()
{
	killDeq(&dataBuf);
	currentBuf=dequeAdd(&dataBuf);
	tickcounter=0;
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

int main(void)
{
	initMem();
	newDeq(&sendBuf);
	newDeq(&dataBuf);
	currentBuf=dequeAdd(&dataBuf);
//	currentBuf=dequeAdd(&dataBuf);
	RCC_ClocksTypeDef RCC_Clocks;
	/* SysTick end of count event each 50us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency/10000);
	NVIC_SetPriority (SysTick_IRQn, 2);
	/* Configure the USB */


	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);

	STM_EVAL_LEDInit(LED9);
	STM_EVAL_LEDInit(LED10);


	STM_EVAL_LEDToggle(LED7);

	initUSB();

	SetEPRxValid(ENDP3);
	while ( 1 );
}


void TimingDelay_Decrement(void)
{
	STM_EVAL_LEDOn(LED9);
	const uint32_t recpos=tickcounter%31;

	uint32_t* pos=(uint32_t*)currentBuf->data;

	pos[recpos]=tickcounter;


    if(recpos==30)
    {
    	STM_EVAL_LEDToggle(LED6);
    	STM_EVAL_LEDToggle(LED7);
    	ASSERT(lock_data==0);
    	if(sendBuf.count+dataBuf.count<BUFSIZE-5)
    	{

			dequeNode* current=dequeAdd(&dataBuf);
			if(current)
			{
				currentBuf=current;
			}
    	}
    }
    tickcounter=(tickcounter+1)%31000;
    STM_EVAL_LEDOff(LED9);
}
