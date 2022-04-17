#ifndef __LASER_H
#define __LASER_H
#include "main.h"


//�򿪼���
#define LASER_ON 	GPIO_SetBits(GPIOC,GPIO_Pin_8)
//�رռ���
#define LASER_OFF 	GPIO_ResetBits(GPIOC,GPIO_Pin_8)

void Laser_Init(void);


#define Left_Infrared !GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)
#define Right_Infrared GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)

#endif
