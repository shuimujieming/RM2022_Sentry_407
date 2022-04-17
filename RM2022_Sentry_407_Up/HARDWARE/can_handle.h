#ifndef __CAN_HANDLE_H
#define __CAN_HANDLE_H
#include "main.h"


typedef struct	
{
	short Current_Speed;
	short Current_MechAngle;
	short Current;
	short Temperature;
	short Critical_MechAngle;
	short Origin_MechAngle;
	short Target_Current;
	short Target_MechAngle;
	short Last_MechAngle;
}CAN_Data_TypeDef;

extern CAN_Data_TypeDef CAN_Chassis[4];
extern CAN_Data_TypeDef CAN_Gimbal[2];	//YAW 205 PITCH 209
extern CAN_Data_TypeDef CAN_Shoot[2];	//Trigger 0x206 Left_Fric 0x207 Right_Fric 0x208
extern CAN_Data_TypeDef CAN_Trigger[2];
extern CAN_Data_TypeDef CAN_Storage;

void CAN1_TX_Chassis(void);
void CAN1_TX_Shoot(void);
void CAN1_TX_Gimbal(void);
void CAN2_TX_DualBoard(void);



#endif
