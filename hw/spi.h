#ifndef _FWUPDATE_SPI_H
#define _FWUPDATE_SPI_H

#define SPI_DEV_NAME        "/dev/spidev0.0"
#define SPIDEV_MODE         (3)
#define SPIDEV_CLK_RATE     (5000000) /* TODO - this also matches the rate in dts */
#define SPIDEV_CHANEL       (0) /* TODO probably should be 1 - CS */


#define DUT_SPI_INSTANCE	(0)
#define MSS_SPI_SLAVE_0		(0)
#define MSS_SPI_MODE3		(0)
#define DUT_SPI_PCLK_DIVIDER	(0)
#define DUT_MSS_SPI_FRAME_BIT_LENGTH	(0)
#define DUT_SPI_SLAVE		(0)


#define SPI_CS_0        (0)
#define SPI_CS_1        (1)


void MSS_SPI_set_slave_select( int _DUT_SPI_INSTANCE, int _DUT_SPI_SLAVE );
void MSS_SPI_clear_slave_select( int _DUT_SPI_INSTANCE, int _DUT_SPI_SLAVE );


void MSS_SPI_init(int inst);
void MSS_SPI_configure_master_mode(
    int _DUT_SPI_INSTANCE,
    int _MSS_SPI_SLAVE_0,
    int _MSS_SPI_MODE3,
    int _DUT_SPI_PCLK_DIVIDER,
    int _DUT_MSS_SPI_FRAME_BIT_LENGTH
    );


void MSS_SPI_transfer_block( int _DUT_SPI_INSTANCE, char *tx_buf, unsigned int tx_buf_len, char *rx_buf, unsigned int rx_buf_len);


#endif //_SPI_H
