#include "cover.h"

#define RCC_APBPeriphClockCmd_TIM_Cover RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE)
#define RCC_AHBPeriphClockCmd_GPIO_Cover RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE)
#define GPIO_AF_TIM_Cover GPIO_AF_TIM1

#define GPIO_COVER GPIOE
#define GPIO_PinSource_Cover GPIO_PinSource9
#define GPIO_Pin_Cover GPIO_Pin_9

#define TIM_Cover TIM1
#define TIM_Psc_Cover 168
#define TIM_IS_ADVANCED_Cover 1

#define TIM_OCInit_Cover	TIM_OC1Init(TIM_Cover, &TIM_OCInitStructure);TIM_OC1PreloadConfig(TIM_Cover, TIM_OCPreload_Enable)

void Cover_PWM_Init()
{
	
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APBPeriphClockCmd_TIM_Cover;  	//TIM1时钟使能    
  RCC_AHBPeriphClockCmd_GPIO_Cover;
	GPIO_PinAFConfig(GPIO_COVER,GPIO_PinSource_Cover,GPIO_AF_TIM_Cover);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Cover;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIO_COVER, &GPIO_InitStructure); 
	
	TIM_TimeBaseStructure.TIM_Prescaler=TIM_Psc_Cover-1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=20000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM_Cover,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_Pulse = 2000;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	
	TIM_OCInit_Cover;
	
  TIM_ARRPreloadConfig(TIM_Cover,ENABLE);//ARPE使能 
	
	
	#if TIM_IS_ADVANCED_Cover
	TIM_CtrlPWMOutputs(TIM_Cover,ENABLE);
	#endif
	
	TIM_Cmd(TIM_Cover, ENABLE); 
}


void Cover_Init()
{
	Cover_PWM_Init();
	COVER_ON;
}