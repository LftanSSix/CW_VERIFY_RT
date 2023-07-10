#include "gpio.h"
#define n_us 4
void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOA, ENABLE);	
	RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOB, ENABLE);	

//	GPIO_InitStruct.Pins = (bv0|bv1|bv2|bv3|bv4|bv5|bv6|bv7);               //HPD_1~8_A
//	GPIO_InitStruct.Pins |= GPIO_PIN_11;                                    //TX_POW_A
//	GPIO_InitStruct.Pins |= GPIO_PIN_10;                                    //FOR BUZZER
//	GPIO_InitStruct.Pins |= GPIO_PIN_15;                                    //PA15 TEST PIN
	GPIO_InitStruct.Pins |= GPIO_PIN_4;                                    
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed= GPIO_SPEED_LOW;
	GPIO_Init( CW_GPIOA , &GPIO_InitStruct);
//  
//	GPIO_WritePin(CW_GPIOA,(bv0|bv1|bv2|bv3|bv4|bv5|bv6|bv7),GPIO_Pin_SET); //下板所有Hpd置低
//	PA11_SETHIGH();                                                         //下板 TX POWER ON
//	PA10_SETLOW();                                                          //BUZZER OFF
//  
//	GPIO_InitStruct.Pins = (bv3|bv4|bv5|bv6|bv7|bv8|bv9|bv12);              //HPD_1~8_B &PB03->TEST PIN
//	GPIO_InitStruct.Pins |= GPIO_PIN_14; 
//	GPIO_InitStruct.Pins |= GPIO_PIN_13;                                    //TX_POW_B
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Speed= GPIO_SPEED_HIGH;
//	GPIO_Init( CW_GPIOB , &GPIO_InitStruct);
//  
//    GPIO_WritePin(CW_GPIOB,(bv3|bv4|bv5|bv6|bv7|bv8|bv9|bv12),GPIO_Pin_SET);//上板所有Hpd置低
//    PB13_SETHIGH();                                                         //上板 TX POWER ON
//  
//	GPIO_InitStruct.Pins = GPIO_PIN_2;                                      //FOR AD_KEY1 IN pb02
//	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//	GPIO_Init( CW_GPIOB , &GPIO_InitStruct);

//	GPIO_InitStruct.Pins = GPIO_PIN_15 ;                                    //FOR IR IN pb15
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 
//	GPIO_InitStruct.Speed= GPIO_SPEED_HIGH;
//	GPIO_Init( CW_GPIOB , &GPIO_InitStruct);
//    PB15_AFx_GTIM4CH1();	                                                  //pb15->gtim4ch1
}

void i2c_init(void)
{
	GPIO_InitTypeDef I2C_GPIO_InitStruct;

	RCC_AHBPeriphClk_Enable(I2C1_CLK, ENABLE);	
	RCC_AHBPeriphClk_Enable(I2C2_CLK, ENABLE);	

	//使用两路i2c控制上下两层板
	I2C_GPIO_InitStruct.Pins = I2C1_SCL_GPIO_PIN | I2C1_SDA_GPIO_PIN;
	I2C_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	I2C_GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_Init( I2C1_SCL_GPIO_PORT , &I2C_GPIO_InitStruct);

	I2C_GPIO_InitStruct.Pins = I2C2_SCL_GPIO_PIN | I2C2_SDA_GPIO_PIN;
	I2C_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	I2C_GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_Init( I2C2_SCL_GPIO_PORT , &I2C_GPIO_InitStruct);
  
	GPIO_WritePin(I2C1_SCL_GPIO_PORT, I2C1_SCL_GPIO_PIN,GPIO_Pin_SET);
	GPIO_WritePin(I2C1_SCL_GPIO_PORT, I2C1_SDA_GPIO_PIN,GPIO_Pin_SET);

	GPIO_WritePin(I2C2_SCL_GPIO_PORT, I2C2_SCL_GPIO_PIN,GPIO_Pin_SET);
	GPIO_WritePin(I2C2_SCL_GPIO_PORT, I2C2_SDA_GPIO_PIN,GPIO_Pin_SET);
}

