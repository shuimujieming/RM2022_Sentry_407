#include "task_init.h"

TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

EventGroupHandle_t EventGroupHandler;
/*
*@title：启动任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void start_task_create()
{
		xTaskCreate((TaskFunction_t )start_task,            //任务函数
							(const char*    )"start_task",          //任务名称
							(uint16_t       )START_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                  //传递给任务函数的参数
							(UBaseType_t    )START_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
		
	  vTaskStartScheduler();          //开启任务调度			
}
/*
*@title：启动任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void start_task(void *pvParameters)
{
	
	EventGroupHandler = xEventGroupCreate();	//创建事件组

	while(1)
	{
		taskENTER_CRITICAL();	//进入临界区
		
		led_task_create();		//创建LED灯任务
		
		imu_task_create();		//创建IMU任务
		
		gimbal_task_create(); //创建云台任务
				
		shoot_task_create();  //创建发射任务
		
		nuc_task_create();		//创建NUC发送任务
		
		can_send_task_create();//创建CAN数据发送任务
		
		mode_task_create();		 //创建模式状态更新任务
		
		vTaskDelete(StartTask_Handler);	//删除开始任务
		
		taskEXIT_CRITICAL();	//退出临界区
	}
	
}
//操作系统任务初始化
void Task_Init()
{
	start_task_create();	//开始任务
}