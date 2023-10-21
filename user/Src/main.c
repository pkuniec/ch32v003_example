#include <string.h>
#include "debug.h"
#include "system_ch32v00x.h"
#include "ch32v00x_conf.h"

#include "common.h"
#include "modbus.h"


void SysTickInit(void) {
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->SR &= ~(1 << 0);
    SysTick->CMP = 4800-1;
    SysTick->CNT = 0;
    SysTick->CTLR = (1<<3) | (1<<2) | (1<<1) | (1<<0);
}

void GPIO_Toggle_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

uint8_t i = 0;

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    SystemInit();
    SysTickInit();
    GPIO_Toggle_init();
    uart_init();
    system_setup();

    // Modbus
    modbusInit();
    modbusReset();
    modbusSetAddres(0x01);

    mbRegisterFunc(0x05, modbusFunc05);
    mbRegisterFunc(0x03, modbusFunc03);
    mbRegisterFunc(0x06, modbusFunc06);
    mbRegisterFunc(0x01, modbusFunc01);

    while(1) {
        
        // if (!uart_recv(&recv)) {
        //     if (recv == '\n' || recv == '\r') {
        //         rxBuff[rxIndex] = 0;
        //         uint8_t buff[20];
        //         snprintf(buff, 12, "recv: %s %d\n\r", rxBuff, rxIndex);
        //         uart_puts(buff);
        //         rxIndex = 0;
        //     } else {
        //         rxBuff[rxIndex++] = recv;
        //         if (rxIndex > 9)
        //             rxIndex = 0;
        //         recv = 0;
        //     }
        // }

        timer_event();
        uart_event();
        modbus_event();
    }
}
