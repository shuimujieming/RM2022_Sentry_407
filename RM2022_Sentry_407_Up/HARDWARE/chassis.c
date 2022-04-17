#include "chassis.h"

/*
*@Description�����̳�ʼ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Init()
{

	for(int i = 0;i<4;i++)
	{	
		Pid_Reset(&PID_Chassis[i]);
		//ע����ٱ�
		Pid_Set(&PID_Chassis[i],7.0f,0.1f,0,16000,500,5000,16000,1,5000,0,0);
	}
	
	Pid_Reset(&PID_Chassis_Omega);
	
	Pid_Set(&PID_Chassis_Omega,100.0f,0,100.0f,16000,50,16000,7000,1,16000,0,2); 
	
	Pid_Reset(&PID_Storage_Angle);
	Pid_Reset(&PID_Storage_Speed);

	Pid_Set(&PID_Storage_Angle,1.0f,0,0.0f,10000,50,10000,10000,1,10000,0,2); 
	Pid_Set(&PID_Storage_Speed,2.0f,0,0.0f,10000,50,10000,10000,1,10000,0,2); 
	
}

