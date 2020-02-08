#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"

void My_USART1_Init(void);
void My_USART2_Init(void);
void My_USART_Send_String(USART_TypeDef* USARTx, char* s);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

#endif

