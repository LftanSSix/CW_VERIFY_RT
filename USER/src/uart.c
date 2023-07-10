#include "uart.h"
/**
 * @brief ≈‰÷√GPIO
 * 
 */
void GPIO_Configuration(void)
{
	RCC_AHBPeriphClk_Enable(DEBUG_USART_GPIO_CLK, ENABLE);	
	RCC_AHBPeriphClk_Enable(DEBUG_USART2_GPIO_CLK, ENABLE);	
  DEBUG_USART_APBClkENx(DEBUG_USART_CLK, ENABLE);
  DEBUG_USART2_APBClkENx(DEBUG_USART2_CLK, ENABLE);

  GPIO_InitTypeDef UART_GPIO_InitStructure;

  //UART TX RX ∏¥”√ 
  DEBUG_USART_AFTX;                     
  DEBUG_USART_AFRX;
  DEBUG_USART2_AFTX;  
  DEBUG_USART2_AFRX;           
	
  UART_GPIO_InitStructure.Pins = DEBUG_USART_TX_GPIO_PIN;
  UART_GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  UART_GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &UART_GPIO_InitStructure);
    
  UART_GPIO_InitStructure.Pins = DEBUG_USART_RX_GPIO_PIN;
  UART_GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &UART_GPIO_InitStructure);

  UART_GPIO_InitStructure.Pins = DEBUG_USART2_TX_GPIO_PIN;
  UART_GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  UART_GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &UART_GPIO_InitStructure);

  UART_GPIO_InitStructure.Pins = DEBUG_USART2_RX_GPIO_PIN;
  UART_GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
  UART_GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &UART_GPIO_InitStructure);
}

/**
 * @brief ≈‰÷√UART
 * 
 */
void UART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = DEBUG_USART_BaudRate;
  USART_InitStructure.USART_Over = USART_Over_16;
  USART_InitStructure.USART_Source = USART_Source_PCLK;
  USART_InitStructure.USART_UclkFreq = DEBUG_USART_UclkFreq;
  USART_InitStructure.USART_StartBit = USART_StartBit_FE;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USARTx, &USART_InitStructure); 

  USART_InitStructure.USART_BaudRate = DEBUG_USART2_BaudRate;
  USART_InitStructure.USART_Over = USART_Over_16;
  USART_InitStructure.USART_Source = USART_Source_PCLK;
  USART_InitStructure.USART_UclkFreq = DEBUG_USART2_UclkFreq;
  USART_InitStructure.USART_StartBit = USART_StartBit_FE;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USART2, &USART_InitStructure); 
}

void UART_Init(void)
{
	GPIO_Configuration();
	UART_Configuration();
}

/**
 * @brief Retargets the C library printf function to the USART.
 * 
 */
int fputc(int ch, FILE *f)
{
  #if 0 //0:use uart2 1:use uart1
  USART_SendData_8bit(DEBUG_USARTx, (uint8_t)ch);
  while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
  #else
  USART_SendData_8bit(DEBUG_USART2, (uint8_t)ch);
  while (USART_GetFlagStatus(DEBUG_USART2, USART_FLAG_TXE) == RESET);
  #endif
  return ch;
}

