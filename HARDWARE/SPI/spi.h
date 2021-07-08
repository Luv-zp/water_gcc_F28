#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
	

extern SPI_HandleTypeDef SPI2_Handler;  //SPI¾ä±ú

void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
u8 SPI2_WriteByte(u8 TxData);
u8 SPI2_ReadByte(void);
#endif