//void delayus(uint8_t Cnt)
//{
//	uint8_t i,j;
//	for(i=0;i<Cnt;i++)
//	{
//		for(j=0;j<1;j++)
//		{
//			;
//		}
//	}
//}
// /*******************I2C1 BEGIN*******************/
// /*******************************************************************************
//  * @FunctionName: i2c_start
//  * @Author:       ftansiX
//  * @DateTime:     2022年7月7日T15:50:39+0800
//  * @Purpose:      
//  * @param:        void               
// *******************************************************************************/
// void i2c_start(void)
// {
//  I2C1_SDA_HIGH;
//  I2C1_SCL_HIGH;
//  delayus(n_us);
// 	I2C1_SDA_LOW;
//  delayus(n_us);
// 	I2C1_SCL_LOW;
//  delayus(n_us);
// }
// /*******************************************************************************
//  * @FunctionName: i2c_master_r_ack
//  * @Author:       ftansiX
//  * @DateTime:     2022年7月7日T15:51:56+0800
//  * @Purpose:      等待从机应答
//  * @param:        void               
//  * @return:       0->收到应答，1->没有应答
// *******************************************************************************/
// uint8_t i2c_master_r_ack(void)
// {
//  uint8_t i=0;
// 	I2C1_SDA_HIGH;
//  delayus(n_us);
//  I2C1_SCL_HIGH;  //拉高SCL准备读取SDA状态
//  delayus(n_us);
//  I2C1_SDA_IN;
// //  while((i<200)&&(I2C1_SDA_READ()==1))GPIO_ReadPin(CW_GPIOB,GPIO_PIN_11)
//  while((i<200)&&(GPIO_ReadPin(CW_GPIOB,GPIO_PIN_11)==1))
//  {
//    i++;
//    if(i==200)return 1;
//  }     
//  I2C1_SDA_OUT;
//  I2C1_SCL_LOW; //避免误触发start信号
//  delayus(n_us);
//  return 0;  
// }
// /*******************************************************************************
//  * @FunctionName: i2c_master_s_ack
//  * @Author:       ftansiX
//  * @DateTime:     2022年7月7日T15:56:55+0800
//  * @Purpose:      读数据，主机发送ACK或者NACK
//  * @param:        0->发送ACK ,1->发送NACK         
// *******************************************************************************/
// void i2c_master_s_ack(uint8_t	state)
// {
//  if(!state)
//  {
//    I2C1_SDA_LOW; 
//    delayus(n_us);
//    I2C1_SCL_LOW;
//    delayus(n_us);
//    I2C1_SCL_HIGH;
//    delayus(n_us);
//    I2C1_SCL_LOW;
//    delayus(n_us);
//  }
//  else
//  {
//    I2C1_SDA_HIGH; //发送NACK,SDA LOW <-->SCL LOW to HIGH
//    delayus(n_us);
//    I2C1_SCL_LOW;
//    delayus(n_us);
//    I2C1_SCL_HIGH;
//    delayus(n_us);
//    I2C1_SCL_LOW;
//    delayus(n_us);
//  }    
// }

// void i2c_stop(void)
// {
//  I2C1_SDA_LOW;
//  delayus(n_us);
//  I2C1_SCL_HIGH;
//  delayus(n_us);
//  I2C1_SDA_HIGH;
//  delayus(n_us);  
// }

// void i2c_write_byte(uint8_t addr)
// {
//  uint8_t i;
//  I2C1_SCL_LOW;
//  delayus(n_us);
//  for(i=0;i<8;i++)
//  {
//    if(addr&0x80)I2C1_SDA_HIGH;
//    else I2C1_SDA_LOW; 
//    I2C1_SCL_HIGH;
//    addr = addr<<1;
//    delayus(n_us);
//    I2C1_SCL_LOW; 
//    delayus(n_us);
//  }
// }
// uint8_t i2c_read_byte(void)
// {
//  uint8_t i=0,read_data=0;
//  I2C1_SCL_LOW;
//  delayus(n_us);
//  for(i=0;i<8;i++)
//  {
//    I2C1_SCL_HIGH;
//    I2C1_SDA_IN;
//    delayus(n_us);
// //    read_data = (read_data<<1)|I2C1_SDA_READ(); 
//    read_data = (read_data<<1)|GPIO_ReadPin(CW_GPIOB,GPIO_PIN_11); 
   
