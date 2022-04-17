#include "can_send_task.h"


TaskHandle_t CAN_SEND_Task_Handler;
void can_send_task(void *p_arg);

/*
*@title：CAN数据发送任务创建
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void can_send_task_create()
{
		xTaskCreate((TaskFunction_t )can_send_task,          //任务函数
							(const char*    )"can_send_task",          //任务名称
							(uint16_t       )CAN_SEND_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                //传递给任务函数的参数
							(UBaseType_t    )CAN_SEND_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&CAN_SEND_Task_Handler);  //任务句柄  
}

/*
*@title：CAN数据发送任务
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void can_send_task(void *p_arg)
{

	EventBits_t  EventValue;

	
	while(1)
	{
		EventValue = xEventGroupWaitBits((EventGroupHandle_t)EventGroupHandler,
																	(EventBits_t      )GIMBAL_SIGNAL,
																	(BaseType_t       )pdTRUE,
																	(BaseType_t       )pdFALSE,
																	(TickType_t       )portMAX_DELAY);
		
			//云台、底盘、弹仓、发射、超级电容CAN数据等待发送
		if(EventValue & GIMBAL_SIGNAL)
		{
			CAN1_TX_Gimbal();
			CAN1_TX_Shoot();
			CAN2_TX_DualBoard();
		}	
	}

}