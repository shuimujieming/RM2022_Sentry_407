#ifndef __GIMBAL_H
#define __GIMBAL_H

typedef struct 
{
	int Yaw_Angle_Init;
	int Pitch_Angle_Init;
	int Pitch_Angle_Max;
	int Pitch_Angle_Min;
} Init_Angle_Data;

#include "main.h"
void Gimbal_Init();

#endif