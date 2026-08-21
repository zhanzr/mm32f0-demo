/***********************************************************************************************************************
    @file    pwr_stop_iwdg_wakeup.c
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
#define _PWR_STOP_IWDG_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_stop_iwdg_wakeup.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_Stop_IWDG_Wakeup
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
void PWR_GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOD, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_IWDG_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_IWDG, ENABLE);

    RCC_LSICmd(ENABLE);

    while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSIRDY))
    {
    }

    PVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    PVU_CheckStatus();

    RVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(0xFFF);
    RVU_CheckStatus();

    IVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetIGen(0x500);
    IVU_CheckStatus();

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_OverflowConfig(IWDG_Overflow_Interrupt);

    IWDG_ReloadCounter();

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line24;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = WWDG_IWDG_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority  = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    IWDG_Enable();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Pretreatment(void)
{
    printf("\r\n-->Entry stop mode");

    PLATFORM_DelayMS(10);

    PWR_GPIO_Configure();

    PWR_IWDG_Configure();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Reinitialize(void)
{
    SystemInit();

    PLATFORM_Init();

    PLATFORM_DelayMS(100);

    printf("\r\n-->Wakeup by IWDG");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Stop_IWDG_Wakeup_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s", __FUNCTION__);

    while (1)
    {
        printf("\r\n-->Running...");

        for (i = 0; i < 20; i++)
        {
            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(99);

            IWDG_ReloadCounter();
        }

        PWR_Pretreatment();

        /* Stop Mode */
        RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);
        PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);

        PWR_Reinitialize();
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

