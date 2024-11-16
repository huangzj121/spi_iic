#include    "std_iic.h"


#define  SDA      (11)   //NRF_GPIO_PIN_MAP(0, 11)
#define  SCL      (12)   //NRF_GPIO_PIN_MAP(0, 12)



#define    PER_BIT    (4)


//-----------------------------------------------------------------------
void    SCL_OUT_Mode(void)
{
	  nrf_gpio_cfg_output(SCL);
}

void    SDA_OUT_Mode(void)
{
	  nrf_gpio_cfg_output(SDA);
}

void    SDA_IN_Mode(void)
{
	  nrf_gpio_cfg_input(SDA,  NRF_GPIO_PIN_NOPULL);
}

unsigned char    SDA_Read(void)
{
	  return   nrf_gpio_pin_read(SDA);
}

void    SDA_Control(const bool      tb)
{
          if( tb )
	      nrf_gpio_pin_set(SDA);
	  else
	      nrf_gpio_pin_clear(SDA);
}

void    SCL_Control(const bool      tb)
{
          if( tb )
	      nrf_gpio_pin_set(SCL);
	  else
	      nrf_gpio_pin_clear(SCL);
}

void    iic_us(const unsigned long     ts)
{
	  nrf_delay_us(ts);
}

void    iic_ms(const unsigned long     ts)
{
	  nrf_delay_us(1000*ts);
}
//-----------------------------------------------------------------------


//IIC初始化
void    IIC_Init(void)
{
    //GPIO_Init(SCL_PORT , SCL_PIN , GPIO_Mode_Out_PP_Low_Fast);   //输出
    SCL_OUT_Mode();
    SDA_OUT_Mode();
		
    SDA_Control(1);
    SCL_Control(1);
	
}


//IIC起始信号，SDA下降沿
void    iic_start(void)
{
    SDA_OUT_Mode();   //设置SDA为输出
    SDA_Control(1);
    SCL_Control(1);
    iic_us(4);   // hold on

    SDA_Control(0);   //下降沿
    iic_us(4);
    SCL_Control(0);   //准备发送数据
    iic_us(4);
}


//IIC停止信号，SDA上升沿
void    iic_stop(void)
{
    SDA_OUT_Mode();   //设置SDA为输出
    SCL_Control(0);
    SDA_Control(0);
    iic_us(4);   // hold on

    SCL_Control(1);
    iic_us(4);
    SDA_Control(1);   //上升沿
    iic_us(4);
}


//等待应答信号到来
//返回值： 1，接收应答失败
//        0，接收应答成功
unsigned char    iic_wait_ack(void)
{
    unsigned char    outTime = 0;

    SDA_OUT_Mode();   //设置SDA为输出
    SDA_Control(1);   iic_us(1);   //此条后SDA电平已经无变化
    SCL_Control(1);   iic_us(1);
    
    SDA_IN_Mode();   //设置SDA为输入
    while( SDA_Read() ) {
       ++outTime;
       if(outTime > 250) {
          iic_stop();
          return  1;
       }
    }
    SCL_Control(0);   //时钟输出0
    iic_us(1);
    return  0;
}


//产生ACK应答
void    iic_ack(void)
{
    SCL_Control(0);
    SDA_OUT_Mode();

    SDA_Control(0);
    iic_us(2);
    SCL_Control(1);
    iic_us(2);
    SCL_Control(0);
    iic_us(2);
    SDA_Control(1);
}


//不产生ACK应答
void    iic_nack(void)
{
    SCL_Control(0);
    SDA_OUT_Mode();

    SDA_Control(1);
    iic_us(2);
    SCL_Control(1);
    iic_us(2);
    SCL_Control(0);
    iic_us(2);
}


//IIC发送一个字节
void    iic_write_byte(const unsigned char       a)
{
   unsigned char    c, i;
   
   SDA_OUT_Mode();   //设置SDA为输出
   SCL_Control(0);   //拉低时钟开始数据传输

   c = a;
   for(i = 0; i < 8; ++i) {
      SDA_Control((c&0x80) >> 7);   //数据变化
      c <<= 1;
      iic_us(PER_BIT);
      SCL_Control(1);   //时钟线产生下降沿
      iic_us(PER_BIT);
      SCL_Control(0);
      iic_us(PER_BIT);
   }
}



//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
unsigned char    iic_read_byte(const unsigned char       ack)
{
    unsigned char    i, recv;
	
    recv = 0;
    SCL_Control(0);   //拉低时钟开始数据传输
    SDA_OUT_Mode();   //设置SDA为输出
    SDA_Control(1);   //此条后SDA电平已经无变化
    SDA_IN_Mode();    //设置SDA为输入
    
    for(i = 0; i < 8; ++i) {
       SCL_Control(1);   //锁定数据，让数据不变化
       recv <<= 1;
       if( SDA_Read() )   ++recv;   //读取数据
       iic_us(PER_BIT);
       SCL_Control(0);   //释放锁定，开始下一个数据检测
       iic_us(PER_BIT);
    }
    
    if ( !ack )   iic_nack();   //发送nACK
    else          iic_ack();    //发送ACK
    
    return   recv;
}


//根据地址写数据
void    IIC_Write(const unsigned char     addr,  const unsigned char   data)
{
     iic_start();   //起始信号
     
     iic_write_byte(addr & 0xFE);   //发器件地址，低位为0，表示写
     iic_wait_ack();   //等待应答

     iic_write_byte(addr);   //发送数据地址
     iic_wait_ack();

     iic_write_byte(data);   //发送数据     
     iic_wait_ack();
	 
     iic_stop();   //产生一个停止条件
}


//根据地址读取数据
unsigned char    IIC_Read(const unsigned char     addr)   //读寄存器或读数据
{
     unsigned char    data;

     iic_start();   //起始信号
     
     iic_write_byte(addr & 0xFE);   //发器件地址，低位为0，表示写
     iic_wait_ack();   //等待应答

     iic_write_byte(addr);   //发送数据地址
     iic_wait_ack();

     iic_start();
	 
     iic_write_byte(addr | 0x01);   //发器件地址,低位为1，表示读
     iic_wait_ack();

     data = iic_read_byte(0);   //读取一个字节
     
     iic_stop();   //产生一个停止条件
     return   data;
}



