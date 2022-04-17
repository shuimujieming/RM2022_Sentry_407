#include "mode_task.h"

void Get_Gimbal_Mode();
void Get_Chassis_RC();
void Get_Shoot_Mode();
void Get_Cover_Mode();


TaskHandle_t MODE_Task_Handler;
void mode_task(void *p_arg);

/*
*@title：模式状态更新任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void mode_task_create()
{
		xTaskCreate((TaskFunction_t )mode_task,          //任务函数
							(const char*    )"mode_task",          //任务名称
							(uint16_t       )MODE_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )MODE_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&MODE_Task_Handler);  //任务句柄  
}

//遥控器衰减信号
extern int DR16_Signal;

extern Gimbal_Mode_Enum Gimbal_Mode;
extern Shoot_Mode_Enum Shoot_Mode;
extern Cover_Mode_Enum Cover_Mode;

int Cover_Open_Flag = 0; //键盘
int Shoot_Run_Flag = 0; //摩擦轮开关状态标志位
int Chassis_Rotate_Flag = 0;
extern int CAN1_Signal ;
extern int CAN2_Signal ;
/*
*@title：模式状态更新任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void mode_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
				//信号衰减
		CAN1_Signal --;
		CAN2_Signal --;
		
		//各功能模式状态更新
		Get_Gimbal_Mode();
		Get_Shoot_Mode();
		Get_Cover_Mode();
		
		//信号量递减，在此之前读值应该不为0,否则视为遥控器断电
		DR16_Signal --;
		if(DR16_Signal <= 0)
		{
			Gimbal_Mode = GIMBAL_MODE_STOP;
			Shoot_Mode = SHOOT_MODE_STOP;
			DR16_Signal = 0;
		}
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}

/*
*@Description：获取云台模式
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Get_Gimbal_Mode()
{	
	//左上右下
	if(DBUS.RC.Switch_Left == RC_SW_UP && DBUS.RC.Switch_Right == RC_SW_DOWN)
	{
		Gimbal_Mode = GIMBAL_MODE_FOLLOW;
	}
	//左中右下
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_DOWN)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	//自瞄模式
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_MID)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	//保持模式
	else
	{
		Gimbal_Mode = GIMBAL_MODE_KEEP;
	}
	
	//左下无力
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		Gimbal_Mode = GIMBAL_MODE_STOP;
	}
	
}


int Shoot_Ready_Flag = 0; //遥控器
/*
*@Description：获取发射模式
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Get_Shoot_Mode()
{
	
	//下头模式才允许使用
	
		if(DBUS.RC.Switch_Right == RC_SW_DOWN)
	{	
		
	//拨轮控制弹仓盖，往下拨，回正到一定位置后变更一次状态
	if(DBUS.RC.ch4 < -600)
	{
		Shoot_Ready_Flag = 1;
	}
	if(Shoot_Ready_Flag == 1)
	{
		//回正到接近中间位置
		if(DBUS.RC.ch4 > -100 && DBUS.RC.ch4 < 0)
		{
			
			if(Shoot_Mode == SHOOT_MODE_STOP)
			{
				Shoot_Mode = SHOOT_MODE_RUN;
			}
			else if(Shoot_Mode == SHOOT_MODE_RUN)
			{
				Shoot_Mode = SHOOT_MODE_STOP;
			}
			Shoot_Ready_Flag = 0;
		}
	}
	
	}
	
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		Shoot_Mode = SHOOT_MODE_STOP;
	}
	
}



//弹仓盖翻转标志位
int Cover_Ready_Flag = 0; //遥控器
extern NUC_Typedef NUC_Data;

int cover_value = 1000;

/*
*@Description：获取弹仓盖模式
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Get_Cover_Mode()
{
	
	if(DBUS.RC.Switch_Right == RC_SW_DOWN)
	{
		//拨轮控制弹仓盖，往下拨，回正到一定位置后变更一次状态
		if(DBUS.RC.ch4 > 600)
		{
			Cover_Ready_Flag = 1;
		}
		if(Cover_Ready_Flag == 1)
		{
			//回正到接近中间位置
			if(DBUS.RC.ch4 < 100 && DBUS.RC.ch4 > 0)
			{
				if(Cover_Mode == COVER_MODE_CLOSE)
				{
					Cover_Mode = COVER_MODE_OPEN;
				}
				else if(Cover_Mode == COVER_MODE_OPEN)
				{
					Cover_Mode = COVER_MODE_CLOSE;
				}		
				Cover_Ready_Flag = 0;
			}
		}
		
	}
	
	//自瞄模式下自动击打
	if(Gimbal_Mode == GIMBAL_MODE_AUTOAIM)
	{
		if(NUC_Data.NUC_Shoot_Allow_Flag == 1)
		{
			Cover_Mode = COVER_MODE_OPEN;
		}		
		else
		{
			Cover_Mode = COVER_MODE_CLOSE;			
		}
	}
	
	if(Shoot_Mode == SHOOT_MODE_STOP)
	{
		COVER_CCR = COVER_CLOSE_VAL;
		Cover_Mode = COVER_MODE_CLOSE;
	}
	
}

