#pragma once

#include <inttypes.h>

#define OS_FLAGS_TIMER_100US    0x01
#define OS_FLAGS_TIMER_10MS     0x02
#define OS_FLAGS_TIMER_1S       0x04

typedef struct {
    uint8_t flags;
} sys_t;


void system_setup(void);
sys_t* GetSysHandler(void);
void timer_event(void);
void uart_event(void);

void sys_timer_func(void *arg);

void output_set(uint8_t num, uint8_t mode);
uint8_t* GetRegHandler(void);