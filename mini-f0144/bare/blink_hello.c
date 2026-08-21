/***********************************************************************************************************************
  @file    blink_hello.c
  @brief   App: blink 4 LEDs + periodic console report of system clock and ADC internal channels
           (Vref converted to voltage, die temperature sensor converted to Celsius).
***********************************************************************************************************************/

#define _BLINK_HELLO_C_

#include <stdio.h>
#include "platform.h"
#include "blink_hello.h"

#define  AVG_SLOPE   5.313f        /* mV/C, from MM32F0140 datasheet */
#define  VREF_FACTORY_VDD  3.3f    /* factory calibration reference */

/* Calibration values in flash */
#define  VREF_CAL_ADDR   0x1FFFF7E0   /* internal Vref (1.2V) raw at 3.3V VDD */
#define  TEMP25_CAL_ADDR 0x1FFFF7F6   /* die temp sensor raw at 25C          */

/* Scheduler periods */
#define  BLINK_PERIOD_MS   1000
#define  REPORT_PERIOD_MS  5000
#define  TICK_MS           100

static uint32_t LoopCounter  = 0;
static uint32_t ElapsedMS    = 0;

/***********************************************************************************************************************
  @brief  Configure ADC1 for the internal channels:
          TempSensor (Die Temperature) + Vrefint (internal Voltage Reference)
***********************************************************************************************************************/
static void ADC_Configure(void)
{
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution         = ADC_Resolution_12b;
    ADC_InitStruct.ADC_PRESCARE           = ADC_PCLK2_PRESCARE_16;
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Scan;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC1_ExternalTrigConv_T1_CC1;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor,    0, ADC_Samctl_240_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_VoltReference, 0, ADC_Samctl_240_5);
    ADC_ChannelCmd(ADC1, ADC_Channel_TempSensor, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channel_VoltReference, ENABLE);

    ADC_TempSensorVrefintCmd(ENABLE);

    ADC_Cmd(ADC1, ENABLE);
}

/***********************************************************************************************************************
  @brief  One ADC conversion round of the internal channels.
***********************************************************************************************************************/
static void ADC_ReadInternal(uint16_t *vref_val, uint16_t *temp_val)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
    {
    }

    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    /* Internal voltage reference (Vrefint), channel 15 */
    *vref_val = ADC_GetChannelConvertedValue(ADC1, ADC_Channel_VoltReference);

    /* Internal die temperature sensor, channel 14 */
    *temp_val = ADC_GetChannelConvertedValue(ADC1, ADC_Channel_TempSensor);
}

void BlinkHello_Init(void)
{
    printf("\r\n");
    printf("\r\n[ADC] detecting internal channels ...");

    ADC_Configure();
}

/***********************************************************************************************************************
  @brief  Report system clock + internal ADC channels.
          Vref:   raw converted to internal reference voltage (V)
          Dies:   raw converted to die temperature (C) using factory calibration.
          VDDA is derived from the internal Vref calibration (12.7.7).
***********************************************************************************************************************/
static void BlinkHello_Report(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
    uint16_t vref_val = 0;
    uint16_t temp_val = 0;
    uint16_t vref_cal = *(volatile uint16_t *)VREF_CAL_ADDR      & 0x0FFF;
    uint16_t temp_cal = *(volatile uint16_t *)TEMP25_CAL_ADDR    & 0x0FFF;
    float    vdda     = 0.0f;
    float    vref_v   = 0.0f;
    float    temp_c   = 0.0f;

    ADC_ReadInternal(&vref_val, &temp_val);

    /* Internal reference voltage (calibrated, ~1.2V) */
    vref_v = (float)vref_cal * VREF_FACTORY_VDD / 4096.0f;

    /* Derive current VDDA: VDDA(V) = Vref_cal * 3.3 / ADC_ADDR(VSensor), 12.7.7 */
    if (vref_val > 0)
    {
        vdda = (float)vref_cal * VREF_FACTORY_VDD / (float)vref_val;
    }

    /* Die temperature: T = (Vsense - V25)/Avg_Slope + 25, 12.7.6
       Vsense(mV) = temp_raw * Vdda / 4096 * 1000,
       V25(mV)    = temp_cal * VDD(cal) / 4096 * 1000 */
    if ((temp_val > 0) && (temp_val < 0x0FFF) && (vdda > 0.0f))
    {
        temp_c = 25.0f + (((float)temp_val * vdda * 1000.0f) -
                          ((float)temp_cal * VREF_FACTORY_VDD * 1000.0f)) /
                         (4096.0f * AVG_SLOPE);
    }

    RCC_GetClocksFreq(&RCC_Clocks);

    printf("\r\n");
    printf("\r\n[%lu] SYSCLK=%luHz (%lu.%03luMHz)  HCLK=%luHz",
           (unsigned long)LoopCounter,
           (unsigned long)RCC_Clocks.SYSCLK_Frequency,
           (unsigned long)(RCC_Clocks.SYSCLK_Frequency / 1000000),
           (unsigned long)((RCC_Clocks.SYSCLK_Frequency / 1000) % 1000),
           (unsigned long)RCC_Clocks.HCLK_Frequency);

    printf("  VDDA=%.2fV", (double)vdda);

    if ((vref_val > 0) && (vref_val < 0x0FFF))
    {
        printf("  VREF[ch15]=%u -> %.2fV", (unsigned)vref_val, (double)vref_v);
    }
    else
    {
        printf("  VREF[ch15]=%u (NOT DETECTED)", (unsigned)vref_val);
    }

    if ((temp_val > 0) && (temp_val < 0x0FFF))
    {
        printf("  TEMP[ch14]=%u -> %.1fC", (unsigned)temp_val, (double)temp_c);
    }
    else
    {
        printf("  TEMP[ch14]=%u (NOT DETECTED)", (unsigned)temp_val);
    }
}

/***********************************************************************************************************************
  @brief  Run once per scheduler tick: toggle LEDs at BLINK_PERIOD_MS,
          report at REPORT_PERIOD_MS.
***********************************************************************************************************************/
void BlinkHello_Run(void)
{
    ElapsedMS += TICK_MS;

    if (ElapsedMS >= BLINK_PERIOD_MS)
    {
        ElapsedMS = 0;

        LoopCounter++;

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_LED_Toggle(LED2);
        PLATFORM_LED_Toggle(LED3);
        PLATFORM_LED_Toggle(LED4);

        if (0 == (LoopCounter % (REPORT_PERIOD_MS / BLINK_PERIOD_MS)))
        {
            BlinkHello_Report();
        }
    }

    PLATFORM_DelayMS(TICK_MS);
}

/********************************************** (C) Copyright MindMotion **********************************************/