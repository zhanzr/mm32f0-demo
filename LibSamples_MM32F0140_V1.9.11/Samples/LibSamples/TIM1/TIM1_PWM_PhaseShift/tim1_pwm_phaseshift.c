/***********************************************************************************************************************
    @file    tim1_pwm_phaseshift.c
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
#define _TIM1_PWM_PHASESHIFT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim1_pwm_phaseshift.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_PWM_PhaseShift
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
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
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
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseStruct.TIM_Period            = 1000 - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse       = 500 - 1;
    TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_PWMShiftConfig(TIM1, TIM_PDER_CCR1SHIFTEN, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_6);    /* TIM1_CH1 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_6);    /* TIM1_CH2 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    TIM_Cmd(TIM1, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_PWM_PhaseShift_Sample(void)
{
    uint16_t ShiftValue = 0;

    printf("\r\nTest %s", __FUNCTION__);

    TIM1_Configure();

    while (1)
    {
        TIM_SetCompare1(TIM1, (500 - 1) - ShiftValue);
        TIM_SetCCR1FALL(TIM1, (500 - 1) + ShiftValue);

        ShiftValue = (ShiftValue + 0x1) % (500 - 1);

        PLATFORM_DelayMS(1);
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

