/*
 * Startup file for MM32F0140 (Cortex-M0) - GCC toolchain
 * Based on the vendor KEIL startup, adapted to GNU assembler syntax.
 */

    .syntax unified
    .cpu cortex-m0
    .thumb

    .section .isr_vector, "a", %progbits
    .globl __Vectors
    .globl __Vectors_End
    .globl __Vectors_Size
    .align 2

__Vectors:
    .word   __initial_sp                /* Top of Stack */
    .word   Reset_Handler               /* Reset Handler */
    .word   NMI_Handler                 /* -14 NMI Handler */
    .word   HardFault_Handler           /* -13 Hard Fault Handler */
    .word   0                           /* -12 Reserved */
    .word   0                           /* -11 Reserved */
    .word   0                           /* -10 Reserved */
    .word   0                           /*  -9 Reserved */
    .word   0                           /*  -8 Reserved */
    .word   0                           /*  -7 Reserved */
    .word   0                           /*  -6 Reserved */
    .word   SVC_Handler                 /*  -5 SVCall Handler */
    .word   0                           /*  -4 Reserved */
    .word   0                           /*  -3 Reserved */
    .word   PendSV_Handler              /*  -2 PendSV Handler */
    .word   SysTick_Handler             /*  -1 SysTick Handler */

    /* External Interrupts */
    .word   WWDG_IWDG_IRQHandler        /*   0 Window Watchdog */
    .word   PVD_VDT_IRQHandler          /*   1 PVD through EXTI Line detect */
    .word   0                           /*   2 Reserved */
    .word   FLASH_IRQHandler            /*   3 FLASH */
    .word   RCC_IRQHandler              /*   4 RCC */
    .word   EXTI0_1_IRQHandler          /*   5 EXTI Line 0 and 1 */
    .word   EXTI2_3_IRQHandler          /*   6 EXTI Line 2 and 3 */
    .word   EXTI4_15_IRQHandler         /*   7 EXTI Line 4 to 15 */
    .word   HWDIV_IRQHandler            /*   8 HWDIV */
    .word   DMA1_Channel1_IRQHandler    /*   9 DMA1 Channel 1 */
    .word   DMA1_Channel2_3_IRQHandler  /*  10 DMA1 Channel 2 and 3 */
    .word   DMA1_Channel4_5_IRQHandler  /*  11 DMA1 Channel 4 and 5 */
    .word   ADC_COMP_IRQHandler         /*  12 ADC & COMP */
    .word   TIM1_BRK_UP_TRG_COM_IRQHandler /* 13 TIM1 */
    .word   TIM1_CC_IRQHandler          /*  14 TIM1 Capture Compare */
    .word   TIM2_IRQHandler             /*  15 TIM2 */
    .word   TIM3_IRQHandler             /*  16 TIM3 */
    .word   0                           /*  17 Reserved */
    .word   0                           /*  18 Reserved */
    .word   TIM14_IRQHandler            /*  19 TIM14 */
    .word   0                           /*  20 Reserved */
    .word   TIM16_IRQHandler            /*  21 TIM16 */
    .word   TIM17_IRQHandler            /*  22 TIM17 */
    .word   I2C1_IRQHandler             /*  23 I2C1 */
    .word   0                           /*  24 Reserved */
    .word   SPI1_IRQHandler             /*  25 SPI1 */
    .word   SPI2_IRQHandler             /*  26 SPI2 */
    .word   UART1_IRQHandler            /*  27 UART1 */
    .word   UART2_IRQHandler            /*  28 UART2 */
    .word   UART3_IRQHandler            /*  29 UART3 */
    .word   FLEX_CAN_IRQHandler         /*  30 FLEX_CAN */
    .word   0                           /*  31 Reserved */
__Vectors_End:
__Vectors_Size = __Vectors_End - __Vectors

    .section .text, "ax", %progbits

    .thumb_func
    .globl Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    ldr   r0, =__initial_sp
    msr   msp, r0

    /* Copy .data from flash to RAM */
    ldr   r1, =_sdata
    ldr   r2, =_edata
    ldr   r3, =_sidata
.loop_data:
    cmp   r1, r2
    bge   .zero_bss
    ldr   r0, [r3]
    str   r0, [r1]
    adds  r1, r1, #4
    adds  r3, r3, #4
    b     .loop_data

.zero_bss:
    /* Zero .bss */
    ldr   r1, =_sbss
    ldr   r2, =_ebss
    movs  r0, #0
.loop_bss:
    cmp   r1, r2
    bge   .end_bss
    str   r0, [r1]
    adds  r1, r1, #4
    b     .loop_bss

