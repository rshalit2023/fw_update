#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio.h"
#include "spi.h"
#include "dpDUTspi.h"
#include "dpcom.h"
#include "dputil.h"
#include "dpalg.h"
#include "dpG5alg.h"

#define MAX_FILE_SIZE (10 * 1024 * 1024)

FILE *fp = NULL;
static DPULONG f_sz = 0;
extern DPUCHAR Action_code;
extern DPUCHAR *image_buffer; 
static char dat_file[MAX_FILE_SIZE];

int main(int argc, char **argv)
{
    int rc;
    unsigned char rByte = 0;
    DPULONG num_read;

    if(argc < 2)
    {
        printf("FPGA UPDATE App reuires 1 parms: image file\n");
        goto exit_err;
    }

    gpio_init();
    gpio_en_spi(); 
    dut_spi_flash_init();

    rc = MSS_SPI_setup(SPIDEV_CLK_RATE, SPIDEV_MODE);
    if(rc == -1)
    {
        printf("SPI setupError\n");
        goto exit_err;
    }

    fp = fopen(argv[1], "rb");
    if (!fp) {
            printf ("error: file open failed '%s'.\n", argv[1]);
            goto exit_err;
    }
    fseek(fp, 0L, SEEK_END);
    f_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

#ifndef USE_PAGING
    if (sizeof(dat_file) < f_sz) {
        printf("Not enough space in memory, try to build with paging");
        goto exit_err;
    }

    num_read = dp_read_dat_file(dat_file, f_sz);

    if (num_read < f_sz) {
        printf("Not all file dat been read %ld (%ld)\n", num_read, f_sz);
        goto exit_err;
    }

    image_buffer = dat_file;
#endif
    Action_code = DP_PROGRAM_ACTION_CODE;
    printf("\n ============= ACTION_CODE  %x =============\n", Action_code);
    rc = dp_top();
    printf("\n ============= ACTION_CODE  %x  Done %s:%d =============\n", Action_code, (rc==DPE_SUCCESS)?"OK": "ERROR Code", rc);

    gpio_dis_spi();
    return 0 ;

exit_err:
    gpio_dis_spi();
    printf("FW Update failed\n");
    printf("Suggest to build with ENABLE_DISPLAY or ENABLE_PAGING\n");

    return 1;
}


