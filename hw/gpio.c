#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <sys/mman.h>

#define SPI_EN_REG      0x1c
#define JTAG_TRST_REG   0x18

#define SYS_CTRL_AXI_MEM_NAME   "/proc/device-tree/amba_pl@0/pl_sys_ctrl@a4000000/reg"

#define SYS_CTRL_LEN    0x100
#include <endian.h>
static void *map_base;
uint64_t buf[2];
void gpio_init(void)
{
    int mem_fd, sys_ctrl_fd = open(SYS_CTRL_AXI_MEM_NAME, O_RDONLY);

    read(sys_ctrl_fd, (char *)&buf[0], sizeof(buf));
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    map_base = mmap(NULL,
                be64toh(buf[1]),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                mem_fd,
                be64toh(buf[0]));
}

static void write_gpio(uint32_t reg, uint32_t val)
{
    void *virt_addr = (char *)map_base + reg;
    *(uint32_t *) virt_addr = val;
}

static void print_gpio(uint32_t reg, uint32_t val)
{
    void *virt_addr = (char *)map_base + reg;
    printf("gpio 0x%x:0x%x \n",reg, *(uint32_t *) virt_addr);
}

void gpio_en_spi(void)
{
    write_gpio(SPI_EN_REG, 1);
    write_gpio(JTAG_TRST_REG, 1);
}

void gpio_dis_spi(void)
{
    write_gpio(SPI_EN_REG, 0);
    write_gpio(JTAG_TRST_REG, 0);
}

void gpio_print(void)
{
    print_gpio(SPI_EN_REG, 1);
    print_gpio(JTAG_TRST_REG, 1);
}
