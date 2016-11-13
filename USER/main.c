#include "spi.h"
#include "global.h"

int main(void)
{   //system_stm32f4xx.c #define PLL_M=8 PLL_N=336 HSE -> SYSCLK 168MHZ
	// RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; APHB1_CLK 84MHZ
	//AHB 168MHZ  APB2 84MHZ  APB1 42MHZ
	
	delay_init(168);  //初始化延时函数
	uart_init(115200);//初始化串口波特率为9600
	SPI2_Init();
    while(1) 
	{
	}
}
