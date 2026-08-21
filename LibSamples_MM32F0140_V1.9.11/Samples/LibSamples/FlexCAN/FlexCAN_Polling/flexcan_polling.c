/***********************************************************************************************************************
    @file    flexcan_polling.c
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
#define _FLEXCAN_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flexcan_polling.h"

/**
  * @addtogroup MM32F0140_LibSamples
  * @{
  */

/**
  * @addtogroup FlexCAN
  * @{
  */

/**
  * @addtogroup FlexCAN_Polling
  * @{
  */


/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/
flexcan_frame_t FlexCAN_MB10_FrameStruct;
flexcan_frame_t FlexCAN_MB11_FrameStruct;
flexcan_frame_t FlexCAN_MB12_FrameStruct;
flexcan_frame_t FlexCAN_MB13_FrameStruct;

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FlexCAN_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_ClocksTypeDef RCC_Clocks;

    flexcan_config_t       FlexCAN_ConfigStruct;
    flexcan_rx_mb_config_t FlexCAN_RxMB_ConfigStruct;

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_CAN, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_3);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    FLEXCAN_GetDefaultConfig(&FlexCAN_ConfigStruct);
    FlexCAN_ConfigStruct.baudRate             = 250000; /* 250kbps */
    FlexCAN_ConfigStruct.clkSrc               = Enum_Flexcan_ClkSrc1;
    FlexCAN_ConfigStruct.enableLoopBack       = false;
    FlexCAN_ConfigStruct.disableSelfReception = true;
    FlexCAN_ConfigStruct.enableIndividMask    = true;

#if 1    /* Baudrate calculate by automatically */
    FLEXCAN_CalculateImprovedTimingValues(FlexCAN_ConfigStruct.baudRate, RCC_Clocks.PCLK1_Frequency, &FlexCAN_ConfigStruct.timingConfig);
#else  /* You can modify the parameters yourself */
    FlexCAN_ConfigStruct.timingConfig.preDivider = 23;
    FlexCAN_ConfigStruct.timingConfig.propSeg    = 2;
    FlexCAN_ConfigStruct.timingConfig.phaseSeg1  = 5;
    FlexCAN_ConfigStruct.timingConfig.phaseSeg2  = 1;
    FlexCAN_ConfigStruct.timingConfig.rJumpwidth = 1;
