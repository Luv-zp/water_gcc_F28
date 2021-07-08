#ifndef _GUI_FUNCTION_H
#define _GUI_FUNCTION_H
#include "sys.h"

void float2str(char * p_str, float input);
float str2float(const char * p_str);
void intToStr(char * p_str, int input, int mode);
void dLongTostr(char * p_str, long long input);
int strToInt(const char * p_str);
void numToStr(long long num, u8 * str);
long long strToNum(u8 * str);
	
	
#endif

