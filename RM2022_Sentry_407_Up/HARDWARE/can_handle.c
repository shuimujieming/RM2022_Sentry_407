#include "can_handle.h"

//����
CAN_Data_TypeDef CAN_Chassis[4];
CAN_Data_TypeDef CAN_Storage;

//��̨
CAN_Data_TypeDef CAN_Gimbal[2];	//Yaw 205 Pitch 206

//����
CAN_Data_TypeDef CAN_Shoot[2];	   //Left_Fric 0x203 Right_Fric 0x204
CAN_Data_TypeDef CAN_Trigger[2];   //Trigger 0x207 0x208 
int CAN1_Signal = 0;																		//CAN1�ź�����־
int CAN2_Signal = 0;																		//CAN2�ź�����־
unsigned char CAN1_Tx_Message_Flag = 0;	//CAN1������Ϣ��־
unsigned char CAN2_Tx_Message_Flag = 0;	//CAN2������Ϣ��־
/*
*@title��CAN1�����жϺ���
*@description��
*@param 1��	
*@param 2��	
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
*@title��CAN2�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
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
*@Description��CAN���ݽ���
*@param 1��	  ����1
*@param 2��	  ����2
*@return:��	  ����ֵ
*/
void CAN_Data_Decode(CAN_Data_TypeDef *CAN_Data,CanRxMsg *CAN_Rx_Message)
{
	CAN_Data->Origin_MechAngle = (CAN_Rx_Message->Data[0]<<8)|(CAN_Rx_Message->Data[1]);
	CAN_Data->Current_Speed 					= (CAN_Rx_Message->Data[2]<<8)|(CAN_Rx_Message->Data[3]);
	CAN_Data->Current								  = (CAN_Rx_Message->Data[4]<<8)|(CAN_Rx_Message->Data[5]);
	CAN_Data->Temperature					    = (CAN_Rx_Message->Data[6]);
	
	//��ǰ����Ϊ��̨���ʹ�ã�����Ƕ�����,��ʵ���Ǽ�һȦ,����8191�ĺ���,ԭ��8191������0������ֱ�ӱ��8192.
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
	//��ͨ���ʹ��
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
  //��һ�ν��룬�趨��ʼֵ
	if(Storage_Init_Flag)
	{
		Storage_Init_Flag --;
		Storage_Init_Angle = CAN_Data->Current_MechAngle;
		CAN_Data->Last_MechAngle = Storage_Init_Angle;
	}
  //ͨ����е���ж�Ȧ�����ӻ����
	if((CAN_Data->Current_MechAngle - CAN_Data->Last_MechAngle) > 4095)
	{
		Storage_Num -= 1;
	}
	else if((CAN_Data->Current_MechAngle - CAN_Data->Last_MechAngle) < -4095)
	{
		Storage_Num += 1;		
	}
	
	CAN_Data->Last_MechAngle = CAN_Data->Current_MechAngle;
	//�ܵ�ת����Ȧ��
	Storage_Angle = Storage_Num * 8192 + CAN_Data->Current_MechAngle;
}
/*
*@title��CAN1�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_RX0_IRQHandler(void)                                             
{   
	CanRxMsg CAN1_Rx_Message;	
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_Receive(CAN1,CAN_FIFO0,&CAN1_Rx_Message);
		if ( (CAN1_Rx_Message.IDE == CAN_Id_Standard) && (CAN1_Rx_Message.RTR == CAN_RTR_Data) && (CAN1_Rx_Message.DLC == 8) )//��׼֡������֡�����ݳ���Ϊ8�ֽ�
		{
						//CAN1�ź������
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
					CAN_Data_Decode(&CAN_Trigger[1],&CAN1_Rx_Message);    //�ϲ����̣�ʵ��ʹ��
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
*@title��CAN2�����жϺ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN2_RX0_IRQHandler(void)                                                 
{   
	if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
	{
		CAN_Receive(CAN2,CAN_FIFO0,&CAN2_Rx_Message);

		if ( (CAN2_Rx_Message.IDE == CAN_Id_Standard) && (CAN2_Rx_Message.RTR == CAN_RTR_Data) && (CAN2_Rx_Message.DLC == 8) )//��׼֡������֡�����ݳ���Ϊ8�ֽ�
		{
			//CAN2�ź������
			CAN2_Signal = 50;
			switch (CAN2_Rx_Message.StdId)
			{		
				//�°�����
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
*@title��CAN1�������ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_Chassis(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = 0x200;                               										//֡IDΪ���������CAN_ID

	CAN1_Tx_Message.Data[0] = (CAN_Chassis[0].Target_Current>>8)&0xff;             //201���յ�����8λ
	CAN1_Tx_Message.Data[1] = (CAN_Chassis[0].Target_Current)&0xff;                 //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = (CAN_Storage.Target_Current>>8)&0xff;             //202���յ�����8λ
	CAN1_Tx_Message.Data[3] = (CAN_Storage.Target_Current)&0xff;                //202���յ�����8λ
	CAN1_Tx_Message.Data[4] = 0;             //203���յ�����8λ
	CAN1_Tx_Message.Data[5] = 0;                 //203���յ�����8λ
	CAN1_Tx_Message.Data[6] = 0;             //204���յ�����8λ
	CAN1_Tx_Message.Data[7] = 0;                 //204���յ�����8λ

	CAN1_Tx_Message_Flag = 0;
	//CAN1���ߴ���
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
*@title��CAN1��̨���ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_Gimbal(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = 0x1FF;                               //֡IDΪ���������CAN_ID

	CAN1_Tx_Message.Data[0] = (CAN_Gimbal[0].Target_Current>>8)&0xff;             //201���յ�����8λ
	CAN1_Tx_Message.Data[1] = (CAN_Gimbal[0].Target_Current)&0xff;                 //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = (CAN_Gimbal[1].Target_Current>>8)&0xff;           
	CAN1_Tx_Message.Data[3] = (CAN_Gimbal[1].Target_Current)&0xff;    

//	CAN1_Tx_Message.Data[0] = 0;             //201���յ�����8λ
//	CAN1_Tx_Message.Data[1] = 0;                 //201���յ�����8λ
//	CAN1_Tx_Message.Data[2] = 0;           
//	CAN1_Tx_Message.Data[3] = 0;    
	
	CAN1_Tx_Message.Data[4] = (CAN_Trigger[0].Target_Current>>8)&0xff;            
	CAN1_Tx_Message.Data[5] = (CAN_Trigger[0].Target_Current)&0xff;               
	CAN1_Tx_Message.Data[6] = (CAN_Trigger[1].Target_Current>>8)&0xff;            
	CAN1_Tx_Message.Data[7] = (CAN_Trigger[1].Target_Current)&0xff;              

	CAN1_Tx_Message_Flag = 0;
	//CAN1���ߴ���
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
*@title��CAN1����������ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/
void CAN1_TX_Shoot(void)
{
	CanTxMsg CAN1_Tx_Message;

	CAN1_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN1_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN1_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN1_Tx_Message.StdId = 0x200;                               //֡IDΪ���������CAN_ID

	CAN1_Tx_Message.Data[0] = 0;             //201���յ�����8λ
	CAN1_Tx_Message.Data[1] = 0;                 //201���յ�����8λ
	CAN1_Tx_Message.Data[2] = 0;           
	CAN1_Tx_Message.Data[3] = 0;           
	CAN1_Tx_Message.Data[4] = (CAN_Shoot[0].Target_Current>>8)&0xff;           
	CAN1_Tx_Message.Data[5] = (CAN_Shoot[0].Target_Current)&0xff;             
	CAN1_Tx_Message.Data[6] = (CAN_Shoot[1].Target_Current>>8)&0xff;               
	CAN1_Tx_Message.Data[7] = (CAN_Shoot[1].Target_Current)&0xff;        

	CAN1_Tx_Message_Flag = 0;
	//CAN1���ߴ���
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
*@title��CAN2˫�����ݷ���
*@description��
*@param 1��	
*@param 2��	
*@return:��	
*/

