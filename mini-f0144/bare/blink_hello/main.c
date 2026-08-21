/***********************************************************************************************************************
  @file    main.c
  @brief   Main entry.
***********************************************************************************************************************/

#define _MAIN_C_

#include "platform.h"
#include "blink_hello.h"
#include "main.h"

int main(void)
{
    PLATFORM_Init();

    PLATFORM_PrintInfo();

    BlinkHello_Init();

    while (1)
    {
        BlinkHello_Run();
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/