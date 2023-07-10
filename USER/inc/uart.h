#include "main.h"

/*UART1------------------------------------*/
//UART1
#define  DEBUG_USARTx                    CW_UART1
#define  DEBUG_USART_CLK                 RCC_APB2_PERIPH_UART1
#define  DEBUG_USART_APBClkENx           RCC_APBPeriphClk_Enable2
#define  DEBUG_USART_BaudRate            19200
#define  DEBUG_USART_UclkFreq            24000000
//UART1 GPIO
#define  DEBUG_USART_GPIO_CLK            RCC_AHB_PERIPH_GPIOA
#define  DEBUG_USART_TX_GPIO_PORT        CW_GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN         GPIO_PIN_8
#define  DEBUG_USART_RX_GPIO_PORT        CW_GPIOA
#define  DEBUG_USART_RX_GPIO_PIN         GPIO_PIN_9
//GPIO AF
#define  DEBUG_USART_AFTX                PA08_AFx_UART1TXD()
#define  DEBUG_USART_AFRX                PA09_AFx_UART1RXD()
//ÖÐ¶Ï
#define  DEBUG_USART_IRQ                 UART1_IRQn

//UART2 for debug printf
#define  DEBUG_USART2                    CW_UART2
#define  DEBUG_USART2_CLK                RCC_APB1_PERIPH_UART2
#define  DEBUG_USART2_APBClkENx          RCC_APBPeriphClk_Enable1
#define  DEBUG_USART2_BaudRate           19200
#define  DEBUG_USART2_UclkFreq           24000000

//UART2 GPIO
#define  DEBUG_USART2_GPIO_CLK           RCC_AHB_PERIPH_GPIOB
#define  DEBUG_USART2_TX_GPIO_PORT       CW_GPIOB   
#define  DEBUG_USART2_TX_GPIO_PIN        GPIO_PIN_1
#define  DEBUG_USART2_RX_GPIO_PORT       CW_GPIOB
#define  DEBUG_USART2_RX_GPIO_PIN        GPIO_PIN_0

//GPIO AF
#define  DEBUG_USART2_AFTX               PB01_AFx_UART2TXD()
#define  DEBUG_USART2_AFRX               PB00_AFx_UART2RXD()

void GPIO_Configuration(void);
void UART_Configuration(void);
void UART_Init(void);
void USART_SendString(UART_TypeDef* USARTx, char *String);

