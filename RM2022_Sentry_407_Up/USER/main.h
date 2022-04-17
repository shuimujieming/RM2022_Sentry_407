#ifndef __MAIN_H
#define __MAIN_H
#include "sys.h"
#include "stdio.h"	
#include <math.h>

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool_t;
typedef float fp32;
typedef double fp64;

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "task_init.h"
#include "led_task.h"
#include "imu_task.h"
#include "shoot_task.h"
#include "gimbal_task.h"
#include "chassis_task.h"
#include "ui_task.h"
#include "can_send_task.h"
#include "mode_task.h"
#include "supercap_task.h"
#include "nuc_task.h"


//���ڴ�ӡ�Ĵ�������
#define USART_PRINT USART1

//��������
#include "delay.h"
#include "led.h"
#include "laser.h"
#include "dr16.h"
#include "tim6.h"
#include "pid.h"
#include "can.h"
#include "spi.h"
#include "imu.h"
#include "power.h"
#include "nuc.h"
#include "shoot.h"
#include "cover.h"
#include "referee.h"
#include "beep.h"
#include "dual_board.h"
#include "bmi088.h"
#include "bmi088driver.h"
#include "flash.h"

//����ϵͳ
#include "referee_info.h"
#include "crc.h"
#include "custom_ui.h"

//��̨���̿���
#include "gimbal.h"
#include "chassis.h"
#include "can_handle.h"

#endif

