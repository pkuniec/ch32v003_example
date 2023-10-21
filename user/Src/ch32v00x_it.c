/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v00x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include <ch32v00x_it.h>
#include "common.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

static uint8_t usec = 100;
static uint8_t msec = 100;
static uint32_t irq_flags;

uint32_t *GetTimeHandler(void) {
    return &irq_flags;
}

void SysTick_Handler(void)
{
  SysTick->SR = 0;

  // 100 us
  irq_flags |= OS_FLAGS_TIMER_100US;

  // Timer 10ms
  if (!usec--) {
    usec = 100;
    msec--;
    irq_flags |= OS_FLAGS_TIMER_10MS;
  }

  // Timer 1s
  if (!msec) {
    msec = 100;
    irq_flags |= OS_FLAGS_TIMER_1S;
  }
}





