/***********************************************************************************************************************
    @file    spi_slave_interrupt.c
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
#define _SPI_SLAVE_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "spi_slave_interrupt.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup SPI
  * @{
  */

/**
  * @addtogroup SPI_Slave_Interrupt
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
void SPI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    SPI_InitTypeDef  SPI_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SPI1, ENABLE);

    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode      = SPI_Mode_Slave;
    SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStruct.SPI_DataWidth = 8;
    SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);
    SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Tx);

    NVIC_InitStruct.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_0);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line15;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    SPI_Cmd(SPI1, ENABLE);

    SPI_SendData(SPI1, 0xFF);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_RxTxData_Interrupt(uint8_t *rBuffer, uint8_t *wBuffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        SPI_RxStruct.Buffer[i] = 0;
        SPI_TxStruct.Buffer[i] = wBuffer[i];
    }

    SPI_RxStruct.Length = Length;
    SPI_RxStruct.CurrentCount = 0;
    SPI_RxStruct.CompleteFlag = 0;

    SPI_TxStruct.Length = Length;
    SPI_TxStruct.CurrentCount = 0;
    SPI_TxStruct.CompleteFlag = 0;

    SPI_ITConfig(SPI1, SPI_IT_RX, ENABLE);
    SPI_ITConfig(SPI1, SPI_IT_TX, ENABLE);

    while (0 == SPI_TxStruct.CompleteFlag)
    {
    }

    while (0 == SPI_RxStruct.CompleteFlag)
    {
    }

    for (i = 0; i < Length; i++)
    {
        rBuffer[i] = SPI_RxStruct.Buffer[i];
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_Slave_Interrupt_Sample(void)
{
    uint8_t i = 0;
    uint8_t rBuffer[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t wBuffer[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};

    printf("\r\nTest %s", __FUNCTION__);

    SPI_Configure();

    while (1)
    {
        printf("\r\nWait SPI master R/W...");

        SPI_RxTxData_Interrupt(rBuffer, wBuffer, 10);

        printf("\r\nRx Data : ");

        for(i = 0; i < 10; i++)
        {
            printf("0x%02X ", rBuffer[i]);
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

