#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_conf.h"

typedef enum
{
    LED1,
    LED2,
    LED3,
    LED4
} LEDn_TypeDef;

void PLATFORM_InitDelay(void);
void PLATFORM_InitConsole(uint32_t Baudrate);
void PLATFORM_InitLED(void);
void PLATFORM_LED_Enable(LEDn_TypeDef LEDn, FunctionalState State);
void PLATFORM_LED_Toggle(LEDn_TypeDef LEDn);
void PLATFORM_PrintInfo(void);
void PLATFORM_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORM_H_ */