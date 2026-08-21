/***********************************************************************************************************************
    @file    tim14_timebase.c
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
#define _TIM14_TIMEBASE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim14_timebase.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM14
  * @{
  */

/**
  * @addtogroup TIM14_TimeBase
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
void TIM14_Configure(void)
{
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    RCC_ClocksTypeDef       RCC_Clocks;
    uint32_t                TIM_ClockFrequency = 0;
    uint32_t                PPRE2 = 0;

    RCC_GetClocksFreq(&RCC_Clocks);

    if (PPRE2 < 4)
    {
        TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency;
    }
    else
    {
        TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 2;
    }

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM14, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler         = (TIM_ClockFrequency / 10000 - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period            = (5000 - 1);
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM14, TIM_FLAG_Update);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM14, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM14_TimeBase_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM14_UpdateFlag = 0;

    TIM14_Configure();

    while (1)
    {
        if (0 != TIM14_UpdateFlag)
        {
            TIM14_UpdateFlag = 0;

            PLATFORM_LED_Toggle(LED1);
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

