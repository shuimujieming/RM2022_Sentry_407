#include "can_handle.h"

//底盘
CAN_Data_TypeDef CAN_Chassis[4];
CAN_Data_TypeDef CAN_Storage;

//云台
CAN_Data_TypeDef CAN_Gimbal[2];	//Yaw 205 Pitch 206

//发射
CAN_Data_TypeDef CAN_Shoot[2];	   //Left_Fric 0x203 Right_Fric 0x204
CAN_Data_TypeDef CAN_Trigger[2];   //Trigger 0x207 0x208 
int CAN1_Signal = 0;																		//CAN1信号量标志
int CAN2_Signal = 0;																		//CAN2信号量标志
unsigned char CAN1_Tx_Message_Flag = 0;	//CAN1发送消息标志
unsigned char CAN2_Tx_Message_Flag = 0;	//CAN2发送消息标志
/*
*@title：CAN1发送中断函数
*@description：
*@param 1：	
*@param 2：	
*@return:
*/
void CAN1_TX_IRQHandler(void)
{  
  if (CAN_GetITStatus (CAN1,CAN_IT_TME)!=RESET)                             
	{
		CAN1_Tx_Message_Flag=1;
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);    
	}
}


/*
*@title：CAN2发送中断函数
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void CAN2_TX_IRQHandler(void)
{  
  if (CAN_GetITStatus (CAN2,CAN_IT_TME)!=RESET)                            
	{
		CAN2_Tx_Message_Flag=1;
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);    
	}
}



/*
*@Description：CAN数据解码
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void CAN_Data_Decode(CAN_Data_TypeDef *CAN_Data,CanRxMsg *CAN_Rx_Message)
{
	CAN_Data->Origin_MechAngle = (CAN_Rx_Message->Data[0]<<8)|(CAN_Rx_Message->Data[1]);
	CAN_Data->Current_Speed 					= (CAN_Rx_Message->Data[2]<<8)|(CAN_Rx_Message->Data[3]);
	CAN_Data->Current								  = (CAN_Rx_Message->Data[4]<<8)|(CAN_Rx_Message->Data[5]);
	CAN_Data->Temperature					    = (CAN_Rx_Message->Data[6]);
	
	//当前数据为云台电机使用，处理角度数据,其实就是加一圈,补到8191的后面,原来8191后面是0，现在直接变成8192.
	if(CAN_Data->Critical_MechAngle !=0)
	{
		if(CAN_Data->Origin_MechAngle < CAN_Data->Critical_MechAngle)
		{
			CAN_Data->Current_MechAngle = CAN_Data->Origin_MechAngle + 8192;
		}
		else
		{
			CAN_Data->Current_MechAngle = CAN_Data->Origin_MechAngle;
		}		
	}
	//普通电机使用
	else
	{
			CAN_Data->Current_MechAngle = CAN_Data->Origin_MechAngle;		
	}
}
int Storage_Num = 0;
int Storage_Angle = 0;

int Storage_Init_Flag = 1;
int Storage_Init_Angle = 0;

void CAN_Storage_Angle_Decode(CAN_Data_TypeDef *CAN_Data)
{
  //第一次进入，设定初始值
	if(Storage_Init_Flag)
	{
		Storage_Init_Flag --;
		Storage_Init_Angle = CAN_Data->Current_MechAngle;
		CAN_Data->Last_MechAngle = Storage_Init_Angle;
	}
  //通过机械角判断圈数增加或减少
	if((CAN_Data->Current_MechAngle - CAN_Data->Last_MechAngle) > 4095)
	{
		Storage_Num -= 1;
	}
	else if((CAN_Data->Current_MechAngle - CAN_Data->Last_MechAngle) < -4095)
	{
		Storage_Num += 1;		
	}
	
	CAN_Data->Last_MechAngle = CAN_Data->Current_MechAngle;
	//总的转过的圈数
	Storage_Angle = Storage_Num * 8192 + CAN_Data->Current_MechAngle;
}
/*
*@title：CAN1接收中断函数
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void CAN1_RX0_IRQHandler(void)                                             
{   
	CanRxMsg CAN1_Rx_Message;	
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_Receive(CAN1,CAN_FIFO0,&CAN1_Rx_Message);
		if ( (CAN1_Rx_Message.IDE == CAN_Id_Standard) && (CAN1_Rx_Message.RTR == CAN_RTR_Data) && (CAN1_Rx_Message.DLC == 8) )//标准帧、数据帧、数据长度为8字节
		{
						//CAN1信号量填充
			CAN1_Signal = 50;
			switch (CAN1_Rx_Message.StdId)
			{
				case 0x201:
				{
					CAN_Data_Decode(&CAN_Chassis[0],&CAN1_Rx_Message);
				}break;
				case 0x202:
				{
					CAN_Data_Decode(&CAN_Storage,&CAN1_Rx_Message);
					CAN_Storage_Angle_Decode(&CAN_Storage);
				}break;
				case 0x203:
				{
					CAN_Data_Decode(&CAN_Shoot[0],&CAN1_Rx_Message);//Left Fric
				}break;
				case 0x204:
				{
					CAN_Data_Decode(&CAN_Shoot[1],&CAN1_Rx_Message);//Right Fric
				}break;
				case 0x205:
				{
					CAN_Data_Decode(&CAN_Gimbal[0],&CAN1_Rx_Message);//Yaw
				}break;						
				case 0x206:
				{
					CAN_Data_Decode(&CAN_Gimbal[1],&CAN1_Rx_Message);//Pitch
				}break;		
				case 0x207:
				{
					CAN_Data_Decode(&CAN_Trigger[0],&CAN1_Rx_Message);
				}break;		
				case 0x208:
				{
					CAN_Data_Decode(&CAN_Trigger[1],&CAN1_Rx_Message);    //上拨弹盘，实际使用
				}break;						
				default:
				{
					break;
				}   
			}
		}
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);		
	}
}
	CanRxMsg CAN2_Rx_Message;	

extern Down_To_Up_Typedef  Down_To_Up_Data;

/*
*@title：CAN2接收中断函数
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void CAN2_RX0_IRQHandler(void)                                                 
{   
	if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
	{
		CAN_Receive(CAN2,CAN_FIFO0,&CAN2_Rx_Message);

		if ( (CAN2_Rx_Message.IDE == CAN_Id_Standard) && (CAN2_Rx_Message.RTR == CAN_RTR_Data) && (CAN2_Rx_Message.DLC == 8) )//标准帧、数据帧、数据长度为8字节
		{
			//CAN2信号量填充
			CAN2_Signal = 50;
			switch (CAN2_Rx_Message.StdId)
			{		
				//下板数据
				case 0x101:
				{
					Down_To_Up_Data.Target_Locked = CAN2_Rx_Message.Data[0];
					
				}break;	
				default:
				{
					break; 
				}
					
			}
		}
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);		
	}
}
/*
*@title：CAN1底盘数据发送
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void CAN1_TX_Chassis(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = 0x200;                               										//帧ID为传入参数的CAN_ID

	CAN1_Tx_Message.Data[0] = (CAN_Chassis[0].Target_Current>>8)&0xff;             //201接收电流高8位
	CAN1_Tx_Message.Data[1] = (CAN_Chassis[0].Target_Current)&0xff;                 //201接收电流低8位
	CAN1_Tx_Message.Data[2] = (CAN_Storage.Target_Current>>8)&0xff;             //202接收电流高8位
	CAN1_Tx_Message.Data[3] = (CAN_Storage.Target_Current)&0xff;                //202接收电流低8位
	CAN1_Tx_Message.Data[4] = 0;             //203接收电流高8位
	CAN1_Tx_Message.Data[5] = 0;                 //203接收电流低8位
	CAN1_Tx_Message.Data[6] = 0;             //204接收电流高8位
	CAN1_Tx_Message.Data[7] = 0;                 //204接收电流低8位

	CAN1_Tx_Message_Flag = 0;
	//CAN1掉线处理
	if(CAN1_Signal > 0)
	{
	CAN_Transmit(CAN1,&CAN1_Tx_Message);	
	}
	
	while(CAN1_Tx_Message_Flag == 0)
	{
		if(CAN1_Signal <= 0)
		{
			break;
		}
	}
}

/*
*@title：CAN1云台数据发送
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void CAN1_TX_Gimbal(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = 0x1FF;                               //帧ID为传入参数的CAN_ID

	CAN1_Tx_Message.Data[0] = (CAN_Gimbal[0].Target_Current>>8)&0xff;             //201接收电流高8位
	CAN1_Tx_Message.Data[1] = (CAN_Gimbal[0].Target_Current)&0xff;                 //201接收电流低8位
	CAN1_Tx_Message.Data[2] = (CAN_Gimbal[1].Target_Current>>8)&0xff;           
	CAN1_Tx_Message.Data[3] = (CAN_Gimbal[1].Target_Current)&0xff;    

//	CAN1_Tx_Message.Data[0] = 0;             //201接收电流高8位
//	CAN1_Tx_Message.Data[1] = 0;                 //201接收电流低8位
//	CAN1_Tx_Message.Data[2] = 0;           
//	CAN1_Tx_Message.Data[3] = 0;    
	
	CAN1_Tx_Message.Data[4] = (CAN_Trigger[0].Target_Current>>8)&0xff;            
	CAN1_Tx_Message.Data[5] = (CAN_Trigger[0].Target_Current)&0xff;               
	CAN1_Tx_Message.Data[6] = (CAN_Trigger[1].Target_Current>>8)&0xff;            
	CAN1_Tx_Message.Data[7] = (CAN_Trigger[1].Target_Current)&0xff;              

	CAN1_Tx_Message_Flag = 0;
	//CAN1掉线处理
	if(CAN1_Signal > 0)
	{
	CAN_Transmit(CAN1,&CAN1_Tx_Message);	
	}
	
	while(CAN1_Tx_Message_Flag == 0)
	{
		if(CAN1_Signal <= 0)
		{
			break;
		}
	}
}


/*
*@title：CAN1发射机构数据发送
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void CAN1_TX_Shoot(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN1_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN1_Tx_Message.StdId = 0x200;                               //帧ID为传入参数的CAN_ID

	CAN1_Tx_Message.Data[0] = 0;             //201接收电流高8位
	CAN1_Tx_Message.Data[1] = 0;                 //201接收电流低8位
	CAN1_Tx_Message.Data[2] = 0;           
	CAN1_Tx_Message.Data[3] = 0;           
	CAN1_Tx_Message.Data[4] = (CAN_Shoot[0].Target_Current>>8)&0xff;           
	CAN1_Tx_Message.Data[5] = (CAN_Shoot[0].Target_Current)&0xff;             
	CAN1_Tx_Message.Data[6] = (CAN_Shoot[1].Target_Current>>8)&0xff;               
	CAN1_Tx_Message.Data[7] = (CAN_Shoot[1].Target_Current)&0xff;        

	CAN1_Tx_Message_Flag = 0;
	//CAN1掉线处理
	if(CAN1_Signal > 0)
	{
	CAN_Transmit(CAN1,&CAN1_Tx_Message);	
	}
	
	while(CAN1_Tx_Message_Flag == 0)
	{
		if(CAN1_Signal <= 0)
		{
			break;
		}
	}
}

/*
*@title：CAN2双板数据发送
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/

void CAN2_TX_DualBoard(void)
{
	CanTxMsg CAN2_Tx_Message;

	CAN2_Tx_Message.IDE = CAN_ID_STD;                                               //标准帧
	CAN2_Tx_Message.RTR = CAN_RTR_DATA;                                             //数据帧
	CAN2_Tx_Message.DLC = 0x08;                                                     //帧长度为8
	CAN2_Tx_Message.StdId = 0x102;                               //帧ID为传入参数的CAN_ID

	CAN2_Tx_Message.Data[0] = (DBUS.RC.ch2>>8)&0xff;   
	CAN2_Tx_Message.Data[1] = (DBUS.RC.ch2)&0xff;              
	CAN2_Tx_Message.Data[2] = (DBUS.RC.ch3>>8)&0xff;   
	CAN2_Tx_Message.Data[3] = (DBUS.RC.ch3)&0xff;          
	CAN2_Tx_Message.Data[4] = (DBUS.RC.ch4>>8)&0xff;   
	CAN2_Tx_Message.Data[5] = (DBUS.RC.ch4)&0xff;              
	CAN2_Tx_Message.Data[6] = DBUS.RC.Switch_Left;               
	CAN2_Tx_Message.Data[7] = DBUS.RC.Switch_Right;        

	CAN2_Tx_Message_Flag = 0;
	//哨兵上下头通信问题，不能同时失去信号，所以必须上头要一直发
	//CAN2掉线处理
//	if(CAN2_Signal > 0)
	{
		CAN_Transmit(CAN2,&CAN2_Tx_Message);
	}
	
	while(CAN2_Tx_Message_Flag == 0)
	{
		if(CAN2_Signal <= 0)
		{
			break;
		}
	}
}
