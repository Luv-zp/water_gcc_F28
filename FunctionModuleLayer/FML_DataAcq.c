#include "FML_DataAcq.h"
#include "ads1256.h"
#include "math.h"
#include "GeneralFunc.h"
#include "led.h"
#include "userApp.h"

#define PRES_BUFF_LEN 1000		//ѹ�����ݻ���������
#define WATER_PRES_LEN	200		//ˮѹ�˲�����������
#define PRES_DISP_LEN 50		//ʵʱ������ʾ���õ������ݳ���
#define FILTER_LEN 5				//�˲�����

TimerHandle_t daqTimerHandle;		//�����ʱ������
DataServer ad_daqDev;	//���ݲɼ��豸����
float waterPressBuff[PRES_BUFF_LEN];		//ˮѹ������ѹ����������
float waterPres[FILTER_LEN];	//��������ɼ������ݣ��Ա��������˲�

static void AD_DaqDev_init(DataServer * const me);
static float AD_DaqDev_getPresData(DataServer * const me, PrestDataType dataType);
//static float AD_DaqDev_getMeanPres(void);
//static float AD_DaqDev_getResMeanPres(void);

static float daq2waterPres(int input);	//������תΪˮѹ
static float calcMeanF(const float * const input, int len); //����ָ��+ָ�볣��
//static float deleteGrossError(const float * const input, int len);
static void daqTimerCb(TimerHandle_t xTimer);

DataServer * DataServer_create(void)
{
	static DataServer * me = 0 ;
	if(me == 0)
	{
		me = &ad_daqDev;
		me->init = AD_DaqDev_init;
		me->getPresData = AD_DaqDev_getPresData;
		
		me->waterPres = 0.0f;
	
		
		me->init(me);
	}
	return me;
}

static void AD_DaqDev_init(DataServer * const me)
{
	ADS1256_Init();
	daqTimerHandle = xTimerCreate((const char *)"daqCb",
								  (TickType_t  )10,
								  (UBaseType_t )pdTRUE,
								  (void *      )7,
								  (TimerCallbackFunction_t)daqTimerCb);
	xTimerStart(daqTimerHandle, 10);	//�������ݲɼ���ʱ��
}

static float AD_DaqDev_getPresData(DataServer * const me, PrestDataType dataType)
{
	if(dataType == CUR_WATER_DATA)	//��ȡʵʱˮѹ
	{
		return calcMeanF(&waterPressBuff[PRES_BUFF_LEN - PRES_DISP_LEN], PRES_DISP_LEN);
	}
	else if(dataType == PRO_DEC_WATER) //��Ʒ�������Ҫ��ˮѹ
	{
		return calcMeanF(&waterPressBuff[PRES_BUFF_LEN - WATER_PRES_LEN], WATER_PRES_LEN);
	}
	else if(dataType == AFTER_ADD_WATER) //��ѹ���̺��ˮѹ
	{
		return calcMeanF(&waterPressBuff[PRES_BUFF_LEN - WATER_PRES_LEN], WATER_PRES_LEN);
	}
	else
		return 0;
}

static float calcMeanF(const float * const input, int len)
{
	float sum = 0;
	u32 i = 0;
	if(len == 0)
		return 0;
	for(i = 0; i < len; i++)
	{
		sum = sum + input[i];
	}
	return sum / len;
}

/*
 * ���ɼ�������ת��Ϊˮѹ(�ź�1-5v,ѹ����Χ0-1MPa),���ݾ���ѹ��������
 * input����������
 * ���أ� ѹ��ֵ ��λkPa
 */
static float daq2waterPres(int input)	
{
	float out = 0.0f;
	out = (input * 5.0 / 8388607.0 - 1) * 1000.0 / 4.0;
	return out;
}

/*
 * ���ɼ�������ת��Ϊѹ��(�ź�0.5v-4.5v,ѹ����Χ0-150psi),���ݾ���ѹ��������
 * input����������
 * ���أ� ѹ��ֵ��λkPa
 */
//static float daq2gagePres(int input)
//{
//	float out = 0.0f;
//	out = (input * 5 / 32768 - 0.5) * 1034.21355f / (4.5 - 0.5);	//��λkPa ����Τ��40PC150G
//	return out;
//}


//��ȡһ��ʱ���ڵĲ���ѹ����ֵ(�޳��ִ����), ��λPa
//static float AD_DaqDev_getMeanPres(void)
//{
//	return deleteGrossError(gagePressBuff, PUMP_AIR_LEN);
//}

//��ȡһ��ʱ���ڵĲ���ѹ����ֵ(�޳��ִ����)
//static float AD_DaqDev_getResMeanPres(void)
//{
//	return deleteGrossError(&gagePressBuff[PRES_BUFF_LEN - RES_ARR_LEN], RES_ARR_LEN);
//}

/* 
 * ��һ�����ȵ������޳��ִ����������޳�֮���ƽ��ֵ
 * input�������׵�ַ��len�����鳤��
 * ���أ��޳��ִ�����ľ�ֵ
 */
