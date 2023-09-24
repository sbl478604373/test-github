#ifndef _CAN_H
#define _CAN_H

#include "stm32f10x.h" 

#define  PASS_ID                ((uint32_t)0X1314)

// CAN1
#define  CAN                    CAN1
#define  CAN_CLK                RCC_APB1Periph_CAN1
#define  CAN_APBxClk_Fun        RCC_APB1PeriphClockCmd  //ʹ��ʱ�ӵĺ���

// CAN1 GPIO ���ź궨��
#define  CAN_GPIO_CLK           RCC_APB2Periph_GPIOB
#define  CAN_GPIO_APBxClk_Fun   RCC_APB2PeriphClockCmd

#define  CAN_TX_GPIO_PORT       GPIOB
#define  CAN_TX_GPIO_PIN        GPIO_Pin_9

#define  CAN_RX_GPIO_PORT       GPIOB
#define  CAN_RX_GPIO_PIN        GPIO_Pin_8


/*��Ϣ���*/
#define CAN_DEBUG_ON           0

#define CAN_INFO(fmt,arg...)           printf("<<-CAN-INFO->> "fmt"\n",##arg)
#define CAN_ERROR(fmt,arg...)          printf("<<-CAN-ERROR->> "fmt"\n",##arg)
#define CAN_DEBUG(fmt,arg...)          do{\
                                          if(CAN_DEBUG_ON)\
                                          printf("<<-CAN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

void CAN_Config(void);																					
																							
																					
#endif /* _CAN_H */


