#include "stm32f10x.h"	//相当于51单片机中的 #include <reg52.h>
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

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal;        

// 软件延时
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

//1. 初始化IIC相关的GPIO外设
//2. 配置IIC外设的工作模式
//3. 编写IIC写入EEPROM的Byte write函数
//4. 编写IIC读取EEPROM的RANDOM read函数
//5. 使用read函数和write函数进行读写校验
//6. 编写page write函数 和 seq read函数并校验


int main(void)
{
	
	//uint8_t a[10] = {1,2,3,4,5,6,7,8,9,10};
//	uint32_t i;
//	uint32_t id;
	
	
	//来到这里的时候，系统时钟已经被配置成72M
	//初始化串口
	USART_Config();
	
	//Usart_SendByte(DEBUG_USARTx, 123);	
	//Usart_SendHalfWord(DEBUG_USARTx, 0xff56);	
	//Usart_SendArray(DEBUG_USARTx, a, 10);
	
	
	
	//IIC读写测试
	/*
	printf("这是一个IIC通讯实验\r\n");	
	//配置IIC
	I2C_Config();	
	//向地址11写入一个字节
	IIC_EE_WriteByte(11, 0x55);	
	//等待EEPROM写入数据
	IIC_EE_WaitForWritting();	
	IIC_EE_WriteByte(12, 0x52);
	IIC_EE_WaitForWritting();	
	//页写入,addr%8 == 0,即为地址对齐
	IIC_EE_WritePage(0, dataWrite, 8);
	IIC_EE_WaitForWritting();	
	//读取数据
	IIC_EE_Read(0, dataRead, 8);	
	for(i=0; i<8; i++)
	{	
		printf("%d ", dataRead[i]);
	}
	//printf("接收到的数据是0x%x，0x%x\r\n", dataRead[0], dataRead[1] );
	*/
	
	
	//SPI读写测试
	/*
	printf("这是一个SPI通讯实验\r\n");
	
	//初始化SPI
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
	
	printf("这是一个CAN通讯实验\r\n");
	printf("按KEY1发送数据\r\n");
	
	
	ADC_Init_Config();
	printf("这是一个ADC测试实验\r\n");
	
	
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
//			printf("\r\n 数据包发送完成 \r\n");
//		}
//		
//		
//		
//		if(flag == 1)
//		{
//			printf("接收到的数据：%d \r\n", CAN_Receive_Data.Data[0]);
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

