#include "gimbal_task.h"

TaskHandle_t GIMBAL_Task_Handler;
void gimbal_task(void *p_arg);
Gimbal_Mode_Enum Gimbal_Mode;

void Gimbal_Stop_Control();
void Gimbal_Follow_Control();
void Gimbal_AutoAim_Control();
void Gimbal_Keep_Control();

/*
*@title：云台任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void gimbal_task_create()
{
		xTaskCreate((TaskFunction_t )gimbal_task,          //任务函数
							(const char*    )"gimbal_task",          //任务名称
							(uint16_t       )GIMBAL_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )GIMBAL_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&GIMBAL_Task_Handler);  //任务句柄  
}


/*
1.随动模式
2.自瞄模式
3.陀螺模式
4.无力模式
*/
/*
*@title：云台任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void gimbal_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		switch(Gimbal_Mode)
		{
			//云台跟随模式
			case GIMBAL_MODE_FOLLOW:
				Gimbal_Follow_Control();
				break;
			//云台自瞄模式
			case GIMBAL_MODE_AUTOAIM:
				Gimbal_AutoAim_Control();
				break;
			//云台无力模式
			case GIMBAL_MODE_STOP:
				Gimbal_Stop_Control();
				break;
			//云台保持模式
			case GIMBAL_MODE_KEEP:
				Gimbal_Keep_Control();
				break;
			default:
				break;
		}
		
		xEventGroupSetBits(EventGroupHandler,GIMBAL_SIGNAL);	//云台事件组置位
		xEventGroupSetBits(EventGroupHandler,SHOOT_EXE_SIGNAL);	//发射任务处理置位
	
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
float Yaw_Aim_Angle = 0.0;
float Pitch_Aim_Angle ;
float Pitch_Angle_Max ;
float Pitch_Angle_Min ;

//云台手瞄控制
/*
*@Description：云台跟随控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
short test_data = 0;

void Gimbal_Follow_Control()
{
	//+
		Pitch_Aim_Angle += DBUS.RC.ch3 / 50.0f;
	
		if(Pitch_Aim_Angle > Pitch_Angle_Max)
		{Pitch_Aim_Angle = Pitch_Angle_Max;}
		else if(Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Aim_Angle = Pitch_Angle_Min;}			
			
		PID_Gimbal_Angle[1].PIDout = Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Pitch_Aim_Angle);
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],mpu_data.gy * 0.1f,PID_Gimbal_Angle[1].PIDout);
		test_data = CAN_Gimbal[1].Target_Current;
	//+
		Yaw_Aim_Angle +=DBUS.RC.ch2/ 500.0f;
	
		
		PID_Gimbal_Angle[0].PIDout = Pid_Calc(&PID_Gimbal_Angle[0],Yaw_Angle,Yaw_Aim_Angle);
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],mpu_data.gz * 0.1f,PID_Gimbal_Angle[0].PIDout);
}

extern NUC_Typedef NUC_Data;
float Pitch_Speed = 6.0;
float Yaw_Speed = 0.25;
//云台自瞄控制
/*
*@Description：云台自瞄控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Gimbal_AutoAim_Control()
{
	
		//视觉识别到目标机器人 开启自瞄
	if(NUC_Data.Armor_Type != 0)
	{
		Pitch_Aim_Angle = CAN_Gimbal[1].Current_MechAngle;
		Pitch_Aim_Angle -=NUC_Data.Pitch_Angle;
	
	
		if(Pitch_Aim_Angle > Pitch_Angle_Max)
		{Pitch_Aim_Angle = Pitch_Angle_Max;}
		else if(Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Aim_Angle = Pitch_Angle_Min;}			
			
		
		Yaw_Aim_Angle = Yaw_Angle;
		Yaw_Aim_Angle -=NUC_Data.Yaw_Angle;
				
	}
	//视觉未识别到目标时自动摇头
	else
	{
		Pitch_Aim_Angle += Pitch_Speed;
		if(Pitch_Aim_Angle > Pitch_Angle_Max || Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Speed = -Pitch_Speed;}
		
		
		Yaw_Aim_Angle += Yaw_Speed;
	}
		
		PID_Gimbal_Angle[1].PIDout = Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Pitch_Aim_Angle);
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],mpu_data.gy * 0.1f,PID_Gimbal_Angle[1].PIDout);
	
		
		PID_Gimbal_Angle[0].PIDout = Pid_Calc(&PID_Gimbal_Angle[0],Yaw_Angle,Yaw_Aim_Angle);
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],mpu_data.gz * 0.1f,PID_Gimbal_Angle[0].PIDout);
}

/*
*@Description：云台跟随控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Gimbal_Keep_Control()
{
		if(Pitch_Aim_Angle > Pitch_Angle_Max)
		{Pitch_Aim_Angle = Pitch_Angle_Max;}
		else if(Pitch_Aim_Angle < Pitch_Angle_Min)
		{Pitch_Aim_Angle = Pitch_Angle_Min;}			
		
		Pitch_Aim_Angle = CAN_Gimbal[1].Current_MechAngle;			
		Yaw_Aim_Angle = Yaw_Angle;
		
		PID_Gimbal_Angle[1].PIDout = Pid_Calc(&PID_Gimbal_Angle[1],CAN_Gimbal[1].Current_MechAngle,Pitch_Aim_Angle);
		CAN_Gimbal[1].Target_Current = Pid_Calc(&PID_Gimbal_Speed[1],mpu_data.gy * 0.1f,PID_Gimbal_Angle[1].PIDout);
			
		
		PID_Gimbal_Angle[0].PIDout = Pid_Calc(&PID_Gimbal_Angle[0],Yaw_Angle,Yaw_Aim_Angle);
		CAN_Gimbal[0].Target_Current = Pid_Calc(&PID_Gimbal_Speed[0],mpu_data.gz * 0.1f,PID_Gimbal_Angle[0].PIDout);
}
/*
*@Description：云台无力控制 
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Gimbal_Stop_Control()
{
	CAN_Gimbal[0].Target_Current = 0;
	CAN_Gimbal[1].Target_Current = 0;
}