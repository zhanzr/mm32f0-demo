#define _MAIN_C_

#include "platform.h"
#include "pwm_test.h"
#include "main.h"

int main(void)
{
    PLATFORM_Init();

    PLATFORM_PrintInfo();

    PWMTEST_Init();

    PWMTEST_Run();

    while (1)
    {
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/