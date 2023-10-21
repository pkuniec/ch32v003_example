#include "timer.h"

static os_Timer_t os_timer;


void os_timer_init(os_Timer_t *timer, uint8_t time, uint8_t repeat) {
    if ( !timer ) {
        timer = &os_timer;
    }
    timer->timer_expire = time;
    if ( repeat ) {
        timer->timer_period = time;
    } else {
        timer->timer_period = 0;
    }
}


void os_timer_setfn(os_Timer_t *timer, os_TimerFunc *func, void *arg) {
    if ( !timer ) {
        timer = &os_timer;
    }
    timer->timer_func = func;
	timer->arg = arg;
}


int8_t os_timer_add(os_Timer_t *timer) {
    os_Timer_t *cur = &os_timer;

    if ( !timer->timer_func ) {
        return -1;
    }

    while ( cur->timer_next ) {
        cur = cur->timer_next;
    }

    cur->timer_next = timer;
    return 0;
}


void os_timer_disarm(os_Timer_t *timer) {
    timer->timer_expire = 0;
    timer->timer_func = 0;
    timer->timer_period = 0;
    timer->arg = 0;
}


void os_timer_event(void) {
    os_Timer_t *cur = &os_timer;

    do {
		cur->timer_expire--;
		if ( !cur->timer_expire ) {
            if( cur->timer_func ) {
                cur->timer_func( cur->arg );

                if( cur->timer_period ) {
                    cur->timer_expire = cur->timer_period;
                } else {
                    cur->timer_func = 0;
                }
            }
        }
        cur = cur->timer_next;
    } while ( cur );
}