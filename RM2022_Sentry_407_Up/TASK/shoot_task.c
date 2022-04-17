#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control(void);
void Shoot_Run_Control(void);
void Shoot_Run_Down_Control(void);

TaskHandle_t SHOOT_Task_Handler;
void shoot_task(void *p_arg);

/*
*@title���������񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void shoot_task_create()
{
		xTaskCreate((TaskFunction_t )shoot_task,          //������
							(const char*    )"shoot_task",          //��������
							(uint16_t       )SHOOT_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )SHOOT_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&SHOOT_Task_Handler);  //������  
}
Cover_Mode_Enum Cover_Mode;
/*
*@title�����������ж�ģʽ������Գ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
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
		//����̨�������
		Shoot_Run_Down_Control();       //��������
		switch(Shoot_Mode)
		{
			//����ֹͣ
			case SHOOT_MODE_STOP:
				Shoot_Stop_Control();
				break;
			//��������
			case SHOOT_MODE_RUN:
				Shoot_Run_Control();
				break;

			default:
				break;
		}

		xEventGroupSetBits(EventGroupHandler,SHOOT_SIGNAL);	//�����¼�����λ
	}
}
/*
*@Description������ֹͣ���ƣ�
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Stop_Control()
{
	CAN_Trigger[1].Target_Current = Pid_Calc(&PID_Trigger[1],CAN_Trigger[1].Current_Speed,0);

	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,0);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,0);		
}

void Bullet_Block_Control(void);

int Shoot_Frequency_Speed = 0;    //��Ƶ

extern NUC_Typedef NUC_Data;

int Cover_Value = COVER_CLOSE_VAL;
extern Down_To_Up_Typedef  Down_To_Up_Data;
extern Up_To_Down_Typedef  Up_To_Down_Data;

int shoot_speed_test = 9000;

/*
*@Description���������п���
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Run_Control()
{
	//���ݿ���ģʽȷ������Ƿ��
	if(Cover_Mode == COVER_MODE_CLOSE)
	{
		Cover_Value = COVER_CLOSE_VAL;
		Shoot_Frequency_Speed = 0;			
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;	
		Shoot_Frequency_Speed = 5000;	  //������ת��5000
	}
	//���俨������
	//�Ϲ���
	Bullet_Block_Control();           //������ֿ��������󣬸ı�Shoot_Frequency_Speed=-2000�����п���
	
//ע������Ϊ������ʵ�������ô����Ҳ���ᳬ����
//�������ƣ���ֹ���������������ŵ�����������֤����Ƶ�²�������
//	if(game_robot_state.shooter_id1_17mm_cooling_limit < power_heat_data.shooter_id1_17mm_cooling_heat + 30)
//	{
//		Cover_Value = COVER_CLOSE_VAL;
//		Shoot_Frequency_Speed = 0;
//		//�رն��
//	}

	//�������
	//����д������Ϊ�˷�ֹ������Ƶ�PWMƵ������
	COVER_CCR = Cover_Value;
	

	//�Ϲ���
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
*@Description������̨�������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void Shoot_Run_Down_Control()
{
	//�������ƣ���ֹ���������������ŵ�����������֤����Ƶ�²�������
	if(game_robot_state.shooter_id2_17mm_cooling_limit < power_heat_data.shooter_id2_17mm_cooling_heat + 30)
	{
		//�����°����ڽ�ֹ����
		Up_To_Down_Data.Shoot_Allow_Flag = 0;
	}	
	else
	{
		//������
		Up_To_Down_Data.Shoot_Allow_Flag = 1;
	}
}

//��ת���ڼ�ʱ
int Block_Time = 0;
//��תʱ���ʱ
int Block_Reverse_Time = 0;

//�Ϲ��������̣���ֹ�����ĳ���
void Bullet_Block_Control()
{
	//�����������ʱ��ⷴת
	if(CAN_Trigger[1].Current > 10000)
	{
		Block_Time ++;
	}
	

	
	//��ת
	if(Block_Reverse_Time >= 1)
	{
		Block_Reverse_Time++;
		
		//��תʱ�� 3 * 2ms = 6ms
		if(Block_Reverse_Time > 30)
		{
			Block_Reverse_Time = 0;
			Block_Time = 0;
		}
		else
		{
			//��תʱ������Сʱ������ת����ֹ��ת��ת
			if(CAN_Trigger[1].Current > -4000)
			{Shoot_Frequency_Speed = -2000;}    //ִ�з�ת
			//�����ϴ�ָ���ת
			else
			{
				Block_Reverse_Time = 0;
				Block_Time = 0;
			}
		}
	}
	else
	{
			//��תʱ��10*�����������ڣ�2ms��= 20ms
	if(Block_Time == 10)
	{
		Block_Reverse_Time = 1;
	}
	}
		
	
}
