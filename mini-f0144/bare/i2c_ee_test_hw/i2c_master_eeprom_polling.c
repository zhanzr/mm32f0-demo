/***********************************************************************************************************************
    @file    i2c_master_eeprom_polling.c
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
#define _I2C_MASTER_EEPROM_POLLING_C_

/* Files include */
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "i2c_master_eeprom_polling.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_EEPROM_Polling
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define EEPROM_I2C_ADDRESS      0xA0
#define EEPROM_PAGE_SIZE        0x08
#define EEPROM_CAPACITY         256

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
void EEPROM_WritePage(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_Polling((uint8_t *)&Address, 0x01);

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
void EEPROM_ReadData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_Polling((uint8_t *)&Address, 0x01);

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
void I2C_Master_EEPROM_Polling_Sample(void)
{
    uint8_t i = 0;
    uint8_t ReadBuffer[20], WriteBuffer[20];
    uint8_t DiskBuf[EEPROM_PAGE_SIZE];
    uint16_t a;
    uint32_t t0, t1;
    uint32_t tErase, tProg, tRead;
    uint8_t ok = 1;

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

    /* --- quick sanity: 20-byte write/read --- */
    for (i = 0; i < 20; i++)
    {
        ReadBuffer[i]  = 0;
        WriteBuffer[i] = i + 0x30;
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

    /* --- full-device speed: erase / program / read all 256 bytes --- */
    printf("\r\n\r\n=== full 24C02 (256 B) speed ===\r\n");

    memset(DiskBuf, 0xFF, EEPROM_PAGE_SIZE);

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += EEPROM_PAGE_SIZE)
    {
        EEPROM_WriteData(a, DiskBuf, EEPROM_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tErase = t1 - t0;

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += EEPROM_PAGE_SIZE)
    {
        for (i = 0; i < EEPROM_PAGE_SIZE; i++)
        {
            DiskBuf[i] = (uint8_t)(a + i);
        }
        EEPROM_WriteData(a, DiskBuf, EEPROM_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tProg = t1 - t0;

    t0 = PLATFORM_GetTick();
    EEPROM_ReadData(0, ReadBuffer, 20);   /* exercise sequential read entry */
    for (a = 0; a < EEPROM_CAPACITY; a += 20)
    {
        EEPROM_ReadData(a, ReadBuffer, 20);
        for (i = 0; i < 20; i++)
        {
            if (ReadBuffer[i] != (uint8_t)(a + i))
            {
                ok = 0;
            }
        }
    }
    t1 = PLATFORM_GetTick();
    tRead = t1 - t0;

    printf("\r\nErase  time: %lu ms", (unsigned long)tErase);
    printf("\r\nProgram time: %lu ms", (unsigned long)tProg);
    printf("\r\nRead   time: %lu ms", (unsigned long)tRead);
    printf("\r\nErase  speed: %lu B/s",
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tErase ? tErase : 1)));
    printf("\r\nProgram speed: %lu B/s",
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tProg ? tProg : 1)));
    printf("\r\nRead   speed: %lu B/s",
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tRead ? tRead : 1)));
    printf("\r\nReadback match: %s", ok ? "PASS" : "FAIL");

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

