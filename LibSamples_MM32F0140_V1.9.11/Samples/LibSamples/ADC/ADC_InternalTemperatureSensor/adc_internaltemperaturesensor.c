/***********************************************************************************************************************
    @file    adc_internaltemperaturesensor.c
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
#define _ADC_INTERNALTEMPERATURESENSOR_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_internaltemperaturesensor.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_InternalTemperatureSensor
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/
#define  AVG_SLOPE  5.313

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
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution         = ADC_Resolution_12b;
    ADC_InitStruct.ADC_PRESCARE           = ADC_PCLK2_PRESCARE_16;
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Scan;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC1_ExternalTrigConv_T1_CC1;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor,    0, ADC_Samctl_240_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_VoltReference, 0, ADC_Samctl_240_5);
    ADC_ChannelCmd(ADC1, ADC_Channel_TempSensor, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channel_VoltReference, ENABLE);

    ADC_TempSensorVrefintCmd(ENABLE);

    ADC_Cmd(ADC1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_InternalTemperatureSensor_Sample(void)
{
    uint16_t Value;

    float Voltage, Temperature;

    uint16_t OffsetV = *(volatile uint16_t *)(0x1FFFF7E0) & 0x0FFF;
    uint16_t OffsetT = *(volatile uint16_t *)(0x1FFFF7F6) & 0x0FFF;

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    while (1)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        {
        }

        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

        /* Internal voltage sensor */
        Value = ADC_GetChannelConvertedValue(ADC1, ADC_Channel_VoltReference);

        Voltage = (float)(OffsetV) * (float)3.3 / (float)Value;

        /* Internal temperature sensor */
        Value = ADC_GetChannelConvertedValue(ADC1, ADC_Channel_TempSensor);

        Temperature = 25 + ((float)Value * (float)Voltage * (float)1000 - (float)OffsetT * (float)3300) / ((float)4096 * (float)AVG_SLOPE);

        printf("\r\nVDD : %0.2f, Temperature : %0.2f", Voltage, Temperature);

        PLATFORM_DelayMS(500);
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

