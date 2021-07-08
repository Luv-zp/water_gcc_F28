#include "gui_function.h"

/*
 **************************************************
 *����ת��Ϊ�ַ����飬ת��������鳤��Ϊ6���������һ��'\0'������
 *���������str, ת������ַ���
 *		   input, Ҫת�������� 
 *
 **************************************************
 */
void float2str(char * p_str, float input)
{
//	char str[6] = {0};
	int intPart = 0;
	float floatPart = 0;
//	int i = 0;
//	int len = 0;	//���������������
	if (input > 999.99f)  input = 999.99f;
	if (input < 0)  input = 0;
	intPart = (int)input;
	floatPart = input - intPart;
	//�ж��������ִ�С�������������鳤��
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
//	int len = 0;//���������������
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
*			��������		intToStr
*			����˵����   ��int���͵�����ת�����ַ���
*			�βΣ�		* p_str������ת������ַ�����
*                 		input  : ��Ҫת������������
*                 		mode   : 0��ת�����ݷ�ΧΪ0-999��1��ת����Χ��0-9
*			����ֵ��		��
*******************************************************
*/
void intToStr(char * p_str, int input, int mode)
{
//	int i;
//	int len = 0;//���������������
//	int upLimit = 10;
//	int loLimit = 0;
//	int temp;
	switch (mode)
	{
		case 0:
			if (input > 999)  input = 999;
			if (input < 0)  input = 0;
			//�ж��������ִ�С�������������鳤��
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
 *�������ܣ���������ת��Ϊ�ַ���
 *���������*p_str �����ַ���, input ������ �������
 *�����������
 *����ֵ��ת�������������
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
 *�������ܣ����ַ���ת��Ϊint��
 *���������*p_str �����ַ���
 *�����������
 *����ֵ��ת�������������
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
	//���ݵĳ���Ϊlen
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

