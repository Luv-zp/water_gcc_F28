#include "ad7606.h"
#include "delay.h"
#include "spi.h"

FIFO_T	g_tAD;  /*����һ�����������������ڴ洢AD�ɼ�����*/

/*********************************************************************
  * @brief	AD7606��Ҫ�����ų�ʼ��
  * @param	
  * @retval	
  */
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOB, AD_OS2_PIN|AD_OS1_PIN|AD_OS0_PIN|AD_RANGE_PIN|AD_CONVST_PIN, GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);
	/*Configure GPIO pins : PE2 PE3 PE4 PE5 
                           PE6*/
	GPIO_InitStruct.Pin = AD_RESET_PIN | AD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = AD_CONVST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	/*�˲��ֵ����ŵ�ƽ�Ѿ��̶�����˲���Ҫ��ʼ��IO��*/
//  GPIO_InitStruct.Pin = AD_OS2_PIN|AD_OS1_PIN|AD_OS0_PIN;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	/*Configure GPIO pin : PC7 */
//  GPIO_InitStruct.Pin = AD_BUSY_PIN;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//  HAL_GPIO_Init(AD_BUSY_GPIO_PORT, &GPIO_InitStruct);

}

/*********************************************************************
  * @brief	AD7606��ʼ��
  * @param	
  * @retval	
  */
void ad7606_init(void)
{
	MX_GPIO_Init();
	SPI2_Init();
	//����RANGE,�̶�Ϊ�͵�ƽ5V
	//AD_RANGE_5V();
	//���ù�����ģʽ���������ŵ�ƽ�Ѿ��̶�
	//ad7606_setOS(6);
	//����GPIO��ʼ״̬
	ad7606_reset();
	AD_CONVST_HIGH();//CONVST��������Ϊ�ߵ�ƽ
}

/*********************************************************************
  * @brief	��λAD
  * @param	
  * @retval	
  */
void ad7606_reset(void)
{
	/*AD7606�Ǹߵ�ƽ��λ��Ҫ����С����50ns*/
	AD_RESET_LOW();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_LOW();
}

/*********************************************************************
  * @brief	���ù�����ģʽ�������������Ѿ��̶����������ã�
  * @param	uCMode:0-6
  * @retval	
  */
void ad7606_setOS(u8 uCMode)
{
//	if (uCMode == 1)
//	{
//		AD_OS2_0();
//		AD_OS1_0();
//		AD_OS0_1();
//	}else if (uCMode == 2)
//	{
//		AD_OS2_0();
//		AD_OS1_1();
//		AD_OS0_0();
//	}else if (uCMode == 3)
//	{
//		AD_OS2_0();
//		AD_OS1_1();
//		AD_OS0_1();
//	}else if (uCMode == 4)
//	{
//		AD_OS2_1();
//		AD_OS1_0();
//		AD_OS0_0();
//	}else if (uCMode == 5)
//	{
//		AD_OS2_1();
//		AD_OS1_0();
//		AD_OS0_1();
//	}else if (uCMode == 6)
//	{
//		AD_OS2_1();
//		AD_OS1_1();
//		AD_OS0_0();
//	}else		/*��0����*/
//	{
//		AD_OS2_0();
//		AD_OS1_0();
//		AD_OS0_0();
//	}
}

/*********************************************************************
  * @brief	����ADת��
  * @param	
  * @retval	
  */
void ad7606_startConv(void)
{
	//�����ؿ�ʼת�����͵�ƽ����ʱ������25ns
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();

	AD_CONVST_HIGH();
}

/*********************************************************************
  * @brief	��ȡ16λ����
  * @param	
  * @retval	��ȡ��������
  */
u16 ad7606_readBytes(void)
{
	u16 usData;
	usData = SPI2_ReadWriteByte(0xffff);
	return usData;
}

/*********************************************************************
  * @brief	��ȡ��������
  * @param	data:ָ�򱣴��ȡ���ݵ�����
  * @retval	
  */
void ad7606_readData(u16 * data)
{
	u8 i = 0;
	u16 readValue;
	u16 *p_value = 0;
	p_value = (u16 *)data;
	AD_RESET_LOW();
	ad7606_startConv();//��ʼ�ɼ�
	delay_us(1);
	AD_CS_LOW();
	delay_us(1);
	//ÿ�ζ�ȡ8��ͨ��������
	for (i = 0; i < 8; i ++)	//ֻ������ͨ�����ã���Ϊֻ������������
	{
		readValue = ad7606_readBytes();
		*p_value = readValue;
		p_value ++;
	}
	AD_CS_HIGH();
}

