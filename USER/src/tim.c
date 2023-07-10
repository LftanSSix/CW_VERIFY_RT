#include "tim.h"


void GTIM4xx_Init(void)
{
  RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_GTIM4,ENABLE);

  GTIM_InitTypeDef GTIM_InitStruct;
  GTIM_ICInitTypeDef GTIM_ICInitStruct;

  GTIM_InitStruct.Mode = GTIM_MODE_TIME;
  GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;
  GTIM_InitStruct.Prescaler = GTIM_PRESCALER_DIV8; 				//24/8=3MHZ 1/3us计数一次
  GTIM_InitStruct.ReloadValue = 30000-1;  								//10ms溢出一次
  GTIM_InitStruct.ToggleOutState = DISABLE;
  GTIM_TimeBaseInit(CW_GTIM4, &GTIM_InitStruct);

  GTIM_ICInitStruct.CHx = GTIM_CHANNEL1;
  GTIM_ICInitStruct.ICFilter = GTIM_CHx_FILTER_NONE;
  GTIM_ICInitStruct.ICInvert = GTIM_CHx_INVERT_ON; 				//反相输入开启
  GTIM_ICInitStruct.ICPolarity = GTIM_ICPolarity_Rising;  //上升沿
  GTIM_ICInit(CW_GTIM4, &GTIM_ICInitStruct);

  GTIM_ITConfig(CW_GTIM4, GTIM_IT_CC1|GTIM_IT_OV, ENABLE);
  GTIM_Cmd(CW_GTIM4, ENABLE);
}

void GTIM3xx_Init(void)
{
  RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_GTIM3,ENABLE);

  GTIM_InitTypeDef GTIM_InitStruct;
	
  GTIM_InitStruct.Mode = GTIM_MODE_TIME;
  GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;
  GTIM_InitStruct.Prescaler = GTIM_PRESCALER_DIV8; 				//24/8=3MHZ 1/3us计数一次
  GTIM_InitStruct.ReloadValue = 30000-1; 								 	//10ms溢出一次
  GTIM_InitStruct.ToggleOutState = DISABLE;
  GTIM_TimeBaseInit(CW_GTIM3, &GTIM_InitStruct);

  GTIM_ITConfig(CW_GTIM3, GTIM_IT_OV, ENABLE);
  GTIM_Cmd(CW_GTIM3, ENABLE); //Implement in ADCIN1_Init()
}

void TIM_Init(void)
{
  GTIM3xx_Init();
//  GTIM4xx_Init();
//  ATIMxx_Init();
//  GTIM2xx_Init();
//  BTIM1xx_Init(); 
//  BTIM2xx_Init();
}
#if 0
void GTIM2xx_Init(void)
{
  RCC_APBPeriphClk_Enable1(RCC_APB1_PERIPH_GTIM2,ENABLE);

  GTIM_InitTypeDef GTIM_InitStruct;

  GTIM_InitStruct.Mode = GTIM_MODE_TIME;
  GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;
  GTIM_InitStruct.Prescaler = GTIM_PRESCALER_DIV8; //24/8=3MHZ 1/3us计数一次
  GTIM_InitStruct.ReloadValue = 30000-1;  //10ms溢出一次
  GTIM_InitStruct.ToggleOutState = DISABLE;
  GTIM_TimeBaseInit(CW_GTIM2, &GTIM_InitStruct);

  GTIM_ITConfig(CW_GTIM2, GTIM_IT_OV, ENABLE);
  GTIM_Cmd(CW_GTIM2, ENABLE);
}

void BTIM1xx_Init(void)
{
  RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_BTIM,ENABLE);
  __RCC_BTIM_CLK_ENABLE();
  
  BTIM_TimeBaseInitTypeDef BTIM1_TimeBaseInitStruct;
	
  BTIM1_TimeBaseInitStruct.BTIM_Mode = BTIM_Mode_TIMER; //定时器模式
  BTIM1_TimeBaseInitStruct.BTIM_Period = 30000-1; //30000->10ms中断间隔。     24Mhz/8
  BTIM1_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV8;  //24/8分频

  BTIM_TimeBaseInit(CW_BTIM1, &BTIM1_TimeBaseInitStruct);
  
  BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);  //计数器溢出中断使能控制
//  BTIM_Cmd(CW_BTIM1, ENABLE); 
}

void BTIM2_Init(void)
{
  RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_BTIM,ENABLE);
  
  BTIM_TimeBaseInitTypeDef BTIM2_TimeBaseInitStruct;
	
  BTIM2_TimeBaseInitStruct.BTIM_Mode = BTIM_Mode_TIMER; //定时器模式
  BTIM2_TimeBaseInitStruct.BTIM_Period = 30000-1; //3000->1ms中断间隔。     24Mhz/8
  BTIM2_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV8;  //8分频

  BTIM_TimeBaseInit(CW_BTIM2, &BTIM2_TimeBaseInitStruct);
  BTIM_ITConfig(CW_BTIM2, BTIM_IT_OV, ENABLE);  //OV 溢出中断使能控制
  
	BTIM_Cmd(CW_BTIM2, ENABLE);
}

void ATIMxx_Init(void)
{
  ATIM_InitTypeDef ATIM_InitStruct;

  __RCC_ATIM_CLK_ENABLE();  

  ATIM_InitStruct.BufferState = ENABLE;            
  ATIM_InitStruct.ClockSelect = ATIM_CLOCK_PCLK;  
  ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;    
  ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;       
  ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;   
  ATIM_InitStruct.OverFlowMask = DISABLE;       
  ATIM_InitStruct.Prescaler = ATIM_Prescaler_DIV8;  
  ATIM_InitStruct.ReloadValue = 30000-1;               
  ATIM_InitStruct.RepetitionCounter = 0;            
  ATIM_InitStruct.UnderFlowMask = DISABLE;          

  ATIM_Init(&ATIM_InitStruct);
  ATIM_ITConfig(ATIM_CR_IT_OVE, ENABLE);            
  ATIM_Cmd(ENABLE);
	
}
#endif



