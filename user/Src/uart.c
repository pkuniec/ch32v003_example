#include "uart.h"
#include "queue.h"
#include "ch32v00x_usart.h"

static queue_t rx_queue, tx_queue;

void uart_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure = {0};
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 9600;
    // USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}


void uart_cp2txbuf(const uint8_t *buff, uint32_t len) {
    while (len--) {
        if (add_queue(&tx_queue, *buff++)) {
            // TX buff full - enable interrupt and waite for empty space
        }
    }
    // Enable TX interrupt
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
}

int uart_putc(uint8_t data) {
    // Disable RX
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

    if ( !add_queue(&tx_queue, data) ) {
        // Enable TX
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        USART_ITConfig(USART1, USART_IT_TC, ENABLE);
        return 0;
    } else {
        return -1;
    }

    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void uart_puts(const uint8_t *str) {
    while( *str ) {
        if( !uart_putc( *str ) )
            *(str++);
    }
}

int uart_recv(uint8_t *data) {
    return get_queue(&rx_queue, data);
}

// ISR
void USART1_IRQHandler(void)
{
    // IT_TXE
    if (USART_GetITStatus(USART1, USART_IT_TXE) == SET) {
        uint8_t data = 0;
        if (!get_queue(&tx_queue, &data)) {
            USART_SendData(USART1, (uint16_t)data);
        } else {
            // Disable TX
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            // Enable RX
            USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        }
    }

    // IT_RXNE
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        add_queue(&rx_queue, (uint8_t)USART_ReceiveData(USART1));
    }

    // IT_TC
    if (USART_GetITStatus(USART1, USART_IT_TC) == SET) {
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        USART1->STATR &= ~(USART_STATR_TC);
    }
}