//    I2C1_SCL_LOW;
//    delayus(n_us);
//  }
//  I2C1_SDA_OUT;
//  return  read_data;
// }

// void I2C1_Send_Data(uint8_t addr,uint8_t offset,uint8_t data)
// {
//  i2c_start();
//  i2c_write_byte(addr);
//  i2c_master_r_ack();
 
//  i2c_write_byte(offset);
//  i2c_master_r_ack();
 
//  i2c_write_byte(data);
//  i2c_master_r_ack();
 
//  i2c_stop();
//  delayus(n_us);
// }

// uint8_t I2C1_Read_Data(uint8_t addr,uint8_t offset)
// {
//  uint8_t read_data=0;
   
//  i2c_start();
//  i2c_write_byte(addr);
//  i2c_master_r_ack();
 
//  i2c_write_byte(offset);
//  i2c_master_r_ack();

//  i2c_start();
//  i2c_write_byte(addr|0x01);
//  i2c_master_r_ack();
 
//  read_data = i2c_read_byte();
//  i2c_master_s_ack(1);  //Reply NoAck
 
//  i2c_stop();
//  delayus(n_us);
//  return read_data;
// }

// u8 it6635_i2c_write(u8 addr, u8 offset, u8 length, u8 *buffer) //write length data
// {
// 	i2c_start();           
// 	i2c_write_byte(addr);      
// 	i2c_master_r_ack();
// 	i2c_write_byte(offset);    
// 	i2c_master_r_ack();
// 	while(length--)
// 	{
// 		i2c_write_byte(*buffer);
// 		buffer++;
// 		i2c_master_r_ack();
// 		delayus(n_us);//
// 	}
// 	i2c_stop();
// 	delayus(20);
// 	return 1;
// }

// u8 it6635_i2c_read(u8 addr, u8 offset, u8 length, u8 *buffer) //read length data
// {	
// 	i2c_start();           
// 	i2c_write_byte(addr);      
// 	i2c_master_r_ack();
// 	i2c_write_byte(offset);    
// 	i2c_master_r_ack();

// 	i2c_start();           
// 	i2c_write_byte(addr|0x01);      
// 	i2c_master_r_ack();
// 	while(length--)
// 	{
// 		*buffer=i2c_read_byte();
		
// 		buffer++;
// 		delayus(n_us);//
		
// 		if(length != 0)
// 		{
// 			i2c_master_s_ack(0); //send Ack
// 		}else
// 		{
// 			i2c_master_s_ack(1); //send Nack 			
// 		}
// 	}	
// 	i2c_stop();
// 	delayus(20);
// 	return 1;
// }



///*******************I2C1 END*******************/

