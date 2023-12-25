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

FILE *fp = NULL;
DPULONG f_sz = 0;

static const char actions_tbl[][64] =
{
    "DP_NO_ACTION_FOUND",
    "DP_DEVICE_INFO_ACTION_CODE",
    "DP_READ_IDCODE_ACTION_CODE",
    "DP_ERASE_ACTION_CODE",
    "DP_PROGRAM_ACTION_CODE",
    "DP_VERIFY_ACTION_CODE",
    "DP_ENC_DATA_AUTHENTICATION_ACTION_CODE",
    "DP_VERIFY_DIGEST_ACTION_CODE", //7
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_NA",
    "DP_READ_DEVICE_CERTIFICATE_ACTION_CODE", //	30u
    "DP_ZEROIZE_LIKE_NEW_ACTION_CODE",
    "DP_ZEROIZE_UNRECOVERABLE_ACTION_CODE",
    "DP_REPROGRAM_INFLIGHT_ACTION_CODE"
};

/* adding comment */
extern DPUCHAR Action_code;
extern DPUCHAR *image_buffer; 
char mybuff[10079858]; /* TODO - fix */

int main(int argc, char **argv)
{
    int rc;
	unsigned char rByte = 0;

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
    if (sizeof(mybuff) < f_sz) {
        printf("Not enough space in memory, try to build with paging");
        goto exit_err;
    }
    readFromFile(mybuff, f_sz);
    image_buffer = mybuff;
#endif
    Action_code = DP_PROGRAM_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ZEROIZE_LIKE_NEW_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ERASE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; //DP_ZEROIZE_UNRECOVERABLE_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; ////DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_READ_DEVICE_CERTIFICATE_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ZEROIZE_LIKE_NEW_ACTION_CODE; //;DP_ZEROIZE_LIKE_NEW_ACTION_CODE; //DP_VERIFY_DIGEST_ACTION_CODE; //DP_READ_DEVICE_CERTIFICATE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ZEROIZE_LIKE_NEW_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; // DP_ERASE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ERASE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ZEROIZE_LIKE_NEW_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; ////DP_PROGRAM_ACTION_CODE; //DP_READ_IDCODE_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_ZEROIZE_LIKE_NEW_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //DP_DEVICE_INFO_ACTION_CODE; //DP_PROGRAM_ACTION_CODE; //
    printf("\n ============= ACTION_CODE  %x::%s =============\n", Action_code, actions_tbl[Action_code]);
    rc = dp_top();
    printf("\n ============= ACTION_CODE  %x::%s  Done %s:%d =============\n", Action_code, actions_tbl[Action_code], (rc==DPE_SUCCESS)?"OK": "ERROR Code", rc);

    gpio_dis_spi();
	return 0 ;

exit_err:
    gpio_dis_spi();
    printf("FW Update failed\n");
    printf("Try to build with ENABLE_DISPLAY and check ENABLE_PAGING\n");

    return 1;
}


