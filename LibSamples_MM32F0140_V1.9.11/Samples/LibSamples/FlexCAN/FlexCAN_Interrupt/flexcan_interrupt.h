/***********************************************************************************************************************
    @file    flexcan_interrupt.h
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
#ifndef _FLEXCAN_INTERRUPT_H_
#define _FLEXCAN_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "hal_conf.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/
#define MB_Index_0     0
#define MB_Index_1     1
#define MB_Index_2     2
#define MB_Index_3     3
#define MB_Index_4     4
#define MB_Index_5     5
#define MB_Index_6     6
#define MB_Index_7     7
#define MB_Index_8     8
#define MB_Index_9     9
#define MB_Index_10    10
#define MB_Index_11    11
#define MB_Index_12    12
#define MB_Index_13    13
#define MB_Index_14    14
#define MB_Index_15    15

#define TX_StandardFrame_ID    0x214
#define TX_ExtendFrame_ID      0x250
#define TX_RemoteFrame_ID      0x251

#define RX_StandardFrame_ID    0x111
#define RX_ExtendFrame_ID      0x333
#define RX_RemoteFrame_ID      0x555

#define MASK_EXT          0x1FFFFFFF
#define MASK_STD          0x7FF

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _FLEXCAN_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN flexcan_handle_t FlexCAN_Handle;

/* Exported functions *************************************************************************************************/
void FlexCAN_SendStandardFrameMessage(uint32_t ID, uint8_t *Buffer, uint8_t Length);
void FlexCAN_SendExtendFrameMessage(uint32_t ID, uint8_t *Buffer, uint8_t Length);
void FlexCAN_SendRemoteFrameMessage(uint32_t ID, uint8_t *Buffer, uint8_t Length);
void FlexCAN_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _FLEXCAN_INTERRUPT_H_ */

/********************************************** (C) Copyright MindMotion **********************************************/

