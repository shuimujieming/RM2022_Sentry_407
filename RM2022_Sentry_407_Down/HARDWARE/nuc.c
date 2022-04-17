#include "nuc.h"
#define RCC_AHBPeriphClockCmd_GPIO_NUC RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE)
#define RCC_APBPeriphClockCmd_USART_NUC RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)

#define GPIO_NUC_1 GPIOA
#define GPIO_PinSource_NUC_1 GPIO_PinSource9

#define GPIO_NUC_2 GPIOB
#define GPIO_PinSource_NUC_2 GPIO_PinSource7

#define GPIO_AF_USART_NUC GPIO_AF_USART1

#define GPIO_Pin_NUC_1 GPIO_Pin_9
#define GPIO_Pin_NUC_2 GPIO_Pin_7

#define USART_NUC USART1
#define USART_NUC_IRQn USART1_IRQn
#define USART_NUC_IRQHandler USART1_IRQHandler

#define RCC_AHBPeriphClockCmd_DMA_NUC RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE)

#define DMA_Channel_NUC_RX DMA_Channel_4
#define DMA_Stream_NUC_RX DMA2_Stream2

#define DMA_Channel_NUC_TX DMA_Channel_4
#define DMA_Stream_NUC_TX DMA2_Stream7

#define DMA_Stream_NUC_TX_IRQn DMA2_Stream7_IRQn
#define DMA_Stream_NUC_TX_IRQHandler DMA2_Stream7_IRQHandler

#define DMA_IT_TCIF_NUC DMA_IT_TCIF7

#define NUC_RX_BUFF_SIZE 10
#define NUC_TX_BUFF_SIZE 23

uint8_t NUC_rx_buff[2][NUC_RX_BUFF_SIZE];
uint8_t NUC_tx_buff[NUC_TX_BUFF_SIZE];

NUC_Typedef NUC_Data;

