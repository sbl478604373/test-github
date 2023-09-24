#include "i2c_ee.h"  





void I2C_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
// �� IIC GPIO ��ʱ��
	I2C_EE_GPIO_APBxClkCmd(I2C_EE_SCL_GPIO_CLK | I2C_EE_SDA_GPIO_CLK, ENABLE);
	
// �� IIC �����ʱ��
	I2C_EE_APBxClkCmd(I2C_EE_CLK, ENABLE);
	
// ��IIC SCL��GPIO����Ϊ��©����ģʽ
	GPIO_InitStructure.GPIO_Pin = I2C_EE_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_EE_SCL_GPIO_PORT, &GPIO_InitStructure);
	
//��IIC SDA��GPIO����Ϊ��©����ģʽ
	GPIO_InitStructure.GPIO_Pin = I2C_EE_SDA_GPIO_PIN;
	GPIO_Init(I2C_EE_SDA_GPIO_PORT, &GPIO_InitStructure);
	
//����IIC �Ĺ�������
//ʹ��Ӧ��
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ; 
//ѡ��7λ��ַģʽ
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit ;
//����SCLʱ��Ƶ��
	I2C_InitStructure.I2C_ClockSpeed = I2C_EE_BAUDRATE ; 
//
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2 ;
//����IIC����ģʽ
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ;
//����IIC�����ַ��ֻҪ��������Ψһ��ַ����
	I2C_InitStructure.I2C_OwnAddress1 = I2C_EE_OWN_ADDRESS7 ;
	
	I2C_Init(I2C_EE, &I2C_InitStructure);
	
//ʹ�ܴ���
	I2C_Cmd (I2C_EE, ENABLE);

}



/* ��EEPROMд��һ���ֽ� */
void IIC_EE_WriteByte(uint8_t addr, uint8_t data)
{
	//������ʼ�ź�
	I2C_GenerateSTART(I2C_EE, ENABLE);
	
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5�¼�����⵽�����ʹӻ��豸��ַ
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
		
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	//EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ
	I2C_SendData(I2C_EE, addr);
	
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	//EV8�¼�����⵽������Ҫ�洢����
	I2C_SendData(I2C_EE, data);
	
	//���EV8_2�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	
	
	//���ݷ�����ɣ�����ֹͣ�ź�
	I2C_GenerateSTOP (I2C_EE, ENABLE);
	
}


/* ��EEPROMд�����ֽڣ�ҳд�룩��ÿ�β������˸��ֽ� */
void IIC_EE_WritePage(uint8_t addr, uint8_t *data, uint8_t numByteToWrite)
{
	//������ʼ�ź�
	I2C_GenerateSTART(I2C_EE, ENABLE);
	
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5�¼�����⵽�����ʹӻ��豸��ַ
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
		
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	//EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ
	I2C_SendData(I2C_EE, addr);
	
	//���EV8�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	
	while(numByteToWrite)
	{
		//EV8�¼�����⵽������Ҫ�洢����
		I2C_SendData(I2C_EE, *data);
		
		//���EV8_2�¼�
		while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
		
		data++;
		numByteToWrite--;
		
	}
	
	
	//���ݷ�����ɣ�����ֹͣ�ź�
	I2C_GenerateSTOP (I2C_EE, ENABLE);
	
}


/* ��EEPROM��ȡ���� */
void IIC_EE_Read(uint8_t addr, uint8_t *data, uint8_t numByteToRead)
{
	//������ʼ�ź�
	I2C_GenerateSTART(I2C_EE, ENABLE);
	
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5�¼�����⵽�������豸��ַ
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
		
	//���EV6�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	//EV6�¼�����⵽������Ҫ�����Ĵ洢��Ԫ��ַ
	I2C_SendData(I2C_EE, addr);

	//���EV8�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	
	
	//�����ڶ�����ʼ�ź�
	I2C_GenerateSTART (I2C_EE, ENABLE);
	
	//���EV5�¼�
	while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	//EV5�¼�����⵽�������豸��ַ������ģʽ��
	I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Receiver);
	
	//��ʼ��ȡ����
	while(numByteToRead)
	{
		if (numByteToRead == 1)
		{
			//���Ϊ���һ���ֽڣ��ر�ʹ��Ӧ��
			I2C_AcknowledgeConfig (I2C_EE, DISABLE);
			
		}
		
		//���EV7�¼�
		while(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
		
		//EV7�¼�����⵽������Ҫ�洢����
		*data = I2C_ReceiveData(I2C_EE);
		
		data++;
		numByteToRead--;
		
	}
		
	//���ݷ�����ɣ�����ֹͣ�ź�
	I2C_GenerateSTOP(I2C_EE, ENABLE);
	
	//���¿���ACKʹ��
	I2C_AcknowledgeConfig(I2C_EE, ENABLE);
	
	
}


void IIC_EE_WaitForWritting(void)  /* �ȴ��豸д������ */
{
	
	do
	{		
		//������ʼ�ź�
		I2C_GenerateSTART(I2C_EE, ENABLE);
		
		//�����ʼ�ź��Ƿ��ͳɹ�
		while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_SB) == RESET);
		
		//��ʼ�źű���⵽�����ʹӻ��豸��ַ
		I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS7, I2C_Direction_Transmitter);
			
	 //����ַ�Ƿ��ͳɹ�
	}while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_ADDR) == RESET);

	//����д����ɣ�����ֹͣ�ź�
	I2C_GenerateSTOP(I2C_EE, ENABLE);
	
}






/* ���������ֽ� */
