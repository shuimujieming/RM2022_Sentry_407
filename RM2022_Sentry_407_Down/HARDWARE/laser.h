#ifndef __LASER_H
#define __LASER_H
#include "main.h"


//打开激光
#define LASER_ON 	GPIO_SetBits(GPIOG,GPIO_Pin_13)
//关闭激光
#define LASER_OFF 	GPIO_ResetBits(GPIOG,GPIO_Pin_13)

void Laser_Init(void);


#define Left_Ultrasonic GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_12)
#define Right_Ultrasonic GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11)

#endif