.end_bss:
    /* Call SystemInit */
    bl    SystemInit

    /* Call the application's entry point */
    bl    main
    b     .

    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    b     .

    .thumb_func
    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    b     .

    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    b     .

    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    b     .

    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    b     .

    .thumb_func
    .weak WWDG_IWDG_IRQHandler
    .type WWDG_IWDG_IRQHandler, %function
WWDG_IWDG_IRQHandler:
    b     .

    .thumb_func
    .weak PVD_VDT_IRQHandler
    .type PVD_VDT_IRQHandler, %function
PVD_VDT_IRQHandler:
    b     .

    .thumb_func
    .weak FLASH_IRQHandler
    .type FLASH_IRQHandler, %function
FLASH_IRQHandler:
    b     .

    .thumb_func
    .weak RCC_IRQHandler
    .type RCC_IRQHandler, %function
RCC_IRQHandler:
    b     .

    .thumb_func
    .weak EXTI0_1_IRQHandler
    .type EXTI0_1_IRQHandler, %function
EXTI0_1_IRQHandler:
    b     .

    .thumb_func
    .weak EXTI2_3_IRQHandler
    .type EXTI2_3_IRQHandler, %function
EXTI2_3_IRQHandler:
    b     .

    .thumb_func
    .weak EXTI4_15_IRQHandler
    .type EXTI4_15_IRQHandler, %function
EXTI4_15_IRQHandler:
    b     .

    .thumb_func
    .weak HWDIV_IRQHandler
    .type HWDIV_IRQHandler, %function
HWDIV_IRQHandler:
    b     .

    .thumb_func
    .weak DMA1_Channel1_IRQHandler
    .type DMA1_Channel1_IRQHandler, %function
DMA1_Channel1_IRQHandler:
    b     .

    .thumb_func
    .weak DMA1_Channel2_3_IRQHandler
    .type DMA1_Channel2_3_IRQHandler, %function
DMA1_Channel2_3_IRQHandler:
    b     .

    .thumb_func
    .weak DMA1_Channel4_5_IRQHandler
    .type DMA1_Channel4_5_IRQHandler, %function
DMA1_Channel4_5_IRQHandler:
    b     .

    .thumb_func
    .weak ADC_COMP_IRQHandler
    .type ADC_COMP_IRQHandler, %function
ADC_COMP_IRQHandler:
    b     .

    .thumb_func
    .weak TIM1_BRK_UP_TRG_COM_IRQHandler
    .type TIM1_BRK_UP_TRG_COM_IRQHandler, %function
TIM1_BRK_UP_TRG_COM_IRQHandler:
    b     .

    .thumb_func
    .weak TIM1_CC_IRQHandler
    .type TIM1_CC_IRQHandler, %function
TIM1_CC_IRQHandler:
    b     .

    .thumb_func
    .weak TIM2_IRQHandler
    .type TIM2_IRQHandler, %function
TIM2_IRQHandler:
    b     .

    .thumb_func
    .weak TIM3_IRQHandler
    .type TIM3_IRQHandler, %function
TIM3_IRQHandler:
    b     .

    .thumb_func
    .weak TIM14_IRQHandler
    .type TIM14_IRQHandler, %function
TIM14_IRQHandler:
    b     .

    .thumb_func
    .weak TIM16_IRQHandler
    .type TIM16_IRQHandler, %function
TIM16_IRQHandler:
    b     .

    .thumb_func
    .weak TIM17_IRQHandler
    .type TIM17_IRQHandler, %function
TIM17_IRQHandler:
    b     .

    .thumb_func
    .weak I2C1_IRQHandler
    .type I2C1_IRQHandler, %function
I2C1_IRQHandler:
    b     .

    .thumb_func
    .weak SPI1_IRQHandler
    .type SPI1_IRQHandler, %function
SPI1_IRQHandler:
    b     .

    .thumb_func
    .weak SPI2_IRQHandler
    .type SPI2_IRQHandler, %function
SPI2_IRQHandler:
    b     .

    .thumb_func
    .weak UART1_IRQHandler
    .type UART1_IRQHandler, %function
UART1_IRQHandler:
    b     .

    .thumb_func
    .weak UART2_IRQHandler
    .type UART2_IRQHandler, %function
UART2_IRQHandler:
    b     .

    .thumb_func
    .weak UART3_IRQHandler
    .type UART3_IRQHandler, %function
UART3_IRQHandler:
    b     .

    .thumb_func
    .weak FLEX_CAN_IRQHandler
    .type FLEX_CAN_IRQHandler, %function
FLEX_CAN_IRQHandler:
    b     .

    .end
