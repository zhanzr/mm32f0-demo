/***********************************************************************************************************************
    @file    tim16_17_pwm_output_deadtime_break.c
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
#define _TIM16_17_PWM_OUTPUT_DEADTIME_BREAK_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim16_17_pwm_output_deadtime_break.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM16_17
  * @{
  */

/**
  * @addtogroup TIM16_17_PWM_Output_DeadTime_Break
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
    TIM_BDTRInitTypeDef     TIM_BDTRInitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM17, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler     = 0;
    TIM_TimeBaseStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period        = 10000 - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStruct);

    TIM_ARRPreloadConfig(TIM17, ENABLE);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = 5000 - 1;
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OC1Init(TIM17, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);

    TIM_BDTRStructInit(&TIM_BDTRInitStruct);
    TIM_BDTRInitStruct.TIM_OSSRState       = TIM_OSSRState_Enable;
    TIM_BDTRInitStruct.TIM_OSSIState       = TIM_OSSIState_Enable;
    TIM_BDTRInitStruct.TIM_LOCKLevel       = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStruct.TIM_DeadTime        = 1;
    TIM_BDTRInitStruct.TIM_Break           = TIM_Break_Enable;
    TIM_BDTRInitStruct.TIM_BreakPolarity   = TIM_BreakPolarity_Low;
    TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM17, &TIM_BDTRInitStruct);

    TIM_BreakInputFilterConfig(TIM17, TIM_BKINF_128, TIM17_BKIN_PB4);
    TIM_BreakInputFilterCmd(TIM17, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_5);    /* TIM17_BKIN1 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);    /* TIM17_CH1  */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_2);    /* TIM17_CH1N */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    TIM_Cmd(TIM17, ENABLE);

    TIM_CtrlPWMOutputs(TIM17, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM16_17_PWM_Output_DeadTime_Break_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM16_17_Configure();

    printf("\r\nShort PB4 to PB11, Press K4 to Break, Use Logic Analyzer to monitor PA7 and PB7.");

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

