#include "main.h"

#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f30x.h"
#include<stm32f30x_rcc.h>
#include "ringBuffer.h"


__IO uint32_t overflow=0;


__IO uint32_t USBConnectTimeOut = 100;


__IO uint32_t tickcounter=0;

volatile block allMem[100];

volatile ringBuffer rb;

void resetRecv()
{
	initRingBuffer(&rb,100,allMem);
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
    resetRecv();
	RCC_ClocksTypeDef RCC_Clocks;
	/* SysTick end of count event each 50us */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency/10000);
	NVIC_SetPriority (SysTick_IRQn, 0);
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

	uint32_t* pos=(uint32_t*)getCurrent(&rb);

	pos[recpos]=tickcounter;


    if(recpos==30)
    {
    	STM_EVAL_LEDToggle(LED6);
    	STM_EVAL_LEDToggle(LED7);
    	commitCurrent(&rb);

    }
    tickcounter=(tickcounter+1)%31000;
    STM_EVAL_LEDOff(LED9);

}
