#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef void (*dma_callback_t)(void);

int dma_init(bool high_priority);

void dma_start_transfer(uint8_t chan, const void *src, void *dst, uint32_t count);
bool dma_is_busy(uint8_t chan);
void dma_wait_complete(uint8_t chan);

void dma_configure_peripheral(uint8_t chan, const volatile void *src, volatile void *dst, size_t count, bool read_from_peripheral, dma_callback_t callback);

void dma_free(uint8_t chan);

#endif