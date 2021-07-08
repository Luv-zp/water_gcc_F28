#ifndef _AD7606_H
#define _AD7606_H
#include "sys.h"


/*每个样品2字节，采集通道*/
#define CH_NUM						2          /*采集2通道*/
#define FIFO_SIZE					8*1024*2	

/*定义AD7606的SPI GPIO*/
#define AD_SPI						SPI2
#define AD_SPI_CLK					RCC_APB1Periph_SPI2

#define AD_SPI_CLK_PIN				GPIO_PIN_13
#define AD_SPI_CLK_GPIO_PORT		GPIOB

#define AD_SPI_MISO_PIN				GPIO_PIN_14
#define AD_SPI_MISO_GPIO_PORT		GPIOB

#define AD_CS_PIN					GPIO_PIN_6
#define AD_CS_GPIO_PORT				GPIOE

/*定义AD7606的其他GPIO*/
//#define AD_BUSY_PIN				GPIO_PIN_5
//#define	AD_BUSY_GPIO_PORT		GPIOE

#define AD_RESET_PIN				GPIO_PIN_4
#define	AD_RESET_GPIO_PORT			GPIOE

#define AD_CONVST_PIN				GPIO_PIN_6
#define	AD_CONVST_GPIO_PORT			GPIOC

//#define AD_RANGE_PIN				GPIO_PIN_2
//#define	AD_RANGE_GPIO_PORT		GPIOE

//#define AD_OS0_PIN				GPIO_PIN_9
//#define	AD_OS0_GPIO_PORT		GPIOB

//#define AD_OS1_PIN				GPIO_PIN_8
//#define	AD_OS1_GPIO_PORT		GPIOB

//#define AD_OS2_PIN				GPIO_PIN_7
//#define	AD_OS2_GPIO_PORT		GPIOB

#define AD_CS_LOW()					HAL_GPIO_WritePin(AD_CS_GPIO_PORT,AD_CS_PIN, GPIO_PIN_RESET)
#define AD_CS_HIGH()				HAL_GPIO_WritePin(AD_CS_GPIO_PORT,AD_CS_PIN, GPIO_PIN_SET)

#define AD_RESET_LOW()				HAL_GPIO_WritePin(AD_RESET_GPIO_PORT,AD_RESET_PIN, GPIO_PIN_RESET)
#define AD_RESET_HIGH()				HAL_GPIO_WritePin(AD_RESET_GPIO_PORT,AD_RESET_PIN, GPIO_PIN_SET)

#define AD_CONVST_LOW()				HAL_GPIO_WritePin(AD_CONVST_GPIO_PORT,AD_CONVST_PIN, GPIO_PIN_RESET)
#define AD_CONVST_HIGH()			HAL_GPIO_WritePin(AD_CONVST_GPIO_PORT,AD_CONVST_PIN, GPIO_PIN_SET)

//#define AD_RANGE_5V()				HAL_GPIO_WritePin(AD_RANGE_GPIO_PORT,AD_RANGE_PIN, GPIO_PIN_RESET)
//#define AD_RANGE_10V()			HAL_GPIO_WritePin(AD_RANGE_GPIO_PORT,AD_RANGE_PIN, GPIO_PIN_SET)

//#define AD_OS0_0()				HAL_GPIO_WritePin(AD_OS0_GPIO_PORT,AD_OS0_PIN, GPIO_PIN_RESET)
//#define AD_OS0_1()				HAL_GPIO_WritePin(AD_OS0_GPIO_PORT,AD_OS0_PIN, GPIO_PIN_SET)

//#define AD_OS1_0()				HAL_GPIO_WritePin(AD_OS1_GPIO_PORT,AD_OS1_PIN, GPIO_PIN_RESET)
//#define AD_OS1_1()				HAL_GPIO_WritePin(AD_OS1_GPIO_PORT,AD_OS1_PIN, GPIO_PIN_SET)

//#define AD_OS2_0()				HAL_GPIO_WritePin(AD_OS2_GPIO_PORT,AD_OS2_PIN, GPIO_PIN_RESET)
//#define AD_OS2_1()				HAL_GPIO_WritePin(AD_OS2_GPIO_PORT,AD_OS2_PIN, GPIO_PIN_SET)

//#define BUSY 						(u8)HAL_GPIO_ReadPin(AD_BUSY_GPIO_PORT,AD_BUSY_PIN)

/*ad采集数据缓冲区*/
typedef struct _FIFO_T
{
	u16 usRead;
	u16 usWrite;
	u16 usCount;
	u16 usBuf[FIFO_SIZE];
}FIFO_T;


/*接口函数*/
void ad7606_reset(void);
void ad7606_setOS(u8 uCMode);
void setTim2Freq(u32 freq);
void ad7606_init(void);
void ad7606_startRecord(u32 freq);
void ad7606_stopRecord(void);
u8 getAdcFormFIFO(u16 *readAdc);
void ad7606_readData(u16 * data);
void ad7606_startConv(void);

extern FIFO_T g_tAD;

#endif

