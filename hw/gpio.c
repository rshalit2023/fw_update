#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/mman.h>
#include <stdint.h>

#define JTAG_TRSTN_REG          0x18
#define SPI_EN_REG              0x1C
#define SYS_CTRL_SPACE_OFFSET   0xa4000000
#define SYS_CTRL_SPACE_LEN      0x100
#define SYS_CTRL_GPIO "/proc/device-tree/amba_pl@0/pl_sys_ctrl@a4000000/"

void *map_base;

int gpio_init(void)
{
    uint32_t  read_result;
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    void *map_base = mmap(NULL,
                SYS_CTRL_SPACE_LEN,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                mem_fd,
                SYS_CTRL_SPACE_OFFSET);	// phys_addr should be page-aligned.	
    return 0;
}

void gpio_en_spi(void)
{
    void *virt_addr = (char *)map_base + JTAG_TRSTN_REG;
    *(volatile uint32_t*)virt_addr = 1;
    virt_addr = (char *)map_base + SPI_EN_REG;
    *(volatile uint32_t*)virt_addr = 1;
}

void gpio_dis_spi(void)
{
    void *virt_addr = (char *)map_base + JTAG_TRSTN_REG;
    *(volatile uint32_t*)virt_addr = 0;
    virt_addr = (char *)map_base + SPI_EN_REG;
    *(volatile uint32_t*)virt_addr = 0;
}
