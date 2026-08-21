/***********************************************************************************************************************
    @file    comp_outputtotim1bkin.c
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
#define _COMP_OUTPUTTOTIM1BKIN_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "comp_outputtotim1bkin.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup COMP
  * @{
  */

/**
  * @addtogroup COMP_OutputToTIM1BKIN
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
void COMP_Configure(void)
{
    COMP_InitTypeDef COMP_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_COMP, ENABLE);

    COMP_StructInit(&COMP_InitStruct);
    COMP_InitStruct.Invert     = COMP_InvertingInput_IO0;
    COMP_InitStruct.NonInvert  = COMP_NonInvertingInput_IO0;
    COMP_InitStruct.Output     = COMP_Output_TIM1BKIN;
    COMP_InitStruct.OutputPol  = COMP_NonInverted;
    COMP_InitStruct.Hysteresis = COMP_Hysteresis_No;
    COMP_InitStruct.Mode       = COMP_Mode_MediumSpeed;
    COMP_InitStruct.OutAnaSel  = COMP_AnalogOutput_Sync;
    COMP_InitStruct.OFLT       = COMP_Filter_4_Period;
    COMP_Init(COMP1, &COMP_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    COMP_Cmd(COMP1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line19;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = ADC_COMP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_BDTRInitTypeDef     TIM_BDTRInitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = 0;
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = 10000 - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = 0;
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Set;

    TIM_OCInitStruct.TIM_Pulse = 5000 - 1;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_Pulse = 5000 - 1;
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_Pulse = 5000 - 1;
    TIM_OC3Init(TIM1, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_BDTRStructInit(&TIM_BDTRInitStruct);
    TIM_BDTRInitStruct.TIM_OSSRState       = TIM_OSSRState_Enable;
    TIM_BDTRInitStruct.TIM_OSSIState       = TIM_OSSIState_Enable;
    TIM_BDTRInitStruct.TIM_LOCKLevel       = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStruct.TIM_DeadTime        = 1;
    TIM_BDTRInitStruct.TIM_Break           = TIM_Break_Enable;
    TIM_BDTRInitStruct.TIM_BreakPolarity   = TIM_BreakPolarity_Low;
    TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStruct);

    TIM_BreakInputFilterConfig(TIM1, TIM_BKINF_128, TIM1_BKIN_CPT);
    TIM_BreakInputFilterCmd(TIM1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_6);    /* TIM1_CH1  */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_6);    /* TIM1_CH2  */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_6);    /* TIM1_CH3  */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_2);   /* TIM1_CH1N */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_2);   /* TIM1_CH2N */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_2);   /* TIM1_CH2N */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
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
void COMP_OutputToTIM1BKIN_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    COMP_Configure();

    TIM1_Configure();

    printf("\r\nTurn around RV1 or RV3, adjust the comparator channel voltage to control TIM1 PWM output.");

    while (1)
    {
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

