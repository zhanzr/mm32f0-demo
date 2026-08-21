/***********************************************************************************************************************
    @file    adc_tim_cc_trigger.c
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
#define _ADC_TIM_CC_TRIGGER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_tim_cc_trigger.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_TIM_CC_Trigger
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
void ADC_Configure(void)
{
    ADC_InitTypeDef         ADC_InitStruct;
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;

    RCC_ClocksTypeDef RCC_Clocks;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution         = ADC_Resolution_12b;
    ADC_InitStruct.ADC_PRESCARE           = ADC_PCLK2_PRESCARE_16;
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Scan;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC1_ExternalTrigConv_T1_CC1;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 0, ADC_Samctl_240_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 0, ADC_Samctl_240_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 0, ADC_Samctl_240_5);
    ADC_ChannelCmd(ADC1, ADC_Channel_1, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channel_4, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channel_5, ENABLE);

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    /* PA5(RV1) PA4(RV2) PA1(RV3) */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_Cmd(ADC1, ENABLE);

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (RCC_Clocks.PCLK2_Frequency / 2000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (2000 - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode          = TIM_OCMode_Timing;
    TIM_OCInitStruct.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse           = (2000 / 2 - 1);
    TIM_OCInitStruct.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    TIM_Cmd(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_TIM_CC_Trigger_Sample(void)
{
    float RVxVoltage[3];

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    while (1)
    {
        if (RESET != ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        {
            ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

            RVxVoltage[0] = (float)ADC_GetChannelConvertedValue(ADC1, ADC_Channel_5) * (float)3.3 / (float)4096.0;
            RVxVoltage[1] = (float)ADC_GetChannelConvertedValue(ADC1, ADC_Channel_4) * (float)3.3 / (float)4096.0;
            RVxVoltage[2] = (float)ADC_GetChannelConvertedValue(ADC1, ADC_Channel_1) * (float)3.3 / (float)4096.0;

            printf("\r\nRV1 Voltage = %0.2f  \tRV2 Voltage = %0.2f  \tRV3 Voltage = %0.2f\t", RVxVoltage[0], RVxVoltage[1], RVxVoltage[2]);
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

