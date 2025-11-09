#include "dma.h"
#include "gpio.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define DMA_READ_ADDR(n) (*(volatile uint32_t *)(DMA_BASE + 0x00 + (n)*0x40))
#define DMA_WRITE_ADDR(n) (*(volatile uint32_t *)(DMA_BASE + 0x04 + (n)*0x40))
#define DMA_TRANS_COUNT(n) (*(volatile uint32_t *)(DMA_BASE + 0x08 + (n)*0x40))
#define DMA_CTRL_TRIG(n) (*(volatile uint32_t *)(DMA_BASE + 0x0C + (n)*0x40))

#define DMA_CTRL_TRIG_EN (1 << 0)
#define DMA_CTRL_TRIG_DATA_SIZE_8 (0 << 2)
#define DMA_CTRL_TRIG_DATA_SIZE_16 (1 << 2)
#define DMA_CTRL_TRIG_DATA_SIZE_32 (2 << 2)
#define DMA_CTRL_TRIG_INCR_READ (1 << 4)
#define DMA_CTRL_TRIG_INCR_WRITE (1 << 5)
#define DMA_CTRL_TRIG_CHAIN_TO_SHIFT 11

int dma_init(bool high_priority) {
    for (uint8_t i = 0; i < 12; i++) {
        DMA_READ_ADDR(i) = 0;
        DMA_WRITE_ADDR(i) = 0;
        DMA_CTRL_TRIG(i) = 0;
        DMA_TRANS_COUNT(i) = 0;
    }
    return 0;
}

void dma_start_transfer(uint8_t chan, const void *src, void *dst, uint32_t count) {
    DMA_CTRL_TRIG(chan) = 0;

    DMA_READ_ADDR(chan) = (uintptr_t)src;
    DMA_WRITE_ADDR(chan) = (uintptr_t)dst;
    DMA_TRANS_COUNT(chan) = count;

    DMA_CTRL_TRIG(chan) = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_INCR_READ | DMA_CTRL_TRIG_INCR_WRITE | DMA_CTRL_TRIG_DATA_SIZE_32;
}

bool dma_is_busy(uint8_t chan) {
    return (DMA_CTRL_TRIG(chan) & DMA_CTRL_TRIG_EN) != 0;
}

void dma_wait_complete(uint8_t chan) {
    while (dma_is_busy(chan));
}

void dma_configure_peripheral(uint8_t chan, const volatile void *src, volatile void *dst, size_t count, bool read_from_peripheral) {
    DMA_CTRL_TRIG(chan) = 0;

    DMA_READ_ADDR(chan) = (uintptr_t)src;
    DMA_WRITE_ADDR(chan) = (uintptr_t)dst;
    if (read_from_peripheral) {
        DMA_CTRL_TRIG(chan) = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_INCR_WRITE | DMA_CTRL_TRIG_DATA_SIZE_32;
    } else {
        DMA_CTRL_TRIG(chan) = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_INCR_READ | DMA_CTRL_TRIG_DATA_SIZE_32;
    }
    DMA_TRANS_COUNT(chan) = count;
}
