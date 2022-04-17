#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control(void);
void Shoot_Run_Control(void);
void Shoot_Run_Down_Control(void);

TaskHandle_t SHOOT_Task_Handler;
void shoot_task(void *p_arg);

/*
*@title：发射任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void shoot_task_create()
{
		xTaskCreate((TaskFunction_t )shoot_task,          //任务函数
							(const char*    )"shoot_task",          //任务名称
							(uint16_t       )SHOOT_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )SHOOT_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&SHOOT_Task_Handler);  //任务句柄  
}
Cover_Mode_Enum Cover_Mode;
/*
*@title：发射任务，判断模式进入各自程序
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void shoot_task(void *p_arg)
{
		EventBits_t  EventValue;
	
	while(1)
	{
		
		EventValue = xEventGroupWaitBits((EventGroupHandle_t)EventGroupHandler,
																	(EventBits_t      )SHOOT_EXE_SIGNAL,
																	(BaseType_t       )pdTRUE,
																	(BaseType_t       )pdFALSE,
																	(TickType_t       )portMAX_DELAY);
		//下云台发射控制
		Shoot_Run_Down_Control();       //不超热量
		switch(Shoot_Mode)
		{
			//发射停止
			case SHOOT_MODE_STOP:
				Shoot_Stop_Control();
				break;
			//发射运行
			case SHOOT_MODE_RUN:
				Shoot_Run_Control();
				break;

			default:
				break;
		}

		xEventGroupSetBits(EventGroupHandler,SHOOT_SIGNAL);	//发射事件组置位
	}
}
/*
*@Description：发射停止控制，
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Stop_Control()
{
	CAN_Trigger[1].Target_Current = Pid_Calc(&PID_Trigger[1],CAN_Trigger[1].Current_Speed,0);

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,0);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,0);		
}

void Bullet_Block_Control(void);

int Shoot_Frequency_Speed = 0;    //射频

extern NUC_Typedef NUC_Data;

int Cover_Value = COVER_CLOSE_VAL;
extern Down_To_Up_Typedef  Down_To_Up_Data;
extern Up_To_Down_Typedef  Up_To_Down_Data;

int shoot_speed_test = 9000;

/*
*@Description：发射运行控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Run_Control()
{
	//根据控制模式确定舵机是否打开
	if(Cover_Mode == COVER_MODE_CLOSE)
	{
		Cover_Value = COVER_CLOSE_VAL;
		Shoot_Frequency_Speed = 0;			
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;	
		Shoot_Frequency_Speed = 5000;	  //拨弹盘转速5000
	}
	//发射卡弹控制
	//上供弹
	Bullet_Block_Control();           //如果出现卡弹的现象，改变Shoot_Frequency_Speed=-2000，进行控制
	
//注掉是因为按照现实情况，怎么发射也不会超热量
//热量限制（防止超热量）留出三颗弹丸余量，保证高射频下不超热量
//	if(game_robot_state.shooter_id1_17mm_cooling_limit < power_heat_data.shooter_id1_17mm_cooling_heat + 30)
//	{
//		Cover_Value = COVER_CLOSE_VAL;
//		Shoot_Frequency_Speed = 0;
//		//关闭舵机
//	}

	//舵机控制
	//单独写这句控制为了防止舵机控制的PWM频繁跳动
	COVER_CCR = Cover_Value;
	

	//上供弹
	CAN_Trigger[1].Target_Current = Pid_Calc(&PID_Trigger[1],CAN_Trigger[1].Current_Speed,Shoot_Frequency_Speed);							

	
	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,shoot_speed_test);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,-shoot_speed_test);
	
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		CAN_Trigger[1].Target_Current = 0;	
	}
}

void Bullet_Block_Down_Control(void);

/*
*@Description：下云台发射控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Run_Down_Control()
{
	//热量限制（防止超热量）留出三颗弹丸余量，保证高射频下不超热量
	if(game_robot_state.shooter_id2_17mm_cooling_limit < power_heat_data.shooter_id2_17mm_cooling_heat + 30)
	{
		//告诉下板现在禁止发射
		Up_To_Down_Data.Shoot_Allow_Flag = 0;
	}	
	else
	{
		//允许发射
		Up_To_Down_Data.Shoot_Allow_Flag = 1;
	}
}

//堵转周期计时
int Block_Time = 0;
//反转时间计时
int Block_Reverse_Time = 0;

//上供弹拨弹盘，防止卡弹的程序
void Bullet_Block_Control()
{
	//电机电流过大时检测反转
	if(CAN_Trigger[1].Current > 10000)
	{
		Block_Time ++;
	}
	

	
	//反转
	if(Block_Reverse_Time >= 1)
	{
		Block_Reverse_Time++;
		
		//反转时间 3 * 2ms = 6ms
		if(Block_Reverse_Time > 30)
		{
			Block_Reverse_Time = 0;
			Block_Time = 0;
		}
		else
		{
			//反转时电流较小时才允许反转，防止反转堵转
			if(CAN_Trigger[1].Current > -4000)
			{Shoot_Frequency_Speed = -2000;}    //执行反转
			//电流较大恢复正转
			else
			{
				Block_Reverse_Time = 0;
				Block_Time = 0;
			}
		}
	}
	else
	{
			//堵转时间10*发射任务周期（2ms）= 20ms
	if(Block_Time == 10)
	{
		Block_Reverse_Time = 1;
	}
	}
		
	
}
