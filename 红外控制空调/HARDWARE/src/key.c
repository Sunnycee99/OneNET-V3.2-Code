#include "key.h"
#include "delay.h"

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

u8 Key_Scan(void)
{
    if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)&GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12)))  //ÅÐ¶Ï11 12Î»
    {
        delay_ms(10);
        if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)&GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12)))
        {
            if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11))
							return 2;
						if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12))
							return 3;
        }
    }
		return 0;
}
