#include "stm32f10x.h"
#include "usart.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void My_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitTypeStructure;
	USART_InitTypeDef USART_InitTypeStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitTypeStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitTypeStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitTypeStructure);
	
	GPIO_InitTypeStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitTypeStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitTypeStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitTypeStructure);

	USART_InitTypeStructure.USART_BaudRate = 115200;
	USART_InitTypeStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitTypeStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitTypeStructure.USART_Parity = USART_Parity_No;
	USART_InitTypeStructure.USART_StopBits = USART_StopBits_1;
	USART_InitTypeStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitTypeStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}


void My_USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//TXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//RXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	
	//使能串口
	USART_Cmd(USART2, ENABLE);
	//使能串口接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	//配置中断优先级分组
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
}

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsprintf((char *)UsartPrintfBuf, fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}

}


void My_USART_Send_String(USART_TypeDef* USARTx, char* s)
{
	char* temp;
	
	temp = s;
	while(*temp != '\0')
	{
		USART_SendData(USARTx, *temp++);
		//检测是够传输完成
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == 0)  //传输完成后硬件置1
			;
	}
}


void USART1_IRQHandler(void)
{
	u16 res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		res = USART_ReceiveData(USART1);
		USART_SendData(USART1, res);
	}
}

