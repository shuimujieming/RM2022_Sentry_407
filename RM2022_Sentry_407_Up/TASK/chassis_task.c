#include "chassis_task.h"
void Chassis_Speed_Calc(float vx,float vy,float vw);
void Chassis_Follow_Control(void);
void Chassis_Stop_Control(void);
void Chassis_Auto_Control(void);

TaskHandle_t CHASSIS_Task_Handler;
void chassis_task(void *p_arg);

Chassis_Speed_Typedef Chassis_Speed;
Chassis_Mode_Enum Chassis_Mode;
Chassis_Control_Speed_Typedef rc;
Chassis_Control_Speed_Typedef keyboard;	
/*
*@title���������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void chassis_task_create()
{
		xTaskCreate((TaskFunction_t )chassis_task,          //������
							(const char*    )"chassis_task",          //��������
							(uint16_t       )CHASSIS_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )CHASSIS_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&CHASSIS_Task_Handler);  //������  
}

/*
1.�涯ģʽ
2.����ģʽ
3.����ģʽ
4.����ģʽ
*/
extern int Storage_Init_Angle;
extern int Storage_Angle;
int storage_time = 0;

/*
*@title����������
*@description������3ms
*@param 1��	
*@param 2��	
*@return:��	
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
			//����ģʽ
			case CHASSIS_MODE_FOLLOW:
				Chassis_Follow_Control();
				break;
			//����ģʽ
			case CHASSIS_MODE_AUTO:
				Chassis_Auto_Control();
				break;
			//����ģʽ
			case CHASSIS_MODE_STOP:
				Chassis_Stop_Control();
				break;
			default:
				break;
		}
		
		//�����ٶȽ���
		Chassis_Speed_Calc(Chassis_Speed.vx,Chassis_Speed.vy,Chassis_Speed.vw);
		
		//���̹��ʿ��ƣ�200J�Ļ���������ʹ�õ�����������ʼ���Ƶ����ٶȣ�
		if(power_heat_data.chassis_power_buffer < 200)
		{
			for(int i =0;i<4;i++)
			{
				Chassis_Speed.wheel_speed[i] *= power_heat_data.chassis_power_buffer/200.0f;
			}
		}
		
		if(abs(storage_time)< 350)
		{
			CAN_Chassis[0].Target_Current = Pid_Calc(&PID_Chassis[0], CAN_Chassis[0].Current_Speed, Chassis_Speed.wheel_speed[0]);		
		}	
		else
		{
		CAN_Chassis[0].Target_Current = 0;	
		}
	
		CAN_Storage.Target_Current = Pid_Calc(&PID_Storage_Speed, CAN_Storage.Current_Speed, PID_Storage_Angle.PIDout);					
		
		//CAN���ݸ��·���
		xEventGroupSetBits(EventGroupHandler,CHASSIS_SIGNAL);	//�����¼�����λ
		xEventGroupSetBits(EventGroupHandler,DUAL_SIGNAL);	//������������λ
		
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
//������봫����״̬
int left_status = 0;
int right_status = 0;
/*
*@Description�����̸������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Follow_Control()
{
	left_status = Left_Infrared;
	right_status = Right_Infrared;

	//ս���ٶȷ�����ֵ
	Chassis_Speed.vy = rc.vy;
	
	if(left_status == 0 && Chassis_Speed.vy < 0)
	{
		Chassis_Speed.vy = 0;
	}
	
	if(right_status == 0 && Chassis_Speed.vy > 0)
	{
		Chassis_Speed.vy = 0;
	}
	
	
		CAN_Storage.Target_Current = 0;		

}

//�����ƶ�����
int move_direction = 1;
int move_direction_last = 1;



//�����ƶ�����Ŀ��λ��ʱ��
int rand_move_position = 0;
//����ƶ���־λ
int rand_once_flag = 0;
//����ҡ��ʱ��
int round_time = 500;

extern NUC_Typedef NUC_Data;
extern Down_To_Up_Typedef  Down_To_Up_Data;

/*
*@Description�������Զ�����
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Auto_Control()
{
	//������״̬���
	left_status = Left_Infrared;
	right_status = Right_Infrared;
	
	//�˶��ٶ�����
	Chassis_Speed.vy = 6000 * move_direction;

	
	//���°�֮1 ʶ��Ŀ��ʱȡ���������
	if(NUC_Data.Armor_Type != 0 || Down_To_Up_Data.Target_Locked != 0)
	{
	 rand_once_flag = 0;
	 rand_move_position = 0;
	 round_time = 500;
	}
	
	/*
	�����˶�������ذڶ�����
	*/
	//���������
	srand(0);
	//���������
	if(rand_once_flag == 0)
	{
		//1500ms - 3000ms
		rand_move_position = (abs(rand()) % 5) * 300;
		
		rand_once_flag = 1;
	}
	//���λ����Ҫ�˶�ʱ��ļ�ʱ
	rand_move_position --;
	if(rand_move_position <=0){rand_move_position = 0;}
	//����λ��
	if(rand_move_position <= 0)
	{
		//ҡ��һ�������Ĺ켣
		round_time --;
		if(round_time == 0)
		{
			move_direction*=-1;
		}
		if(round_time == -500)
		{
			//�ص�λ�õ�
			move_direction*=-1;
			rand_once_flag = 0;
			round_time = 500;
		}
	}
	
	//ս���ٶȷ�����ֵ
	//��ֹײ����	
	if(left_status == 0 )
	{
		move_direction = 1;
	}
	
	if(right_status == 0 )
	{
		move_direction = -1;
	}
	
	//�ٶȱ���ʱ
	if(move_direction != move_direction_last)
	{
		//����ʱ
		if(move_direction == 1)
		{
			//2s
			storage_time = 400;
		}
		//����ʱ
		else if(move_direction == -1)
		{
			storage_time = -400;
		}
	}