///*******************I2C2 BEGIN*******************/
///*******************************************************************************
//  * @FunctionName: i2c_2_start
//  * @Author:       ftansiX
//  * @DateTime:     2022年7月7日T15:50:39+0800
//  * @Purpose:      
//  * @param:        void               
//*******************************************************************************/
//void i2c_2_start(void)
//{
//  I2C2_SDA_HIGH;
//  I2C2_SCL_HIGH;
//  delayus(n_us);
//	I2C2_SDA_LOW;
//  delayus(n_us);
//	I2C2_SCL_LOW;
//  delayus(n_us);
//}
///*******************************************************************************
//  * @FunctionName: i2c_2_master_r_ack
//  * @Author:       ftansiX
//  * @DateTime:     2022年7月7日T15:51:56+0800
//  * @Purpose:      等待从机应答
//  * @param:        void               
//  * @return:       0->收到应答，1->没有应答
//*******************************************************************************/
//uint8_t i2c_2_master_r_ack(void)
//{
//  uint8_t i=0;
//	I2C2_SDA_HIGH;
//  delayus(n_us);
//  I2C2_SCL_HIGH;  //拉高SCL准备读取SDA状态
//  delayus(n_us);
//  I2C2_SDA_IN;
////  while((i<200)&&(I2C2_SDA_READ()==1))GPIO_ReadPin(CW_GPIOB,GPIO_PIN_11)
//  while((i<200)&&(GPIO_ReadPin(CW_GPIOF,GPIO_PIN_7)==1))
//  {
//    i++;
//    if(i==200)return 1;
//  }     
//  I2C2_SDA_OUT;
//  I2C2_SCL_LOW; //避免误触发start信号
//  delayus(n_us);
//  return 0;  
//}
///*******************************************************************************
//  * @FunctionName: i2c_2_master_s_ack
//  * @Author:       ftansiX
//  * @DateTime:     2022年7月7日T15:56:55+0800
//  * @Purpose:      读数据，主机发送ACK或者NACK
//  * @param:        0->发送ACK ,1->发送NACK         
//*******************************************************************************/
//void i2c_2_master_s_ack(uint8_t	state)
//{
//  if(!state)
//  {
//    I2C2_SDA_LOW; 
//    delayus(n_us);
//    I2C2_SCL_LOW;
//    delayus(n_us);
//    I2C2_SCL_HIGH;
//    delayus(n_us);
//    I2C2_SCL_LOW;
//    delayus(n_us);
//  }
//  else
//  {
//    I2C2_SDA_HIGH; //发送NACK,SDA LOW <-->SCL LOW to HIGH
//    delayus(n_us);
//    I2C2_SCL_LOW;
//    delayus(n_us);
//    I2C2_SCL_HIGH;
//    delayus(n_us);
//    I2C2_SCL_LOW;
//    delayus(n_us);
//  }    
//}

//void i2c_2_stop(void)
//{
//  I2C2_SDA_LOW;
//  delayus(n_us);
//  I2C2_SCL_HIGH;
//  delayus(n_us);
//  I2C2_SDA_HIGH;
//  delayus(n_us);  
//}

//void i2c_2_write_byte(uint8_t addr)
//{
//  uint8_t i;
//  I2C2_SCL_LOW;
//  delayus(n_us);
//  for(i=0;i<8;i++)
//  {
//    if(addr&0x80)I2C2_SDA_HIGH;
//    else I2C2_SDA_LOW; 
//    I2C2_SCL_HIGH;
//    addr = addr<<1;
//    delayus(n_us);
//    I2C2_SCL_LOW; 
//    delayus(n_us);
//  }
//}
//uint8_t i2c_2_read_byte(void)
//{
//  uint8_t i=0,read_data=0;
//  I2C2_SCL_LOW;
//  delayus(n_us);
//  for(i=0;i<8;i++)
//  {
//    I2C2_SCL_HIGH;
//    I2C2_SDA_IN;
//    delayus(n_us);
////    read_data = read_data<<1|I2C2_SDA_READ();  //GPIO_ReadPin(CW_GPIOF,GPIO_PIN_7)  
//    read_data = read_data<<1|GPIO_ReadPin(CW_GPIOF,GPIO_PIN_7);    
//    I2C2_SCL_LOW;
//    delayus(n_us);
//  }
//  I2C2_SDA_OUT;
//  return  read_data;
//}

//void I2C2_Send_Data(uint8_t addr,uint8_t offset,uint8_t data)
//{
//  i2c_2_start();
//  i2c_2_write_byte(addr);
//  i2c_2_master_r_ack();
//  
//  i2c_2_write_byte(offset);
//  i2c_2_master_r_ack();
//  
//  i2c_2_write_byte(data);
//  i2c_2_master_r_ack();
//  
//  i2c_2_stop();
//  delayus(n_us);
//}

//uint8_t I2C2_Read_Data(uint8_t addr,uint8_t offset)
//{
//  uint8_t read_data=0;
//    
//  i2c_2_start();
//  i2c_2_write_byte(addr);
//  i2c_2_master_r_ack();
//  
//  i2c_2_write_byte(offset);
//  i2c_2_master_r_ack();

//  i2c_2_start();
//  i2c_2_write_byte(addr|0x01);
//  i2c_2_master_r_ack();
//  
//  read_data = i2c_2_read_byte();
//  i2c_2_master_s_ack(1);  //Reply NoAck
//  
//  i2c_2_stop();
//  delayus(n_us);
//  return read_data;
//}



/*******************I2C2 END*******************/

