#include "spi_flash.h"  


static __IO uint32_t  SPITimeout =  SPIT_LONG_TIMEOUT;  
static uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);


static void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� SPI �����ʱ��
	SPI_FLASH_APBxClk_Fun(SPI_FLASH_CLK, ENABLE);
	
	// �� SPI GPIO ��ʱ��
	SPI_FLASH_GPIO_APBxClk_Fun(SPI_FLASH_SCK_CLK, ENABLE);
	SPI_FLASH_GPIO_APBxClk_Fun(SPI_FLASH_CS_CLK, ENABLE);
	
	// SPI CS���Ų���������ƣ��������ó��������
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_FLASH_CS_GPIO_PORT, &GPIO_InitStructure);
	
	// ��IIC SCK��GPIO����Ϊ���츴�����ģʽ
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SCK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_FLASH_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	// ��SPI MOSI��GPIO����Ϊ���츴�����ģʽ
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_MOSI_GPIO_PIN;
	GPIO_Init(SPI_FLASH_MOSI_GPIO_PORT, &GPIO_InitStructure);
	
	// ��SPI MISO��GPIO����Ϊ������������ģʽ
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_MISO_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_FLASH_MISO_GPIO_PORT, &GPIO_InitStructure);
	

	SPI_FLASH_CS_HIGH();
	

}

static void SPI_Mode_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	

//����IIC �Ĺ�������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	
	SPI_InitStructure.SPI_CRCPolynomial = 0; //��ʹ��CRCУ��
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//˫��ȫ˫��	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB ;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI_FLASH, &SPI_InitStructure);//д�����õ��Ĵ���
	
	SPI_Cmd(SPI_FLASH, ENABLE);//ʹ��SPI
	

}

void SPI_FLASH_Init(void)
{
	SPI_GPIO_Config();
	SPI_Mode_Config();

}



//д�����ֽ�(ȫ˫����д��ͬ�����е�)
uint8_t SPI_FLASH_WriteByte(uint8_t data)
{
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	
	//������ݷ��ͼĴ����Ƿ�Ϊ��
	while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
		
	}
	
	//��������
	SPI_I2S_SendData(SPI_FLASH, data);
	
	
	SPITimeout =  SPIT_FLAG_TIMEOUT;
	
	//������ݽ��ռĴ����Ƿ�Ϊ��
	while(SPI_I2S_GetFlagStatus(SPI_FLASH, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
		
	}
	
	return SPI_I2S_ReceiveData(SPI_FLASH);				 //��������
	

}



//�������ֽ�
uint8_t SPI_FLASH_ReadByte(void)
{
	return SPI_FLASH_WriteByte(DUMMY);
}




//��ȡFlash ID��
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



//�ȴ�д�����
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



//����flashָ������
void SPI_FLASH_SectorErase(uint32_t addr)
{
	
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteed();
	
	SPI_FLASH_CS_LOW();
	
	//���Ͳ���ָ��
	SPI_FLASH_WriteByte(SectorErase);
	
	//��ַ�����η��ͣ���8λ����8λ
	SPI_FLASH_WriteByte((addr>>16) & 0xff);
	
	SPI_FLASH_WriteByte((addr>>8) & 0xff);
	
	SPI_FLASH_WriteByte(addr & 0xff);

	SPI_FLASH_CS_HIGH();

}


//��ȡflash�洢����
void SPI_FLASH_ReadData(uint32_t addr, uint8_t *readBuff, uint32_t numToReadData)
{
	SPI_FLASH_CS_LOW();
	
	//���Ͷ�ȡ����ָ��
	SPI_FLASH_WriteByte(ReadData);
	
	//��ַ�����η��ͣ���8λ����8λ
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



//��flashд�����ݣ�ҳд�����ݴ�С���ܳ���256
void SPI_FLASH_WriteData(uint32_t addr, uint8_t *writeBuff, uint32_t numToWriteData)
{
	//����д��ʹ��
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteed();
	
	SPI_FLASH_CS_LOW();
	
	//����ҳд��ָ��
	SPI_FLASH_WriteByte(PageProgram);
	
	//��ַ�����η��ͣ���8λ����8λ
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
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾSP��ȡʧ��.
  */
static uint32_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  FLASH_ERROR("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
}

