#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control();
void Shoot_Run_Control();

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
/*
*@Description：获取运行频率
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/

Cover_Mode_Enum Cover_Mode;

/*
*@title：发射任务
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
*@Description：发射无力控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Shoot_Stop_Control()
{
	CAN_Trigger[0].Target_Current = Pid_Calc(&PID_Trigger[0],CAN_Trigger[0].Current_Speed,0);

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,0);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,0);		
}

int Cover_Value = COVER_CLOSE_VAL;
int Shoot_Speed = 0;
void Bullet_Block_Control();
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
		Shoot_Speed = 0;			
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;	
		Shoot_Speed = 4000;	
	}

	//上板发送来的禁止发射标志位，裁判系统热量
//	if(Up_To_Down_Data.Shoot_Allow_Flag == 0)
//	{
//		Cover_Value = COVER_CLOSE_VAL;		
//	}

	//舵机控制
	//单独写这句控制为了防止舵机控制的PWM频繁跳动
	COVER_CCR = Cover_Value;
	
	//发射卡弹控制
	//上供弹
	Bullet_Block_Control();
	
	//下供弹
	CAN_Trigger[0].Target_Current = Pid_Calc(&PID_Trigger[0],CAN_Trigger[0].Current_Speed,Shoot_Speed);							

	
	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,-9000);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,9000);

}


//堵转周期计时
int Block_Time = 0;
//反转时间计时
int Block_Reverse_Time = 0;

//下供弹拨弹盘
void Bullet_Block_Control()
{
	//电机电流过大时检测反转
	if(CAN_Trigger[0].Current > 10000)
	{
		Block_Time ++;
	}
	
	//反转
	if(Block_Reverse_Time != 0)
	{
		Block_Reverse_Time += 1;
		
		//反转时间 3 * 2ms = 6ms
		if(Block_Reverse_Time > 30)
		{
			Block_Reverse_Time = 0;
			Block_Time = 0;
		}
		else
		{
			//反转时电流较小时才允许反转，防止反转堵转
			if(CAN_Trigger[0].Current > -4000)
			{Shoot_Speed = -2000;}
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
