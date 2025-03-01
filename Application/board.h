/*
 * HEE_board.h
 *
 *  Created on: Jul 27, 2024
 *      Author: duong
 */

#ifndef BOARD_H_
#define BOARD_H_

/*********************ADC***********************/
#define ADC_FEEDBACK_HANDLE     ADC1
#define ADC_FEEDBACK_IRQ        ADC1_IRQn

#define ADC_CHANNEL_COUNT       2
#define ADC_300V_CHANNEL        LL_ADC_CHANNEL_3
#define ADC_50V_CHANNEL         LL_ADC_CHANNEL_4
/***********************************************/

/*********************FLYBACK*******************/
#define FLYBACK_SW1_HANDLE      TIM17
#define FLYBACK_SW1_CHANNEL     LL_TIM_CHANNEL_CH1

#define FLYBACK_SD1_PORT        GPIOC
#define FLYBACK_SD1_PIN         LL_GPIO_PIN_13

#define FLYBACK_SW2_HANDLE      TIM3
#define FLYBACK_SW2_CHANNEL     LL_TIM_CHANNEL_CH2

#define FLYBACK_SD2_PORT        GPIOA
#define FLYBACK_SD2_PIN         LL_GPIO_PIN_10
/***********************************************/

/*******************DISCHARGE*******************/
#define DISCHARGE_300V_HANDLE   TIM16
#define DISCHARGE_300V_CHANNEL  LL_TIM_CHANNEL_CH1
#define DISCHARGE_300V_PORT     GPIOB
#define DISCHARGE_300V_PIN      LL_GPIO_PIN_8

#define DISCHARGE_50V_HANDEL    TIM1
#define DISCHARGE_50V_CHANNEL   LL_TIM_CHANNEL_CH2
#define DISCHARGE_50V_PORT      GPIOA
#define DISCHARGE_50V_PIN       LL_GPIO_PIN_9
/***********************************************/

/*********************OVP***********************/
#define OVP_300_PORT            GPIOB
#define OVP_300_PIN             LL_GPIO_PIN_2

#define OVP_50_PORT             GPIOB
#define OVP_50_PIN              LL_GPIO_PIN_1
/***********************************************/

/*********************UART**********************/
#define RS232_UART_HANDLE       USART4
#define RS232_UART_IRQ          USART3_6_IRQn

#define RF_UART_HANDLE          USART1
#define RF_UART_IRQ             USART1_IRQn

#define GPP_UART_HANDLE         USART3
#define GPP_UART_IRQ            USART3_6_IRQn

#define GPP_TX_SIZE			    64
#define	GPP_RX_SIZE			    64
#define FSP_BUF_LEN				64

/**********************SPI**********************/
#define FRAM_SPI_HANDLE         SPI1

#define FRAM_SPI_CS_PORT        GPIOB
#define FRAM_SPI_CS_PIN         LL_GPIO_PIN_0
/***********************************************/

/*******************DEBUG LED*******************/
#define DEBUG_LED_PORT          GPIOA
#define DEBUG_LED_PIN           LL_GPIO_PIN_2
/***********************************************/

/*******************DEBUG GPIO******************/
#define DEBUG_GPIO1_PORT        GPIOA
#define DEBUG_GPIO1_PIN         LL_GPIO_PIN_11

#define DEBUG_GPIO2_PORT        GPIOA
#define DEBUG_GPIO2_PIN         LL_GPIO_PIN_12

#define DEBUG_GPIO3_PORT        GPIOB
#define DEBUG_GPIO3_PIN         LL_GPIO_PIN_13

#define DEBUG_GPIO4_PORT        GPIOB
#define DEBUG_GPIO4_PIN         LL_GPIO_PIN_14

#define DEBUG_GPIO5_PORT        GPIOB
#define DEBUG_GPIO5_PIN         LL_GPIO_PIN_15
/***********************************************/

#endif /* BOARD_H_ */
