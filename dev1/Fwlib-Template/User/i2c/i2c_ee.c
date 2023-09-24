#include "i2c_ee.h"  





void I2C_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
// 打开 IIC GPIO 的时钟
	I2C_EE_GPIO_APBxClkCmd(I2C_EE_SCL_GPIO_CLK | I2C_EE_SDA_GPIO_CLK, ENABLE);
	
// 打开 IIC 外设的时钟
	I2C_EE_APBxClkCmd(I2C_EE_CLK, ENABLE);
	
// 将IIC SCL的GPIO配置为开漏复用模式
	GPIO_InitStructure.GPIO_Pin = I2C_EE_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_EE_SCL_GPIO_PORT, &GPIO_InitStructure);
	
//将IIC SDA的GPIO配置为开漏复用模式
	GPIO_InitStructure.GPIO_Pin = I2C_EE_SDA_GPIO_PIN;
	GPIO_Init(I2C_EE_SDA_GPIO_PORT, &GPIO_InitStructure);
	
//配置IIC 的工作参数
//使能应答
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ; 
//选择7位地址模式
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit ;
//配置SCL时钟频率
	I2C_InitStructure.I2C_ClockSpeed = I2C_EE_BAUDRATE ; 
//
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2 ;
//配置IIC运行模式
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ;
//配置IIC自身地址，只要是总线上唯一地址即可
	I2C_InitStructure.I2C_OwnAddress1 = I2C_EE_OWN_ADDRESS7 ;
	
	I2C_Init(I2C_EE, &I2C_InitStructure);
	
//使能串口
	I2C_Cmd (I2C_EE, ENABLE);

}



/* 向EEPROM写入一个字节 */
void IIC_EE_WriteByte(uint8_t addr, uint8_t data)
{
	//产生起始信号
	I2C_GenerateSTART(I2C_EE, ENABLE);
	
	//检测EV5事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5事件被检测到，发送从机设备地址
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
		
	//检测EV6事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	//EV6事件被检测到，发送要操作的存储单元地址
	I2C_SendData(I2C_EE, addr);
	
	//检测EV8事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	//EV8事件被检测到，发送要存储数据
	I2C_SendData(I2C_EE, data);
	
	//检测EV8_2事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	
	
	//数据发送完成，产生停止信号
	I2C_GenerateSTOP (I2C_EE, ENABLE);
	
}


/* 向EEPROM写入多个字节（页写入），每次不超过八个字节 */
void IIC_EE_WritePage(uint8_t addr, uint8_t *data, uint8_t numByteToWrite)
{
	//产生起始信号
	I2C_GenerateSTART(I2C_EE, ENABLE);
	
	//检测EV5事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5事件被检测到，发送从机设备地址
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
		
	//检测EV6事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	//EV6事件被检测到，发送要操作的存储单元地址
	I2C_SendData(I2C_EE, addr);
	
	//检测EV8事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	
	while(numByteToWrite)
	{
		//EV8事件被检测到，发送要存储数据
		I2C_SendData(I2C_EE, *data);
		
		//检测EV8_2事件
		while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
		
		data++;
		numByteToWrite--;
		
	}
	
	
	//数据发送完成，产生停止信号
	I2C_GenerateSTOP (I2C_EE, ENABLE);
	
}


/* 向EEPROM读取数据 */
void IIC_EE_Read(uint8_t addr, uint8_t *data, uint8_t numByteToRead)
{
	//产生起始信号
	I2C_GenerateSTART(I2C_EE, ENABLE);
	
	//检测EV5事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5事件被检测到，发送设备地址
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
		
	//检测EV6事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	//EV6事件被检测到，发送要操作的存储单元地址
	I2C_SendData(I2C_EE, addr);

	//检测EV8事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	
	
	//产生第二次起始信号
	I2C_GenerateSTART (I2C_EE, ENABLE);
	
	//检测EV5事件
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5事件被检测到，发送设备地址（接收模式）
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Receiver);
	
	//开始读取数据
	while(numByteToRead)
	{
		if (numByteToRead == 1)
		{
			//如果为最后一个字节，关闭使能应答
			I2C_AcknowledgeConfig (I2C_EE, DISABLE);
			
		}
		
		//检测EV7事件
		while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
		
		//EV7事件被检测到，发送要存储数据
		*data = I2C_ReceiveData(I2C_EE);
		
		data++;
		numByteToRead--;
		
	}
		
	//数据发送完成，产生停止信号
	I2C_GenerateSTOP(I2C_EE, ENABLE);
	
	//重新开启ACK使能
	I2C_AcknowledgeConfig(I2C_EE, ENABLE);
	
	
}


void IIC_EE_WaitForWritting(void)  /* 等待设备写完数据 */
{
	
	do
	{		
		//产生起始信号
		I2C_GenerateSTART(I2C_EE, ENABLE);
		
		//检测起始信号是否发送成功
		while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_SB) == RESET);
		
		//起始信号被检测到，发送从机设备地址
		I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
			
	 //检测地址是否发送成功
	}while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_ADDR) == RESET);

	//数据写入完成，产生停止信号
	I2C_GenerateSTOP(I2C_EE, ENABLE);
	
}






/* 发送两个字节 */
