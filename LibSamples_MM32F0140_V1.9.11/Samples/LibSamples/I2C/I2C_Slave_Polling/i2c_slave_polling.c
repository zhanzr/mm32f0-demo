/***********************************************************************************************************************
    @file    i2c_slave_polling.c
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
#define _I2C_SLAVE_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_slave_polling.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Slave_Polling
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
void I2C_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef  I2C_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_I2C1, ENABLE);

    I2C_DeInit(I2C1);

    I2C_StructInit(&I2C_InitStruct);
    I2C_InitStruct.I2C_Mode       = I2C_Mode_SLAVE;
    I2C_InitStruct.I2C_Speed      = I2C_Speed_STANDARD;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_SendSlaveAddress(I2C1, 0xB0);
    I2C_SlaveConfigure(I2C1, ENABLE);

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
void I2C_Slave_Polling_Sample(void)
{
    uint8_t i = 0, Buffer[10];
    uint8_t RxLength = 0, TxLength = 0;

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

    while (1)
    {
        if(RESET != I2C_GetFlagStatus(I2C1, I2C_FLAG_RX_FULL))
        {
            Buffer[RxLength++] = I2C_ReceiveData(I2C1);

            if(RxLength == 10)
            {
                I2C_GenerateSTOP(I2C1, ENABLE);
                TxLength = 0;

                printf("\r\nI2C Slave Receive : ");

                for(i = 0; i < RxLength; i++)
                {
                    printf("0x%02X ", Buffer[i]);
                }
            }
        }

        if(RESET != I2C_GetFlagStatus(I2C1, I2C_FLAG_RD_REQ))
        {
            I2C_SendData(I2C1, Buffer[TxLength++]);

            I2C_ClearFlag(I2C1, I2C_FLAG_RD_REQ);

            if(TxLength == RxLength)
            {
                RxLength = 0;

                printf("\r\nI2C Slave Send Finish.");
            }
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

