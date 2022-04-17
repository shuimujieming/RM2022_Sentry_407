#include "led.h" 


/*
*@Description��LED��ʼ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void LED_Init()
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);//ʹ��GPIOFʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOH, &GPIO_InitStructure);//LED_G 
	
	
	//���
	GPIO_ResetBits(GPIOH,GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
}

/*
*@Description��LED����
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void LED_Run(void)
{
	GPIO_ToggleBits(GPIOH,GPIO_Pin_11);
}






