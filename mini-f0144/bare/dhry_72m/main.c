/* Define to prevent recursive inclusion */
#define _MAIN_C_

#include <stdio.h>
#include "platform.h"
#include "custom_def.h"
#include "dhry.h"
#include "main.h"

extern volatile uint32_t SystemCoreClock;

int main(void)
{
    PLATFORM_Init();

    PLATFORM_PrintInfo();

    while (1)
    {
        dhry_main(SystemCoreClock);

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_LED_Toggle(LED2);
        PLATFORM_LED_Toggle(LED3);
        PLATFORM_LED_Toggle(LED4);

        HAL_Delay(1000);
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/