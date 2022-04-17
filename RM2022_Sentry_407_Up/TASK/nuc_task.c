#include "nuc_task.h"

TaskHandle_t NUC_Task_Handler;
void nuc_task(void *p_arg);

/*
*@title：NUC任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void nuc_task_create()
{
		xTaskCreate((TaskFunction_t )nuc_task,          //任务函数
							(const char*    )"nuc_task",          //任务名称
							(uint16_t       )NUC_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )NUC_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&NUC_Task_Handler);  //任务句柄  
}

/*
*@title：NUC任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void nuc_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(2);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		NUC_Send_Data();
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}
