#pragma once

#include <inttypes.h>

typedef void os_TimerFunc(void* arg);

typedef struct _os_TIMER_ {
    struct _os_TIMER_ *timer_next;
    uint8_t timer_expire;
    uint8_t timer_period;
    os_TimerFunc *timer_func;
    void *arg;
} os_Timer_t;

void os_timer_init(os_Timer_t *timer, uint8_t time, uint8_t repeat);
void os_timer_setfn(os_Timer_t *timer, os_TimerFunc *func, void *arg);
int8_t os_timer_add(os_Timer_t *timer);
void os_timer_disarm(os_Timer_t *timer);
void os_timer_event(void);