/***********************************************************************************************************************
    @file    rcc_mco.c
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
#define _RCC_MCO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "rcc_mco.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup RCC
  * @{
  */

/**
  * @addtogroup RCC_MCO
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
void RCC_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_0);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_MCOConfig(RCC_MCO_HSI);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void RCC_MCO_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    RCC_Configure();

    printf("\r\nUse an oscilloscope to monitor PA8 output frequency.");

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

