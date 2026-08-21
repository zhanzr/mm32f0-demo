/***********************************************************************************************************************
  @file    platform.c
  @brief   Platform layer for MM32F0140:
           - Console via UART2 on PA2 (Txd) / PA3 (Rxd), printf redirected with _write
           - SysTick based millisecond delay
           - LED helpers (low active: ENABLE == pin low)
           - Boot info + system frequency print
***********************************************************************************************************************/

#define _PLATFORM_C_

#include <stdio.h>
#include "platform.h"

#define CONSOLE_UART    UART2
#define CONSOLE_BAUDRATE 115200

static void CONSOLE_PutChar(uint8_t ch)
{
    UART_SendData(CONSOLE_UART, ch);

    while (RESET == UART_GetFlagStatus(CONSOLE_UART, UART_FLAG_TXEPT))
    {
    }
}

/***********************************************************************************************************************
  SysTick delay
***********************************************************************************************************************/
void PLATFORM_InitDelay(void)
{
    if (SysTick_Config(RCC_GetHCLKFreq() / 1000))
    {
        while (1)
        {
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void PLATFORM_DelayMS(uint32_t Millisecond)
{
    PLATFORM_DelayTick = Millisecond;

    while (0 != PLATFORM_DelayTick)
    {
    }
}

/***********************************************************************************************************************
  Console: UART2 TX on PA2 (Txd), 115200 (MCU -> PC only)
***********************************************************************************************************************/
void PLATFORM_InitConsole(uint32_t Baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    UART_InitTypeDef UART_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_UART2, ENABLE);

    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.BaudRate      = Baudrate;
    UART_InitStruct.WordLength    = UART_WordLength_8b;
    UART_InitStruct.StopBits      = UART_StopBits_1;
    UART_InitStruct.Parity        = UART_Parity_No;
    UART_InitStruct.HWFlowControl = UART_HWFlowControl_None;
    UART_InitStruct.Mode          = UART_Mode_Tx;
    UART_Init(UART2, &UART_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    /* PA2 = TXD, mapped to UART2 via AF1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    UART_Cmd(CONSOLE_UART, ENABLE);
}

#if defined (__GNUC__)
/***********************************************************************************************************************
  printf retarget for GCC/newlib
***********************************************************************************************************************/
int _write(int file, char *ptr, int len)
{
    int i;

    (void)file;

    for (i = 0; i < len; i++)
    {
        CONSOLE_PutChar((uint8_t)ptr[i]);
    }

    return len;
}
#elif defined (__ARMCC_VERSION)
int fputc(int ch, FILE *f)
{
    (void)f;
    CONSOLE_PutChar((uint8_t)ch);
    return (ch);
}
#endif

/***********************************************************************************************************************
  LED helpers - mini-F0144 LEDs: PA15, PB3, PB4, PB5 (low active: LED on == pin low)
***********************************************************************************************************************/
void PLATFORM_InitLED(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* all LEDs off (low active => high pin) */
    PLATFORM_LED_Enable(LED1, DISABLE);
    PLATFORM_LED_Enable(LED2, DISABLE);
    PLATFORM_LED_Enable(LED3, DISABLE);
    PLATFORM_LED_Enable(LED4, DISABLE);
}

void PLATFORM_LED_Enable(LEDn_TypeDef LEDn, FunctionalState State)
{
    switch (LEDn)
    {
        case LED1:
            GPIO_WriteBit(GPIOA, GPIO_Pin_15, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        case LED2:
            GPIO_WriteBit(GPIOB, GPIO_Pin_3, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        case LED3:
            GPIO_WriteBit(GPIOB, GPIO_Pin_4, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        case LED4:
            GPIO_WriteBit(GPIOB, GPIO_Pin_5, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        default:
            break;
    }
}

void PLATFORM_LED_Toggle(LEDn_TypeDef LEDn)
{
    switch (LEDn)
    {
        case LED1:
            GPIO_WriteBit(GPIOA, GPIO_Pin_15, GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15) ? Bit_RESET : Bit_SET);
            break;

        case LED2:
            GPIO_WriteBit(GPIOB, GPIO_Pin_3, GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_3) ? Bit_RESET : Bit_SET);
            break;

        case LED3:
            GPIO_WriteBit(GPIOB, GPIO_Pin_4, GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_4) ? Bit_RESET : Bit_SET);
            break;

        case LED4:
            GPIO_WriteBit(GPIOB, GPIO_Pin_5, GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5) ? Bit_RESET : Bit_SET);
            break;

        default:
            break;
    }
}

/***********************************************************************************************************************
  Boot info + current system frequency
***********************************************************************************************************************/
void PLATFORM_PrintInfo(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    printf("\r\n");
    printf("\r\n================================");
    printf("\r\nBOARD : Mini-F0144");
    printf("\r\nMCU   : MM32F0144C6P");
    printf("\r\n--------------------------------");

    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
        case 0x00:
            printf("\r\nClock source : HSI");
            break;

        case 0x04:
            printf("\r\nClock source : HSE");
            break;

        case 0x08:
            if (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC)
            {
                printf("\r\nClock source : PLL (clocked by HSE)");
            }
            else
            {
                printf("\r\nClock source : PLL (clocked by HSI)");
            }
            break;

        case 0x0C:
            printf("\r\nClock source : LSI");
            break;

        default:
            break;
    }

    RCC_GetClocksFreq(&RCC_Clocks);

    printf("\r\nSYSCLK Frequency : %u Hz (%u.%03u MHz)",
           (unsigned)RCC_Clocks.SYSCLK_Frequency,
           (unsigned)(RCC_Clocks.SYSCLK_Frequency / 1000000),
           (unsigned)((RCC_Clocks.SYSCLK_Frequency / 1000) % 1000));
    printf("\r\nHCLK   Frequency : %u Hz", (unsigned)RCC_Clocks.HCLK_Frequency);
    printf("\r\nPCLK1  Frequency : %u Hz", (unsigned)RCC_Clocks.PCLK1_Frequency);
    printf("\r\nPCLK2  Frequency : %u Hz", (unsigned)RCC_Clocks.PCLK2_Frequency);
    printf("\r\n================================");
    printf("\r\n");
}

/***********************************************************************************************************************
  Platform init
***********************************************************************************************************************/
void PLATFORM_Init(void)
{
    PLATFORM_InitDelay();

    PLATFORM_InitConsole(CONSOLE_BAUDRATE);

    PLATFORM_InitLED();
}