void CAN2_TX_DualBoard(void)
{
	CanTxMsg CAN2_Tx_Message;

	CAN2_Tx_Message.IDE = CAN_ID_STD;                                               //��׼֡
	CAN2_Tx_Message.RTR = CAN_RTR_DATA;                                             //����֡
	CAN2_Tx_Message.DLC = 0x08;                                                     //֡����Ϊ8
	CAN2_Tx_Message.StdId = 0x102;                               //֡IDΪ���������CAN_ID

	CAN2_Tx_Message.Data[0] = (DBUS.RC.ch2>>8)&0xff;   
	CAN2_Tx_Message.Data[1] = (DBUS.RC.ch2)&0xff;              
	CAN2_Tx_Message.Data[2] = (DBUS.RC.ch3>>8)&0xff;   
	CAN2_Tx_Message.Data[3] = (DBUS.RC.ch3)&0xff;          
	CAN2_Tx_Message.Data[4] = (DBUS.RC.ch4>>8)&0xff;   
	CAN2_Tx_Message.Data[5] = (DBUS.RC.ch4)&0xff;              
	CAN2_Tx_Message.Data[6] = DBUS.RC.Switch_Left;               
	CAN2_Tx_Message.Data[7] = DBUS.RC.Switch_Right;        

	CAN2_Tx_Message_Flag = 0;
	//�ڱ�����ͷͨ�����⣬����ͬʱʧȥ�źţ����Ա�����ͷҪһֱ��
	//CAN2���ߴ���
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
