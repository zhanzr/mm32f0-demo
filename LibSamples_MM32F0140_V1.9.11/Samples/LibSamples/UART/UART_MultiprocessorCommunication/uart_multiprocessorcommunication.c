/***********************************************************************************************************************
    @file    uart_multiprocessorcommunication.c
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
#define _UART_MULTIPROCESSORCOMMUNICATION_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "uart_multiprocessorcommunication.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup UART
  * @{
  */

/**
  * @addtogroup UART_MultiprocessorCommunication
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

void UART_SendFrame(uint8_t Address, uint8_t Data)
{
    UART_Set9bitLevel(UART2, ENABLE);

    UART_SendData(UART2, Address);

    while (RESET == UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
    {
    }

    UART_SendData(UART2, Data);

    while (RESET == UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
    {
    }
}

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

    UART_Enable9bit(UART2, ENABLE);

    /* B8TXD = B8POL = 1 */
    UART_Set9bitLevel(UART2, ENABLE);
    UART_Set9bitPolarity(UART2, UART_9bit_Polarity_High);

    UART_Set9bitAutomaticToggle(UART2, ENABLE);

    UART_WakeUpConfig(UART2, UART_WakeUp_AddressMark);
    UART_SetRXAddress(UART2, 0x05);
    UART_SetRXMASK(UART2, 0xFF);

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
void UART_MultiprocessorCommunication_Sample(void)
{
    uint8_t Data = 0;

    printf("\r\nTest %s", __FUNCTION__);

    UART_Configure(115200);

#if 0
    while (1)
    {
        UART_SendFrame(0x05, Data++);
        PLATFORM_DelayMS(500);
    }
#else
    while (1)
    {
        if (RESET != UART_GetFlagStatus(UART2, UART_FLAG_RXAVL))
        {
            if (ENABLE == ((UART2->CCR & UART_CCR_B8RXD) ? ENABLE : DISABLE))
            {
                printf("\r\nAddress : 0x%02x", UART_ReceiveData(UART2));
            }
            else
            {
                printf("\tData : 0x%02x", UART_ReceiveData(UART2));
            }
        }
    }
#endif
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

