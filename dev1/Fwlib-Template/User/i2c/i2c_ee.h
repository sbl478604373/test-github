#ifndef _I2C_EE_H
#define _I2C_EE_H

#include "stm32f10x.h"
#include "./usart/usart.h" 


										  
										  

// I2C1
#define  I2C_EE                    I2C1
#define  I2C_EE_CLK                RCC_APB1Periph_I2C1
#define  I2C_EE_APBxClkCmd         RCC_APB1PeriphClockCmd  //使能时钟的函数
#define  I2C_EE_BAUDRATE           400000

// I2C1 GPIO 引脚宏定义
#define  I2C_EE_SCL_GPIO_CLK       (RCC_APB2Periph_GPIOB)
#define  I2C_EE_SDA_GPIO_CLK       (RCC_APB2Periph_GPIOB)

#define  I2C_EE_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd  

#define  I2C_EE_SCL_GPIO_PORT      GPIOB
#define  I2C_EE_SCL_GPIO_PIN       GPIO_Pin_6

#define  I2C_EE_SDA_GPIO_PORT      GPIOB
#define  I2C_EE_SDA_GPIO_PIN       GPIO_Pin_7

#define  I2C_EE_OWN_ADDRESS7       0X5F  
#define  EEPROM_ADDRESS7           0XA0


void I2C_Config(void);
void IIC_EE_WriteByte(uint8_t addr, uint8_t data);
void IIC_EE_Read(uint8_t addr, uint8_t *data, uint8_t numByteToRead);
void IIC_EE_WaitForWritting(void);
void IIC_EE_WritePage(uint8_t addr, uint8_t *data, uint8_t numByteToWrite);



#endif /* _I2C_EE_H */


