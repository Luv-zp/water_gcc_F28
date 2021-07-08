#ifndef _ADS_1256_H
#define _ADS_1256_H

#include "stm32f7xx.h"
#include "sys.h"

/* ����ads1256�� SPI GPIO */
#define AD_CLK_PIN				GPIO_PIN_13
#define AD_CLK_GPIO_PORT		GPIOB

#define AD_DOUT_PIN				GPIO_PIN_14
#define AD_DOUT_GPIO_PORT		GPIOB

#define AD_DIN_PIN				GPIO_PIN_15
#define AD_DIN_GPIO_PORT		GPIOB

#define AD_CS_PIN				GPIO_PIN_7
#define AD_CS_GPIO_PORT			GPIOB

/* ����ads1206 ���������� */
#define AD_DRDY_PIN				GPIO_PIN_12
#define AD_DRDY_GPIO_PORT		GPIOC

#define AD_RST_PIN				GPIO_PIN_8
#define AD_RST_GPIO_PORT		GPIOB

#define AD_SYNC_PIN				GPIO_PIN_9
#define AD_SYNC_GPIO_PORT		GPIOB

#define AD_CS_LOW()				HAL_GPIO_WritePin(AD_CS_GPIO_PORT, AD_CS_PIN, GPIO_PIN_RESET)
#define AD_CS_HIGH()			HAL_GPIO_WritePin(AD_CS_GPIO_PORT, AD_CS_PIN, GPIO_PIN_SET)

#define AD_RST_LOW()			HAL_GPIO_WritePin(AD_RST_GPIO_PORT, AD_RST_PIN, GPIO_PIN_RESET)
#define AD_RST_HIGH()			HAL_GPIO_WritePin(AD_RST_GPIO_PORT, AD_RST_PIN, GPIO_PIN_SET)

#define AD_SYNC_LOW()			HAL_GPIO_WritePin(AD_SYNC_GPIO_PORT, AD_SYNC_PIN, GPIO_PIN_RESET)
#define AD_SYNC_HIGH()			HAL_GPIO_WritePin(AD_SYNC_GPIO_PORT, AD_SYNC_PIN, GPIO_PIN_SET) 

#define DRDY_IS_LOW()			(HAL_GPIO_ReadPin(AD_DRDY_GPIO_PORT, AD_DRDY_PIN) == GPIO_PIN_RESET)
//#define DO_IS_HIGH()	        (HAL_GPIO_ReadPin(AD_DOUT_GPIO_PORT, AD_DOUT_PIN) == GPIO_PIN_SET)

//#define DI_0()		        HAL_GPIO_WritePin(AD_DIN_GPIO_PORT, AD_DIN_PIN, GPIO_PIN_RESET)
//#define DI_1()		        HAL_GPIO_WritePin(AD_DIN_GPIO_PORT, AD_DIN_PIN, GPIO_PIN_SET)

//#define SCK_0()				HAL_GPIO_WritePin(AD_CLK_GPIO_PORT, AD_CLK_PIN, GPIO_PIN_RESET)
//#define SCK_1()				HAL_GPIO_WritePin(AD_CLK_GPIO_PORT, AD_CLK_PIN, GPIO_PIN_SET)


/* ����ѡ�� */
typedef enum
{
	ADS1256_GAIN_1			= (0),	/* ����1��ȱʡ�� */
	ADS1256_GAIN_2			= (1),	/* ����2 */
	ADS1256_GAIN_4			= (2),	/* ����4 */
	ADS1256_GAIN_8			= (3),	/* ����8 */
	ADS1256_GAIN_16			= (4),	/* ����16 */
	ADS1256_GAIN_32			= (5),	/* ����32 */
	ADS1256_GAIN_64			= (6),	/* ����64 */
}ADS1256_GAIN_E;

/* ��������ѡ�� */
/* ����ת����ѡ��
	11110000 = 30,000SPS (default)
	11100000 = 15,000SPS
	11010000 = 7,500SPS
	11000000 = 3,750SPS
	10110000 = 2,000SPS
	10100001 = 1,000SPS
	10010010 = 500SPS
	10000010 = 100SPS
	01110010 = 60SPS
	01100011 = 50SPS
	01010011 = 30SPS
	01000011 = 25SPS
	00110011 = 15SPS
	00100011 = 10SPS
	00010011 = 5SPS
	00000011 = 2.5SPS
*/
typedef enum
{
	ADS1256_30000SPS = 0,
	ADS1256_15000SPS,
	ADS1256_7500SPS,
	ADS1256_3750SPS,
	ADS1256_2000SPS,
	ADS1256_1000SPS,
	ADS1256_500SPS,
	ADS1256_100SPS,
	ADS1256_60SPS,
	ADS1256_50SPS,
	ADS1256_30SPS,
	ADS1256_25SPS,
	ADS1256_15SPS,
	ADS1256_10SPS,
	ADS1256_5SPS,
	ADS1256_2d5SPS,

	ADS1256_DRATE_MAX
}ADS1256_DRATE_E;

#define ADS1256_DRAE_COUNT = 15;


