/***********************************************************************************************************************
    @file    uart_simulatewithgpio.h
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
#ifndef _UART_SIMULATEWITHGPIO_H_
#define _UART_SIMULATEWITHGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "hal_conf.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/
#define sUART_BAUDRATE_DELTA    (2)
#define sUART_BAUTRATE_1200     (833)
#define sUART_BAUTRATE_2400     (417)
#define sUART_BAUTRATE_4800     (208)
#define sUART_BAUTRATE_9600     (104)
#define sUART_BAUTRATE_14400    (69)
#define sUART_BAUTRATE_19200    (52)
#define sUART_BAUTRATE_DELAY    (sUART_BAUTRATE_9600 - sUART_BAUDRATE_DELTA)

#define sUART_RX_BIT_START      0
#define sUART_RX_BIT_0          1
#define sUART_RX_BIT_1          2
#define sUART_RX_BIT_2          3
#define sUART_RX_BIT_3          4
#define sUART_RX_BIT_4          5
#define sUART_RX_BIT_5          6
#define sUART_RX_BIT_6          7
#define sUART_RX_BIT_7          8
#define sUART_RX_BIT_STOP       9

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _UART_SIMULATEWITHGPIO_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint8_t SimulateUART_RxData;
EXTERN volatile uint8_t SimulateUART_RxFlag;
EXTERN volatile uint8_t SimulateUART_RxStep;

/* Exported functions *************************************************************************************************/
void SimulateUART_DelayUS(uint32_t Tick);
void SimulateUART_SendData(uint8_t Data);
void UART_SimulateWithGPIO_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _UART_SIMULATEWITHGPIO_H_ */

/********************************************** (C) Copyright MindMotion **********************************************/

