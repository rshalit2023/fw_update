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

#define SYS_CTRL_BASE   0xa4000000
#define SYS_CTRL_LEN    0x100

static void *map_base;

void gpio_init(void)
{
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    map_base = mmap(NULL,
                SYS_CTRL_LEN,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                mem_fd,
                SYS_CTRL_BASE);
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
