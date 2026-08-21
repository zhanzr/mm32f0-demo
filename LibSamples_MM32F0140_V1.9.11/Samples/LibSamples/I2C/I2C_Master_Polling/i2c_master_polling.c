/***********************************************************************************************************************
    @file    i2c_master_polling.c
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
#define _I2C_MASTER_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_master_polling.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_Polling
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
void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    /* K1->PC1->EXTI_Line1 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line1;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* K2->PB2->EXTI_Line2 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line2;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* EXTI Interrupt */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

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
void I2C_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef  I2C_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_I2C1, ENABLE);

    I2C_DeInit(I2C1);

    I2C_StructInit(&I2C_InitStruct);
    I2C_InitStruct.I2C_Mode       = I2C_Mode_MASTER;
    I2C_InitStruct.I2C_Speed      = I2C_Speed_STANDARD;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Send7bitAddress(I2C1, 0xB0, 0);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_TxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        I2C_SendData(I2C1, Buffer[i]);

        while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE))
        {
        }
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_RxData_Polling(uint8_t *Buffer, uint16_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        I2C_ReadCmd(I2C1);

        while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_RFNE))
        {
        }

        Buffer[i] = I2C_ReceiveData(I2C1);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_WriteBuffer(uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_Polling((uint8_t *)Buffer, Length);

    while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE))
    {
    }

    I2C_GenerateSTOP(I2C1, ENABLE);

    while (RESET == I2C_GetITStatus(I2C1, I2C_IT_STOP_DET))
    {
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_ReadBuffer(uint8_t *Buffer, uint8_t Length)
{
    I2C_RxData_Polling((uint8_t *)Buffer, Length);

    I2C_GenerateSTOP(I2C1, ENABLE);

    while (!I2C_GetITStatus(I2C1, I2C_IT_STOP_DET))
    {
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_ShowOperationTips(void)
{
    printf("\r\n------------------------------");
    printf("\r\nK1 : write");
    printf("\r\nK2 : read ");
    printf("\r\n------------------------------");
    printf("\r\n");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Master_Polling_Sample(void)
{
    uint8_t i, Buffer[10];

    printf("\r\nTest %s", __FUNCTION__);

    K1_PressFlag = 0;
    K2_PressFlag = 0;

    EXTI_Configure();

    I2C_Configure();

    I2C_ShowOperationTips();

    while (1)
    {
        if (K1_PressFlag == 1)
        {
            K1_PressFlag = 0;

            printf("\r\nWrite...");

            I2C_WriteBuffer(Buffer, sizeof(Buffer));

            printf("\r\nOK");

            I2C_ShowOperationTips();
        }

        if (K2_PressFlag == 1)
        {
            K2_PressFlag = 0;

            printf("\r\nRead...");

            I2C_ReadBuffer(Buffer, sizeof(Buffer));

            printf("\r\nOK : ");

            for(i = 0; i < sizeof(Buffer); i++)
            {
                printf("0x%02X ", Buffer[i]);
            }

            I2C_ShowOperationTips();
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

