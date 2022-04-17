#include "beep.h"

#define RCC_APBPeriphClockCmd_TIM_BEEP RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE)
#define RCC_AHBPeriphClockCmd_GPIO_BEEP RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)

#define GPIO_BEEP GPIOD
#define GPIO_PinSource_BEEP GPIO_PinSource14
#define GPIO_AF_TIM_BEEP GPIO_AF_TIM4
#define GPIO_Pin_BEEP GPIO_Pin_14

#define TIM_BEEP TIM4
#define TIM_Psc_BEEP 84
#define TIM_IS_ADVANCED_BEEP 0

#define TIM_OCInit_BEEP		TIM_OC3Init(TIM4, &TIM_OCInitStructure);TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable)


void Beep_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APBPeriphClockCmd_TIM_BEEP;  	//TIM14时钟使能    
  RCC_AHBPeriphClockCmd_GPIO_BEEP;
	GPIO_PinAFConfig(GPIO_BEEP,GPIO_PinSource_BEEP,GPIO_AF_TIM_BEEP);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_BEEP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIO_BEEP, &GPIO_InitStructure); 
	
	TIM_TimeBaseStructure.TIM_Prescaler=TIM_Psc_BEEP-1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=600-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM_BEEP,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	
	TIM_OCInit_BEEP;
	
  TIM_ARRPreloadConfig(TIM_BEEP,ENABLE);//ARPE使能 
	
	#if TIM_IS_ADVANCED_BEEP
	TIM_CtrlPWMOutputs(TIM_BEEP,ENABLE);
	#endif	
	
	TIM_Cmd(TIM_BEEP, ENABLE);  //使能TIM12
}

void Beep_Ctrl(uint16_t tune, uint16_t sound)	//
{
  BEEP_TUNE = tune;
  BEEP_SOUND = sound;//0
}
