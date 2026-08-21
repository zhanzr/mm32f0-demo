/***********************************************************************************************************************
    @file    dbg_tim_stop.c
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
#define _DBG_TIM_STOP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "dbg_tim_stop.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup DBG
  * @{
  */

/**
  * @addtogroup DBG_TIM_Stop
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
void DBG_PWM_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    /* Compute the value to be set in ARR regiter to generate signal frequency at 100 Khz */
    uint32_t TimerPeriod = (RCC_GetPCLK1Freq() / 100000 ) - 1;

    /* Compute CCR1 value to generate a duty cycle at 80% for channel 1 */
    uint32_t Channel1Pulse = ((uint32_t)800 * (TimerPeriod - 1)) / 1000;

    /* Compute CCR2 value to generate a duty cycle at 40% for channel 2 */
    uint32_t Channel2Pulse = ((uint32_t)400 * (TimerPeriod - 1)) / 1000;

    /* Compute CCR3 value to generate a duty cycle at 25% for channel 3 */
    uint32_t Channel3Pulse = ((uint32_t)250 * (TimerPeriod - 1)) / 1000;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = 0;
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = TimerPeriod;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = 0;
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;

    TIM_OCInitStruct.TIM_Pulse = Channel1Pulse;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_Pulse = Channel2Pulse;
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_Pulse = Channel3Pulse;
    TIM_OC3Init(TIM1, &TIM_OCInitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);  /* TIM1_CH1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);  /* TIM1_CH2 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2); /* TIM1_CH3 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_Cmd(TIM1, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DBG_TIM_Stop_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    DBG_PWM_Configure();

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_DBGMCU, ENABLE);

    DBGMCU_Configure(DBGMCU_CR_TIM1_STOP, ENABLE);

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

