#include "can.h"  




static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	// 打开 CAN 外设的时钟
//	CAN_APBxClk_Fun(CAN_CLK, ENABLE);
//	
//	// 打开 CAN GPIO 的时钟
//	CAN_GPIO_APBxClk_Fun(CAN_GPIO_CLK, ENABLE);
//	
//	//复用引脚 PB8, PB9第二功能
//	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
//	// CAN TX引脚采用软件控制，所以设置成推挽输出
//	GPIO_InitStructure.GPIO_Pin = CAN_TX_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
//	
//	// 将CAN RX的GPIO配置为上拉输入模式
//	GPIO_InitStructure.GPIO_Pin = CAN_RX_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);	
//	
	/*外设时钟设置*/
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIOA, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	  
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA,&GPIO_InitStructure);	

}

static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef CAN_InitStructure;
	

//配置IIC 的工作参数
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = ENABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;//调试时建议设置成回环面模式，调试完改回NORMAL
	CAN_InitStructure.CAN_NART = ENABLE;//错误自动重传
	CAN_InitStructure.CAN_RFLM = ENABLE;//锁定FIFO，FIFO满了将不接收新数据
	CAN_InitStructure.CAN_TTCM = DISABLE;//时间触发功能
	CAN_InitStructure.CAN_TXFP = DISABLE;//按照报文ID优先级发送
	//配置成1Mbps通讯速率
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 4;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;//
	
	CAN_Init(CAN, &CAN_InitStructure);//写入配置到寄存器

}

void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;//是否使能过滤器
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0 ;//过滤之后存在哪个FIFO
	CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStruct.CAN_FilterNumber = 0;
	CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
	
	CAN_FilterInitStruct.CAN_FilterIdHigh = (PASS_ID<<3 | CAN_Id_Extended | CAN_RTR_Data)&0xffff0000>>16;
	CAN_FilterInitStruct.CAN_FilterIdLow = (PASS_ID<<3 | CAN_Id_Extended | CAN_RTR_Data)&0xffff;
	
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0xffff;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0xffff;
	
	CAN_FilterInit(&CAN_FilterInitStruct); 
	
	//配置CAN接受邮箱0产生的中断
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);

}

void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置NVIC为优先级组1 */
	/* 提示 NVIC_PriorityGroupConfig() 在整个工程只需要调用一次来配置优先级分组*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源：CAN1 */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  /* 配置抢占优先级 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

void CAN_Config(void)
{
	CAN_GPIO_Config();
	CAN_Mode_Config();
	CAN_Filter_Config();
	CAN_NVIC_Config();

}







/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
  * @retval 返回0，表示SP读取失败.
  */
static uint32_t CAN_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  CAN_ERROR("CAN 等待超时!errorCode = %d",errorCode);
  return 0;
}

