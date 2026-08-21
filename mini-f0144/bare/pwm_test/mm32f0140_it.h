#ifndef _MM32F0140_IT_H_
#define _MM32F0140_IT_H_

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* _MM32F0140_IT_H_ */