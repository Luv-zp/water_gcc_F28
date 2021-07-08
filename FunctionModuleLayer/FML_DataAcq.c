#include "FML_DataAcq.h"
#include "ads1256.h"
#include "math.h"
#include "GeneralFunc.h"
#include "led.h"
#include "userApp.h"

#define PRES_BUFF_LEN 1000		//压力数据缓冲区长度
#define WATER_PRES_LEN	200		//水压滤波缓冲区长度
#define PRES_DISP_LEN 50		//实时数据显示所用到的数据长度
#define FILTER_LEN 5				//滤波长度

TimerHandle_t daqTimerHandle;		//软件定时器对象
DataServer ad_daqDev;	//数据采集设备对象
float waterPressBuff[PRES_BUFF_LEN];		//水压传感器压力缓冲数组
float waterPres[FILTER_LEN];	//用来缓冲采集的数据，以便对其进行滤波

static void AD_DaqDev_init(DataServer * const me);
static float AD_DaqDev_getPresData(DataServer * const me, PrestDataType dataType);
//static float AD_DaqDev_getMeanPres(void);
//static float AD_DaqDev_getResMeanPres(void);

static float daq2waterPres(int input);	//将数据转为水压
static float calcMeanF(const float * const input, int len); //常量指针+指针常量
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
	xTimerStart(daqTimerHandle, 10);	//启动数据采集定时器
}

static float AD_DaqDev_getPresData(DataServer * const me, PrestDataType dataType)
{
	if(dataType == CUR_WATER_DATA)	//获取实时水压
	{
		return calcMeanF(&waterPressBuff[PRES_BUFF_LEN - PRES_DISP_LEN], PRES_DISP_LEN);
	}
	else if(dataType == PRO_DEC_WATER) //产品检测所需要的水压
	{
		return calcMeanF(&waterPressBuff[PRES_BUFF_LEN - WATER_PRES_LEN], WATER_PRES_LEN);
	}
	else if(dataType == AFTER_ADD_WATER) //加压过程后的水压
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
 * 将采集的数据转换为水压(信号1-5v,压力范围0-1MPa),根据具体压力传感器
 * input：采样数据
 * 返回： 压力值 单位kPa
 */
static float daq2waterPres(int input)	
{
	float out = 0.0f;
	out = (input * 5.0 / 8388607.0 - 1) * 1000.0 / 4.0;
	return out;
}

/*
 * 将采集的数据转换为压力(信号0.5v-4.5v,压力范围0-150psi),根据具体压力传感器
 * input：采样数据
 * 返回： 压力值单位kPa
 */
//static float daq2gagePres(int input)
//{
//	float out = 0.0f;
//	out = (input * 5 / 32768 - 0.5) * 1034.21355f / (4.5 - 0.5);	//单位kPa 霍尼韦尔40PC150G
//	return out;
//}


//获取一段时间内的测试压力均值(剔除粗大误差), 单位Pa
//static float AD_DaqDev_getMeanPres(void)
//{
//	return deleteGrossError(gagePressBuff, PUMP_AIR_LEN);
//}

//获取一段时间内的测试压力均值(剔除粗大误差)
//static float AD_DaqDev_getResMeanPres(void)
//{
//	return deleteGrossError(&gagePressBuff[PRES_BUFF_LEN - RES_ARR_LEN], RES_ARR_LEN);
//}

/* 
 * 对一定长度的数据剔除粗大误差，并返回剔除之后的平均值
 * input：数组首地址，len：数组长度
 * 返回：剔除粗大误差后的均值
 */
//static float deleteGrossError(const float * const input, int len)
//{
//	u32 errBitArr[40] = {0};	//粗大误差错误位
//	float sum = 0;
//	int i = 0;
//	float sigma_3 = 0;	//3_sigma
//	float means = 0;
//	int errCount = 0;
//	int preErrCount = 0;
//	if(len > (32 * 40))
//		return 0;
//	/*---------------------------------首次剔除--------------------------------*/
//	/*计算平均值*/
//	for(i = 0; i < len; i++)
//	{
//		sum += input[i];
//	}
//	means = sum / len;
//	/*计算样本标准差和3_sigma*/
//	sum = 0;
//	for(i = 0; i < len; i++)
//	{
//		sum += (input[i] - means) * (input[i] - means);	//计算平方和
//	}
//	sigma_3 = 3 * sqrtf(sum / (len - 1));
//	sum = 0;
//	/*遍历所有数据，只记录小于3_sigma的数据和，记录粗大误差的下标*/
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
//	/*如果粗大误差计数为0，直接返回其均值*/
//	means = sum / (len - errCount);
//	if(errCount == 0)
//	{
//		return means;
//	}
//	/*---------------------------------二次剔除--------------------------------*/
//	preErrCount = errCount;
//	/*计算样本标准差和3_sigma*/
//	sum = 0;
//	for(i = 0; i < len; i++)
//	{
//		if(!(errBitArr[i/32] & (1<<i%32)))
//		{
//			sum += (input[i] - means) * (input[i] - means);		//计算平方和
//		}
//	}
//	sigma_3 = 3 * sqrtf(sum / (len - errCount - 1));
//	sum = 0;
//	/*遍历所有数据，只记录小于3_sigma的数据和，记录粗大误差的下标*/
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
//	/*如果粗大误差没有增加，则直接返回其均值*/
//	if(preErrCount == errCount)
//		return means;
//	/*---------------------------------最后剔除--------------------------------*/
//	/*计算样本标准差和3_sigma*/
//	sum = 0;
//	for(i = 0; i < len; i++)
//	{
//		if(!(errBitArr[i/32] & (1<<i%32)))
//		{
//			sum += (input[i] - means) * (input[i] - means);		//计算平方和
//		}
//	}
//	sigma_3 = 3 * sqrtf(sum / (len - errCount - 1));
//	sum = 0;
//	/*遍历所有数据，只记录小于3_sigma的数据和，记录粗大误差的下标*/
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
	fMeanFilter(waterPres, FILTER_LEN, &ad_daqDev.waterPres);	//平滑滤波
	/*将滤波后的数据保存至缓冲数组*/
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

