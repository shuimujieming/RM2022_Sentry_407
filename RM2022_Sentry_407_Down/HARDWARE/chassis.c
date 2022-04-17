#include "chassis.h"

/*
*@Description：底盘初始化
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Init()
{

	for(int i = 0;i<4;i++)
	{	
		Pid_Reset(&PID_Chassis[i]);
		//注意减速比
		Pid_Set(&PID_Chassis[i],7.0f,0.1f,0,16000,500,5000,16000,1,5000,0,0);
	}
	
	Pid_Reset(&PID_Chassis_Omega);
	
	Pid_Set(&PID_Chassis_Omega,100.0f,0,100.0f,16000,50,16000,7000,1,16000,0,2); //Pitch
}