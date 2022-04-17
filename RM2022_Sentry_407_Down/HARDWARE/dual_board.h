#ifndef __Dual_Board_H
#define __Dual_Board_H



#include "main.h"
//void Dual_Board_Init();
//void Dual_Board_Send_Data();

typedef struct
{
	int8_t Get_Hit_flag;                    //2-没有受到攻击，0-装甲 0受到攻击，1-装甲1受到攻击，3-两个装甲都受到了攻击
	int8_t Bass_Satus;                      //2-100%防御，1-50%防御 ， 0-无防御
	short  Chassis_Real_Position;           //正对202电机，左侧开始，单位mm
	short  Chassis_Real_Number;             //每过10s减一个数
	short  Remain_HP;                       //剩余血量除以50
	int8_t Sentry_Satus;                    //1-自身底盘安全，0-底盘断电
	short  Chassis_Speed_Real;              //底盘速度
	char   Bullet_Type ;                    //1--大弹丸     2--小弹丸 
	int8_t Shoot_Allow_Flag;                //0--不能发射   1--可以发射
	char   Bullet_Speed;                    //弹丸速度
	char   Locked_Flag;                     //拨盘堵转标志
	char   Which_Color_I_Am;                //0---红     1---蓝
	char   Enemy_Armor_ID;
	u8     Enemy_State;                //敌方机器人的存活状态
	u8     Enemy_HP_HL;                //敌方机器人血量从高到低排列，01表1号英雄，11表3号步兵，00表四号步兵，10表5号步兵,低位血量低
}Up_To_Down_Typedef;

typedef struct
{
	int8_t Target_Locked;                  //0-没有找到目标，1-找到目标，2-锁定目标
	char   Enemy_Armor_ID;
	short  Yaw_Angle_Real;
	char   NUC_Shoot_Allow_Flag;
	short   Distace;
}Down_To_Up_Typedef;

#endif