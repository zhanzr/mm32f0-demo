/***********************************************************************************************************************
    @file    uart_lin_master.c
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
#define _UART_LIN_MASTER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "uart_lin_master.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup UART
  * @{
  */

/**
  * @addtogroup UART_LIN_Master
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
    NVIC_InitTypeDef NVIC_InitStruct;
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

    UART2->IDLR = 100;    /* LIN Master Only!!! */

    UART_ITConfig(UART2, UART_IT_RXIEN, ENABLE);

    UART2->IER |= UART_IER_TXBRK;

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

    NVIC_InitStruct.NVIC_IRQChannel = UART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority  = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    UART_Cmd(UART2, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t LIN_FrameIDToPID(uint8_t FrameID)
{
    uint8_t i  = 0;
    uint8_t P0 = 0, P1 = 0, PID = 0xFF;
    uint8_t ID_BIT[6] =
    {
        0, 0, 0, 0, 0, 0
    };

    if (FrameID < 0x40)
    {
        PID = FrameID;

        for (i = 0; i < 6; i++)
        {
            if (FrameID & (0x01 << i))
            {
                ID_BIT[i] = 1;
            }
            else
            {
                ID_BIT[i] = 0;
            }
        }

        P0 =  (ID_BIT[0] ^ ID_BIT[1] ^ ID_BIT[2] ^ ID_BIT[4]) & 0x01;
        P1 = ~(ID_BIT[1] ^ ID_BIT[3] ^ ID_BIT[4] ^ ID_BIT[5]) & 0x01;

        if (P0)
        {
            PID |= 0x40;
        }

        if (P1)
        {
            PID |= 0x80;
        }
    }

    printf("\r\nFrameID => PID : 0x%02x => 0x%02x\r\n", FrameID, PID);

    return (PID);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t LIN_ClassicChecksum(uint8_t *Buffer, uint8_t Length)
{
    uint8_t  i = 0;
    uint16_t Checksum = 0;

    for (i = 0; i < Length; i++)
    {
        Checksum += Buffer[i];

        if (Checksum > 0xFF)
        {
            Checksum %= 0xFF;
        }
    }

    return (~(uint8_t)(Checksum & 0x00FF));
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t LIN_EnhancedChecksum(uint8_t PID, uint8_t *Buffer, uint8_t Length)
{
    uint8_t  i = 0;
    uint16_t Checksum = PID;

    for (i = 0; i < Length; i++)
    {
        Checksum += Buffer[i];

        if (Checksum > 0xFF)
        {
            Checksum %= 0xFF;
        }
    }

    return (~(uint8_t)(Checksum & 0x00FF));
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void LIN_MASTER_SendData(uint8_t Data)
{
    UART_SendData(UART2, Data);

    while (RESET == UART_GetFlagStatus(UART2, UART_FLAG_TXEPT))
    {
    }
}

/***********************************************************************************************************************
  * @brief  Synchronisation Break
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void LIN_MASTER_Break(void)
{
    LIN_MASTER_TXBRK_InterruptFlag = 0;

    UART_SetLIN(UART2, ENABLE);
    UART_SendBreak(UART2);

    while (0 == LIN_MASTER_TXBRK_InterruptFlag)
    {
    }
}

/***********************************************************************************************************************
  * @brief  Sync Byte Field
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void LIN_MASTER_SyncByte(void)
{
    LIN_MASTER_SendData(0x55);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void LIN_MASTER_SendHeader(uint8_t PID)
{
    LIN_MASTER_Break();

    LIN_MASTER_SyncByte();

    LIN_MASTER_SendData(PID);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void LIN_Master_SendFrame(uint8_t FrameID, uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;
    uint8_t Checksum = 0;
    uint8_t PID = LIN_FrameIDToPID(FrameID);

    if ((0x3C == FrameID) || (0x3D == FrameID))
    {
        Checksum = LIN_ClassicChecksum(Buffer, Length);
    }
    else
    {
        Checksum = LIN_EnhancedChecksum(PID, Buffer, Length);
    }

    LIN_MASTER_SendHeader(PID);

    for (i = 0; i < Length; i++)
    {
        LIN_MASTER_SendData(Buffer[i]);
    }

    LIN_MASTER_SendData(Checksum);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void UART_LIN_Master_Sample(void)
{
    uint8_t i = 0;
    uint8_t FrameID = 0, Mode = 0;
    uint8_t Buffer[2] =
    {
        0, 0
    };

    printf("\r\nTest %s", __FUNCTION__);

    LIN_MASTER_RxLength = 0;
    LIN_MASTER_RxFinish = 0;

    for (i = 0; i < 100; i++)
    {
        LIN_MASTER_RxBuffer[i] = 0;
    }

    UART_Configure(19200);

    while (1)
    {
        if (Mode == 0)
        {
            printf("\r\nLIN Master Write...");
            LIN_Master_SendFrame(FrameID, Buffer, sizeof(Buffer));
        }
        else
        {
            printf("\r\nLIN Master Read....");
            LIN_MASTER_SendHeader(LIN_FrameIDToPID(FrameID));

            while (0 == LIN_MASTER_RxFinish)
            {
            }

            LIN_MASTER_RxFinish = 0;

            printf("\r\nLIN Master Rx Length : %d, Rx Buffer : ", LIN_MASTER_RxLength);

            for (i = 0; i < LIN_MASTER_RxLength; i++)
            {
                printf("0x%02x ", LIN_MASTER_RxBuffer[i]);
            }

            printf("\r\n");

            for (i = 0; i < sizeof(Buffer); i++)
            {
                Buffer[i]++;
            }

            FrameID = (FrameID + 1) % 0x40;
        }

        Mode = (0 == Mode) ? 1 : 0;

        PLATFORM_DelayMS(500);
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

