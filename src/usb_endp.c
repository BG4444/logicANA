#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include <stm32f3_discovery.h>
#include <core_cmInstr.h>
#include "ringBuffer.h"
#include "assert.h"
#include "mem.h"


/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             (1	)

extern __IO uint32_t overflow;

bool sending=0;

int32_t send_count=0;

extern ringBuffer sendBuf;

void fill(block* start,int i)
{
	if(i<VIRTUAL_COM_PORT_DATA_SIZE-2)
	{
		start->data[VIRTUAL_COM_PORT_DATA_SIZE-2]='\r';
		start->data[VIRTUAL_COM_PORT_DATA_SIZE-1]='\n';
		for(;i<VIRTUAL_COM_PORT_DATA_SIZE-2;i++)
		{
			start->data[i]='Z';
		}
	}
}

bool uartSend(const char* bytes,uint8_t len)
{
	int i=0;
	block* sb=getCurrent(&sendBuf);
	for(;i<len;i++)
	{
		sb->data[i]=bytes[i];
	}
	fill(sb,i);
	commitCurrent(&sendBuf);
	return(1);
}

bool uartSendString(const char* bytes)
{
	int i=0;
	block* sb=getCurrent(&sendBuf);
	for(;bytes[i];i++)
	{
		sb->data[i]=bytes[i];
	}
	fill(sb,i);
	commitCurrent(&sendBuf);
	return(1);
}

extern volatile ringBuffer rb;

volatile int32_t count=0;


void SOF_Callback(void)
{
  //  static uint32_t FrameCount = 0;
		if(bDeviceState == CONFIGURED)
    {
      //  if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
    	if(!sending)
        {
            /* Reset the frame counter */
           // FrameCount = 0;

            /* Check the data to be sent through IN pipe */
    		sending=1;

    		int32_t send_count=getReadyCount(&sendBuf);

            if(send_count)
            {
            	block* current=consume(&sendBuf);
            	UserToPMABufferCopy((unsigned char*)current->data, ENDP1_TXADDR, VIRTUAL_COM_PORT_DATA_SIZE);
				SetEPTxCount(ENDP1, VIRTUAL_COM_PORT_DATA_SIZE);
				SetEPTxValid(ENDP1);
				STM_EVAL_LEDOff(LED4);
				send_count--;
            }
            else
            {
            	if(count)
				{
            		block* current=consume(&rb);
					UserToPMABufferCopy((unsigned char*)current->data, ENDP1_TXADDR, VIRTUAL_COM_PORT_DATA_SIZE);
					SetEPTxCount(ENDP1, VIRTUAL_COM_PORT_DATA_SIZE);
					SetEPTxValid(ENDP1);
					STM_EVAL_LEDOff(LED4);
					count--;
				}
				else
				{
					SetEPTxCount(ENDP1, 0);
					SetEPTxValid(ENDP1);
				}
            }
        }
    }
}


void EP1_IN_Callback (void)
{
	if(sending)
	{
		sending=0;
		STM_EVAL_LEDOff(LED5);
	}
}
static uint8_t Receive_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
void EP3_OUT_Callback(void)
{
  PMAToUserBufferCopy(Receive_Buffer, GetEPRxAddr(EP3_OUT & 0x7F), 1);
  {

	  {
		  STM_EVAL_LEDOn(LED3);
		  switch(Receive_Buffer[0])
  			{
  				case 'g':
  				{
  					if(!count)
  					{
						count=getReadyCount(&rb);
						uartSend(&count,4);
						STM_EVAL_LEDOn(LED4);
						STM_EVAL_LEDOn(LED5);
  					}

  					break;
  				}
  				case 't':
				{
					STM_EVAL_LEDOn(LED5);
					const uint32_t count=1;
					ASSERT(uartSend(&count,4));
					for(int i=0;i<count;i++)
					{
						uartSend(&count,4);
					}
					break;
				}
  				case 'r':
  				{
  					{
						ASSERT(uartSendString("RESET\r\n"));
						resetRecv();
  					}
  				    break;
  				}
  				default:
  				{
  					uartSend(Receive_Buffer,1);
  					break;
  				}
  			}
		  STM_EVAL_LEDOff(LED3);
	  }
  }
  SetEPRxValid(ENDP3);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
