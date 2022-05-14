#include "gimbal.h"
Init_Angle_Data Angle_Init;


void Encoder_Data_Process(CAN_Data_TypeDef *encoder_data,short init_angle)
{
	if(init_angle + 4096 >8191)
	{
		encoder_data->Critical_MechAngle = init_angle + 4096 - 8192;
	}
	else
	{
		encoder_data->Critical_MechAngle = init_angle + 4096;
	}
}

//Flash��Ϣ�����ַ
#define CONFIG_PARAM_SIZE	(1020*1024)
#define CONFIG_PARAM_ADDR 	(FLASH_BASE + CONFIG_PARAM_SIZE)	
//��̨�Ƕȱ궨���궨��ֵΪ���������ԭʼֵ��
void Gimbal_Angle_Calibrate()
{
	if(DBUS.RC.Switch_Left == RC_SW_DOWN && DBUS.RC.Switch_Right == RC_SW_DOWN)
	{
	//���ڰ�״̬�½���궨����
	if(DBUS.RC.ch2 > 600 && DBUS.RC.ch3 < -600)
	{
		//������������
		Beep_Ctrl(1000,100);
		delay_ms(500);
		Beep_Ctrl(800,100);
		delay_ms(500);		
		Beep_Ctrl(800,0);
		//������������
		Beep_Ctrl(1000,100);
		delay_ms(500);
		Beep_Ctrl(800,100);
		delay_ms(500);		
		Beep_Ctrl(800,0);
		
		//�ȴ��󲦸�����
		while(DBUS.RC.Switch_Left != RC_SW_MID)
		{
			Angle_Init.Yaw_Angle_Init = CAN_Gimbal[0].Origin_MechAngle;
		}
		Beep_Ctrl(1000,100);
		delay_ms(500);
		Beep_Ctrl(1000,0);
		//�ȴ��󲦸�����
		while(DBUS.RC.Switch_Left != RC_SW_UP)
		{
			Angle_Init.Pitch_Angle_Init = CAN_Gimbal[1].Origin_MechAngle;
		}
		Beep_Ctrl(1000,100);
		delay_ms(500);
		Beep_Ctrl(1000,0);
		//�ȴ��Ҳ�������
		while(DBUS.RC.Switch_Right != RC_SW_MID)
		{
			Angle_Init.Pitch_Angle_Max = CAN_Gimbal[1].Origin_MechAngle;			
		}
		Beep_Ctrl(1000,100);
		delay_ms(500);
		Beep_Ctrl(1000,0);
		//�ȴ��Ҳ�������
		while(DBUS.RC.Switch_Right != RC_SW_UP)
		{
			Angle_Init.Pitch_Angle_Min = CAN_Gimbal[1].Origin_MechAngle;			
		}		
		Beep_Ctrl(1000,100);
		delay_ms(500);
		Beep_Ctrl(800,100);
		delay_ms(500);		
		Beep_Ctrl(800,0);
		
		STMFLASH_Write(CONFIG_PARAM_ADDR,(u32 *)&Angle_Init, sizeof(Angle_Init));	/*д��stm32 flash*/

	}		
	}

}

//������Flash�Ƕ�ֵ��ȡ
void Gimbal_Angle_Read()
{
	//����սǶ�ֵ
	memset(&Angle_Init,0,sizeof(Angle_Init));
	//��ȡFlashֵ
	STMFLASH_Read(CONFIG_PARAM_ADDR, (u32 *)&Angle_Init, sizeof(Angle_Init));
	//�ж��Ƿ�Ϊ��Чֵ
	if(Angle_Init.Pitch_Angle_Init == 0 && Angle_Init.Pitch_Angle_Max == 0 && Angle_Init.Pitch_Angle_Min == 0 && Angle_Init.Yaw_Angle_Init == 0 )
	{
			Beep_Ctrl(1000,100);
			delay_ms(500);
			Beep_Ctrl(800,100);
			delay_ms(500);	
			Beep_Ctrl(1000,100);
			delay_ms(500);
			Beep_Ctrl(800,0);
			//�����ȴ�
			while(1){}
	}
	
}
//���ص�ǰ��ʵ�Ƕ�ֵ
static int Angle_Correct_Get(int angle_to_resize , int angle_critical)
{	
		if(angle_to_resize < angle_critical)
		{
			return angle_to_resize + 8192;
		}
		else
		{
			return angle_to_resize;
		}	
}
short Origin_Init_Yaw_Angle;
short Origin_Init_Pitch_Angle;

extern float Pitch_Aim_Angle;
extern float Pitch_Angle_Max;
extern float Pitch_Angle_Min;
void Gimbal_Init()
{		//��̨�Ƕȱ궨
		Gimbal_Angle_Calibrate();
	
		Pid_Reset(&PID_Gimbal_Angle[0]);
		Pid_Reset(&PID_Gimbal_Angle[1]);
		Pid_Reset(&PID_Gimbal_Speed[0]);
		Pid_Reset(&PID_Gimbal_Speed[1]);

		Pid_Set(&PID_Gimbal_Angle[1],2,0,0,10000,100,30000,10000,1,30000,0,2);	//Pitch
		Pid_Set(&PID_Gimbal_Speed[1],70,2,0,30000,10000,30000,30000,1,30000,0,2); //Pitch	

		Pid_Set(&PID_Gimbal_Angle[0],25,0,0,30000,20,30000,10000,1,30000,0,2);	//Yaw
		Pid_Set(&PID_Gimbal_Speed[0],100,1,0,30000,5000,30000,30000,1,30000,0,2); //Yaw
	
		//��ȡ������Flash����ֵ
		Gimbal_Angle_Read();
	
		//��̨��ʼֵ����
		Encoder_Data_Process(&CAN_Gimbal[0],Angle_Init.Yaw_Angle_Init);
		Encoder_Data_Process(&CAN_Gimbal[1],Angle_Init.Pitch_Angle_Init);
		
		//��̨��ʵ�Ƕȴ���
		Pitch_Aim_Angle = Angle_Correct_Get(Angle_Init.Pitch_Angle_Init,CAN_Gimbal[1].Critical_MechAngle);
		Pitch_Angle_Max = Angle_Correct_Get(Angle_Init.Pitch_Angle_Max,CAN_Gimbal[1].Critical_MechAngle);
		Pitch_Angle_Min = Angle_Correct_Get(Angle_Init.Pitch_Angle_Min,CAN_Gimbal[1].Critical_MechAngle);
		
		if(Pitch_Angle_Max < Pitch_Angle_Min)
		{
			Pitch_Angle_Min = Pitch_Angle_Max;
			Pitch_Angle_Max = Angle_Correct_Get(Angle_Init.Pitch_Angle_Min,CAN_Gimbal[1].Critical_MechAngle);
		}
		//YAW��ʼ��
		Origin_Init_Yaw_Angle = Angle_Correct_Get(Angle_Init.Yaw_Angle_Init,CAN_Gimbal[0].Critical_MechAngle);
		//Pitch��ʼ��
		Origin_Init_Pitch_Angle = Pitch_Aim_Angle;
		
		delay_ms(10);
		
}



