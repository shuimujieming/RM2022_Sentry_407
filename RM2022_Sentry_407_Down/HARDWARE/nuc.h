#ifndef __NUC_H
#define __NUC_H



#include "main.h"
void NUC_Init();
void NUC_Send_Data();


//NUC接收数据结构体
typedef struct
{
	int8_t Armor_Type;             //装甲类型  0-无  1-小  2-大
	float  Yaw_Angle;              //Y轴增量角度
  float  Pitch_Angle;            //P轴增量角度
	short  Enemy_distance;         //敌方距离
	char   Armor_ID;               //敌方装甲号码
	short  Enemy_Real_Speed;
	char   NUC_Shoot_Allow_Flag;
}NUC_Typedef;

#endif