#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
 #include <fcntl.h>
 
#include "gpio.h"
#include "spi.h"

static const unsigned char     spiBPW   = 8 ;
static const unsigned int    spiDelay = 0 ;
static unsigned int    spiSpeeds [2] ;
static int         spiFds [2];

#define  SPI_SCLK_HIGH_TIME	(10)
#define  SPI_SCLK_LOW_TIME	(10)


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


void MSS_SPI_init(int inst)
{
        //inst = inst; //dummy

        switch(inst)
        {
            case SPI_CS_0:
            {
                //set your GPIO for CS0
                break;
            }
            case SPI_CS_1:
            {

                //set your GPIO for CS1
                break;
            }
            default:
                {
                  printf("Err spi CS[%d]\r\n", inst);
                  return;
                }
        }

       // wiringPiSetup () ;

}


void MSS_SPI_configure_master_mode(
    int _DUT_SPI_INSTANCE,
    int _MSS_SPI_SLAVE_0,
    int _MSS_SPI_MODE3,
    int _DUT_SPI_PCLK_DIVIDER,
    int _DUT_MSS_SPI_FRAME_BIT_LENGTH
    )
{

}

void MSS_SPI_set_slave_select( int _DUT_SPI_INSTANCE, int _DUT_SPI_SLAVE )
{
}


void MSS_SPI_clear_slave_select( int _DUT_SPI_INSTANCE, int _DUT_SPI_SLAVE )
{
}


int PiSPISetupMode (int channel, int speed, int mode)
{
  int fd ;
  char spiDev [32] ;

  mode    &= 3 ;	// Mode is 0, 1, 2 or 3

// Channel can be anything - lets hope for the best
//  channel &= 1 ;	// Channel is 0 or 1

  snprintf (spiDev, 31, "/dev/spidev0.%d", channel) ;

  if ((fd = open (spiDev, O_RDWR)) < 0) {
    printf("<<< failed here\n");
  }

  spiSpeeds [channel] = speed ;
  spiFds    [channel] = fd ;

  return fd ;
}

int PiSPIDataRW (int channel, unsigned char *data, int len)
{
  struct spi_ioc_transfer spi ;

  channel &= 1 ;

// Mentioned in spidev.h but not used in the original kernel documentation
//	test program )-:

  memset (&spi, 0, sizeof (spi)) ;

  spi.tx_buf        = (unsigned long)data ;
  spi.rx_buf        = (unsigned long)data ;
  spi.len           = len ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeeds [channel] ;
  spi.bits_per_word = spiBPW ;

  return ioctl (spiFds [channel], SPI_IOC_MESSAGE(1), &spi) ;
}


int SpiRwByte(unsigned char *byte_out, unsigned int len){
    unsigned char byte_in = 0;
    unsigned char bit;


    int rc;

    rc = PiSPIDataRW(SPI0_CHANEL, byte_out, len /*sizeof(byte_out)*/);
    if(-1 == rc)
    {
        printf("%s:%d SPI ERROR !!!\n", __func__, __LINE__);
    }
    return rc;

}





void MSS_SPI_transfer_block( int _DUT_SPI_INSTANCE, char *tx_buf, unsigned int tx_buf_len, char *rx_buf, unsigned int rx_buf_len)
{

	unsigned int i;
    unsigned char *p_data;

    p_data = tx_buf;

    if(max(tx_buf_len, rx_buf_len) == rx_buf_len)
        p_data = rx_buf;

    SpiRwByte(p_data, max(tx_buf_len, rx_buf_len));

}

