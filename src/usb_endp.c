#include "../inc/deque.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "mem.h"
#include <stm32f3_discovery.h>


/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             (1	)


extern __IO uint32_t overflow;

extern deque dataBuf;
extern deque sendBuf;
extern __IO bool lock_data=0;


bool sending=0;

bool uartSend(const char* bytes,uint8_t len)
{
	dequeNode* current=dequeAdd(&sendBuf);
	if(!current)
	{
		return(0);
	}
	int i=0;
	for(;i<len;i++)
	{
		current->data[i]=bytes[i];
	}
	if(i<VIRTUAL_COM_PORT_DATA_SIZE-3)
		{
			current->data[VIRTUAL_COM_PORT_DATA_SIZE-3]='\r';
			current->data[VIRTUAL_COM_PORT_DATA_SIZE-2]='\n';
			for(;i<VIRTUAL_COM_PORT_DATA_SIZE-3;i++)
			{
				current->data[i]='Z';
			}
		}
	return(1);
}

bool uartSendString(const char* bytes)
{
	dequeNode* current=dequeAdd(&sendBuf);
	if(!current)
	{
		return(0);
	}
	int i=0;
	for(;bytes[i];i++)
	{
		current->data[i]=bytes[i];
	}
	if(i<VIRTUAL_COM_PORT_DATA_SIZE-3)
	{
		current->data[VIRTUAL_COM_PORT_DATA_SIZE-3]='\r';
		current->data[VIRTUAL_COM_PORT_DATA_SIZE-2]='\n';
		for(;i<VIRTUAL_COM_PORT_DATA_SIZE-3;i++)
		{
			current->data[i]='Z';
		}
	}
	return(1);
}


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
            dequeNode* current=dequeGet(&sendBuf);
            const dequeNode* tcurrent=current;
            if(current)
            {

            	UserToPMABufferCopy((unsigned char*)current->data, ENDP1_TXADDR, VIRTUAL_COM_PORT_DATA_SIZE);
            	freeNode(current);
				SetEPTxCount(ENDP1, VIRTUAL_COM_PORT_DATA_SIZE);
				SetEPTxValid(ENDP1);
				STM_EVAL_LEDOff(LED4);
            }
            else
            {
            	SetEPTxCount(ENDP1, 0);
            	SetEPTxValid(ENDP1);

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

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
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
  					STM_EVAL_LEDOn(LED4);
  					STM_EVAL_LEDOn(LED5);
  					ASSERT(dataBuf.count>=1);
  					const uint32_t count=dataBuf.count-1;
  					ASSERT(uartSend(&count,4));
  					ASSERT(sendBuf.count>=1);
  					if(count)
  					{
  						lock_data=1;
  						sendBuf.tail->next=dataBuf.head;
  						sendBuf.tail=dataBuf.preTail;
  						sendBuf.tail->next=0;
  						sendBuf.count+=count;

  						dataBuf.head=dataBuf.tail;
  						dataBuf.preTail=0;
  						dataBuf.count=1;
  						lock_data=0;
  					}

  					break;
  				}
  				case 's':
  				{
  					ASSERT(uartSend(allMem,4));
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
  					killDeq(&sendBuf);
  					ASSERT(uartSendString("RESET\r\n"));
  					resetRecv();
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
