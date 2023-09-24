#include "spi_flash.h"  


static __IO uint32_t  SPITimeout =  SPIT_LONG_TIMEOUT;  
static uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


static void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 SPI 外设的时钟
	SPI_FLASH_APBxClk_Fun(SPI_FLASH_CLK, ENABLE);
	
	// 打开 SPI GPIO 的时钟
	SPI_FLASH_GPIO_APBxClk_Fun(SPI_FLASH_SCK_CLK, ENABLE);
	SPI_FLASH_GPIO_APBxClk_Fun(SPI_FLASH_CS_CLK, ENABLE);
	
	// SPI CS引脚采用软件控制，所以设置成推挽输出
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_FLASH_CS_GPIO_PORT, &GPIO_InitStructure);
	
	// 将IIC SCK的GPIO配置为推挽复用输出模式
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SCK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_FLASH_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	// 将SPI MOSI的GPIO配置为推挽复用输出模式
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_MOSI_GPIO_PIN;
	GPIO_Init(SPI_FLASH_MOSI_GPIO_PORT, &GPIO_InitStructure);
	
	// 将SPI MISO的GPIO配置为浮空上拉输入模式
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_MISO_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_FLASH_MISO_GPIO_PORT, &GPIO_InitStructure);
	

	SPI_FLASH_CS_HIGH();
	

}

static void SPI_Mode_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	

//配置IIC 的工作参数
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	
	SPI_InitStructure.SPI_CRCPolynomial = 0; //不使用CRC校验
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线全双工	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB ;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI_FLASH, &SPI_InitStructure);//写入配置到寄存器
	
	SPI_Cmd(SPI_FLASH, ENABLE);//使能SPI
	

}

void SPI_FLASH_Init(void)
{
	SPI_GPIO_Config();
	SPI_Mode_Config();

}



//写单个字节(全双工读写是同步进行的)
uint8_t SPI_FLASH_WriteByte(uint8_t data)
{
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	
	//检查数据发送寄存器是否为空
	while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
		
	}
	
	//发送数据
	SPI_I2S_SendData(SPI_FLASH, data);
	
	
	SPITimeout =  SPIT_FLAG_TIMEOUT;
	
	//检查数据接收寄存器是否为空
	while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
		
	}
	
	return SPI_I2S_ReceiveData(SPI_FLASH);				 //接收数据
	

}



//读单个字节
uint8_t SPI_FLASH_ReadByte(void)
{
	return SPI_FLASH_WriteByte(DUMMY);
}




//读取Flash ID号
uint32_t SPI_READ_ID(void)
{
	uint32_t flash_id;
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_WriteByte(JEDEC_ID);
	
	flash_id = SPI_FLASH_WriteByte(DUMMY);	
	flash_id <<= 8;	
	flash_id |= SPI_FLASH_WriteByte(DUMMY);
	flash_id <<= 8;
	flash_id |= SPI_FLASH_WriteByte(DUMMY);
	
	SPI_FLASH_CS_HIGH();
	
	return flash_id;

}

void SPI_FLASH_WriteEnable(void)
{
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_WriteByte(WriteEnable);
	
	SPI_FLASH_CS_HIGH();
}



//等待写入结束
void SPI_FLASH_WaitForWriteed(void)
{
	uint8_t status_reg = 0;
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_WriteByte(ReadStatus);
	
	do
	{
		status_reg = SPI_FLASH_WriteByte(DUMMY);
	}
	while((status_reg & WriteStatus) == 1);
	
	SPI_FLASH_CS_HIGH();

}



//擦除flash指定扇区
void SPI_FLASH_SectorErase(uint32_t addr)
{
	
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteed();
	
	SPI_FLASH_CS_LOW();
	
	//发送擦除指令
	SPI_FLASH_WriteByte(SectorErase);
	
	//地址分三次发送，从8位到低8位
	SPI_FLASH_WriteByte((addr>>16) & 0xff);
	
	SPI_FLASH_WriteByte((addr>>8) & 0xff);
	
	SPI_FLASH_WriteByte(addr & 0xff);

	SPI_FLASH_CS_HIGH();

}


//读取flash存储内容
void SPI_FLASH_ReadData(uint32_t addr, uint8_t *readBuff, uint32_t numToReadData)
{
	SPI_FLASH_CS_LOW();
	
	//发送读取数据指令
	SPI_FLASH_WriteByte(ReadData);
	
	//地址分三次发送，从8位到低8位
	SPI_FLASH_WriteByte((addr>>16) & 0xff);

	SPI_FLASH_WriteByte((addr>>8) & 0xff);

	SPI_FLASH_WriteByte(addr & 0xff);
	
	while(numToReadData--)
	{
		*readBuff = SPI_FLASH_WriteByte(DUMMY);
		readBuff++;
	}
	
	SPI_FLASH_CS_HIGH();
	
}



//向flash写入内容，页写入数据大小不能超过256
void SPI_FLASH_WriteData(uint32_t addr, uint8_t *writeBuff, uint32_t numToWriteData)
{
	//开启写入使能
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteed();
	
	SPI_FLASH_CS_LOW();
	
	//发送页写入指令
	SPI_FLASH_WriteByte(PageProgram);
	
	//地址分三次发送，从8位到低8位
	SPI_FLASH_WriteByte((addr>>16) & 0xff);

	SPI_FLASH_WriteByte((addr>>8) & 0xff);

	SPI_FLASH_WriteByte(addr & 0xff);

	
	while(numToWriteData--)
	{
		SPI_FLASH_WriteByte(*writeBuff);
		writeBuff++;
	}
	
	SPI_FLASH_CS_HIGH();

}




/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
  * @retval 返回0，表示SP读取失败.
  */
static uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  FLASH_ERROR("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
}

