#define _MAIN_C_

#include "platform.h"
#include "spi_flash_test.h"
#include "main.h"

int main(void)
{
    PLATFORM_Init();

    PLATFORM_PrintInfo();

    SPI_FLASHTEST_Init();

    SPI_FLASHTEST_Run();

    while (1)
    {
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/