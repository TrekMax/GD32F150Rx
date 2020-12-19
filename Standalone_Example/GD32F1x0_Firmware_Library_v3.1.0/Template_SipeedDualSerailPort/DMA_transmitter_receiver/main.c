/*!
    \file  main.c
    \brief USART DMA transmitter receiver
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

uint8_t rxbuffer[10];
uint8_t txbuffer[] = "\n\rUSART DMA receive and transmit example, please input 10 bytes:\n\r";
#define ARRAYNUM(arr_nanme) (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define USART0_TDATA_ADDRESS ((uint32_t)0x40013828) /* 130_150 device */
#define USART0_RDATA_ADDRESS ((uint32_t)0x40013824) /* 130_150 device */
#define USART1_TDATA_ADDRESS ((uint32_t)0x40004428) /* 170_190 device */
#define USART1_RDATA_ADDRESS ((uint32_t)0x40004424) /* 170_190 device */

#ifdef COM1
#define COM_TDATA_ADDRESS USART1_TDATA_ADDRESS
#define COM_RDATA_ADDRESS USART1_RDATA_ADDRESS
#else
#define COM_TDATA_ADDRESS USART0_TDATA_ADDRESS
#define COM_RDATA_ADDRESS USART0_RDATA_ADDRESS
#endif

extern __IO uint8_t txcount;
extern __IO uint16_t rxcount;

/**
  * @函数名       USART_Init
  * @功  能       初始化USART1
  * @参  数       无
  * @返回值       无
  */
void USART_Init(void)
{
    /* 使能GPIOA外设时钟 */
    rcu_periph_clock_enable(COM_GPIO_CLK);

    /* 使能USART1外设时钟 */
    rcu_periph_clock_enable(COM_CLOCK);

    /* 配置USART相关的RX、TX引脚作为复用功能 */
    gpio_af_set(COM_GPIO_PORT, COM_AF, COM_TX_PIN);
    gpio_af_set(COM_GPIO_PORT, COM_AF, COM_RX_PIN);

    gpio_mode_set(COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN);
    gpio_output_options_set(COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_TX_PIN);

    gpio_mode_set(COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN);
    gpio_output_options_set(COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_RX_PIN);

    /* 配置USART */
    usart_deinit(COM_NUM);                                 //端口号
    usart_baudrate_set(COM_NUM, 115200U);                  //波特率115200
    usart_transmit_config(COM_NUM, USART_TRANSMIT_ENABLE); //使能发送
    usart_receive_config(COM_NUM, USART_RECEIVE_ENABLE);   //使能接收
    usart_enable(COM_NUM);                                 //使能USART1
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    USART_Init();

    printf("-------------------------------\r\n");
    printf("Sipeed@QinYUN575\r\n");
    printf("Compile@ %s %s\r\n", __DATE__, __TIME__);
    printf("-------------------------------\r\n");

    printf("\n\ra usart dma function test example!\n\r");

    /* deinitialize DMA channel1 */
    dma_deinit(COM_DMA_TX_CH);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)txbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(txbuffer);
    dma_init_struct.periph_addr = COM_TDATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(COM_DMA_TX_CH, dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(COM_DMA_TX_CH);
    dma_memory_to_memory_disable(COM_DMA_TX_CH);
    /* enable DMA channel1 */
    dma_channel_enable(COM_DMA_TX_CH);
    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(COM_NUM, USART_DENT_ENABLE);

    /* wait DMA channel transfer complete */
    while (RESET == dma_flag_get(COM_DMA_TX_CH, DMA_FLAG_FTF))
        ;
    while (1)
    {
        /* deinitialize DMA channel2 */
        dma_deinit(COM_DMA_RX_CH);
        dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr = (uint32_t)rxbuffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = 10;
        dma_init_struct.periph_addr = COM_RDATA_ADDRESS;
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(COM_DMA_RX_CH, dma_init_struct);

        dma_circulation_disable(COM_DMA_RX_CH);
        dma_memory_to_memory_disable(COM_DMA_RX_CH);

        dma_channel_enable(COM_DMA_RX_CH);
        usart_dma_receive_config(COM_NUM, USART_DENR_ENABLE);
        printf("wait DMA channel transfer complete!\r\n");
        /* wait DMA channel transfer complete */
        while (RESET == dma_flag_get(COM_DMA_RX_CH, DMA_FLAG_FTF))
            ;
        printf("\n\r%s\n\r", rxbuffer);
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(COM_NUM, (uint8_t)ch);
    while (RESET == usart_flag_get(COM_NUM, USART_FLAG_TBE))
        ;
    return ch;
}
