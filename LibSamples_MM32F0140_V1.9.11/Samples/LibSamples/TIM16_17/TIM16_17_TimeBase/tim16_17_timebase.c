/***********************************************************************************************************************
    @file    tim16_17_timebase.c
    @author  FAE Team
    @date    25-May-2023
    @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
  **********************************************************************************************************************
    @attention

    <h2><center>&copy; Copyright(c) <2023> <MindMotion></center></h2>

      Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
    following conditions are met:
    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
       the following disclaimer in the documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
       promote products derived from this software without specific prior written permission.

      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *********************************************************************************************************************/

/* Define to prevent recursive inclusion */
#define _TIM16_17_TIMEBASE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim16_17_timebase.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM16_17
  * @{
  */

/**
  * @addtogroup TIM16_17_TimeBase
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM16_17_Configure(void)
{
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    RCC_ClocksTypeDef       RCC_Clocks;
    uint32_t                TIM_ClockFrequency = 0;
    uint32_t                PPRE2 = 0;

    PPRE2 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

    RCC_GetClocksFreq(&RCC_Clocks);

    if (PPRE2 < 4)
    {
        TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency;
    }
    else
    {
        TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 2;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM16, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler     = (TIM_ClockFrequency / 10000 - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period        = (2000 - 1);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM16, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM16, TIM_FLAG_Update);
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM16_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM16, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM17, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler     = (RCC_Clocks.PCLK2_Frequency / 10000 - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period        = (5000 - 1);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM17, TIM_FLAG_Update);
    TIM_ITConfig(TIM17, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM17_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM17, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM16_17_TimeBase_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM16_17_Configure();

    while (1)
    {
        if (0 != TIM16_UpdateFlag)
        {
            TIM16_UpdateFlag = 0;

            PLATFORM_LED_Toggle(LED1);
        }

        if (0 != TIM17_UpdateFlag)
        {
            TIM17_UpdateFlag = 0;

            PLATFORM_LED_Toggle(LED2);
        }
    }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/********************************************** (C) Copyright MindMotion **********************************************/

