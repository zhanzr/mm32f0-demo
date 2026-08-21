/***********************************************************************************************************************
    @file    dma_memorytomemory.c
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
#define _DMA_MEMORYTOMEMORY_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "dma_memorytomemory.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup DMA
  * @{
  */

/**
  * @addtogroup DMA_MemoryToMemory
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/
uint8_t DMA_RxBuffer[64 * 4];
uint8_t DMA_TxBuffer[64 * 4];

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_ByteToByte_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint8_t *p = (uint8_t *)DMA_TxBuffer;
    uint8_t *q = (uint8_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x22;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel2, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC2))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC2);

    for (i = 0; i < 64; i++)
    {
        if ((uint8_t)(*p++) != (uint8_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_ByteToHalfWord_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint8_t  *p = (uint8_t  *)DMA_TxBuffer;
    uint16_t *q = (uint16_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x11;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel1, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC1))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC1);

    for (i = 0; i < 64; i++)
    {
        if ((uint16_t)(*p++) != (uint16_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_ByteToWord_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint8_t  *p = (uint8_t  *)DMA_TxBuffer;
    uint32_t *q = (uint32_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x33;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel3, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC3))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC3);

    for (i = 0; i < 64; i++)
    {
        if ((uint32_t)(*p++) != (uint32_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_HalfWordToByte_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint16_t *p = (uint16_t *)DMA_TxBuffer;
    uint8_t  *q = (uint8_t  *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x44;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel4, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC4))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC4);

    for (i = 0; i < 64; i++)
    {
        if ((uint8_t)(*p++) != (uint8_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_HalfWordToHalfWord_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint16_t *p = (uint16_t *)DMA_TxBuffer;
    uint16_t *q = (uint16_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x55;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC5))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC5);

    for (i = 0; i < 64; i++)
    {
        if ((uint16_t)(*p++) != (uint16_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_HalfWordToWord_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint16_t *p = (uint16_t *)DMA_TxBuffer;
    uint32_t *q = (uint32_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x55;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC5))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC5);

    for (i = 0; i < 64; i++)
    {
        if ((uint32_t)(*p++) != (uint32_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_WordToByte_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint32_t *p = (uint32_t *)DMA_TxBuffer;
    uint8_t  *q = (uint8_t  *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x55;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC5))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC5);

    for (i = 0; i < 64; i++)
    {
        if ((uint8_t)(*p++) != (uint8_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_WordToHalfWord_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint32_t *p = (uint32_t *)DMA_TxBuffer;
    uint16_t *q = (uint16_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x55;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC5))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC5);

    for (i = 0; i < 64; i++)
    {
        if ((uint16_t)(*p++) != (uint16_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_WordToWord_Test(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    uint8_t i = 0;
    uint8_t ErrorCount = 0;

    uint32_t *p = (uint32_t *)DMA_TxBuffer;
    uint32_t *q = (uint32_t *)DMA_RxBuffer;

    for (i = 0; i < 64; i++)
    {
        *(p + i) = i + 0x55;
    }

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)DMA_RxBuffer;
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize         = 64;
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Enable;
    DMA_InitStruct.DMA_Auto_reload        = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (RESET == DMA_GetFlagStatus(DMA1_FLAG_TC5))
    {
    }

    DMA_ClearFlag(DMA1_FLAG_TC5);

    for (i = 0; i < 64; i++)
    {
        if ((uint32_t)(*p++) != (uint32_t)(*q++))
        {
            ErrorCount++;
        }
    }

    printf("\r\n %-27s error count : %d", __FUNCTION__, ErrorCount);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DMA_MemoryToMemory_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    DMA_ByteToByte_Test();

    DMA_ByteToHalfWord_Test();

    DMA_ByteToWord_Test();

    DMA_HalfWordToByte_Test();

    DMA_HalfWordToHalfWord_Test();

    DMA_HalfWordToWord_Test();

    DMA_WordToByte_Test();

    DMA_WordToHalfWord_Test();

    DMA_WordToWord_Test();

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