//	
	if(storage_time < 0)
	{
		storage_time ++;
		Pid_Calc(&PID_Storage_Angle, Storage_Angle, Storage_Init_Angle - 10000);
	}
	else if(storage_time > 0)
	{
		storage_time --;
		Pid_Calc(&PID_Storage_Angle, Storage_Angle, Storage_Init_Angle + 10000);
	}
	else if(storage_time == 0)
	{
    Pid_Calc(&PID_Storage_Angle, Storage_Angle, Storage_Init_Angle);
	}
	

		
	//��¼ǰһ�η���״̬
	move_direction_last = move_direction;
	
}
/*
*@Description��������������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Stop_Control()
{
	left_status = Left_Infrared;
	right_status = Right_Infrared;
	
	Chassis_Speed.vx = 0;
	Chassis_Speed.vy = 0;
	Chassis_Speed.vw = 0;
	
	CAN_Storage.Target_Current = 0;
}



/*
*@Description�������ٶȽ��㣨���֣�
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Chassis_Speed_Calc(float vx,float vy,float vw)
{
	float Speed_Max = 0;
	
	Chassis_Speed.wheel_speed[0] = +vx - vy + vw;
	Chassis_Speed.wheel_speed[1] = -vx - vy + vw;
	Chassis_Speed.wheel_speed[2] = -vx + vy + vw;
	Chassis_Speed.wheel_speed[3] = +vx + vy + vw;
	
	//�ٶ��޷�����
	//���ֵѰ��
	for(int i = 0;i<4;i++)
	{
		if(abs(Chassis_Speed.wheel_speed[i]) > Speed_Max)
		{
			Speed_Max = abs(Chassis_Speed.wheel_speed[i]);
		}
	}
	//�ȱ����µ��ٶ�
	//��������ٶ�
	if(Speed_Max > 8500)
	{
		for(int i = 0;i<4;i++)
		{
			Chassis_Speed.wheel_speed[i] *= 8500/Speed_Max;
		}
	}
	
}
