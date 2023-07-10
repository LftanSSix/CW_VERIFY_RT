#include "main.h"

#define  I2C1_CLK									RCC_AHB_PERIPH_GPIOB
#define  I2C1_SCL_GPIO_PORT       CW_GPIOB
#define  I2C1_SCL_GPIO_PIN        GPIO_PIN_10   //����Ķ�������GPIO��ʼ��������Ҫ��ͬ���޸�
#define  I2C1_SDA_GPIO_PORT       CW_GPIOB   
#define  I2C1_SDA_GPIO_PIN        GPIO_PIN_11   //����Ķ�������GPIO��ʼ��������Ҫ��ͬ���޸�

#define  I2C2_CLK									RCC_AHB_PERIPH_GPIOF
#define  I2C2_SCL_GPIO_PORT       CW_GPIOF
#define  I2C2_SCL_GPIO_PIN        GPIO_PIN_6    //����Ķ�������GPIO��ʼ��������Ҫ��ͬ���޸�
#define  I2C2_SDA_GPIO_PORT       CW_GPIOF   
#define  I2C2_SDA_GPIO_PIN        GPIO_PIN_7    //����Ķ�������GPIO��ʼ��������Ҫ��ͬ���޸�

// /****************I2C1****************/
// #define  I2C1_SCL_HIGH       PB10_SETHIGH() 
// #define  I2C1_SCL_LOW        PB10_SETLOW() 
// #define  I2C1_SDA_HIGH       PB11_SETHIGH() 
// #define  I2C1_SDA_LOW        PB11_SETLOW() 

// #define  I2C1_SDA_IN         PB11_DIR_INPUT() 
// #define  I2C1_SDA_OUT        PB11_DIR_OUTPUT()
// // #define  I2C1_SDA_READ()     PB11_GETVALUE()	//PA11_GETVALUE()    

// /****************I2C2****************/
// #define  I2C2_SCL_HIGH       PF06_SETHIGH() 
// #define  I2C2_SCL_LOW        PF06_SETLOW() 
// #define  I2C2_SDA_HIGH       PF07_SETHIGH() 
// #define  I2C2_SDA_LOW        PF07_SETLOW()

// #define  I2C2_SDA_IN         PF07_DIR_INPUT() 
// #define  I2C2_SDA_OUT        PF07_DIR_OUTPUT()  
// #define  I2C2_SDA_READ()     PF07_GETVALUE()

void gpio_init(void);
void i2c_init(void);
void delayus(uint8_t Cnt);

void i2c_start(void);
uint8_t i2c_master_r_ack(void);
void i2c_master_s_ack(uint8_t	state);
void i2c_stop(void);
void i2c_write_byte(uint8_t addr);
uint8_t i2c_read_byte(void);
void I2C1_Send_Data(uint8_t addr,uint8_t offset,uint8_t data);
uint8_t I2C1_Read_Data(uint8_t addr,uint8_t offset);

void i2c_2_start(void);
uint8_t i2c_2_master_r_ack(void);
void i2c_2_master_s_ack(uint8_t	state);
void i2c_2_stop(void);
void i2c_2_write_byte(uint8_t addr);
uint8_t i2c_2_read_byte(void);
void I2C2_Send_Data(uint8_t addr,uint8_t offset,uint8_t data);
uint8_t I2C2_Read_Data(uint8_t addr,uint8_t offset);

