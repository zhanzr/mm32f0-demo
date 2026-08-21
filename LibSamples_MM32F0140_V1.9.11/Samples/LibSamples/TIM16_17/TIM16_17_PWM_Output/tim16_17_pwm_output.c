/***********************************************************************************************************************
    @file    tim16_17_pwm_output.c
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
#define _TIM16_17_PWM_OUTPUT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim16_17_pwm_output.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM16_17
  * @{
  */

/**
  * @addtogroup TIM16_17_PWM_Output
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
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
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

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM16, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler     = 0;
    TIM_TimeBaseStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period        = (TIM_ClockFrequency / 100000) - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = (TIM_ClockFrequency / 100000) * 4 / 5;   /* 80% */
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM16, &TIM_OCInitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_5);    /* TIM16_CH1 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_Cmd(TIM16, ENABLE);
    TIM_CtrlPWMOutputs(TIM16, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM17, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler     = 0;
    TIM_TimeBaseStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period        = (TIM_ClockFrequency / 100000) - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = (TIM_ClockFrequency / 100000) * 1 / 2;   /* 50% */
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM17, &TIM_OCInitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);    /* TIM17_CH1 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_Cmd(TIM17, ENABLE);
    TIM_CtrlPWMOutputs(TIM17, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM16_17_PWM_Output_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM16_17_Configure();

    printf("\r\nUse Logic Analyzer to monitor PA6 and PA7.");

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(100);
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

