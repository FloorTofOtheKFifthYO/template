
#include "stdio.h"	
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "stdarg.h"
#include "delay.h"
#include <stdbool.h>

#define USART_printf USART1
#define _printf 1

#ifndef bluetooth
#define bluetooth UART5
#endif


#define STR_BUFFER_LEN 128

#define USART_REC_LEN  			200 
extern u8  UART5_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 UART5_RX_STA;         		//接收状态标记	
extern u8  USART1_RX_BUF[USART_REC_LEN];    
extern u16 USART1_RX_STA;

void uart_init(u32 bound);
void rcc_config(void);
void gpio_config(void);
void NVIC_Configuration(uint8_t NVIC_IRQChannel,uint8_t NVIC_IRQChannelPreemptionPriority,
	uint8_t NVIC_IRQChannelSubPriority,FunctionalState NVIC_IRQChannelCmd);
void GPIO_Configuration(uint16_t GPIO_Pin,GPIOMode_TypeDef  GPIO_Mode,GPIOOType_TypeDef GPIO_OType,
	GPIOSpeed_TypeDef GPIO_Speed,GPIOPuPd_TypeDef GPIO_PuPd,GPIO_TypeDef* GPIOx);
void EXTI_Configuration(uint32_t EXTI_Line,
                EXTIMode_TypeDef EXTI_Mode,
                EXTITrigger_TypeDef EXTI_Trigger,
                FunctionalState EXTI_LineCmd);
void nvic_config(void);
void USART_SendString(USART_TypeDef* USARTx, char *fmt, ...);

extern u8 cmd,sticks[4];



