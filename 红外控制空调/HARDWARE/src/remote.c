#include "remote.h"
#include "delay.h"
#include "usart.h"

void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 输入 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//下拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//初始化GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 65000; //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_Prescaler =72; 	//预分频器,1M的计数频率,1us加1.	   	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // 选择输入端 IC4映射到TI4上
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
  TIM_ICInit(TIM4, &TIM_ICInitStructure);//初始化定时器输入捕获通道

  TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断								 
}

u8 High_Low_Flag = 1, num = 0, first_flag = 0;  //当前高低电平
u16 IR_Code[400] = {0};
//定时器4中断服务程序	 
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
		if(RDATA)//上升沿捕获
		{
			if(first_flag)
			{
				IR_Code[num++] = TIM_GetCounter(TIM4);
			}
			first_flag = 1;
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);						//CC4P=1	设置为下降沿捕获
			TIM_SetCounter(TIM4,0);							//清空定时器值
			High_Low_Flag = 0;
		}else //下降沿捕获
		{
			IR_Code[num++] = TIM_GetCounter(TIM4);					//读取CCR4也可以清CC4IF标志位
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);				//CC4P=0	设置为上升沿捕获
			TIM_SetCounter(TIM4, 0);	
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);	 	    
}

