#ifndef _GPIO_FPGA_H
#define _GPIO_FPGA_H

//

#define VALUE_HIGH                  "1"
#define VALUE_LOW                   "0"


#define SPI_CS_IO       "11" //7" //""4" //"11" "7"
#define SPI_CLK_IO      "14" //bcm "11"
#define SPI_MOSI_IO     "12" //bcm "10"
#define SPI_MISO_IO     "13"

#define FPGA_DEVRST_N   "23"
#define FPGA_SPI_EN     "24"
#define EXP_POWER_DIS_RSBP     "16"

/*TODO - should configure to VCK190*/
typedef enum _gpio_spi_io_e
{
    E_SPI_CS0_IO 	=    10, 
    E_SPI_CS_IO 	=    11, 
    E_SPI_CLK_IO 	=    14, 
    E_SPI_MOSI_IO 	=    12, 
    E_SPI_MISO_IO 	=    13,
    E_FPGA_DEVRST_N =    23,
    E_FPGA_SPI_EN   =    24,
} gpio_spi_io_e;

typedef enum _gpio_dir_e
{
	GPIO_IN,
	GPIO_OUT,
} gpio_dir_e;






int setGpio(char *gpioId, gpio_dir_e gpio_dir, unsigned char val);
int setGpioOutGpioIn(char *gpioOut, char *gpioIn);
int gpio_unexport(int gpio_num);
int gpio_set_value(int gpio_num, const char *value);

#endif // _GPIO_FPGA_H

