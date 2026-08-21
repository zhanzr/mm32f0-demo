/***********************************************************************************************************************
  @file    mm32f0140_it.c
  @brief   Exception & interrupt service routines.
***********************************************************************************************************************/

#define _MM32F0140_IT_C_

#include "platform.h"
#include "mm32f0140_it.h"

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
    if (0 != PLATFORM_DelayTick)
    {
        PLATFORM_DelayTick--;
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/