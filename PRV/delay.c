#include "delay.h"

u8	fac_us = 8000000/1000000; //1us时间所需要的计数次数
u16	fac_ms = 8000000/1000;
////nus 为要延时的 us 数.
//void delay_us(u32 nus)
//{ 
//	u32 temp; 
//	SysTick->LOAD=(u32)nus*fac_us; //时间加载 
//	SysTick->VAL=0x00; //清空计数器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器
//	SysTick->VAL =0X00; //清空计数器 
//}

//Delay nus
//nus: the number of us to delay	
//NUS: 0 ~ 204522252 (max. 2 ^ 32 / FAC)_ us@fac_us =21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//Value of LOAD	    	 
	ticks=nus*fac_us; 						//Number of beats required 
//	delay_osschedlock();					//Prevent OS scheduling and interrupt us delay
	told=SysTick->VAL;        				//Counter value when entering
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//Note that SYSTICK is a decrement counter
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//If the time exceeds / equals the time to be delayed, exit
		}  
	};
//	delay_osschedunlock();					//Resume OS scheduling											    
} 
//延时 nms
//注意 nms 的范围
//SysTick->LOAD 为 24 位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK 单位为 Hz,nms 单位为 ms
//对 72M 条件下,nms<=1864
void delay_ms(u16 nms)
{ 
	u32 temp; 
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD 为 24bit)
	SysTick->VAL =0x00; //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器
	SysTick->VAL =0X00; //清空计数器
}