//static float deleteGrossError(const float * const input, int len)
//{
//	u32 errBitArr[40] = {0};	//�ִ�������λ
//	float sum = 0;
//	int i = 0;
//	float sigma_3 = 0;	//3_sigma
//	float means = 0;
//	int errCount = 0;
//	int preErrCount = 0;
//	if(len > (32 * 40))
//		return 0;
//	/*---------------------------------�״��޳�--------------------------------*/
//	/*����ƽ��ֵ*/
//	for(i = 0; i < len; i++)
//	{
//		sum += input[i];
//	}
//	means = sum / len;
//	/*����������׼���3_sigma*/
//	sum = 0;
//	for(i = 0; i < len; i++)
//	{
//		sum += (input[i] - means) * (input[i] - means);	//����ƽ����
//	}
//	sigma_3 = 3 * sqrtf(sum / (len - 1));
//	sum = 0;
//	/*�����������ݣ�ֻ��¼С��3_sigma�����ݺͣ���¼�ִ������±�*/
//	for(i = 0; i < len; i++)
//	{
//		if(fabsf(input[i] - means) < sigma_3)
//			sum += input[i];
//		else
//		{
//			errBitArr[i/32] |= (1 << (i%32));
//			errCount ++;
//		}
//	}
//	/*����ִ�������Ϊ0��ֱ�ӷ������ֵ*/
//	means = sum / (len - errCount);
//	if(errCount == 0)
//	{
//		return means;
//	}
//	/*---------------------------------�����޳�--------------------------------*/
//	preErrCount = errCount;
//	/*����������׼���3_sigma*/
//	sum = 0;
//	for(i = 0; i < len; i++)
//	{
//		if(!(errBitArr[i/32] & (1<<i%32)))
//		{
//			sum += (input[i] - means) * (input[i] - means);		//����ƽ����
//		}
//	}
//	sigma_3 = 3 * sqrtf(sum / (len - errCount - 1));
//	sum = 0;
//	/*�����������ݣ�ֻ��¼С��3_sigma�����ݺͣ���¼�ִ������±�*/
//	for(i = 0; i < len; i++)
//	{
//		if(!(errBitArr[i/32] & (1<<i%32)))
//		{
//			if(fabsf(input[i] - means) < sigma_3)
//				sum += input[i];
//			else
//			{
//				errBitArr[i/32] |= (1 << (i%32));
//				errCount ++;
//			}
//		}	
//	}
//	means = sum / (len - errCount);
//	/*����ִ����û�����ӣ���ֱ�ӷ������ֵ*/
//	if(preErrCount == errCount)
//		return means;
//	/*---------------------------------����޳�--------------------------------*/
//	/*����������׼���3_sigma*/
//	sum = 0;
//	for(i = 0; i < len; i++)
//	{
//		if(!(errBitArr[i/32] & (1<<i%32)))
//		{
//			sum += (input[i] - means) * (input[i] - means);		//����ƽ����
//		}
//	}
//	sigma_3 = 3 * sqrtf(sum / (len - errCount - 1));
//	sum = 0;
//	/*�����������ݣ�ֻ��¼С��3_sigma�����ݺͣ���¼�ִ������±�*/
//	for(i = 0; i < len; i++)
//	{
//		if(!(errBitArr[i/32] & (1<<i%32)))
//		{
//			if(fabsf(input[i] - means) < sigma_3)
//				sum += input[i];
//			else
//			{
//				errBitArr[i/32] |= (1 << (i%32));
//				errCount ++;
//			}
//		}	
//	}
//	means = sum / (len - errCount);
//	return means;
//}

static void daqTimerCb(TimerHandle_t xTimer)
{

	int i = 0;
	static u32 waterBuffNum = 0;
	
	if(waterBuffNum < FILTER_LEN)
	{
		waterPres[waterBuffNum] = daq2waterPres(ADS1256_ReadAdc(1));
		ad_daqDev.waterPres = waterPres[waterBuffNum];
	}
	else
	{
		for(i = 0; i < (FILTER_LEN - 1); i++)
		{
			waterPres[i] = waterPres[i+1];
		}
		waterPres[FILTER_LEN - 1] = daq2waterPres(ADS1256_ReadAdc(1));
	}
	fMeanFilter(waterPres, FILTER_LEN, &ad_daqDev.waterPres);	//ƽ���˲�
	/*���˲�������ݱ�������������*/
	if(waterBuffNum < PRES_BUFF_LEN)
		waterPressBuff[waterBuffNum] = ad_daqDev.waterPres;
	else
	{
		for(i = 0; i < (PRES_BUFF_LEN - 1); i++)
		{
			waterPressBuff[i] = waterPressBuff[i + 1];
		}
		waterPressBuff[PRES_BUFF_LEN - 1] = ad_daqDev.waterPres;
	}
	waterBuffNum++;
}

