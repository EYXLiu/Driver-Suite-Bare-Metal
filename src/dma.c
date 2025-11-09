#include "dma.h"
#include "gpio.h"
#include "irq.h"
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

#define DMA_IRQ_0 11

#define MAX_DMA 12
static dma_callback_t dma_callbacks[MAX_DMA] = {0};

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

void dma_configure_peripheral(uint8_t chan, const volatile void *src, volatile void *dst, size_t count, bool read_from_peripheral, dma_callback_t callback) {
    DMA_CTRL_TRIG(chan) = 0;

    DMA_READ_ADDR(chan) = (uintptr_t)src;
    DMA_WRITE_ADDR(chan) = (uintptr_t)dst;
    DMA_TRANS_COUNT(chan) = count;

    if (read_from_peripheral) {
        DMA_CTRL_TRIG(chan) = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_INCR_WRITE | DMA_CTRL_TRIG_DATA_SIZE_32;
    } else {
        DMA_CTRL_TRIG(chan) = DMA_CTRL_TRIG_EN | DMA_CTRL_TRIG_INCR_READ | DMA_CTRL_TRIG_DATA_SIZE_32;
    }

    dma_callbacks[chan] = callback;
    irq_enable(DMA_IRQ_0 + (chan/6));
}

void dma_free(uint8_t chan) {
    DMA_CTRL_TRIG(chan) = 0;
    dma_callbacks[chan] = NULL;

    bool empty = true;
    for (int i = chan*6; i <= (chan+1)*6; i++) {
        if (dma_callbacks[i]) empty = false; break;
    }
    if (empty) irq_disable(DMA_IRQ_0 + (chan/6));
}

void DMA_IRQ_Handler(void) {
    for (uint8_t i = 0; i < 12; i++) {
        if (dma_is_busy(i)) continue;

        if (dma_callbacks[i]) {
            dma_callbacks[i]();
            dma_callbacks[i] = NULL;
        }
    }
}
