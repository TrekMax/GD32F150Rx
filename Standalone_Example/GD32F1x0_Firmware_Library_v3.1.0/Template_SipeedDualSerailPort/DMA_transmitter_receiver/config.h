#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "gd32f1x0.h"

#define COM1

#ifdef COM1
    #define COM_NUM             USART1
    #define COM_CLOCK           RCU_USART1
    #define COM_IRQn            USART1_IRQn

    #define COM_GPIO_PORT       GPIOA
    #define COM_GPIO_CLK        RCU_GPIOA
    #define COM_AF              GPIO_AF_1
    #define COM_TX_PIN          GPIO_PIN_2
    #define COM_RX_PIN          GPIO_PIN_3
    #define COM_DMA_TX_CH       DMA_CH3
    #define COM_DMA_RX_CH       DMA_CH4
#else
    #define COM_NUM             USART0
    #define COM_CLOCK           RCU_USART0
    #define COM_IRQn            USART0_IRQn

    #define COM_GPIO_PORT       GPIOB
    #define COM_GPIO_CLK        RCU_GPIOB
    #define COM_AF              GPIO_AF_0
    #define COM_TX_PIN          GPIO_PIN_6
    #define COM_RX_PIN          GPIO_PIN_7
    #define COM_DMA_TX_CH       DMA_CH1
    #define COM_DMA_RX_CH       DMA_CH2
#endif

#endif
