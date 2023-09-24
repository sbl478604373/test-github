#include "stm32f10x.h"	//�൱��51��Ƭ���е� #include <reg52.h>
#include "./usart/usart.h"
#include "./i2c/i2c_ee.h"
#include "./spi/spi_flash.h"
#include "./can/can.h"
#include "./key/bsp_key.h" 
#include "./adc/adc.h"


uint8_t dataRead[10] = {0};
uint8_t dataWrite[8] = {1,2,3,4,5,6,7,8};
uint8_t readBuff[4096];
uint8_t writeBuff[256];

CanRxMsg CAN_Receive_Data;
CanTxMsg CAN_Transmit_Data;
uint8_t flag;

extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_ConvertedValueLocal;        

// �����ʱ
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

//1. ��ʼ��IIC��ص�GPIO����
//2. ����IIC����Ĺ���ģʽ
//3. ��дIICд��EEPROM��Byte write����
//4. ��дIIC��ȡEEPROM��RANDOM read����
//5. ʹ��read������write�������ж�дУ��
//6. ��дpage write���� �� seq read������У��


int main(void)
{
	
	//uint8_t a[10] = {1,2,3,4,5,6,7,8,9,10};
//	uint32_t i;
//	uint32_t id;
	
	
	//���������ʱ��ϵͳʱ���Ѿ������ó�72M
	//��ʼ������
	USART_Config();
	
	//Usart_SendByte(DEBUG_USARTx, 123);	
	//Usart_SendHalfWord(DEBUG_USARTx, 0xff56);	
	//Usart_SendArray(DEBUG_USARTx, a, 10);
	
	
	
	//IIC��д����
	/*
	printf("����һ��IICͨѶʵ��\r\n");	
	//����IIC
	I2C_Config();	
	//���ַ11д��һ���ֽ�
	IIC_EE_WriteByte(11, 0x55);	
	//�ȴ�EEPROMд������
	IIC_EE_WaitForWritting();	
	IIC_EE_WriteByte(12, 0x52);
	IIC_EE_WaitForWritting();	
	//ҳд��,addr%8 == 0,��Ϊ��ַ����
	IIC_EE_WritePage(0, dataWrite, 8);
	IIC_EE_WaitForWritting();	
	//��ȡ����
	IIC_EE_Read(0, dataRead, 8);	
	for(i=0; i<8; i++)
	{	
		printf("%d ", dataRead[i]);
	}
	//printf("���յ���������0x%x��0x%x\r\n", dataRead[0], dataRead[1] );
	*/
	
	
	//SPI��д����
	/*
	printf("����һ��SPIͨѶʵ��\r\n");
	
	//��ʼ��SPI
	SPI_FLASH_Init();
	
	id = SPI_READ_ID();
	
	printf("ID = 0x%x\r\n", id);
	
	SPI_FLASH_SectorErase(0);
	SPI_FLASH_WaitForWriteed();

	for(i=0; i<256; i++)
	{
		writeBuff[i] = i;
	}	
	SPI_FLASH_WriteData(0, writeBuff, 256);
	SPI_FLASH_WaitForWriteed();
	
	
	SPI_FLASH_ReadData(0, readBuff, 1024);
	for(i=0; i<1024; i++)
	{
		printf("0x%x ", readBuff[i]);

	}
	*/
	CAN_Config();
	
	Key_GPIO_Config();
	
	printf("����һ��CANͨѶʵ��\r\n");
	printf("��KEY1��������\r\n");
	
	
	ADC_Init_Config();
	printf("����һ��ADC����ʵ��\r\n");
	
	
	while(1)
	{
//		uint8_t box;
//		
//		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
//		{
//			CAN_Transmit_Data.StdId = 0;
//			CAN_Transmit_Data.ExtId = PASS_ID;
//			CAN_Transmit_Data.RTR = CAN_RTR_Data;
//			CAN_Transmit_Data.IDE = CAN_Id_Extended ;
//			CAN_Transmit_Data.DLC = 1;
//			CAN_Transmit_Data.Data[0] = 10;
//			
//			box = CAN_Transmit(CAN, &CAN_Transmit_Data);
//			while(CAN_TransmitStatus(CAN, box) == CAN_TxStatus_Failed);
//		
//			printf("\r\n ���ݰ�������� \r\n");
//		}
//		
//		
//		
//		if(flag == 1)
//		{
//			printf("���յ������ݣ�%d \r\n", CAN_Receive_Data.Data[0]);
//			flag = 0;
//		}
	
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; 
	
		printf("\r\n The current AD value = 0x%04X \r\n", 
		       ADC_ConvertedValue); 
		printf("\r\n The current AD value = %f V \r\n",
		       ADC_ConvertedValueLocal); 
		printf("\r\n\r\n");

		Delay(0xffffee); 
	
	}
	
}



void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN, CAN_FIFO0, &CAN_Receive_Data);
	flag = 1;
	
}

