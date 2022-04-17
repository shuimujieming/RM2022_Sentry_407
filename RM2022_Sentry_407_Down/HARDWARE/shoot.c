#include "shoot.h"


void Shoot_Init()
{
	Pid_Reset(&PID_Shoot[0]);
	Pid_Reset(&PID_Shoot[1]);
	Pid_Reset(&PID_Trigger[0]);
	Pid_Reset(&PID_Trigger[1]);
	
	
	Pid_Set(&PID_Shoot[0],10,0,0,16000,50,16000,16000,1,16000,0,2); 
	Pid_Set(&PID_Shoot[1],10,0,0,16000,50,16000,16000,1,16000,0,2);
	Pid_Set(&PID_Trigger[0],10,0,0,16000,50,16000,16000,1,16000,0,2);
	Pid_Set(&PID_Trigger[1],10,0,0,16000,50,16000,16000,1,16000,0,2);
	
}