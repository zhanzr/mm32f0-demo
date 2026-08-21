/***********************************************************************************************************************
    @file    gpio_swd_remap.c
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
#define _GPIO_SWD_REMAP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "gpio_swd_remap.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup GPIO
  * @{
  */

/**
  * @addtogroup GPIO_SWD_Remap
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
void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_8);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_8);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_IO_Toggle(GPIO_TypeDef *GPIOn, uint16_t PINn)
{
    if (Bit_RESET == GPIO_ReadOutputDataBit(GPIOn, PINn))
    {
        GPIO_WriteBit(GPIOn, PINn, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOn, PINn, Bit_RESET);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   download program after manual reset
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_SWD_Remap_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s...", __FUNCTION__);

    printf("\r\nDownloadable programm...");

    for (i = 0; i < 10; i++)
    {
        PLATFORM_LED_Toggle(LED1);
        printf(".");

        PLATFORM_DelayMS(500);
    }

    GPIO_Configure();

    printf("\r\nCan't download program unless reset manually!");

    while (1)
    {
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_13);
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_14);

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

