/***********************************************************************************************************************
    @file    uart_dma_interrupt.c
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
#define _UART_DMA_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "uart_dma_interrupt.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup UART
  * @{
  */

/**
  * @addtogroup UART_DMA_Interrupt
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
void UART_Configure(uint32_t Baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    UART_InitTypeDef UART_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_UART2, ENABLE);

    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.BaudRate      = Baudrate;
    UART_InitStruct.WordLength    = UART_WordLength_8b;
    UART_InitStruct.StopBits      = UART_StopBits_1;
    UART_InitStruct.Parity        = UART_Parity_No;
    UART_InitStruct.HWFlowControl = UART_HWFlowControl_None;
    UART_InitStruct.Mode          = UART_Mode_Rx | UART_Mode_Tx;
    UART_Init(UART2, &UART_InitStruct);

    UART_DMACmd(UART2, UART_DMAReq_EN, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    UART_Cmd(UART2, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void UART_RxData_DMA_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    DMA_InitTypeDef  DMA_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel5);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(UART2->RDR);
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)Buffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize         = Length;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);

    DMA_ClearFlag(DMA1_FLAG_TC5);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    UART_RX_DMA_InterruptFlag = 0;

    DMA_Cmd(DMA1_Channel5, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void UART_TxData_DMA_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    DMA_InitTypeDef  DMA_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(UART2->TDR);
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)Buffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = Length;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);

    DMA_ClearFlag(DMA1_FLAG_TC4);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    UART_TX_DMA_InterruptFlag = 0;

    DMA_Cmd(DMA1_Channel4, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void UART_DMA_Interrupt_Sample(void)
{
    uint8_t Buffer[10];

    printf("\r\nTest %s", __FUNCTION__);

    UART_Configure(115200);

    UART_RxData_DMA_Interrupt(Buffer, 10);

    printf("\r\nSend 10 bytes to UART every time");

    while (1)
    {
        if (0 != UART_RX_DMA_InterruptFlag)
        {
            UART_TxData_DMA_Interrupt(Buffer, 10);

            while (0 == UART_TX_DMA_InterruptFlag)
            {
            }

            UART_RxData_DMA_Interrupt(Buffer, 10);
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

