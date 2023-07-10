#ifndef __SIM_I2C_H_
#define __SIM_I2C_H_
#include "main.h"

#define  TIME_FOR_RACK   300 //TODO...
#define  n_us            4 //TODO...

#define I2C1_SCL_HIGH           PB10_SETHIGH() 
#define I2C1_SCL_LOW            PB10_SETLOW() 
#define I2C1_SDA_HIGH           PB11_SETHIGH() 
#define I2C1_SDA_LOW            PB11_SETLOW() 

/* note? f103 need change io dir? */
#define I2C1_SDA_IN             PB11_DIR_INPUT() 
#define I2C1_SDA_OUT            PB11_DIR_OUTPUT()
#define I2C1_SDA_READ()         GPIO_ReadPin(CW_GPIOB,GPIO_PIN_11)//PB11_GETVALUE()	

#define I2C1_SCL_HIGH_1           PF06_SETHIGH()
#define I2C1_SCL_LOW_1            PF06_SETLOW() 
#define I2C1_SDA_HIGH_1           PF07_SETHIGH()
#define I2C1_SDA_LOW_1            PF07_SETLOW()

/* note? f103 need change io dir? */
#define I2C1_SDA_IN_1             PF07_DIR_INPUT() 
#define I2C1_SDA_OUT_1            PF07_DIR_OUTPUT()
#define I2C1_SDA_READ_1()         GPIO_ReadPin(CW_GPIOF,GPIO_PIN_7)//PF07_GETVALUE()


void i2c_init(void);

#endif
