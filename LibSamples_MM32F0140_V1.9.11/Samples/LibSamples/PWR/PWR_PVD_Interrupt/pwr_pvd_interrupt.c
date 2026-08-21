/***********************************************************************************************************************
    @file    pwr_pvd_interrupt.c
    @author  FAE Team
    @date    23-Aug-2023
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
#define _PWR_PVD_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_pvd_interrupt.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_PVD_Interrupt
  * @{
  */

/* Private typedef ****************************************************************************************************/
typedef struct
{
    float    Voltage;
    uint32_t Level;
} PVD_MAP_TypeDef;

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/
PVD_MAP_TypeDef PVD_MapTable[11] =
{
    { 1.8, PWR_PVDLevel_1V8 },            /* 0:  1.8V */
    { 2.1, PWR_PVDLevel_2V1 },            /* 1:  2.1V */
    { 2.4, PWR_PVDLevel_2V4 },            /* 2:  2.4V */
    { 2.7, PWR_PVDLevel_2V7 },            /* 3:  2.7V */
    { 3.0, PWR_PVDLevel_3V0 },            /* 4:  3.0V */
    { 3.3, PWR_PVDLevel_3V3 },            /* 5:  3.3V */
    { 3.6, PWR_PVDLevel_3V6 },            /* 6:  3.6V */
    { 3.9, PWR_PVDLevel_3V9 },            /* 7:  3.9V */
    { 4.2, PWR_PVDLevel_4V2 },            /* 8:  4.2V */
    { 4.5, PWR_PVDLevel_4V5 },            /* 9:  4.5V */
    { 4.8, PWR_PVDLevel_4V8 },            /* 10: 4.8V */
};

uint8_t PVD_LevelSelection = 0;

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Configure(void)
{
    ADC_InitTypeDef  ADC_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* For measure VDD_M voltage */
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_PRESCARE   = ADC_PCLK2_PRESCARE_16;
    ADC_InitStruct.ADC_Mode       = ADC_Mode_Continue;
    ADC_InitStruct.ADC_DataAlign  = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_VoltReference, 0, ADC_Samctl_240_5);
    ADC_ChannelCmd(ADC1, ADC_Channel_VoltReference, ENABLE);

    ADC_VrefintCmd(ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    /* PVD Interrupt->EXTI_Line_16 */
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    PWR_PVDLevelConfig(PVD_MapTable[PVD_LevelSelection].Level);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line16;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    PWR_PVDCmd(ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_PVD_Interrupt_Sample(void)
{
    static uint8_t PVD_Count  = 0;

    uint16_t ConversionResult = 0;
    uint16_t CalibrationValue = *((uint16_t *)(0x1FFFF7E0));

    float VDD_M_Voltage;

    printf("\r\nTest %s", __FUNCTION__);

    PVD_LevelSelection = 6;

    PWR_Configure();

    printf("\r\nPVD Level(%d) Selection : %0.1fV", PVD_LevelSelection, PVD_MapTable[PVD_LevelSelection].Voltage);

    while (1)
    {
        /* Read VDD_M voltage */
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        {
        }

        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

        ConversionResult = ADC_GetConversionValue(ADC1);

        VDD_M_Voltage = (float)3.3 * (float)CalibrationValue / (float)ConversionResult;

        /* PVD detection */
        if (0 != PWR_PVD_InterruptFlag)
        {
            PWR_PVD_InterruptFlag = 0;

            if (RESET != (PWR->CSR & PWR_CSR_PVDO))
            {
                printf("\r\n%03d : VDD/VDDA(%0.1fV) <= PVD(%0.1fV)", PVD_Count++, VDD_M_Voltage, PVD_MapTable[PVD_LevelSelection].Voltage);
            }
            else
            {
                printf("\r\n%03d : VDD/VDDA(%0.1fV) >= PVD(%0.1fV)", PVD_Count++, VDD_M_Voltage, PVD_MapTable[PVD_LevelSelection].Voltage);
            }
        }

        PLATFORM_DelayMS(50);
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

