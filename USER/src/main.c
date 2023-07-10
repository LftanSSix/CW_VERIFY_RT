#include "main.h"
/* Standard includes. */
#include "string.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
 #include "queue.h"
 
// ������Ϣ���о���Ͷ��г���
#define QUEUE_LENGTH 5
QueueHandle_t xQueue = NULL;

static TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t testTask1Handle = NULL;
TaskHandle_t testTask2Handle = NULL;
TaskHandle_t sendTaskHandle = NULL;
TaskHandle_t revTaskHandle = NULL;

static void AppTaskCreate(void);
void testTask1(void);
void testTask2(void);
void SenderTask(void* pvParameters);
void ReceiverTask(void* pvParameters);

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
u8 flg;

int32_t main(void)
{ 
  RCC_Config(); 
//  Lvd_Config();//Ƿѹ��λ����
	
  UART_Init();
  gpio_init();	
  i2c_init();

  // NVIC_Configuration();
  TIM_Init();

  // USART_ITConfig(DEBUG_USARTx, USART_IT_RC, ENABLE); //UART1ʹ�ܽ����ж�
  // USART_ITConfig(DEBUG_USART2, USART_IT_RC, ENABLE); //UART2ʹ�ܽ����ж�
	
  BaseType_t xReturn = pdPASS;
  
   /* Create AppTaskCreate Task */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )200,
                        (void*          )NULL,
                        (UBaseType_t    )1,
                        (TaskHandle_t*  )&AppTaskCreate_Handle);
  if(xReturn == pdTRUE)
	{
	  printf("Create task ok, Start the scheduler\r\n"); //Create task ok
	
		/* Start the scheduler. */
		vTaskStartScheduler();
	}

  for( ;; );
}

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;
  taskENTER_CRITICAL();
	
	// ������Ϣ����
	xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(char*));
	if(xQueue != NULL)
		printf("Create Queue ok \r\n");
	
	
  xReturn = xTaskCreate((TaskFunction_t )SenderTask,
                        (const char*    )"SenderTask",
                        (uint16_t       )50, //
                        (void*          )NULL, 
                        (UBaseType_t    )2,
                        (TaskHandle_t*  )&sendTaskHandle);
  if(xReturn == pdTRUE)
	  printf("Create SenderTask ok\r\n");
  
	xReturn = xTaskCreate((TaskFunction_t )testTask1,
                        (const char*    )"testTask1",
                        (uint16_t       )50, //
                        (void*          )NULL, 
                        (UBaseType_t    )3,
                        (TaskHandle_t*  )&testTask1Handle);
  if(xReturn == pdTRUE)
	  printf("Create task1 ok\r\n");
	
  xReturn = xTaskCreate((TaskFunction_t )ReceiverTask,
                        (const char*    )"ReceiverTask",
                        (uint16_t       )50,
                        (void*          )NULL,
                        (UBaseType_t    )2,
                        (TaskHandle_t*  )&revTaskHandle);
  if(xReturn == pdTRUE)
	  printf("Create ReceiverTask ok\r\n");
	printf("Before TaskDelete FreeHeapSize is %d\r\n", xPortGetFreeHeapSize());
  vTaskDelete(AppTaskCreate_Handle);
	printf("After TaskDelete FreeHeapSize is %d\r\n", xPortGetFreeHeapSize());
  taskEXIT_CRITICAL();
}

void testTask1(void)
{
	printf("FreeHeapSize is %d\r\n", xPortGetFreeHeapSize());
	char *tt1 = "testTask1 2s run";
//	TickType_t tickcnt[2] = {0};
	for( ;; )
	{	
		//PA03_TOG();
		PA04_SETLOW();
		xQueueSend(xQueue, &tt1, portMAX_DELAY);
//		tickcnt[0] = xTaskGetTickCount();
		vTaskDelay(4000);
//		tickcnt[1] = xTaskGetTickCount();
//		printf("interval is : %d\r\n", (tickcnt[1] - tickcnt[0]));
	}
}

void testTask2(void)
{
	for( ;; )
	{
		//PA04_TOG();
		PA04_SETHIGH();
		//vTaskDelay(5);
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

//add test

// ��������
void SenderTask(void* pvParameters)
{
    char *data = "setokoiyfcdcfgkkokkkkkkkkkk";
		int tet1 = 12;
    while (1)
    {
        // �������ݵ���Ϣ����
        xQueueSend(xQueue, &tet1, portMAX_DELAY);

        // ��ʱһ��ʱ��
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ��������
void ReceiverTask(void* pvParameters)
{
    int receivedData;
	  char *rechar;
    while (1)
    {
        // ����Ϣ���н�������
        if (xQueueReceive(xQueue, &receivedData, portMAX_DELAY))
        {
            // ������յ�������
            //printf("Received data: %s\r\n", rechar);
						printf("Received data: %d\r\n", receivedData);
        }
    }
}

//enddddd
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
	PA04_TOG();
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}
/*-----------------------------------------------------------*/
