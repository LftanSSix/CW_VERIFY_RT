#include "main.h"

extern uint32_t SystemCoreClock;
extern const u8 EQ_Address[];
extern volatile u8 ucTemp[4];

extern u8 MemData[];
extern u8 KEY_CHK_Flag,BuzzFlag;
extern u8 rx_count,NewPort,Global_portnum;
extern u8 EQ_Data,EQ_UPData;
extern u8 TxRxBuffer[4];
extern u8 rx_num;
extern u16 Tim_BuzzCnt;
extern void OEM_1_MainLoop_3(void);	
extern void OEM_1_MainLoop(void);
extern u8 it6635_set_active_port3(u8 chipx, u8 port);
extern u8 it6635_set_active_port(u8 port);
u8 g_dev_loop_timeout_3, g_dev_loop_timeout;
u8 YK_KH12S_HdmiPort;
extern u8 it6635_i2c_read(u8 addr, u8 offset, u8 length, u8 *buffer);
u8 bbud[3], bbud1[3];
extern void uartCMDGen(void);
extern void selPortX(u8 px);

int32_t main(void)
{ 
  RCC_Config(); 
  Lvd_Config();//Ƿѹ��λ����
  delay1ms(100);//�ȴ���λ���

  UART_Init();
  gpio_init();	
  i2c_init();

  NVIC_Configuration();
  TIM_Init();

  // USART_ITConfig(DEBUG_USARTx, USART_IT_RC, ENABLE); //UART1ʹ�ܽ����ж�
  USART_ITConfig(DEBUG_USART2, USART_IT_RC, ENABLE); //UART2ʹ�ܽ����ж�
  while(1)
  { 

	
  }
}

void RCC_Config(void)
{
	//����RCC
	RCC_HSI_Enable(RCC_HSIOSC_DIV2);	//24m
	RCC_SYSCLKSRC_Config(RCC_SYSCLKSRC_HSI);  //update SystemCoreClock	
	RCC_HCLKPRS_Config(RCC_HCLK_DIV1);//24M
	RCC_PCLKPRS_Config(RCC_PCLK_DIV1);//24M
	InitTick( SystemCoreClock );

	RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_FLASH, ENABLE); //ʹ��flashʱ��
}

/*******************************************************************************
  * @FunctionName: NVIC_Configuration
  * @Author:       ftansiX
  * @DateTime:     2022��7��8��T11:40:32+0800
  * @Purpose:      �ж����ȼ����ã��ļ����ȼ���0-3��from high to low),ͬһ���ȼ����жϺ�����
  * @param:        void               
*******************************************************************************/
void NVIC_Configuration(void)
{
  // NVIC_SetPriority(DEBUG_USART_IRQ,0); //UART1�ж����ȼ�,�жϺ�27
  // NVIC_EnableIRQ(DEBUG_USART_IRQ);     //UART1�ж�ʹ��
	
  // NVIC_SetPriority(UART2_IRQn,0);      //UART2�ж����ȼ�,�жϺ�28
  // NVIC_EnableIRQ(UART2_IRQn);          //UART2�ж�ʹ��
	
  // NVIC_SetPriority(GTIM3_IRQn,2);      //GTIM3�ж����ȼ�,�жϺ�18
  // NVIC_EnableIRQ(GTIM3_IRQn);          //GTIM3�ж�ʹ��

  // NVIC_SetPriority(GTIM4_IRQn,2);      //GTIM4�ж����ȼ�,�жϺ�19
  // NVIC_EnableIRQ(GTIM4_IRQn);          //GTIM4�ж�ʹ��
  
//   NVIC_SetPriority(ADC_IRQn,2);        //ADC�ж����ȼ�,�жϺ�12
//   NVIC_EnableIRQ(ADC_IRQn);            //ADC�ж�ʹ��
}

/*******************************************************************************
  * @FunctionName: ADC1_Init
  * @Author:       ftansiX
  * @DateTime:     2022��7��11��T9:50:56+0800
  * @Purpose:      PB02->ADC_IN1
  * @param:        void               
*******************************************************************************/
void ADCIN1_Init(void)
{
  RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_ADC, ENABLE);    
  
  ADC_InitTypeDef ADC_InitStructure;
  ADC_SingleChTypeDef ADC_SingleChStructure;
  
  //ADCĬ��ֵ��ʼ��
  ADC_StructInit(&ADC_InitStructure);

  ADC_InitStructure.ADC_VrefSel = ADC_Vref_VDDA;            //�ο���ѹ******xx
  //���õ�ͨ��ת��ģʽ
  ADC_SingleChStructure.ADC_DiscardEn = ADC_DiscardNull;
  ADC_SingleChStructure.ADC_Chmux = ADC_ExInputCH10;        //ͨ��10����PB02
  ADC_SingleChStructure.ADC_InitStruct = ADC_InitStructure;

  ADC_SingleChOneModeCfg(&ADC_SingleChStructure);
  ADC_ITConfig(ADC_IT_EOC, ENABLE);

  ADC_EnableIrq(ADC_INT_PRIORITY);                          //���ȼ��� ��3��
  ADC_ClearITPendingAll();

  //ADCʹ��
  ADC_Enable();

  //ADC�ⲿ�жϴ���Դ����
  ADC_ExtTrigCfg(ADC_TRIG_GTIM3, ENABLE);
  GTIM_Cmd(CW_GTIM3, ENABLE);
}
/*******************************************************************************
  * @FunctionName: Lvd_Config
  * @Author:       ftansiX
  * @DateTime:     2022��7��29��T10:35:12+0800
  * @Purpose:      Ƿѹ��λ,
  * @param:        void  
  * @note:         ��׼��ѹ���ó���3.2v(stlink����)���������ز��ˣ��޷���λ 
*******************************************************************************/
void Lvd_Config(void)
{
  LVD_InitTypeDef LVD_InitStruct = {0};

  LVD_InitStruct.LVD_Source = LVD_Source_VDDA;          //����LVD�����
  LVD_InitStruct.LVD_Threshold = LVD_Threshold_2p88V;   //����LVD��׼��ѹ2.88V,����2.88->reset
  LVD_InitStruct.LVD_FilterTime = LVD_FilterTime_4095Clk;
  LVD_InitStruct.LVD_FilterClk = LVD_FilterClk_RC150K;
  LVD_InitStruct.LVD_Action = LVD_Action_Reset;         //����ʱ����RESET
  LVD_InitStruct.LVD_FilterEn = LVD_Filter_Enable;
  LVD_Init(&LVD_InitStruct);

  LVD_TrigConfig(LVD_TRIG_LEVEL, ENABLE);               //����ѹ������ֵʱ���� LVD ����
  LVD_Enable();                                         //LVDʹ��
}