#endif

    FLEXCAN_Init(FLEXCAN1, &FlexCAN_ConfigStruct);

    FLEXCAN_TxMbConfig(FLEXCAN1, MB_Index_7, ENABLE);
    FLEXCAN_TxMbConfig(FLEXCAN1, MB_Index_8, ENABLE);
    FLEXCAN_TxMbConfig(FLEXCAN1, MB_Index_9, ENABLE);

    /* STD */
    FlexCAN_RxMB_ConfigStruct.id     = FLEXCAN_ID_STD(RX_StandardFrame_ID);
    FlexCAN_RxMB_ConfigStruct.format = Enum_Flexcan_FrameFormatStandard;
    FlexCAN_RxMB_ConfigStruct.type   = Enum_Flexcan_FrameTypeData;
    FLEXCAN_RxMbConfig(FLEXCAN1, MB_Index_10, &FlexCAN_RxMB_ConfigStruct, ENABLE);

    FLEXCAN_SetRxIndividualMask(FLEXCAN1, MB_Index_10, FLEXCAN_RX_MB_STD_MASK(MASK_STD, 0, 0));

    /* EXT */
    FlexCAN_RxMB_ConfigStruct.id     = FLEXCAN_ID_EXT(RX_ExtendFrame_ID);
    FlexCAN_RxMB_ConfigStruct.format = Enum_Flexcan_FrameFormatExtend;
    FlexCAN_RxMB_ConfigStruct.type   = Enum_Flexcan_FrameTypeData;
    FLEXCAN_RxMbConfig(FLEXCAN1, MB_Index_11, &FlexCAN_RxMB_ConfigStruct, ENABLE);

    FLEXCAN_SetRxIndividualMask(FLEXCAN1, MB_Index_11, FLEXCAN_RX_MB_EXT_MASK(MASK_EXT, 0, 1));

    /* RTR */
    FlexCAN_RxMB_ConfigStruct.id     = FLEXCAN_ID_STD(RX_RemoteFrame_ID);
    FlexCAN_RxMB_ConfigStruct.format = Enum_Flexcan_FrameFormatStandard;
    FlexCAN_RxMB_ConfigStruct.type   = Enum_Flexcan_FrameTypeRemote;
    FLEXCAN_RxMbConfig(FLEXCAN1, MB_Index_12, &FlexCAN_RxMB_ConfigStruct, ENABLE);

    FLEXCAN_SetRxIndividualMask(FLEXCAN1, MB_Index_12, FLEXCAN_RX_MB_STD_MASK(MASK_STD, 1, 0));
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FlexCAN_SendStandardFrameMessage(uint32_t ID, uint8_t *Buffer, uint8_t Length)
{
    flexcan_frame_t FlexCAN_FrameStruct;

    FlexCAN_FrameStruct.length = (uint8_t)Length;
    FlexCAN_FrameStruct.type   = (uint8_t)Enum_Flexcan_FrameTypeData;
    FlexCAN_FrameStruct.format = (uint8_t)Enum_Flexcan_FrameFormatStandard;
    FlexCAN_FrameStruct.id     = FLEXCAN_ID_STD(ID);

    FlexCAN_FrameStruct.dataByte0 = Buffer[0];
    FlexCAN_FrameStruct.dataByte1 = Buffer[1];
    FlexCAN_FrameStruct.dataByte2 = Buffer[2];
    FlexCAN_FrameStruct.dataByte3 = Buffer[3];
    FlexCAN_FrameStruct.dataByte4 = Buffer[4];
    FlexCAN_FrameStruct.dataByte5 = Buffer[5];
    FlexCAN_FrameStruct.dataByte6 = Buffer[6];
    FlexCAN_FrameStruct.dataByte7 = Buffer[7];

    FLEXCAN_TransferSendBlocking(FLEXCAN1, MB_Index_7, &FlexCAN_FrameStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FlexCAN_SendExtendFrameMessage(uint32_t ID, uint8_t *Buffer, uint8_t Length)
{
    flexcan_frame_t FlexCAN_FrameStruct;

    FlexCAN_FrameStruct.length = (uint8_t)Length;
    FlexCAN_FrameStruct.type   = (uint8_t)Enum_Flexcan_FrameTypeData;
    FlexCAN_FrameStruct.format = (uint8_t)Enum_Flexcan_FrameFormatExtend;
    FlexCAN_FrameStruct.id     = FLEXCAN_ID_EXT(ID);

    FlexCAN_FrameStruct.dataByte0 = Buffer[0];
    FlexCAN_FrameStruct.dataByte1 = Buffer[1];
    FlexCAN_FrameStruct.dataByte2 = Buffer[2];
    FlexCAN_FrameStruct.dataByte3 = Buffer[3];
    FlexCAN_FrameStruct.dataByte4 = Buffer[4];
    FlexCAN_FrameStruct.dataByte5 = Buffer[5];
    FlexCAN_FrameStruct.dataByte6 = Buffer[6];
    FlexCAN_FrameStruct.dataByte7 = Buffer[7];

    FLEXCAN_TransferSendBlocking(FLEXCAN1, MB_Index_8, &FlexCAN_FrameStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FlexCAN_SendRemoteFrameMessage(uint32_t ID, uint8_t *Buffer, uint8_t Length)
{
    flexcan_frame_t FlexCAN_FrameStruct;

    FlexCAN_FrameStruct.length = (uint8_t)Length;
    FlexCAN_FrameStruct.type   = (uint8_t)Enum_Flexcan_FrameTypeRemote;
    FlexCAN_FrameStruct.format = (uint8_t)Enum_Flexcan_FrameFormatStandard;
    FlexCAN_FrameStruct.id     = FLEXCAN_ID_STD(ID);

    FlexCAN_FrameStruct.dataByte0 = Buffer[0];
    FlexCAN_FrameStruct.dataByte1 = Buffer[1];
    FlexCAN_FrameStruct.dataByte2 = Buffer[2];
    FlexCAN_FrameStruct.dataByte3 = Buffer[3];
    FlexCAN_FrameStruct.dataByte4 = Buffer[4];
    FlexCAN_FrameStruct.dataByte5 = Buffer[5];
    FlexCAN_FrameStruct.dataByte6 = Buffer[6];
    FlexCAN_FrameStruct.dataByte7 = Buffer[7];

    FLEXCAN_TransferSendBlocking(FLEXCAN1, MB_Index_9, &FlexCAN_FrameStruct);
}

void FlexCAN_MB10_RxHandler(void)
{
    uint32_t ID = 0;
    uint8_t  Buffer[8], Length = 0;

    ID     = FlexCAN_MB10_FrameStruct.id;
    Length = FlexCAN_MB10_FrameStruct.length;

    Buffer[0] = FlexCAN_MB10_FrameStruct.dataByte0;
    Buffer[1] = FlexCAN_MB10_FrameStruct.dataByte1;
    Buffer[2] = FlexCAN_MB10_FrameStruct.dataByte2;
    Buffer[3] = FlexCAN_MB10_FrameStruct.dataByte3;
    Buffer[4] = FlexCAN_MB10_FrameStruct.dataByte4;
    Buffer[5] = FlexCAN_MB10_FrameStruct.dataByte5;
    Buffer[6] = FlexCAN_MB10_FrameStruct.dataByte6;
    Buffer[7] = FlexCAN_MB10_FrameStruct.dataByte7;

    if (FlexCAN_MB10_FrameStruct.format == (uint8_t)Enum_Flexcan_FrameFormatStandard)
    {
        if (FlexCAN_MB10_FrameStruct.type == (uint8_t)Enum_Flexcan_FrameTypeData)
        {
            FlexCAN_SendStandardFrameMessage((ID >> FLEXCAN_ID_STD_Pos), Buffer, Length);
        }
        else
        {
            FlexCAN_SendRemoteFrameMessage((ID >> FLEXCAN_ID_STD_Pos), Buffer, Length);
        }
    }
    else if (FlexCAN_MB10_FrameStruct.format == (uint8_t)Enum_Flexcan_FrameFormatExtend)
    {
        FlexCAN_SendExtendFrameMessage((ID >> FLEXCAN_ID_EXT_Pos), Buffer, Length);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FlexCAN_Polling_Sample(void)
{
    uint8_t Buffer[8] = {0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    printf("\r\nTest %s", __FUNCTION__);

    FlexCAN_Configure();

    FlexCAN_SendStandardFrameMessage(TX_StandardFrame_ID, Buffer, sizeof(Buffer));
    FlexCAN_SendExtendFrameMessage(TX_ExtendFrame_ID, Buffer, sizeof(Buffer));
    FlexCAN_SendRemoteFrameMessage(TX_RemoteFrame_ID, Buffer, sizeof(Buffer));

    while (1)
    {
        /* MB10 Read Loop */
        if (0 != FLEXCAN_GetMbStatusFlags(FLEXCAN1, 0x01U << MB_Index_10))
        {
            FLEXCAN_ClearMbStatusFlags(FLEXCAN1, 0x01U << MB_Index_10);

            FLEXCAN_ReadRxMb(FLEXCAN1, MB_Index_10, &FlexCAN_MB10_FrameStruct);
            FlexCAN_MB10_RxHandler();
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
