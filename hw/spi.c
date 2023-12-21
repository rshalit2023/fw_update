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
static unsigned int    spiSpeeds ;
static int         spiFds;

#define  SPI_SCLK_HIGH_TIME	(10)
#define  SPI_SCLK_LOW_TIME	(10)


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


void MSS_SPI_init(int inst)
{
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


int MSS_SPI_setup (int speed, int mode)
{
  int fd ;
  char spiDev [32] ;

  if ((fd = open (SPI_DEV_NAME, O_RDWR)) < 0) {
    printf("failed in file opening %s\n", SPI_DEV_NAME);
    return 1;
  }

  printf("File open is successful\n");

  spiSpeeds = speed ;
  spiFds    = fd ;

  return fd ;
}

void MSS_SPI_transfer_block( int _DUT_SPI_INSTANCE, char *tx_buf, unsigned int tx_buf_len, char *rx_buf, unsigned int rx_buf_len)
{
  struct spi_ioc_transfer spi ;
	unsigned int i, len;
  unsigned char *data;

    len = max(tx_buf_len, rx_buf_len);

    data = tx_buf;
    if(len == rx_buf_len)
        data = rx_buf;

    memset (&spi, 0, sizeof (spi)) ;

    spi.tx_buf        = (unsigned long)data ;
    spi.rx_buf        = (unsigned long)data ;
    spi.len           = len ;
    spi.delay_usecs   = spiDelay ;
    spi.speed_hz      = spiSpeeds;
    spi.bits_per_word = spiBPW ;

    ioctl (spiFds, SPI_IOC_MESSAGE(1), &spi) ;
}

