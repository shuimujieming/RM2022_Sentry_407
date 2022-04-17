#include "supercap_task.h"

TaskHandle_t SUPERCAP_Task_Handler;
void supercap_task(void *p_arg);

/*
*@title：超级电容任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void supercap_task_create()
{
		xTaskCreate((TaskFunction_t )supercap_task,          //任务函数
							(const char*    )"supercap_task",          //任务名称
							(uint16_t       )SUPERCAP_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )SUPERCAP_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&SUPERCAP_Task_Handler);  //任务句柄  
}

/*
*@title：超级电容任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void supercap_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(100);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		
		xEventGroupSetBits(EventGroupHandler,SUPERCAP_SIGNAL);	//云台事件组置位
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}