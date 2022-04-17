#include "laser.h"

#define RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_LASER, ENABLE) 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)

#define GPIO_Pin_LASER GPIO_Pin_8

#define GPIO_LASER GPIOC


/**
  * @brief  红外传感器初始化
  * @param  void
  * @retval void
  * @notes  左PH11 C  右PH12 B  L-遮挡为0，没挡为1
  */
void Infrared_Sensor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitTypeStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitTypeStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitTypeStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOH,&GPIO_InitTypeStruct);

	
}

/*
*@Description：激光外设初始化
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Laser_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_LASER, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LASER;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIO_LASER, &GPIO_InitStructure); 
	
	GPIO_SetBits(GPIO_LASER,GPIO_Pin_LASER);
	
	Infrared_Sensor_Init();
	
}


