/***********************************************************************************************************************
    @file    uart_simulatewithgpio.c
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
#define _UART_SIMULATEWITHGPIO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "uart_simulatewithgpio.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup UART
  * @{
  */

/**
  * @addtogroup UART_SimulateWithGPIO
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
void TIM16_Configure(void)
{
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM16, ENABLE);

    TIM_DeInit(TIM16);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (RCC_Clocks.PCLK2_Frequency / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (sUART_BAUTRATE_DELAY - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStruct);

    TIM_ClearFlag(TIM16, TIM_FLAG_Update);
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM16_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM16, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM17_Configure(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM17, ENABLE);

    TIM_DeInit(TIM17);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (RCC_Clocks.PCLK2_Frequency / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (sUART_BAUTRATE_DELAY - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStruct);

    TIM_ClearFlag(TIM17, TIM_FLAG_Update);

    TIM_Cmd(TIM17, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUART_DelayUS(uint32_t Tick)
{
    TIM_SetAutoreload(TIM17, Tick - 1);

    TIM_Cmd(TIM17, ENABLE);

    while (RESET == TIM_GetFlagStatus(TIM17, TIM_FLAG_Update))
    {
    }

    TIM_ClearFlag(TIM17, TIM_FLAG_Update);

    TIM_Cmd(TIM17, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUART_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Simulate UART Tx */
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET);

    /* Simulate UART Rx */
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line3;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_3_IRQn;
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
void SimulateUART_SendData(uint8_t Data)
{
    uint8_t i = 0;

    GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET);

    SimulateUART_DelayUS(sUART_BAUTRATE_DELAY);

    for (i = 0; i < 8; i++)
    {
        if (Data & 0x01)
        {
            GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET);
        }
        else
        {
            GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_RESET);
        }

        Data >>= 1;

        SimulateUART_DelayUS(sUART_BAUTRATE_DELAY);
    }

    GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET);

    SimulateUART_DelayUS(sUART_BAUTRATE_DELAY);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void UART_SimulateWithGPIO_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    SimulateUART_RxData = 0;
    SimulateUART_RxFlag = 0;
    SimulateUART_RxStep = sUART_RX_BIT_STOP;

    TIM16_Configure();

    TIM17_Configure();

    SimulateUART_Configure();

    while (1)
    {
        if (1 == SimulateUART_RxFlag)
        {
            SimulateUART_RxFlag = 0;

            SimulateUART_SendData(SimulateUART_RxData);
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

