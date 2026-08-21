#define _PWM_TEST_C_

#include <stdio.h>
#include "platform.h"
#include "pwm_test.h"

/* PWM fundamental frequency (most audible for the passive buzzer) */
#define PWM_FREQ_HZ         2700
#define PWM_DUTY_STEP       5      /* percent */

static uint32_t TIM1_TimerPeriod;

/***********************************************************************************************************************
  @brief  Configure TIM1_CH1 PWM output on PA8 (AF2) at PWM_FREQ_HZ.
***********************************************************************************************************************/
static void TIM1_PWM_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    RCC_ClocksTypeDef       RCC_Clocks;
    uint32_t                TIM_ClockFrequency = 0;
    uint32_t                HPRE = 0, PPRE2 = 0;

    HPRE  = READ_BIT(RCC->CFGR, RCC_CFGR_HPRE)  >> RCC_CFGR_HPRE_Pos;
    PPRE2 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

    RCC_GetClocksFreq(&RCC_Clocks);

    if (HPRE < 8)
    {
        if (PPRE2 < 4)
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency;
        }
        else
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 2;
        }
    }
    else
    {
        if (PPRE2 < 4)
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 2;
        }
        else
        {
            TIM_ClockFrequency = RCC_Clocks.PCLK2_Frequency * 4;
        }
    }

    TIM1_TimerPeriod = (TIM_ClockFrequency / PWM_FREQ_HZ) - 1;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = 0;
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = TIM1_TimerPeriod;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse       = 0;
    TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);    /* TIM1_CH1 on PA8 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/***********************************************************************************************************************
  @brief  Set PWM duty in percent, clamp to [0, 99]. Then start the output.
***********************************************************************************************************************/
static void PWM_SetDutyAndStart(uint32_t Percent)
{
    uint32_t pulse;

    if (Percent >= 100)
    {
        Percent = 99;
    }

    pulse = ((uint32_t)Percent * (TIM1_TimerPeriod + 1)) / 100;

    TIM_SetCompare1(TIM1, pulse);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/***********************************************************************************************************************
  @brief  Stop PWM output (keep configuration).
***********************************************************************************************************************/
static void PWM_Stop(void)
{
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    TIM_Cmd(TIM1, DISABLE);
}

/***********************************************************************************************************************
  @brief  Update the LED pattern from the current duty rate (low-active LEDs).
          [0,20) all off; [20,40) LD1; [40,60) LD1+LD2; [60,80) +LD3; [80,100) all on
***********************************************************************************************************************/
static void LED_UpdateDuty(uint32_t Duty)
{
    FunctionalState on = ENABLE;

    if (Duty >= 80)
    {
        PLATFORM_LED_Enable(LED1, on);
        PLATFORM_LED_Enable(LED2, on);
        PLATFORM_LED_Enable(LED3, on);
        PLATFORM_LED_Enable(LED4, on);
    }
    else if (Duty >= 60)
    {
        PLATFORM_LED_Enable(LED1, on);
        PLATFORM_LED_Enable(LED2, on);
        PLATFORM_LED_Enable(LED3, on);
        PLATFORM_LED_Enable(LED4, DISABLE);
    }
    else if (Duty >= 40)
    {
        PLATFORM_LED_Enable(LED1, on);
        PLATFORM_LED_Enable(LED2, on);
        PLATFORM_LED_Enable(LED3, DISABLE);
        PLATFORM_LED_Enable(LED4, DISABLE);
    }
    else if (Duty >= 20)
    {
        PLATFORM_LED_Enable(LED1, on);
        PLATFORM_LED_Enable(LED2, DISABLE);
        PLATFORM_LED_Enable(LED3, DISABLE);
        PLATFORM_LED_Enable(LED4, DISABLE);
    }
    else
    {
        PLATFORM_LED_Enable(LED1, DISABLE);
        PLATFORM_LED_Enable(LED2, DISABLE);
        PLATFORM_LED_Enable(LED3, DISABLE);
        PLATFORM_LED_Enable(LED4, DISABLE);
    }
}

/***********************************************************************************************************************
  @brief  Test entry: initialize TIM1 PWM, then sweep duty in the main loop.
***********************************************************************************************************************/
void PWMTEST_Init(void)
{
    TIM1_PWM_Configure();
}

void PWMTEST_Run(void)
{
    uint32_t Duty = 0;

    while (1)
    {
        uint32_t OldDuty = Duty;

        /* Set PWM duty rate and start PWM output */
        PWM_SetDutyAndStart(Duty);

        /* Increase duty by 5%, wrap into [0, 100) */
        Duty += PWM_DUTY_STEP;
        if (Duty >= 100)
        {
            Duty = 0;
        }

        /* Change LED pattern and report duty over UART */
        LED_UpdateDuty(Duty);
        printf("\r\nPWM duty = %u %%", (unsigned)Duty);

        /* When duty flips (new rate < old rate), delay for 5 s more, then continue the loop */
        if (Duty < OldDuty)
        {
            PLATFORM_DelayMS(5000);
        }
        else
        {
            PLATFORM_DelayMS(500);
        }

        /* Stop PWM output */
        PWM_Stop();

        PLATFORM_DelayMS(250);
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/