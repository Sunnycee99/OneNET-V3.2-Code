#include "remote.h"
#include "delay.h"
#include "usart.h"

void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��PORTBʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//��ʼ��GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 65000; //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_Prescaler =72; 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // ѡ������� IC4ӳ�䵽TI4��
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM4, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��

  TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�								 
}

u8 High_Low_Flag = 1, num = 0, first_flag = 0;  //��ǰ�ߵ͵�ƽ
u16 IR_Code[400] = {0};
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		    	 
 
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		u16 i;
		if(first_flag)
		{
			for(i=0;i<num-1;i++)
				UsartPrintf(USART1, "%d, ", IR_Code[i]);
			UsartPrintf(USART1, "%d\r\n\r\n", IR_Code[num-2]);
		}
		num = 0;
		first_flag = 0;
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{
			if(first_flag)
			{
				IR_Code[num++] = TIM_GetCounter(TIM4);
			}
			first_flag = 1;
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);						//CC4P=1	����Ϊ�½��ز���
			TIM_SetCounter(TIM4,0);							//��ն�ʱ��ֵ
			High_Low_Flag = 0;
		}else //�½��ز���
		{
			IR_Code[num++] = TIM_GetCounter(TIM4);					//��ȡCCR4Ҳ������CC4IF��־λ
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);				//CC4P=0	����Ϊ�����ز���
			TIM_SetCounter(TIM4, 0);	
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);	 	    
}

