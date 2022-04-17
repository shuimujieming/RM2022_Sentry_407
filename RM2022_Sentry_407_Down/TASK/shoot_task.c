#include "shoot_task.h"

Shoot_Mode_Enum Shoot_Mode;

void Shoot_Stop_Control();
void Shoot_Run_Control();

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
/*
*@Description����ȡ����Ƶ��
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/

Cover_Mode_Enum Cover_Mode;

/*
*@title����������
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
*@Description��������������
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
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
		Shoot_Speed = 0;			
	}
	else
	{
		Cover_Value = COVER_OPEN_VAL;	
		Shoot_Speed = 4000;	
	}

	//�ϰ巢�����Ľ�ֹ�����־λ������ϵͳ����
//	if(Up_To_Down_Data.Shoot_Allow_Flag == 0)
//	{
//		Cover_Value = COVER_CLOSE_VAL;		
//	}

	//�������
	//����д������Ϊ�˷�ֹ������Ƶ�PWMƵ������
	COVER_CCR = Cover_Value;
	
	//���俨������
	//�Ϲ���
	Bullet_Block_Control();
	
	//�¹���
	CAN_Trigger[0].Target_Current = Pid_Calc(&PID_Trigger[0],CAN_Trigger[0].Current_Speed,Shoot_Speed);							

	
	CAN_Shoot[0].Target_Current = Pid_Calc(&PID_Shoot[0],CAN_Shoot[0].Current_Speed,-9000);
	CAN_Shoot[1].Target_Current = Pid_Calc(&PID_Shoot[1],CAN_Shoot[1].Current_Speed,9000);

}


//��ת���ڼ�ʱ
int Block_Time = 0;
//��תʱ���ʱ
int Block_Reverse_Time = 0;

//�¹���������
void Bullet_Block_Control()
{
	//�����������ʱ��ⷴת
	if(CAN_Trigger[0].Current > 10000)
	{
		Block_Time ++;
	}
	
	//��ת
	if(Block_Reverse_Time != 0)
	{
		Block_Reverse_Time += 1;
		
		//��תʱ�� 3 * 2ms = 6ms
		if(Block_Reverse_Time > 30)
		{
			Block_Reverse_Time = 0;
			Block_Time = 0;
		}
		else
		{
			//��תʱ������Сʱ������ת����ֹ��ת��ת
			if(CAN_Trigger[0].Current > -4000)
			{Shoot_Speed = -2000;}
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
