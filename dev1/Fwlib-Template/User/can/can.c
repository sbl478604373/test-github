#include "can.h"  




static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	// �� CAN �����ʱ��
//	CAN_APBxClk_Fun(CAN_CLK, ENABLE);
//	
//	// �� CAN GPIO ��ʱ��
//	CAN_GPIO_APBxClk_Fun(CAN_GPIO_CLK, ENABLE);
//	
//	//�������� PB8, PB9�ڶ�����
//	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
//	// CAN TX���Ų���������ƣ��������ó��������
//	GPIO_InitStructure.GPIO_Pin = CAN_TX_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);
//	
//	// ��CAN RX��GPIO����Ϊ��������ģʽ
//	GPIO_InitStructure.GPIO_Pin = CAN_RX_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);	
//	
	/*����ʱ������*/
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
	

//����IIC �Ĺ�������
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = ENABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;//����ʱ�������óɻػ���ģʽ��������Ļ�NORMAL
	CAN_InitStructure.CAN_NART = ENABLE;//�����Զ��ش�
	CAN_InitStructure.CAN_RFLM = ENABLE;//����FIFO��FIFO���˽�������������
	CAN_InitStructure.CAN_TTCM = DISABLE;//ʱ�䴥������
	CAN_InitStructure.CAN_TXFP = DISABLE;//���ձ���ID���ȼ�����
	//���ó�1MbpsͨѶ����
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 4;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;//
	
	CAN_Init(CAN, &CAN_InitStructure);//д�����õ��Ĵ���

}

void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;//�Ƿ�ʹ�ܹ�����
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0 ;//����֮������ĸ�FIFO
	CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStruct.CAN_FilterNumber = 0;
	CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
	
	CAN_FilterInitStruct.CAN_FilterIdHigh = (PASS_ID<<3 | CAN_Id_Extended | CAN_RTR_Data)&0xffff0000>>16;
	CAN_FilterInitStruct.CAN_FilterIdLow = (PASS_ID<<3 | CAN_Id_Extended | CAN_RTR_Data)&0xffff;
	
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0xffff;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0xffff;
	
	CAN_FilterInit(&CAN_FilterInitStruct); 
	
	//����CAN��������0�������ж�
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);

}

void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����NVICΪ���ȼ���1 */
	/* ��ʾ NVIC_PriorityGroupConfig() ����������ֻ��Ҫ����һ�����������ȼ�����*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ��CAN1 */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  /* ������ռ���ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж�ͨ�� */
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
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾSP��ȡʧ��.
  */
static uint32_t CAN_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  CAN_ERROR("CAN �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
}

