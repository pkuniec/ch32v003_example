#ifndef UART_H_
#define UART_H_

#include <inttypes.h>

void uart_init(void);
void uart_cp2txbuf(const uint8_t *buff, uint32_t len);
int uart_putc(uint8_t c);
void uart_puts(const uint8_t *str);
int uart_recv(uint8_t *data);

// ISR
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#endif