/*
*@Description：串口8初始化
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void NUC_USART_Init()
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	DMA_InitTypeDef    DMA_InitStruct;
	NVIC_InitTypeDef   NVIC_InitStruct;
	
	
	RCC_AHBPeriphClockCmd_GPIO_NUC;//此处可以用或运算，因为是控制位的
	RCC_APBPeriphClockCmd_USART_NUC;
	

	GPIO_PinAFConfig(GPIO_NUC_1, GPIO_PinSource_NUC_1, GPIO_AF_USART_NUC);
	GPIO_PinAFConfig(GPIO_NUC_2, GPIO_PinSource_NUC_2, GPIO_AF_USART_NUC);
	
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_NUC_1;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIO_NUC_1, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_NUC_2;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIO_NUC_2, &GPIO_InitStruct);	
	
	USART_InitStruct.USART_BaudRate            = 115200;
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART_NUC, &USART_InitStruct);
	
	USART_ClearFlag(USART_NUC, USART_FLAG_IDLE);
	USART_ITConfig(USART_NUC, USART_IT_IDLE, ENABLE);	
	
	USART_Cmd(USART_NUC, ENABLE);  //使能串口8
}
void NUC_USART_DMA_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd_DMA_NUC;
	
	//DMA1_Stream6_Channel5
	//UART8_RX
	DMA_InitStructure.DMA_Channel = DMA_Channel_NUC_RX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_NUC->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)NUC_rx_buff[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = NUC_RX_BUFF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Stream_NUC_RX, &DMA_InitStructure);
	
	DMA_DoubleBufferModeConfig(DMA_Stream_NUC_RX, (uint32_t)NUC_rx_buff[1], DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA_Stream_NUC_RX, ENABLE);
	
	USART_DMACmd(USART_NUC, USART_DMAReq_Rx, ENABLE);	
	
	DMA_Cmd(DMA_Stream_NUC_RX, ENABLE);
	

	//DMA1_Stream0_Channel5
	//UART8_TX
	DMA_InitStructure.DMA_Channel = DMA_Channel_NUC_TX;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART_NUC->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)NUC_tx_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = NUC_TX_BUFF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Stream_NUC_TX, &DMA_InitStructure);
	
	USART_DMACmd(USART_NUC, USART_DMAReq_Tx, ENABLE);

	DMA_ITConfig(DMA_Stream_NUC_TX,DMA_IT_TC,ENABLE);
	
	
	DMA_Cmd(DMA_Stream_NUC_TX, ENABLE);

}


void NUC_USART_NVIC_Init()
{
	//串口8接收空闲中断
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART_NUC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//发送中断
	NVIC_InitStructure.NVIC_IRQChannel                   =DMA_Stream_NUC_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                =ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

void NUC_Init()
{
	NUC_USART_Init();
	NUC_USART_NVIC_Init();
	NUC_USART_DMA_Init();
}
/*
*@title：NUC发送中断
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
void DMA_Stream_NUC_TX_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA_Stream_NUC_TX, DMA_IT_TCIF_NUC) == SET)
	{
		DMA_Cmd(DMA_Stream_NUC_TX, DISABLE);
		DMA_SetCurrDataCounter(DMA_Stream_NUC_TX, NUC_TX_BUFF_SIZE); 
	}
	DMA_ClearITPendingBit(DMA_Stream_NUC_TX, DMA_IT_TCIF_NUC);
}


static u8 bit32TObit8(int index_need,int bit32)
{
	union
	{
    int  f;
		u8  byte[4];
	}u32val;
   u32val.f = bit32;
	return u32val.byte[index_need];
}
static char shortTou8(char bit,short data)
{
	union
	{
		short i;
		char byte[2];
	}u16val;
	u16val.i=data;
	return u16val.byte[bit];
}

extern short Origin_Init_Yaw_Angle;
extern short Origin_Init_Pitch_Angle;

float Yaw_Angle_NUC,Pitch_Angle_NUC,Yaw_Speed_NUC,Pitch_Speed_NUC;

void NUC_Send_Data()
{
		// Yaw轴的角度就是电机编码器的真实角度,让初始时，yaw为0   
		Yaw_Angle_NUC=(float)(CAN_Gimbal[0].Current_MechAngle-Origin_Init_Yaw_Angle)/8192.0f*180.0f*10;

		// Pitch轴的角度就是真实的机械角处理之后发送过去.8192对应180度，单位：度*10
		Pitch_Angle_NUC=(float)(CAN_Gimbal[1].Current_MechAngle - Origin_Init_Pitch_Angle)/8192.0f*180.0f*10;

		 //单位：度/s*10
		Yaw_Speed_NUC=((float)CAN_Gimbal[0].Current_Speed*360.0f)/(60.0f)*10.0f;

		 //单位：度/s*10
		Pitch_Speed_NUC=((float)CAN_Gimbal[1].Current_Speed*360.0f)/(60.0f)*10.0f;
	
		NUC_tx_buff[0]=0x7a;

		NUC_tx_buff[2]=bit32TObit8(0,(short)Yaw_Angle_NUC);
		NUC_tx_buff[3]=bit32TObit8(1,(short)Yaw_Angle_NUC);	
		NUC_tx_buff[4]=bit32TObit8(2,(short)Yaw_Angle_NUC);
		NUC_tx_buff[5]=bit32TObit8(3,(short)Yaw_Angle_NUC);	
		NUC_tx_buff[6]=shortTou8(0,(short)Pitch_Speed_NUC);//以下速度数据视觉都没有使用
		NUC_tx_buff[7]=shortTou8(1,(short)Pitch_Speed_NUC);
		NUC_tx_buff[8]=shortTou8(0,(short)Yaw_Speed_NUC);
		NUC_tx_buff[9]=shortTou8(1,(short)Yaw_Speed_NUC);	

		NUC_tx_buff[18]=shortTou8(0,(short)Pitch_Angle_NUC);
		NUC_tx_buff[19]=shortTou8(1,(short)Pitch_Angle_NUC);

		NUC_tx_buff[22]=0x7b;
		
		DMA_Cmd(DMA_Stream_NUC_TX,ENABLE);
}

void NUC_Data_Decode(uint8_t *buff)
{
	if((buff[0] + buff[3] + buff[5]) % 255 == buff[9])
	{
		NUC_Data.NUC_Shoot_Allow_Flag = (buff[0] == 0x1f);
		
		NUC_Data.Yaw_Angle   = (short)(buff[2] << 8 | buff[1]) / 100.0f;
		NUC_Data.Pitch_Angle = (short)(buff[4] << 8 | buff[3]);
		
		NUC_Data.Armor_Type  =  buff[6]; //装甲类型 0未识别到 1小装甲 2大装甲
		
		if(NUC_Data.Armor_Type == 0)
		{
		NUC_Data.Yaw_Angle   = 0;
		NUC_Data.Pitch_Angle = 0;			
		}
	}
	else
	{
		NUC_Data.Armor_Type  = 0;
		NUC_Data.Yaw_Angle   = 0;
		NUC_Data.Pitch_Angle = 0;			
	}
}
/*
*@Description：串口8中断处理函数
*@param 1：	  参数1
*@param 2：	  参数2
*@return:：	  返回值
*/
void USART_NUC_IRQHandler()
{
			if(USART_GetITStatus(USART_NUC, USART_IT_IDLE) != RESET)	//判断是否为空闲中断
    {
        uint16_t this_time_rx_len = 0;
        USART_ReceiveData(USART_NUC);

        if(DMA_GetCurrentMemoryTarget(DMA_Stream_NUC_RX) == DMA_Memory_0)	//获取当前目标内存是否为 DMA_Memory_0
        {
            //重新设置DMA
            DMA_Cmd(DMA_Stream_NUC_RX, DISABLE);
            this_time_rx_len = DMA_GetCurrDataCounter(DMA_Stream_NUC_RX);
            DMA_SetCurrDataCounter(DMA_Stream_NUC_RX, NUC_RX_BUFF_SIZE);
            DMA_Cmd(DMA_Stream_NUC_RX, ENABLE);
            if(this_time_rx_len == NUC_RX_BUFF_SIZE)	//校验接收
            {
                //处理数据
               NUC_Data_Decode(NUC_rx_buff[1]);
            }
        }
        else
        {
            //重新设置DMA
            DMA_Cmd(DMA_Stream_NUC_RX, DISABLE);
            this_time_rx_len = DMA_GetCurrDataCounter(DMA_Stream_NUC_RX);
            DMA_SetCurrDataCounter(DMA_Stream_NUC_RX, NUC_RX_BUFF_SIZE);
            DMA_Cmd(DMA_Stream_NUC_RX, ENABLE);
            if(this_time_rx_len == NUC_RX_BUFF_SIZE)	//校验接收
            {
                //处理数据
               NUC_Data_Decode(NUC_rx_buff[0]);
            }
        }
    }
}