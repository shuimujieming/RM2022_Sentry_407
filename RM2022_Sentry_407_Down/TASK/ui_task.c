#include "ui_task.h"

TaskHandle_t UI_Task_Handler;
void ui_task(void *p_arg);

/*
*@title：自定义UI任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void ui_task_create()
{
		xTaskCreate((TaskFunction_t )ui_task,          //任务函数
							(const char*    )"ui_task",          //任务名称
							(uint16_t       )UI_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )UI_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&UI_Task_Handler);  //任务句柄  
}

/*
*@title：自定义UI任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void ui_task(void *p_arg)
{
	const TickType_t TimeIncrement=pdMS_TO_TICKS(10);
	TickType_t	PreviousWakeTime;
	PreviousWakeTime=xTaskGetTickCount();	
	
	while(1)
	{
		//绘制UI
		Show_UI();
		vTaskDelayUntil(&PreviousWakeTime,TimeIncrement);	
	}

}