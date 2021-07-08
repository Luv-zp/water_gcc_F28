#ifndef _HART_COMMUNICATE_H
#define _HART_COMMUNICATE_H

#include "sys.h"

extern float diffGage;
void rs232RecvData(u8 u8Data);
void hartCommunicate_thread(void * pArgs);
void HartDataSend(uint8_t *pData, uint8_t size);

#endif

