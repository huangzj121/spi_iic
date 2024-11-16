#ifndef    STD_IIC_H
#define    STD_IIC_H


#include    "stdint.h"
#include    "stdbool.h"

#include    "nrf_gpio.h"
#include    "nrf_delay.h"



void             iic_start(void) ;
void             iic_stop(void) ;
unsigned char    iic_wait_ack(void) ;
void             iic_ack(void) ;
void             iic_nack(void) ;
void             iic_write_byte(const unsigned char       a) ;
unsigned char    iic_read_byte(const unsigned char       ack) ;

void             IIC_Init(void) ;
void             IIC_Write(const unsigned char     addr,  const unsigned char   data) ;
unsigned char    IIC_Read(const unsigned char     addr) ;
void             iic_ms(const unsigned long     ts);



#endif


