#include "ir.h"
#include "delay.h"

void IR_Init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrnct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);  //ÍêÈ«ÖØÓ³Éä

    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_9;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpio_initstruct);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);

    TIM_TimeBaseInitStrnct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStrnct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrnct.TIM_Period = 25;
    TIM_TimeBaseInitStrnct.TIM_Prescaler = 76;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrnct);

    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC4Init(TIM3, &TIM_OCInitStruct);

    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_SetCompare4(TIM3, 25);

    TIM_Cmd(TIM3, ENABLE);
}

void IR_Send(u8 high_low,u16 time)
{
    if(high_low==1)
    {
//		TIM_Cmd(TIM3, ENABLE);
//		delay_us(time);
        TIM3->CCR4 = 20;
        delay_us(time);
        TIM3->CCR4 = 25;
    }
    if(high_low==0)
    {
//		TIM_Cmd(TIM3, DISABLE);
//		delay_us(time);
        TIM3->CCR4 = 25;
        delay_us(time);
        TIM3->CCR4 = 20;
    }
		if(high_low==2)
    {
//		TIM_Cmd(TIM3, DISABLE);
//		delay_us(time);
        TIM3->CCR4 = 20;
        delay_us(time);
    }
}

