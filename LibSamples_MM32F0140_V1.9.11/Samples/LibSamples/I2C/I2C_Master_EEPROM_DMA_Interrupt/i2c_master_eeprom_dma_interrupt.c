/***********************************************************************************************************************
    @file    i2c_master_eeprom_dma_interrupt.c
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
#define _I2C_MASTER_EEPROM_DMA_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_master_eeprom_dma_interrupt.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_EEPROM_DMA_Interrupt
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define EEPROM_I2C_ADDRESS      0xA0
#define EEPROM_PAGE_SIZE        0x08

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

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

    I2C_Send7bitAddress(I2C1, EEPROM_I2C_ADDRESS, 0);

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
void I2C_TxData_DMA_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    DMA_InitTypeDef  DMA_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr    = (uint32_t)&(I2C1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr        = (uint32_t)Buffer;
    DMA_InitStruct.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize            = Length;
    DMA_InitStruct.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                   = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload           = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStruct);

    DMA_ClearFlag(DMA1_FLAG_TC2);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    I2C_TX_DMA_InterruptFlag = 0;

    I2C_DMAConfigure(I2C1, TDMAE_SET);

    DMA_Cmd(DMA1_Channel2, ENABLE);

    while (0 == I2C_TX_DMA_InterruptFlag)
    {
    }

    I2C_DMAConfigure(I2C1, TRDMA_RESET);

    DMA_Cmd(DMA1_Channel2, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_RxData_DMA_Interrupt(uint8_t *Buffer, uint16_t Length)
{
    DMA_InitTypeDef  DMA_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    uint16_t ReadStatus = 0x100;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr    = (uint32_t)&(I2C1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr        = (uint32_t)&ReadStatus;
    DMA_InitStruct.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize            = Length;
    DMA_InitStruct.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc             = DMA_MemoryInc_Disable;
    DMA_InitStruct.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize        = DMA_MemoryDataSize_Word;
    DMA_InitStruct.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                   = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload           = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStruct);

    DMA_DeInit(DMA1_Channel3);

    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_PeripheralBaseAddr    = (uint32_t)&(I2C1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr        = (uint32_t)Buffer;
    DMA_InitStruct.DMA_DIR                   = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize            = Length;
    DMA_InitStruct.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Word;
    DMA_InitStruct.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStruct.DMA_M2M                   = DMA_M2M_Disable;
    DMA_InitStruct.DMA_Auto_reload           = DMA_Auto_Reload_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStruct);

    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    I2C_RX_DMA_InterruptFlag = 0;

    I2C_DMAConfigure(I2C1, TRDMA_SET);

    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    while (0 == I2C_RX_DMA_InterruptFlag)
    {
    }

    I2C_DMAConfigure(I2C1, TRDMA_RESET);

    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EEPROM_WritePage(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_DMA_Interrupt((uint8_t *)&Address, 0x01);

    I2C_TxData_DMA_Interrupt((uint8_t *)Buffer, Length);

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
void EEPROM_ReadData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_DMA_Interrupt((uint8_t *)&Address, 0x01);

    I2C_RxData_DMA_Interrupt((uint8_t *)Buffer, Length);

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
void EEPROM_WriteData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    uint8_t Start = 0;
    uint8_t StartCount = 0, PageNumber = 0, FinalCount = 0;

    if ((Address % EEPROM_PAGE_SIZE) == 0)
    {
        StartCount = 0;
        PageNumber = Length / EEPROM_PAGE_SIZE;
        FinalCount = Length % EEPROM_PAGE_SIZE;
    }
    else
    {
        Start = Address % EEPROM_PAGE_SIZE;

        if (((Start + Length) / EEPROM_PAGE_SIZE) == 0)
        {
            StartCount = Length;
            PageNumber = 0;
            FinalCount = 0;
        }
        else
        {
            StartCount = EEPROM_PAGE_SIZE - Start;
            PageNumber = (Length - StartCount) / EEPROM_PAGE_SIZE;
            FinalCount = (Length - StartCount) % EEPROM_PAGE_SIZE;
        }
    }

    if (StartCount)
    {
        EEPROM_WritePage(Address, Buffer, StartCount);

        Address += StartCount;
        Buffer  += StartCount;

        PLATFORM_DelayMS(50);
    }

    while (PageNumber--)
    {
        EEPROM_WritePage(Address, Buffer, EEPROM_PAGE_SIZE);

        Address += EEPROM_PAGE_SIZE;
        Buffer  += EEPROM_PAGE_SIZE;

        PLATFORM_DelayMS(50);
    }

    if (FinalCount)
    {
        EEPROM_WritePage(Address, Buffer, FinalCount);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Master_EEPROM_DMA_Interrupt_Sample(void)
{
    uint8_t i = 0;
    uint8_t ReadBuffer[20], WriteBuffer[20];

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

    for (i = 0; i < 20; i++)
    {
        ReadBuffer[i]  = 0;
        WriteBuffer[i] = i;
    }

    printf("\r\n\r\nEEPROM Write : ");

    EEPROM_WriteData(0, WriteBuffer, 20);

    printf("OK");

    printf("\r\n\r\nEEPROM Read  : \r\n");

    EEPROM_ReadData(0, ReadBuffer, 20);

    for (i = 0; i < 20; i++)
    {
        printf("0x%02x ", ReadBuffer[i]);

        if (0 == ((i + 1) % 10))
        {
            printf("\r\n");
        }
    }

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

