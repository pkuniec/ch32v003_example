#include "common.h"
#include "ch32v00x_it.h"
#include "ch32v00x_gpio.h"
#include "timer.h"
#include "uart.h"
#include "modbus.h"

sys_t *GetSysHandler(void) {
    static sys_t sys;
    return &sys;
}

void system_setup(void) {
    os_timer_init(0, 1, 1);
    os_timer_setfn(0, sys_timer_func, 0);
}

void uart_event(void) {
    uint8_t data = 0;
	if( !uart_recv(&data) ) {
        modbus_putdata(data);
	}
}

void timer_event(void) {
    uint32_t *flags = GetTimeHandler();

	// 100 us
    if ( (*flags) & OS_FLAGS_TIMER_100US ) {
        *flags &= ~(OS_FLAGS_TIMER_100US);
        modbusTickTimer();
    }

	// 10 ms
    if ( (*flags) & OS_FLAGS_TIMER_10MS ) {
        *flags &= ~(OS_FLAGS_TIMER_10MS);
    }

	// 1s
    if ( (*flags) & OS_FLAGS_TIMER_1S ) {
        *flags &= ~(OS_FLAGS_TIMER_1S);
        os_timer_event();
    }
}

void sys_timer_func(void *arg) {
	arg = (int8_t *)arg;
    static uint8_t i = 0;
    // uart_puts("Tick ...\n\r");
}

void output_set(uint8_t num, uint8_t mode) {
    if (num == 1)
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, mode);
}

uint8_t* GetRegHandler(void) {
    static uint8_t reg_data;
    return &reg_data;
}