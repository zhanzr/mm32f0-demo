/***********************************************************************************************************************
    @file    tim3_encoder.c
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
#define _TIM3_ENCODER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim3_encoder.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_Encoder
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
void TIM3_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_ICInitTypeDef       TIM_ICInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1);  /* TIM3_CH1 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_1);  /* TIM3_CH2 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_Period    = UINT16_MAX;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

//    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI1, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
//    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI2, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);

    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_Channel  = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICFilter = 6;
    TIM_ICInit(TIM3, &TIM_ICInitStruct);

    TIM_SetCounter(TIM3, 0);

    TIM_Cmd(TIM3, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_Encoder_Sample(void)
{
    uint16_t OldCount = 0;
    uint16_t NewCount = 0;

    printf("\r\nTest %s", __FUNCTION__);

    /* PB4 & PB5 : encoder A & B channel */
    TIM3_Configure();

    while (1)
    {
        NewCount = TIM_GetCounter(TIM3);

        if (NewCount != OldCount)
        {
            OldCount = NewCount;

            printf("\r\nDIR : %d, CNT : %d", (TIM3->CR1 & TIM_CR1_DIR) ? 1 : 0, NewCount);
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

