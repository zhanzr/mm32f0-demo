#define _MM32F0140_IT_C_

#include "platform.h"
#include "mm32f0140_it.h"

extern volatile uint32_t g_ticks;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    g_ticks++;
}