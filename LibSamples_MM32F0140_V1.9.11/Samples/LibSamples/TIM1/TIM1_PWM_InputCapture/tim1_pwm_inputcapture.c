/***********************************************************************************************************************
    @file    tim1_pwm_inputcapture.c
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
#define _TIM1_PWM_INPUTCAPTURE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim1_pwm_inputcapture.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_PWM_InputCapture
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
void TIM1_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_ICInitTypeDef       TIM_ICInitStruct;
    RCC_ClocksTypeDef       RCC_Clocks;
    uint32_t                TIM_ClockFrequency = 0;
    uint32_t                HPRE = 0, PPRE2 = 0;

    HPRE  = READ_BIT(RCC->CFGR, RCC_CFGR_HPRE)  >> RCC_CFGR_HPRE_Pos;
    PPRE2 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

    RCC_GetClocksFreq(&RCC_Clocks);

    if (HPRE < 8)
    {
        if (PPRE2 < 4)
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency;
        }
        else
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 2;
        }
    }
    else
    {
        if (PPRE2 < 4)
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 2;
        }
        else
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 4;
        }
    }

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (TIM_ClockFrequency / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = 0xFFFF;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICFilter    = 0x00;
    TIM_PWMIConfig(TIM1, &TIM_ICInitStruct);

    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);

    TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_6); /* TIM1_CH1 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_PWM_InputCapture_Sample(void)
{
    float Frequency, Duty;

    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nInjection frequency range : [20Hz, 100kHz]");

    TIM1_Configure();

    while (1)
    {
        if (1 == TIM1_CC_InterruptFlag)
        {
            TIM1_CC_InterruptFlag = 0;

            Frequency = (float)1000000 / (float)TIM1_CC_Value1;

            Duty = (float)TIM1_CC_Value2 / (float)TIM1_CC_Value1 * (float)100;

            if (Frequency >= 1000000.0)
            {
                printf("\r\nFrequency = %0.1fMHz, Duty = %0.1f%%", Frequency / (float)1000000, Duty);
            }
            else if (Frequency >= 1000.0)
            {
                printf("\r\nFrequency = %0.1fkHz, Duty = %0.1f%%", Frequency / (float)1000, Duty);
            }
            else
            {
                printf("\r\nFrequency = %0.1fHz,  Duty = %0.1f%%", Frequency, Duty);
            }
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

