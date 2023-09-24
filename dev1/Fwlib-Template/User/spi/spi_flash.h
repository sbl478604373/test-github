#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#include "stm32f10x.h" 

								  

// SPI1
#define  SPI_FLASH                    SPI1
#define  SPI_FLASH_CLK                RCC_APB2Periph_SPI1
#define  SPI_FLASH_APBxClk_Fun        RCC_APB2PeriphClockCmd  //使能时钟的函数

// SPI1 GPIO 引脚宏定义
#define  SPI_FLASH_SCK_CLK            RCC_APB2Periph_GPIOA
#define  SPI_FLASH_CS_CLK             RCC_APB2Periph_GPIOC
#define  SPI_FLASH_GPIO_APBxClk_Fun   RCC_APB2PeriphClockCmd

#define  SPI_FLASH_SCK_GPIO_PORT      GPIOA
#define  SPI_FLASH_SCK_GPIO_PIN       GPIO_Pin_5

#define  SPI_FLASH_MOSI_GPIO_PORT     GPIOA
#define  SPI_FLASH_MOSI_GPIO_PIN      GPIO_Pin_7

#define  SPI_FLASH_MISO_GPIO_PORT     GPIOA
#define  SPI_FLASH_MISO_GPIO_PIN      GPIO_Pin_6

#define  SPI_FLASH_CS_GPIO_PORT       GPIOC
#define  SPI_FLASH_CS_GPIO_PIN        GPIO_Pin_0

#define  SPI_FLASH_CS_HIGH()          GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_GPIO_PIN)
#define  SPI_FLASH_CS_LOW()           GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_GPIO_PIN)




/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))


/*信息输出*/
#define FLASH_DEBUG_ON           0

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

//FLASH常用标志位																					
#define	DUMMY                     0x00
#define WriteStatus						    0x01
#define ReadStatus						    0x05
#define WriteEnable								0x06
#define ReadData									0x03
#define PageProgram								0x02													
																					
#define SectorErase								0x20													
#define	JEDEC_ID                  0x9f 

																					

void SPI_FLASH_Init(void);																					
uint32_t SPI_READ_ID(void);
void SPI_FLASH_WaitForWriteed(void);
void SPI_FLASH_SectorErase(uint32_t addr);
void SPI_FLASH_ReadData(uint32_t addr, uint8_t *readBuff, uint32_t numToReadData);
void SPI_FLASH_WriteData(uint32_t addr, uint8_t *writeBuff, uint32_t numToWriteData);


																					
																							
																					
#endif /* _SPI_FLASH_H */