/**************************ads1248�Ĵ�����ַ******************************/
// define commands 
#define ADS1256_CMD_WAKEUP  	0x00     //���SYNC���˳�����ģʽ
#define ADS1256_CMD_RDATA   	0x01     //������
#define ADS1256_CMD_RDATAC  	0x03     //����������
#define ADS1256_CMD_SDATAC   	0x0f     //ֹͣ���������� 
#define ADS1256_CMD_RREG     	0x10     //�ӼĴ��������� 
#define ADS1256_CMD_WREG     	0x50     //��Ĵ���д���� 
#define ADS1256_CMD_SELFCAL 	0xf0     //ƫ�ƺ������Զ�У׼
#define ADS1256_CMD_SELFOCAL 	0xf1     //ƫ���Զ�У׼ 
#define ADS1256_CMD_SELFGCAL 	0xf2     //�����Զ�У׼ 
#define ADS1256_CMD_SYSOCAL  	0xf3     //ϵͳʧ��У׼ 
#define ADS1256_CMD_SYSGCAL  	0xf4     //ϵͳ����У׼ 
#define ADS1256_CMD_SYNC     	0xfc     //ͬ��ADת�� 
#define ADS1256_CMD_STANDBY 	0xfd     //����ģʽ��ʼ 
#define ADS1256_CMD_REST     	0xfe      //��λ

// define the ADS1256 register values 
#define ADS1256_STATUS       0x00   
#define ADS1256_MUX          0x01   
#define ADS1256_ADCON        0x02   
#define ADS1256_DRATE        0x03   
#define ADS1256_IO           0x04   
#define ADS1256_OFC0         0x05   
#define ADS1256_OFC1         0x06   
#define ADS1256_OFC2         0x07   
#define ADS1256_FSC0         0x08   
#define ADS1256_FSC1         0x09   
#define ADS1256_FSC2         0x0A 
 
// define multiplexer codes 
#define ADS1256_MUXP_AIN0   0x00 
#define ADS1256_MUXP_AIN1   0x10 
#define ADS1256_MUXP_AIN2   0x20 
#define ADS1256_MUXP_AIN3   0x30 
#define ADS1256_MUXP_AIN4   0x40 
#define ADS1256_MUXP_AIN5   0x50 
#define ADS1256_MUXP_AIN6   0x60 
#define ADS1256_MUXP_AIN7   0x70 
#define ADS1256_MUXP_AINCOM 0x80 
 
#define ADS1256_MUXN_AIN0   0x00 
#define ADS1256_MUXN_AIN1   0x01 
#define ADS1256_MUXN_AIN2   0x02 
#define ADS1256_MUXN_AIN3   0x03 
#define ADS1256_MUXN_AIN4   0x04 
#define ADS1256_MUXN_AIN5   0x05 
#define ADS1256_MUXN_AIN6   0x06 
#define ADS1256_MUXN_AIN7   0x07 
#define ADS1256_MUXN_AINCOM 0x08   
 
// define gain codes 
#define ADS1256_GAIN_1      0x00 
#define ADS1256_GAIN_2      0x01 
#define ADS1256_GAIN_4      0x02 
#define ADS1256_GAIN_8      0x03 
#define ADS1256_GAIN_16     0x04 
#define ADS1256_GAIN_32     0x05 
#define ADS1256_GAIN_64     0x06 
//#define ADS1256_GAIN_64     0x07 
 
//define drate codes 
#define ADS1256_DRATE_30000SPS   0xF0 
#define ADS1256_DRATE_15000SPS   0xE0 
#define ADS1256_DRATE_7500SPS   0xD0 
#define ADS1256_DRATE_3750SPS   0xC0 
#define ADS1256_DRATE_2000SPS   0xB0 
#define ADS1256_DRATE_1000SPS   0xA1 
#define ADS1256_DRATE_500SPS    0x92 
#define ADS1256_DRATE_100SPS    0x82 
#define ADS1256_DRATE_60SPS     0x72 
#define ADS1256_DRATE_50SPS     0x63 
#define ADS1256_DRATE_30SPS     0x53 
#define ADS1256_DRATE_25SPS     0x43 
#define ADS1256_DRATE_15SPS     0x33 
#define ADS1256_DRATE_10SPS     0x23 
#define ADS1256_DRATE_5SPS      0x13 
#define ADS1256_DRATE_2_5SPS    0x03


typedef struct
{
	ADS1256_GAIN_E Gain;		/* ���� */
	ADS1256_DRATE_E DataRate;	/* ����������� */
	int32_t AdcNow[8];			/* 8·ADC�ɼ������ʵʱ���з����� */
	uint8_t Channel;			/* ��ǰͨ�� */
	uint8_t ScanMode;			/* ɨ��ģʽ��0��ʾ����8·�� 1��ʾ���4· */
	uint8_t ReadOver;
}ADS1256_VAR_T;

void ADS1256_Init(void);
void ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate);

uint8_t ADS1256_ReadChipID(void);
int32_t ADS1256_GetAdc(uint8_t _ch);
int32_t ADS1256_ReadAdc(uint8_t _ch);
extern ADS1256_VAR_T g_tADS1256;

#endif



