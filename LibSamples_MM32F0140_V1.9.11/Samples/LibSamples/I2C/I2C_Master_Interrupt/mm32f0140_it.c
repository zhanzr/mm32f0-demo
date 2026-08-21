/***********************************************************************************************************************
    @file    mm32f0140_it.c
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
#define _MM32F0140_IT_C_

/* Files include */
#include "platform.h"
#include "i2c_master_interrupt.h"
#include "mm32f0140_it.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_Interrupt
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  This function handles NMI exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void NMI_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles Hard Fault exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/***********************************************************************************************************************
  * @brief  This function handles SVCall exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SVC_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles PendSVC exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PendSV_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles SysTick Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SysTick_Handler(void)
{
    if (0 != PLATFORM_DelayTick)
    {
        PLATFORM_DelayTick--;
    }
}

/***********************************************************************************************************************
  * @brief  This function handles EXTI0_1 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI0_1_IRQHandler(void)
{
    /* K1 */
    if (RESET != EXTI_GetITStatus(EXTI_Line1))
    {
        K1_PressFlag = 1;

        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/***********************************************************************************************************************
  * @brief  This function handles EXTI2_3 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI2_3_IRQHandler(void)
{
    /* K2 */
    if (RESET != EXTI_GetITStatus(EXTI_Line2))
    {
        K2_PressFlag = 1;

        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

/***********************************************************************************************************************
  * @brief  This function handles I2C1 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C1_IRQHandler(void)
{
    uint8_t Data = 0;

    if (RESET != I2C_GetITStatus(I2C1, I2C_IT_RX_FULL))
    {
        I2C_ClearITPendingBit(I2C1, I2C_IT_RX_FULL);

        Data = I2C_ReceiveData(I2C1);

        if (0 == I2C_RxStruct.CompleteFlag)
        {
            I2C_RxStruct.Buffer[I2C_RxStruct.CurrentCount++] = Data;

            if (I2C_RxStruct.CurrentCount == I2C_RxStruct.Length)
            {
                I2C_RxStruct.CompleteFlag = 1;

                I2C_ITConfig(I2C1, I2C_IT_RX_FULL, DISABLE);
            }
            else
            {
                I2C_ReadCmd(I2C1);
            }
        }
    }

    if (RESET != I2C_GetITStatus(I2C1, I2C_IT_TX_EMPTY))
    {
        I2C_ClearITPendingBit(I2C1, I2C_IT_TX_EMPTY);

        if (0 == I2C_TxStruct.CompleteFlag)
        {
            I2C_SendData(I2C1, I2C_TxStruct.Buffer[I2C_TxStruct.CurrentCount++]);

            if (I2C_TxStruct.CurrentCount == I2C_TxStruct.Length)
            {
                I2C_TxStruct.CompleteFlag = 1;

                I2C_ITConfig(I2C1, I2C_IT_TX_EMPTY, DISABLE);
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

