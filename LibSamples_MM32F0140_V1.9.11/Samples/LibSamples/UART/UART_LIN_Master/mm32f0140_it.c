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
#include "uart_lin_master.h"
#include "mm32f0140_it.h"

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
uint8_t UART2_RxBuffer[100];
uint8_t UART2_RxLength = 0;

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
  * @brief  This function handles UART2 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void UART2_IRQHandler(void)
{
    uint8_t i = 0;

    if(RESET != (UART2->ISR & UART_ISR_TXBRK))
    {
        UART2_RxLength = 0;

        UART2->ICR  = UART_ICR_TXBRK;
        UART2->IER |= UART_IER_RXIDLE;

        LIN_MASTER_TXBRK_InterruptFlag = 1;
    }

    if(RESET != UART_GetITStatus(UART2, UART_IT_RXIEN))
    {
        UART2_RxBuffer[UART2_RxLength] = UART_ReceiveData(UART2);

        UART2_RxLength = (UART2_RxLength + 1) % 100;

        UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
    }

    if (RESET != (UART2->ISR & UART_ISR_RXIDLE))
    {
        for(i= 0; i < UART2_RxLength; i++)
        {
            LIN_MASTER_RxBuffer[i] = UART2_RxBuffer[i];
        }

        LIN_MASTER_RxLength = UART2_RxLength;
        LIN_MASTER_RxFinish = 1;

        UART2->ICR  =  UART_ICR_RXIDLE;
        UART2->IER &= ~UART_IER_RXIDLE;
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

