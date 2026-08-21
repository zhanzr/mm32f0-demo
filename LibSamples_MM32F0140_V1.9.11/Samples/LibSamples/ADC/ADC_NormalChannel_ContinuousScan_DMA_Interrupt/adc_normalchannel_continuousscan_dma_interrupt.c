/***********************************************************************************************************************
    @file    adc_normalchannel_continuousscan_dma_interrupt.c
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
#define _ADC_NORMALCHANNEL_CONTINUOUSSCAN_DMA_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_normalchannel_continuousscan_dma_interrupt.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_NormalChannel_ContinuousScan_DMA_Interrupt
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/
uint16_t ADC_Buffer[30];

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_Configure(void)
{
    ADC_InitTypeDef  ADC_InitStruct;
    DMA_InitTypeDef  DMA_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution         = ADC_Resolution_12b;
    ADC_InitStruct.ADC_PRESCARE           = ADC_PCLK2_PRESCARE_16;
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Continue;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC1_ExternalTrigConv_T1_CC1;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_DMACmd(ADC1, ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 0, ADC_Samctl_240_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 0, ADC_Samctl_240_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 0, ADC_Samctl_240_5);
    ADC_ChannelCmd(ADC1, ADC_Channel_1, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channel_4, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channel_5, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    /* PA5(RV1) PA4(RV2) PA1(RV3) */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_Cmd(ADC1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->ADDATA);
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)ADC_Buffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize         = 30;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);

    DMA_ClearFlag(DMA1_FLAG_TC1);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_NormalChannel_ContinuousScan_DMA_Interrupt_Sample(void)
{
    uint8_t  i = 0, j = 0;
    uint32_t RVxSum = 0;
    float    RVxAverage[3], RVxVoltage[3];

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (1)
    {
        if (0 != ADC_DMA_InterruptFlag)
        {
            ADC_DMA_InterruptFlag = 0;

            for (i = 0; i < 3; i++)
            {
                RVxSum = 0;

                for (j = 0; j < 10; j++)
                {
                    RVxSum += ADC_Buffer[i + j * 3];
                }

                RVxAverage[i] = (float)RVxSum / (float)10.0;
            }

            printf("\r\n");

            for (i = 0; i < 3; i++)
            {
                RVxVoltage[i] = RVxAverage[i] * (float)3.3 / (float)4096.0;

                printf("RV%d Voltage = %0.2f\t", 3 - i, RVxVoltage[i]);
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

