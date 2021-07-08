#ifndef _GENERAL_FUNC_H
#define _GENERAL_FUNC_H

#include "sys.h"

#define GF_FLOAT_DATA	0
#define GF_U32_DATA		1
#define GF_INT_DATA		2

/*КЏЪ§жиди*/
#define fMeanFilter(arr, len, res)		meanFilter((void *)arr, len, GF_FLOAT_DATA, (void *)res)
#define u32MeanFilter(arr, len, res)	meanFilter((void *)arr, len, GF_U32_DATA, (void *)res)
#define intMeanFilter(arr, len, res)	meanFilter((void *)arr, len, GD_INT_DATA, (void *)res)


typedef union
{
	float f_data;
	u32 u32_data;
	int int_data;
}DataUnion;

void meanFilter(void * const arr, u32 len, u32 dataType, void * res);

u32 convFloat2u32(float data);

#endif
