/***********************************************************************************************************************
    @file    comp_multichannel_polling.c
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
#define _COMP_MULTICHANNEL_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "comp_multichannel_polling.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup COMP
  * @{
  */

/**
  * @addtogroup COMP_MultiChannel_Polling
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
    GPIO_InitTypeDef GPIO_InitStruct;
    COMP_InitTypeDef COMP_InitStruct;

    COMP_POLL_InitTypeDef COMP_POLL_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_COMP, ENABLE);

    COMP_StructInit(&COMP_InitStruct);
    COMP_InitStruct.Invert     = COMP_InvertingInput_IO0;
    COMP_InitStruct.NonInvert  = COMP_NonInvertingInput_IO0;
    COMP_InitStruct.Output     = COMP_Output_None;
    COMP_InitStruct.OutputPol  = COMP_NonInverted;
    COMP_InitStruct.Hysteresis = COMP_Hysteresis_No;
    COMP_InitStruct.Mode       = COMP_Mode_MediumSpeed;
    COMP_InitStruct.OutAnaSel  = COMP_AnalogOutput_Sync;
    COMP_InitStruct.OFLT       = COMP_Filter_4_Period;
    COMP_Init(COMP1, &COMP_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    /* COMP1 INM0 -> PA5 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* COMP1 INP1 -> PA2 & INP2 -> PA3 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    COMP_Cmd(COMP1, ENABLE);

    COMP_POLL_InitStruct.COMP_Poll_En     = COMP_POLL_EN_ENABLE;
    COMP_POLL_InitStruct.COMP_Poll_Ch     = COMP_POLL_CH_1_2;
    COMP_POLL_InitStruct.COMP_Poll_Fixn   = COMP_POLL_FIXN_FIXED;    /* INM_SEL = SET & INP_SEL = 1/2 */
    COMP_POLL_InitStruct.COMP_Poll_Period = COMP_POLL_PERIOD_128;
    COMP_POLL_Init(COMP1, &COMP_POLL_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   Change Console to PB7!!!
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void COMP_MultiChannel_Polling_Sample(void)
{
    uint32_t OldLevel = 0, NewLevel = 0;

    printf("\r\nTest %s", __FUNCTION__);

    COMP_Configure();

    printf("\r\nAdjust the comparator channel(PA5(RV3) PA2 and PA3) voltage.");

    while (1)
    {
        NewLevel = READ_BIT(COMP->COMP1_POLL, COMP_POLL_POUT);

        if (OldLevel != NewLevel)
        {
            OldLevel = NewLevel;

            printf("\r\nCOMP1 POUT[0]: %d, POUT[1] : %d", (NewLevel & (1 << 8)) ? 1 : 0, (NewLevel & (1 << 9)) ? 1 : 0);
        }

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

