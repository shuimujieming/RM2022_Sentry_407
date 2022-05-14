#include "mode_task.h"

void Get_Chassis_Mode(void);
void Get_Gimbal_Mode(void);
void Get_Chassis_RC(void);
void Get_Shoot_Mode(void);
void Get_Cover_Mode(void);


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


extern Chassis_Mode_Enum Chassis_Mode;
extern Gimbal_Mode_Enum Gimbal_Mode;
extern Shoot_Mode_Enum Shoot_Mode;
extern Cover_Mode_Enum Cover_Mode;

extern Chassis_Control_Speed_Typedef rc;
extern Chassis_Control_Speed_Typedef keyboard;	


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
		
		//底盘速度获取
		Get_Chassis_RC();		
		
		//各功能模式状态更新
		Get_Gimbal_Mode();
		Get_Chassis_Mode();
		Get_Shoot_Mode();
		Get_Cover_Mode();
		
		//信号量递减，在此之前读值应该不为0,否则视为遥控器断电
		DR16_Signal --;
		if(DR16_Signal <= 0)
		{
			Chassis_Mode = CHASSIS_MODE_STOP;
			Gimbal_Mode = GIMBAL_MODE_STOP;
			Shoot_Mode = SHOOT_MODE_STOP;
			DR16_Signal = 0;
		}
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
//遥控器获取底盘速度
void Get_Chassis_RC()
{
	//手动限制最大速度
	rc.vx = 0;
	rc.vy = DBUS.RC.ch0 / 660.0f * 10000.0f;
	rc.vw = 0;
	
}
//底盘状态标志位
typedef enum
{
	CHASSIS_STATUS_STOP = 0,
	CHASSIS_STATUS_ROTATE,
	CHASSIS_STATUS_FOLLOW,

}Chassis_Status_Enum;
//底盘状态（按键用）
Chassis_Status_Enum Chassis_Status = CHASSIS_STATUS_FOLLOW;

/*
*@Description：获取底盘模式
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Get_Chassis_Mode()
{	
	//遥控器底盘模式控制
	if(DBUS.RC.Switch_Left == RC_SW_UP )
	{
		Chassis_Mode = CHASSIS_MODE_FOLLOW;
	}
	if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_MID)
	{
		Chassis_Mode = CHASSIS_MODE_AUTO;
	}
	//双下底盘速度为0
	if(DBUS.RC.Switch_Left == RC_SW_DOWN)
	{
		Chassis_Mode = CHASSIS_MODE_STOP;
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
	//云台模式获取
	//左上右上时上云台可遥控器操作
	if(DBUS.RC.Switch_Left == RC_SW_UP && DBUS.RC.Switch_Right == RC_SW_UP)
	{
		Gimbal_Mode = GIMBAL_MODE_FOLLOW;
	}
	
	//左中右上，上云台自瞄模式
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_UP)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	
	//左中右中，上云台自瞄模式	
	else if(DBUS.RC.Switch_Left == RC_SW_MID && DBUS.RC.Switch_Right == RC_SW_MID)
	{
		Gimbal_Mode = GIMBAL_MODE_AUTOAIM;
	}
	
	//其它键位下云台为保持模式
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






int Shoot_Ready_Flag = 0; //通过遥控器控制
/*
*@Description：获取发射模式，DBUS.RC.ch4在向上拨的时候是负数0~-600，每拨一次就改变一次状态
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Get_Shoot_Mode()
{
	
	 //上云台，右拨杆位于上边才有效
		if(DBUS.RC.Switch_Right == RC_SW_UP)
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

			//左下停止
				if(DBUS.RC.Switch_Left == RC_SW_DOWN)
			{
				Shoot_Mode = SHOOT_MODE_STOP;
			}
}

extern NUC_Typedef NUC_Data;

//舵机开关翻转标志位
int Cover_Ready_Flag = 0; //通过遥控器控制
/*
*@Description：改变舵机开关模式，同时与shoot联动，//DBUS.RC.ch4在向上拨的时候是正数0~600，每拨一次就改变一次状态
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Get_Cover_Mode()
{
	 //上云台，右拨杆位于上边才有效
	if(DBUS.RC.Switch_Right == RC_SW_UP)
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
		//自动
		if(NUC_Data.Auto_Shoot == 1)
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
	}
	//摩擦轮关则限位舵机也关，所以只向下拨或者在把shoot关掉之后舵机就关。
	if(Shoot_Mode == SHOOT_MODE_STOP)
	{
		COVER_CCR = COVER_CLOSE_VAL;
		Cover_Mode = COVER_MODE_CLOSE;
	}
}
