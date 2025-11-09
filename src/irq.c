#include "irq.h"
#include <stdint.h>

#define NVIC_ISER0 (*(volatile uint32_t *)0xE000E100)
#define NVIC_ICER0 (*(volatile uint32_t *)0xE000E180)
#define NVIC_VECTOR (*(volatile uint32_t *)0xE000ED08)

static irq_handler_t irq_vector[32];

void irq_enable(uint8_t irq_num) {
    NVIC_ISER0 = (1 << irq_num);
}

void irq_disable (uint8_t irq_num) {
    NVIC_ICER0 = (1 << irq_num);
}

void irq_set_handler(uint8_t irq_num, irq_handler_t handler) {
    irq_vector[irq_num] = handler;
}

void IRQ_Handler(uint8_t irq_num) {
    if (irq_vector[irq_num])
        irq_vector[irq_num]();
}
