#include "sim_i2c.h"

void delayus(uint8_t Cnt) //TODO...
{
  uint8_t i, j;
  for (i = 0; i < Cnt; i++)
  {
    for (j = 0; j < 1; j++)
    {
      ;
    }
  }
}

/* for 66353 i2c bus start */
void i2c_start(void)
{
  I2C1_SDA_HIGH;
  delayus(n_us);
  I2C1_SCL_HIGH;
  delayus(n_us);
  I2C1_SDA_LOW;
  delayus(n_us);
}

uint8_t i2c_master_r_ack(void)
{
  uint16_t i = 0;

  I2C1_SCL_HIGH; //Pull SCL high to read SDA status

  I2C1_SDA_IN;
  delayus(n_us);
  while ((i < TIME_FOR_RACK) && (I2C1_SDA_READ()))
  {
    i++;
    if (i == TIME_FOR_RACK)
      return 1;
  }
  I2C1_SDA_OUT;
  I2C1_SCL_LOW; //Avoid triggering the start signal by mistake
  delayus(n_us);
  return 0;
}

void i2c_master_s_ack(uint8_t state)
{
  I2C1_SCL_LOW;
  delayus(n_us);
  if(!state) //ACK
  {
    I2C1_SDA_LOW;
  }
  else //NOACK
  {
    I2C1_SDA_HIGH;
  }
  delayus(n_us);
  I2C1_SCL_HIGH;
  delayus(n_us);
  I2C1_SCL_LOW;
  delayus(n_us);
}

void i2c_stop(void)
{
  I2C1_SDA_LOW;
  delayus(n_us);
  I2C1_SCL_HIGH;
  delayus(n_us);
  I2C1_SDA_HIGH;
  delayus(n_us);
}

void i2c_write_byte(uint8_t addr)
{
  uint8_t mask;
  I2C1_SCL_LOW;
  for(mask=0x80;mask!=0;mask>>=1) //from H to L bit
  {
    if((mask&addr)==0)
    {
      I2C1_SDA_LOW;
    }
    else
    {
      I2C1_SDA_HIGH;
    }
    delayus(n_us);
    I2C1_SCL_HIGH;
    delayus(n_us);
    I2C1_SCL_LOW;
  }
}

uint8_t i2c_read_byte(void)
{
  uint8_t i=0,read_data=0;
  I2C1_SCL_LOW;
  delayus(n_us);
  I2C1_SDA_HIGH;
  delayus(n_us);
  for(i=0;i<8;i++)
  {
    I2C1_SCL_HIGH; //I2C_SCL high,ready for read data

    I2C1_SDA_IN; //GPIO INPUT--> read sda
    delayus(n_us);

    read_data=(read_data<<1)|I2C1_SDA_READ(); //read data from I2C_SDA
    I2C1_SCL_LOW; //I2C_SCL low,finish 1 bit data read
    delayus(n_us);
  }
  I2C1_SDA_OUT; //After read sda -->GPIO OUTPUT
  return read_data;
}

u8 it6635_i2c_write(u8 addr, u8 offset, u8 length, u8 *buffer) //write length data
{
    i2c_start();
    i2c_write_byte(addr);
    i2c_master_r_ack();
    i2c_write_byte(offset);
    i2c_master_r_ack();
    while(length--)
    {
        i2c_write_byte(*buffer);
        buffer++;
        i2c_master_r_ack();
        delayus(n_us);
    }
    i2c_stop();
    delayus(20);
    return 1;
}

u8 it6635_i2c_read(u8 addr, u8 offset, u8 length, u8 *buffer) //read length data
{
    i2c_start();
    i2c_write_byte(addr);
    i2c_master_r_ack();

    i2c_write_byte(offset);
    i2c_master_r_ack();

    i2c_start();
    i2c_write_byte(addr|0x01);
    i2c_master_r_ack();

    while(length--)
    {
        *buffer=i2c_read_byte();

        buffer++;
        delayus(n_us);//

        if(length != 0)
        {
            i2c_master_s_ack(0); //send Ack
        }
        else
        {
            i2c_master_s_ack(1); //send Nack
        }
    }
    i2c_stop();
    delayus(20);
    return 1;
}
/* for 66353 i2c bus end */

