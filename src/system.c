#include "system.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define PERIPH_UART0 0
#define PERIPH_UART1 1
#define PERIPH_SPI0 2
#define PERIPH_SPI1 3
#define PERIPH_I2C0 6
#define PERIPH_PWM 12
#define PERIPH_ADC 29

#define RESETS_RESET (*(volatile uint32_t *)(RESET_BASE + 0x0))
#define RESETS_RESET_DONE (*(volatile uint32_t *)(RESET_BASE + 0x8))

#define PLL_SYS_CS (*(volatile uint32_t *)(PLL_SYS_BASE + 0x00))
#define PLL_SYS_PWR (*(volatile uint32_t *)(PLL_SYS_BASE + 0x04))
#define PLL_SYS_FBDIV (*(volatile uint32_t *)(PLL_SYS_BASE + 0x08))
#define PLL_SYS_PRIM (*(volatile uint32_t *)(PLL_SYS_BASE + 0x0C))

#define CLK_SYS_CTRL (*(volatile uint32_t *)(CLK_BASE + 0x3C))
#define CLK_SRC_PLL_SYS 0x0

#define SYS_CLK 12000000u

#define WATCHDOG_CTRL (*(volatile uint32_t *)(WATCHDOG_BASE + 0x00))
#define WATCHDOG_TICK (*(volatile uint32_t *)(WATCHDOG_BASE + 0x04))
#define WATCHDOG_LOAD (*(volatile uint32_t *)(WATCHDOG_BASE + 0x08))
#define WATCHDOG_REASON (*(volatile uint32_t *)(WATCHDOG_BASE + 0x0C))


void system_config(void) {
    system_disable_watchdog();

    system_enable_peripheral(PERIPH_UART0, true);
    system_enable_peripheral(PERIPH_I2C0, true);
    system_enable_peripheral(PERIPH_PWM, true);

    system_set_cpu_freq(125000000);

    system_enable_watchdog(1);
}

void system_enable_peripheral(uint32_t peripheral_id, bool enable) {
    if (enable) {
        RESETS_RESET &= ~(1u << peripheral_id);
        while ((RESETS_RESET_DONE & (1u << peripheral_id)) == 0);
    } else {
        RESETS_RESET |= 1u << peripheral_id;
    }
}

void system_reset_peripheral(uint32_t peripheral_id) {
    RESETS_RESET |= 1u << peripheral_id;
    while ((RESETS_RESET_DONE & (1u << peripheral_id)) == 0);
    RESETS_RESET &= ~(1u << peripheral_id);
    while ((RESETS_RESET_DONE & (1u << peripheral_id)) == 0);
}

void system_set_cpu_freq(uint32_t freq_hz) {
    PLL_SYS_PWR &= ~(1 << 0);
    PLL_SYS_FBDIV = (freq_hz / SYS_CLK);
    PLL_SYS_PWR |= (1 << 0);
    while (!(PLL_SYS_CS & (1 << 31)));
    CLK_SYS_CTRL = CLK_SRC_PLL_SYS;
}

void system_enable_watchdog(uint32_t timeout_ms) {
    uint32_t ticks = timeout_ms * 1000;
    WATCHDOG_LOAD = ticks;
    WATCHDOG_CTRL = 1;
}

void system_disable_watchdog(void) {
    WATCHDOG_CTRL = 0;
}