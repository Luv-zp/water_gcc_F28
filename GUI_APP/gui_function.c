#include "gui_function.h"

/*
 **************************************************
 *数字转换为字符数组，转换后的数组长度为6，包含最后一个'\0'结束符
 *输入参数：str, 转换后的字符串
 *		   input, 要转换的数据 
 *
 **************************************************
 */
void float2str(char * p_str, float input)
{
//	char str[6] = {0};
	int intPart = 0;
	float floatPart = 0;
//	int i = 0;
//	int len = 0;	//最终数组输出长度
	if (input > 999.99f)  input = 999.99f;
	if (input < 0)  input = 0;
	intPart = (int)input;
	floatPart = input - intPart;
	//判断整数部分大小，决定最终数组长度
	if ((intPart / 100) > 0)
	{
		p_str[2] = '0' + intPart % 10;
		intPart = intPart / 10;
		p_str[1] = '0' + intPart % 10;
		intPart = intPart / 10;
		p_str[0] = '0' + intPart % 10;
		p_str[3] = '.';
		p_str[4] = '0' + (int)(floatPart * 10);
		p_str[5] = '0' + ((int)(floatPart * 100))%10;
		p_str[6] = '\0';
	}
	else if ((intPart / 10) > 0)
	{
		p_str[1] = '0' + intPart % 10;
		intPart = intPart / 10;
		p_str[0] = '0' + intPart % 10;
		p_str[2] = '.';
		p_str[3] = '0' + (int)(floatPart * 10);
		p_str[4] = '0' + ((int)(floatPart * 100))%10;
		p_str[5] = '\0';
	}
	else
	{
		p_str[0] = '0' + intPart % 10;
		p_str[1] = '.';
		p_str[2] = '0' + (int)(floatPart * 10);
		p_str[3] = '0' + ((int)(floatPart * 100))%10;
		p_str[4] = '\0';
	}
}	

float str2float(const char * p_str)
{
	int  intPart = 0;
//	int  intPartLen = 0;
	float out = 0;
//	int len = 0;//最终数组输出长度
	while (('.' != * p_str)&&('\0' != * p_str))
	{
		intPart = intPart * 10 + (* p_str - '0');
		p_str ++;
	}
	if ('\0' != * p_str)
	{
		p_str ++;
		if ('\0' != * p_str)  
		{
			out = intPart + (* p_str - '0')/10.0;
			p_str++;
			if('\0' != * p_str)
				out = out + (* p_str - '0')/100.0;						
		}
	}
	else  out = intPart;
	if ((out > 999.99f) || (out < 0)) out = 999.99f;
	return out;
}

/*
*******************************************************
*			函数名：		intToStr
*			功能说明：   将int类型的数据转换成字符串
*			形参：		* p_str：保存转换后的字符串；
*                 		input  : 需要转换的整型数据
*                 		mode   : 0：转换数据范围为0-999；1：转换范围是0-9
*			返回值：		无
*******************************************************
*/
void intToStr(char * p_str, int input, int mode)
{
//	int i;
//	int len = 0;//最终数组输出长度
//	int upLimit = 10;
//	int loLimit = 0;
//	int temp;
	switch (mode)
	{
		case 0:
			if (input > 999)  input = 999;
			if (input < 0)  input = 0;
			//判断整数部分大小，决定最终数组长度
			if ((input / 100) > 0)
			{
				p_str[2] = '0' + input % 10;
				input = input / 10;
				p_str[1] = '0' + input % 10;
				input = input / 10;
				p_str[0] = '0' + input % 10;
				p_str[3] = '\0';
			}
			else if ((input / 10) > 0)
			{
				p_str[1] = '0' + input % 10;
				input = input / 10;
				p_str[0] = '0' + input % 10;
				p_str[2] = '\0';
			}
			else
			{
				p_str[0] = '0' + input % 10;
				p_str[1] = '\0';
			}
			break;
		case 1:
			if (input > 9)  input = 9;
			if (input < 0)  input = 0;
			p_str[0] = '0' + input;
			p_str[1] = '\0';
			break;
		default:
			break;
	}
}

/*
 *函数功能：将长整型转换为字符串
 *输入参数：*p_str 参数字符串, input 长整型 编号数据
 *输出参数：无
 *返回值：转换后的整型密码
 */
void dLongTostr(char * p_str, long long input)
{
	int i = 0;
	for (i = 0; i < 10; i ++)
	{
		p_str[9-i] = input % 10 + '0';
		input = input / 10;
	}
	p_str[10] = '\0';
}

/*
 *函数功能：将字符串转换为int型
 *输入参数：*p_str 参数字符串
 *输出参数：无
 *返回值：转换后的整型密码
 */
int strToInt(const char * p_str)
{
	int out = 0;
	while ('\0' != * p_str)
	{
		out = out * 10 + (* p_str - '0');
		p_str ++;
	}
	return out;
}

void numToStr(long long num, u8 * str)
{
	long long temp = 1;
	int len = 0;
	int i = 0;
	while ((num / temp) != 0)
	{
		len ++;
		temp = temp * 10;
	}
	//数据的长度为len
	if (len >= 30)	len = 30;
	for (i = (len - 1); i >= 0; i --)
	{
		str[i] = num % 10 + '0';
		num = num / 10;
	}
	for (i = len; i < 32; i ++)
	{
		str[i] = '\0';
	}
}

long long strToNum(u8 * str)
{
	long long output = 0;
	int len = 0;
	while (str[len] != '\0')
	{
		output = output * 10 + (str[len] - '0');
		len ++;
	}
	return output;
}

