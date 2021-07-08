#include "ad7606.h"
#include "delay.h"
#include "spi.h"

FIFO_T	g_tAD;  /*定义一个交换缓冲区，用于存储AD采集数据*/

/*********************************************************************
  * @brief	AD7606需要的引脚初始化
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
	/*此部分的引脚电平已经固定，因此不需要初始化IO口*/
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
  * @brief	AD7606初始化
  * @param	
  * @retval	
  */
void ad7606_init(void)
{
	MX_GPIO_Init();
	SPI2_Init();
	//设置RANGE,固定为低电平5V
	//AD_RANGE_5V();
	//设置过采样模式，三个引脚电平已经固定
	//ad7606_setOS(6);
	//设置GPIO初始状态
	ad7606_reset();
	AD_CONVST_HIGH();//CONVST引脚设置为高电平
}

/*********************************************************************
  * @brief	复位AD
  * @param	
  * @retval	
  */
void ad7606_reset(void)
{
	/*AD7606是高电平复位，要求最小脉宽50ns*/
	AD_RESET_LOW();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_LOW();
}

/*********************************************************************
  * @brief	设置过采样模式（本例中引脚已经固定，不用设置）
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
//	}else		/*按0处理*/
//	{
//		AD_OS2_0();
//		AD_OS1_0();
//		AD_OS0_0();
//	}
}

/*********************************************************************
  * @brief	启动AD转换
  * @param	
  * @retval	
  */
void ad7606_startConv(void)
{
	//上升沿开始转换，低电平持续时间至少25ns
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();

	AD_CONVST_HIGH();
}

/*********************************************************************
  * @brief	读取16位数据
  * @param	
  * @retval	读取到的数据
  */
u16 ad7606_readBytes(void)
{
	u16 usData;
	usData = SPI2_ReadWriteByte(0xffff);
	return usData;
}

/*********************************************************************
  * @brief	读取采样数据
  * @param	data:指向保存读取数据的数组
  * @retval	
  */
void ad7606_readData(u16 * data)
{
	u8 i = 0;
	u16 readValue;
	u16 *p_value = 0;
	p_value = (u16 *)data;
	AD_RESET_LOW();
	ad7606_startConv();//开始采集
	delay_us(1);
	AD_CS_LOW();
	delay_us(1);
	//每次读取8个通道的数据
	for (i = 0; i < 8; i ++)	//只有两个通道有用，因为只有两个传感器
	{
		readValue = ad7606_readBytes();
		*p_value = readValue;
		p_value ++;
	}
	AD_CS_HIGH();
}

