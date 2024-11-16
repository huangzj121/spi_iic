#include    "std_spi.h"



#define  SCK      NRF_GPIO_PIN_MAP(1, 12)
#define  MOSI     NRF_GPIO_PIN_MAP(1, 13)
#define  MISO     NRF_GPIO_PIN_MAP(1, 14)
#define  SS1      NRF_GPIO_PIN_MAP(1, 1)
	



#define  PER_BIT    (5)



//-----------------------------------------------------------------
void    PinPull_1(const unsigned char     num)
{
   	  nrf_gpio_pin_set(num);   // high
}

void    PinPull_0(const unsigned char     num)
{
	  nrf_gpio_pin_clear(num);   // low
}

unsigned char    Pin_Read(const unsigned char     num)
{
	  return   nrf_gpio_pin_read(num);
}

void    CS_high(void)
{
	  nrf_gpio_pin_set(SS1);
}

void    CS_low(void)
{
	  nrf_gpio_pin_clear(SS1);
}

void    spi_us(const unsigned long     ts)
{
	  nrf_delay_us(ts);
}

//-----------------------------------------------------------------



void    SPI_init(void)
{
          nrf_gpio_cfg_output(SS1);
	  nrf_gpio_cfg_input(MISO,   NRF_GPIO_PIN_NOPULL);
	  nrf_gpio_cfg_output(MOSI);
	  nrf_gpio_cfg_output(SCK);
	
	  CS_low();
	  CS_high();

	  PinPull_0(SCK);
	  PinPull_1(SCK);
	    
	  //PinPull_0(MOSI);
	  //PinPull_1(MOSI);	
}


void    SPI_write(const unsigned char     addr,  unsigned char   val)
{
	  unsigned char    i, c;

	  
	  spi_us(PER_BIT);
	
	
	  c = addr;
	  for(i = 0; i < 8; ++i) {
			        spi_us(PER_BIT);
			        PinPull_0(SCK);   // 0
			  		  			  		  	
			        if( (c & 0x80) == 0x80 ) {
					  PinPull_1(MOSI);
				}
				else {
					  PinPull_0(MOSI);
				}
				
				spi_us(PER_BIT);
				PinPull_1(SCK);   // 1			
							
				c <<= 1;			
	  }
		
	  c = val;
	  for(i = 0; i < 8; ++i) {
			        spi_us(PER_BIT);
			        PinPull_0(SCK);  // 0			  
			  			  		  	
			        if( (c & 0x80) == 0x80 ) {
					  PinPull_1(MOSI);
				}
				else {
					  PinPull_0(MOSI);
				}
				
				spi_us(PER_BIT);
				PinPull_1(SCK);  // 1			
							
				c <<= 1;			
	  }
		
		
	  spi_us(2*PER_BIT);
}


unsigned char    SPI_read(const unsigned char     addr,  unsigned char*   dat)
{
	  unsigned char    i, c, recv;

	  
	  spi_us(PER_BIT);
	
	
	  c = addr;
	  for(i = 0; i < 8; ++i) {
			        spi_us(PER_BIT);
			        PinPull_0(SCK);  // 0			  
			  			  		  	
			        if( (c & 0x80) == 0x80 ) {
					  PinPull_1(MOSI);
				}
				else {
					  PinPull_0(MOSI);
				}
				
				spi_us(PER_BIT);
				PinPull_1(SCK);  // 1				
							
				c <<= 1;			
	  }
	
	  recv = 0;
	  for(i = 0; i < 8; ++i) {
			        spi_us(PER_BIT);
			        PinPull_0(SCK);  // 0				  
			  	  			
			        recv <<= 1;
			        if( Pin_Read(MISO) ) {
					  recv += 1;
				}
				
				spi_us(PER_BIT);	
				PinPull_1(SCK);  // 1							
	  }
		
		
	  spi_us(2*PER_BIT);
		
	  *dat = recv;
	  return  recv;
}