/* for 6635 i2c bus end */
void i2c_start_1(void)
{
  I2C1_SDA_HIGH_1;
  delayus(n_us);
  I2C1_SCL_HIGH_1;
  delayus(n_us);
  I2C1_SDA_LOW_1;
  delayus(n_us);
}

uint8_t i2c_master_r_ack_1(void)
{
  uint16_t i = 0;

  I2C1_SCL_HIGH_1;

  I2C1_SDA_IN_1;
  delayus(n_us);
  while ((i < TIME_FOR_RACK) && (I2C1_SDA_READ_1()))
  {
    i++;
    if (i == TIME_FOR_RACK)
      return 1;
  }
  I2C1_SDA_OUT_1;
  I2C1_SCL_LOW_1;
  delayus(n_us);
  return 0;
}

void i2c_master_s_ack_1(uint8_t state)
{
  I2C1_SCL_LOW_1;
  delayus(n_us);
  if(!state) //ACK
  {
    I2C1_SDA_LOW_1;
  }
  else //NOACK
  {
    I2C1_SDA_HIGH_1;
  }
  delayus(n_us);
  I2C1_SCL_HIGH_1;
  delayus(n_us);
  I2C1_SCL_LOW_1;
  delayus(n_us);
}

void i2c_stop_1(void)
{
  I2C1_SDA_LOW_1;
  delayus(n_us);
  I2C1_SCL_HIGH_1;
  delayus(n_us);
  I2C1_SDA_HIGH_1;
  delayus(n_us);
}

void i2c_write_byte_1(uint8_t addr)
{
  uint8_t mask;
  I2C1_SCL_LOW_1;
  for(mask=0x80;mask!=0;mask>>=1) //from H to L bit
  {
    if((mask&addr)==0)
    {
      I2C1_SDA_LOW_1;
    }
    else
    {
      I2C1_SDA_HIGH_1;
    }
    delayus(n_us);
    I2C1_SCL_HIGH_1;
    delayus(n_us);
    I2C1_SCL_LOW_1;
  }
}

uint8_t i2c_read_byte_1(void)
{
  uint8_t i=0,read_data=0;
  I2C1_SCL_LOW_1;
  delayus(n_us);
  I2C1_SDA_HIGH_1;
  delayus(n_us);
  for(i=0;i<8;i++)
  {
    I2C1_SCL_HIGH_1; //I2C_SCL high,ready for read data

    I2C1_SDA_IN_1; //GPIO INPUT--> read sda
    delayus(n_us);

    read_data=(read_data<<1)|I2C1_SDA_READ_1(); //read data from I2C_SDA
    I2C1_SCL_LOW_1; //I2C_SCL low,finish 1 bit data read
    delayus(n_us);
  }
  I2C1_SDA_OUT_1; //After read sda -->GPIO OUTPUT
  return read_data;
}

u8 it6635_i2c_write_1(u8 addr, u8 offset, u8 length, u8 *buffer) //write length data
{
    i2c_start_1();
    i2c_write_byte_1(addr);
    i2c_master_r_ack_1();
    i2c_write_byte_1(offset);
    i2c_master_r_ack_1();
    while(length--)
    {
        i2c_write_byte_1(*buffer);
        buffer++;
        i2c_master_r_ack_1();
        delayus(n_us);
    }
    i2c_stop_1();
    delayus(20);
    return 1;
}

u8 it6635_i2c_read_1(u8 addr, u8 offset, u8 length, u8 *buffer) //read length data
{
    i2c_start_1();
    i2c_write_byte_1(addr);
    i2c_master_r_ack_1();

    i2c_write_byte_1(offset);
    i2c_master_r_ack_1();

    i2c_start_1();
    i2c_write_byte_1(addr|0x01);
    i2c_master_r_ack_1();

    while(length--)
    {
        *buffer=i2c_read_byte_1();

        buffer++;
        delayus(n_us);//

        if(length != 0)
        {
            i2c_master_s_ack_1(0); //send Ack
        }
        else
        {
            i2c_master_s_ack_1(1); //send Nack
        }
    }
    i2c_stop_1();
    delayus(20);
    return 1;
}
/* for 6635 i2c bus end */
