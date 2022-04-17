#include "led_task.h"

TaskHandle_t LED_Task_Handler;
void led_task(void *p_arg);

/*
*@title��LED���񴴽�
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void led_task_create()
{
		xTaskCreate((TaskFunction_t )led_task,          //������
							(const char*    )"led_task",          //��������
							(uint16_t       )LED_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                //���ݸ��������Ĳ���
							(UBaseType_t    )LED_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&LED_Task_Handler);  //������  
}

/*
*@title��LED����
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void led_task(void *p_arg)
{
	int led_frequency = 0;
	
	while(1)
	{
		//led��Ƶ�ʿ���
		led_frequency++;
		
		if(led_frequency % 1 == 0)
		{LED_Run();}	

		//����������
		if(led_frequency == 1)
		{
			Beep_Ctrl(1000,10);			
		}
		else if(led_frequency == 2)
		{
			Beep_Ctrl(850,10);						
		}
		else if(led_frequency == 3)
		{
			Beep_Ctrl(700,10);						
		}
		else if(led_frequency == 4)
		{
			Beep_Ctrl(550,10);						
		}
		else if(led_frequency == 5)
		{
			Beep_Ctrl(400,10);						
		}
		else if(led_frequency == 6)
		{
			Beep_Ctrl(250,10);						
		}
		else if(led_frequency == 7)
		{
			Beep_Ctrl(150,10);						
		}
		else
		{
			Beep_Ctrl(50,0);									
		}		
		
		vTaskDelay(500);
	}

}
