#include "debug.h"
#include "system_ch32v00x.h"
#include "ch32v00x_gpio.h"
#include "ch32v00x_misc.h"
#include "ch32v00x_it.h"

void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void SysTickInit(void) {
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->SR &= ~(1 << 0);
    SysTick->CMP = 8000-1;
    SysTick->CNT = 0;
    SysTick->CTLR = (1<<3) | (1<<2) | (1<<1) | (1<<0);
}

int main(void)
{
    uint8_t i = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    SystemInit();
    // Delay_Init();
    // USART_Printf_Init(115200);
    // printf("SystemClk:%d\r\n", SystemCoreClock);
    // printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    // printf("GPIO Toggle TEST\r\n");
    
    SysTickInit();
    GPIO_Toggle_INIT();

    uint32_t next_counter = 0;
    while(1)
    {
        // Delay_Ms(250);
        if (next_counter < get_systick())
        {
            GPIO_WriteBit(GPIOD, GPIO_Pin_4, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
            next_counter = get_systick() + 1000;
        }
        
    }
}
