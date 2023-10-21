#ifndef QUEUE__H
#define QUEUE__H

#include <inttypes.h>

#define Q_SIZE   32

typedef struct {
    volatile uint32_t idx;
    volatile uint32_t cnt;
    uint8_t queue[Q_SIZE];
} queue_t;

void init_queue(queue_t *q);
int add_queue(queue_t *q, uint8_t value);
int get_queue(queue_t *q, uint8_t* value);
//uint32_t size_queue(queue_t *q);

#endif