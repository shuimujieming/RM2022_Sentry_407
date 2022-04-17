#include "chassis_task.h"
void Chassis_Speed_Calc(float vx,float vy,float vw);
void Chassis_Follow_Control();
void Chassis_Stop_Control();
void Chassis_Auto_Control();

TaskHandle_t CHASSIS_Task_Handler;
void chassis_task(void *p_arg);

Chassis_Speed_Typedef Chassis_Speed;
Chassis_Mode_Enum Chassis_Mode;
Chassis_Control_Speed_Typedef rc;
Chassis_Control_Speed_Typedef keyboard;	
/*
*@title：底盘任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void chassis_task_create()
{
		xTaskCreate((TaskFunction_t )chassis_task,          //任务函数
							(const char*    )"chassis_task",          //任务名称
							(uint16_t       )CHASSIS_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )CHASSIS_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&CHASSIS_Task_Handler);  //任务句柄  
}

/*
1.随动模式
2.自瞄模式
3.陀螺模式
4.无力模式
*/

/*
*@title：底盘任务
*@description：周期3ms
*@param 1：	
*@param 2：	
*@return:：	
*/
void chassis_task(void *p_arg)
{
	
	const TickType_t TimeIncrement=pdMS_TO_TICKS(3);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		switch(Chassis_Mode)
		{
			//跟随模式
			case CHASSIS_MODE_FOLLOW:
				Chassis_Follow_Control();
				break;
			//陀螺模式
			case CHASSIS_MODE_AUTO:
				Chassis_Auto_Control();
				break;
			//无力模式
			case CHASSIS_MODE_STOP:
				Chassis_Stop_Control();
				break;
			default:
				break;
		}
		
		//底盘速度解算
		Chassis_Speed_Calc(Chassis_Speed.vx,Chassis_Speed.vy,Chassis_Speed.vw);
		
		//底盘功率控制（200J的缓冲能量，使用到缓冲能量则开始限制底盘速度）
		if(power_heat_data.chassis_power_buffer < 200)
		{
			for(int i =0;i<4;i++)
			{
				Chassis_Speed.wheel_speed[i] *= power_heat_data.chassis_power_buffer/200.0f;
			}
		}
		
		CAN_Chassis[0].Target_Current = Pid_Calc(&PID_Chassis[0], CAN_Chassis[0].Current_Speed, Chassis_Speed.wheel_speed[0]);		

		//CAN数据更新发送
		xEventGroupSetBits(EventGroupHandler,CHASSIS_SIGNAL);	//底盘事件组置位		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
//两侧距离传感器状态
int left_status = 0;
int right_status = 0;
/*
*@Description：底盘跟随控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Follow_Control()
{
	left_status = Left_Ultrasonic;
	right_status = Right_Ultrasonic;

	//战车速度分量赋值
	Chassis_Speed.vy = rc.vy;
	
	if(left_status == 0 && Chassis_Speed.vy < 0)
	{
		Chassis_Speed.vy = 0;
	}
	
	if(right_status == 0 && Chassis_Speed.vy > 0)
	{
		Chassis_Speed.vy = 0;
	}
}

//底盘移动方向
int move_direction = 1;
//底盘移动到的目标位置时间
int rand_move_position = 0;
//随机移动标志位
int rand_once_flag = 0;
//单次摇摆时间
int round_time = 500;
/*
*@Description：底盘自动控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Auto_Control()
{
	//传感器状态检测
	left_status = Left_Ultrasonic;
	right_status = Right_Ultrasonic;
	
	//运动速度上限
	Chassis_Speed.vy = 10000 * move_direction;

	
	/*
	底盘运动随机来回摆动策略
	*/
	//随机数种子
	srand(0);
	//随机数产生
	if(rand_once_flag == 0)
	{
		//1500ms - 3000ms
		rand_move_position = (abs(rand()) % 5) * 300;
		
		rand_once_flag = 1;
	}
	//随机位置需要运动时间的计时
	rand_move_position --;
	if(rand_move_position <=0){rand_move_position = 0;}
	//到达位置
	if(rand_move_position <= 0)
	{
		//摇摆一次完整的轨迹
		round_time --;
		if(round_time == 0)
		{
			move_direction*=-1;
		}
		if(round_time == -500)
		{
			//回到位置点
			move_direction*=-1;
			rand_once_flag = 0;
			round_time = 500;
		}
	}
	
	
	
	//战车速度分量赋值
	//防止撞柱子	
	if(left_status == 0){move_direction = 1;}
	if(right_status == 0){move_direction = -1;}
	
}
/*
*@Description：底盘无力控制
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Stop_Control()
{
	Chassis_Speed.vx = 0;
	Chassis_Speed.vy = 0;
	Chassis_Speed.vw = 0;
}



/*
*@Description：底盘速度解算（麦轮）
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void Chassis_Speed_Calc(float vx,float vy,float vw)
{
	float Speed_Max = 0;
	
	Chassis_Speed.wheel_speed[0] = +vx - vy + vw;
	Chassis_Speed.wheel_speed[1] = -vx - vy + vw;
	Chassis_Speed.wheel_speed[2] = -vx + vy + vw;
	Chassis_Speed.wheel_speed[3] = +vx + vy + vw;
	
	//速度限幅调整
	//最大值寻找
	for(int i = 0;i<4;i++)
	{
		if(abs(Chassis_Speed.wheel_speed[i]) > Speed_Max)
		{
			Speed_Max = abs(Chassis_Speed.wheel_speed[i]);
		}
	}
	//等比例下调速度
	if(Speed_Max > 10000)
	{
		for(int i = 0;i<4;i++)
		{
			Chassis_Speed.wheel_speed[i] *= 10000/Speed_Max;
		}
	}
	
}