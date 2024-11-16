#ifndef    STD_SPI_H
#define    STD_SPI_H


#include    "stdint.h"
#include    "stdbool.h"



#include    "nrf_gpio.h"
#include    "nrf_delay.h"






void             SPI_init(void) ;
void             SPI_write(const unsigned char     addr,  const unsigned char   val) ;
unsigned char    SPI_read(const unsigned char     addr,  unsigned char*   dat) ;
void             CS_high(void) ;
void             CS_low(void) ;



#endif


