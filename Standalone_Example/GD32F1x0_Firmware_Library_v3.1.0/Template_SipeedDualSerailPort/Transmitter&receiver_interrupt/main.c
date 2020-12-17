/*!
    \file  main.c
    \brief USART transmit and receive interrupt
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0.h"
#include <stdio.h>

#include "config.h"

#define ARRAYNUM(arr_nanme) (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE (ARRAYNUM(transmitter_buffer) - 1)

uint8_t transmitter_buffer[] = "\n\rUSART interrupt test\n\r";
uint8_t receiver_buffer[32];
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 32;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;



/**
  * @函数名       USART1_Init
  * @功  能       初始化USART1
  * @参  数       无
  * @返回值       无
  */
void USART1_Init(void)
{
	/* 使能GPIOA外设时钟 */
	rcu_periph_clock_enable( COM_GPIO_CLK);

	/* 使能USART1外设时钟 */
	rcu_periph_clock_enable(COM_CLOCK);

	/* 配置USART相关的RX、TX引脚作为复用功能 */
	gpio_af_set(COM_GPIO_PORT, COM_AF, COM_TX_PIN);
	gpio_af_set(COM_GPIO_PORT, COM_AF, COM_RX_PIN);

	gpio_mode_set(COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_TX_PIN);
	gpio_output_options_set(COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_TX_PIN);

	gpio_mode_set(COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN);
	gpio_output_options_set(COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_RX_PIN);

	/* 配置USART */
	usart_deinit(COM_NUM);	//端口号
	usart_baudrate_set(COM_NUM,115200U);	//波特率115200
	usart_transmit_config(COM_NUM, USART_TRANSMIT_ENABLE);	//使能发送
	usart_receive_config(COM_NUM, USART_RECEIVE_ENABLE);	//使能接收
	usart_enable(COM_NUM);	//使能USART1
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* USART interrupt configuration */
    nvic_irq_enable(COM_IRQn, 0, 0);

    USART1_Init();

    printf("-------------------------------\r\n");
    printf("Sipeed@QinYUN575\r\n");
    printf("Compile@ %s %s\r\n", __DATE__, __TIME__);
    printf("-------------------------------\r\n");

    /* enable USART TBE interrupt */
    usart_interrupt_enable(COM_NUM, USART_INT_TBE);

    /* wait until USART send the transmitter_buffer */
    while (txcount < transfersize)
        ;

    while (RESET == usart_flag_get(COM_NUM, USART_FLAG_TC))
        ;

    usart_interrupt_enable(COM_NUM, USART_INT_RBNE);

    /* wait until USART receive the receiver_buffer */
    while (rxcount < receivesize)
        ;
    if (rxcount == receivesize)
        printf("\n\rUSART receive successfully!\n\r");

    while (1)
        ;

}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(COM_NUM, (uint8_t)ch);
    while (RESET == usart_flag_get(COM_NUM, USART_FLAG_TBE))
        ;
    return ch;
}
