#define _MAIN_C_

#include "platform.h"
#include "i2c_ee_merged.h"
#include "main.h"

int main(void)
{
    PLATFORM_Init();

    PLATFORM_PrintInfo();

    I2C_EEMERGED_Init();

    I2C_EEMERGED_Run();

    while (1)
    {
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/