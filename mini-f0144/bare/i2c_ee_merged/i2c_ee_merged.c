#define _I2C_EE_MERGED_C_

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "i2c_ee_merged.h"

/* 24C01/02 family: 256 bytes, 8-byte pages */
#define EEPROM_7BIT_ADDR     0x50
#define EEPROM_CAPACITY      256
#define EEPROM_PAGE_SIZE     8
#define I2C_TIMEOUT_MAX      200000

/***********************************************************************************************************************
  Shared result reporting
***********************************************************************************************************************/
static void ReportSpeed(const char *Label, uint32_t tErase, uint32_t tProg, uint32_t tRead, uint8_t ok)
{
    printf("\r\n--- %s (24C02: 256 B) ---", Label);
    printf("\r\nErase  time: %lu ms    speed: %lu B/s",
           (unsigned long)tErase,
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tErase ? tErase : 1)));
    printf("\r\nProgram time: %lu ms    speed: %lu B/s",
           (unsigned long)tProg,
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tProg ? tProg : 1)));
    printf("\r\nRead   time: %lu ms    speed: %lu B/s",
           (unsigned long)tRead,
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tRead ? tRead : 1)));
    printf("\r\nReadback match: %s", ok ? "PASS" : "FAIL");
}

/*==========================================================================
   Part 1: Hardware I2C (official HAL I2C1 master on PB6/PB7, 100 kHz)
   ==========================================================================*/
static uint8_t RdHw[EEPROM_CAPACITY];

static void HW_I2C_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef  I2C_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_I2C1, ENABLE);

    I2C_DeInit(I2C1);

    I2C_StructInit(&I2C_InitStruct);
    I2C_InitStruct.I2C_Mode       = I2C_Mode_MASTER;
    I2C_InitStruct.I2C_Speed      = I2C_Speed_STANDARD;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Send7bitAddress(I2C1, EEPROM_7BIT_ADDR << 1, 0);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);   /* SCL */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);   /* SDA */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

static uint8_t HW_Tx(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i;
    uint32_t tout;

    for (i = 0; i < Length; i++)
    {
        I2C_SendData(I2C1, Buffer[i]);

        tout = I2C_TIMEOUT_MAX;
        while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE))
        {
            if (--tout == 0)
            {
                return 0;   /* TX timeout (no ACK / bus stuck) */
            }
        }
    }

    return 1;
}

static uint8_t HW_Rx(uint8_t *Buffer, uint16_t Length)
{
    uint8_t  i;
    uint32_t tout;

    for (i = 0; i < Length; i++)
    {
        I2C_ReadCmd(I2C1);

        tout = I2C_TIMEOUT_MAX;
        while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_RFNE))
        {
            if (--tout == 0)
            {
                return 0;   /* RX timeout */
            }
        }

        Buffer[i] = I2C_ReceiveData(I2C1);
    }

    return 1;
}

/***********************************************************************************************************************
  Probe the EEPROM with a single-byte write; returns 1 if the slave ACKed.
***********************************************************************************************************************/
static uint8_t HW_ProbeAck(void)
{
    uint8_t byte = 0x00;
    uint32_t tout;

    HW_I2C_Configure();

    I2C1->ICR = 0xFF;

    /* send the slave address (write) + one zero byte */
    if (!HW_Tx(&byte, 1))
    {
        return 0;
    }

    /* let the core settle, then check for TX abort (no ACK) */
    tout = I2C_TIMEOUT_MAX;
    while (0 == (I2C1->IC_RAW_INTR_STAT & I2C_RAWISR_TX_ABRT))
    {
        if (--tout == 0)
        {
            break;
        }
    }

    I2C_GenerateSTOP(I2C1, ENABLE);

    return (0 == (I2C1->IC_RAW_INTR_STAT & I2C_RAWISR_TX_ABRT));
}

static void HW_WritePage(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    uint32_t tout;

    HW_Tx(&Address, 0x01);
    HW_Tx(Buffer, Length);

    tout = I2C_TIMEOUT_MAX;
    while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE))
    {
        if (--tout == 0)
        {
            break;
        }
    }

    I2C_GenerateSTOP(I2C1, ENABLE);

    tout = I2C_TIMEOUT_MAX;
    while (RESET == I2C_GetITStatus(I2C1, I2C_IT_STOP_DET))
    {
        if (--tout == 0)
        {
            break;
        }
    }
}

static void HW_WriteData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    uint8_t Start = 0;
    uint8_t StartCount = 0, PageNumber = 0, FinalCount = 0;

    if ((Address % EEPROM_PAGE_SIZE) == 0)
    {
        StartCount = 0;
        PageNumber = Length / EEPROM_PAGE_SIZE;
        FinalCount = Length % EEPROM_PAGE_SIZE;
    }
    else
    {
        Start = Address % EEPROM_PAGE_SIZE;

        if (((Start + Length) / EEPROM_PAGE_SIZE) == 0)
        {
            StartCount = Length;
            PageNumber = 0;
            FinalCount = 0;
        }
        else
        {
            StartCount = EEPROM_PAGE_SIZE - Start;
            PageNumber = (Length - StartCount) / EEPROM_PAGE_SIZE;
            FinalCount = (Length - StartCount) % EEPROM_PAGE_SIZE;
        }
    }

    if (StartCount)
    {
        HW_WritePage(Address, Buffer, StartCount);
        Address += StartCount;
        Buffer  += StartCount;
        PLATFORM_DelayMS(50);
    }

    while (PageNumber--)
    {
        HW_WritePage(Address, Buffer, EEPROM_PAGE_SIZE);
        Address += EEPROM_PAGE_SIZE;
        Buffer  += EEPROM_PAGE_SIZE;
        PLATFORM_DelayMS(50);
    }

    if (FinalCount)
    {
        HW_WritePage(Address, Buffer, FinalCount);
    }
}

static void HW_ReadData(uint8_t Address, uint8_t *Buffer, uint16_t Length)
{
    uint16_t i;
    uint32_t tout;

    HW_Tx(&Address, 0x01);
    HW_Rx(Buffer, Length);

    I2C_GenerateSTOP(I2C1, ENABLE);

    tout = I2C_TIMEOUT_MAX;
    while (!I2C_GetITStatus(I2C1, I2C_IT_STOP_DET))
    {
        if (--tout == 0)
        {
            break;
        }
    }
}

static void HW_SpeedTest(void)
{
    uint8_t  buf[EEPROM_PAGE_SIZE];
    uint16_t a, i;
    uint32_t t0, t1;
    uint32_t tErase, tProg, tRead;
    uint8_t  ok = 1;

    /* If the DesignWare master does not get an ACK, skip (do not hang). */
    if (!HW_ProbeAck())
    {
        printf("\r\n--- Hardware I2C ---");
        printf("\r\n  EEPROM not responding (no ACK) - skipped");
        return;
    }

    memset(buf, 0xFF, sizeof(buf));

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += EEPROM_PAGE_SIZE)
    {
        HW_WriteData(a, buf, EEPROM_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tErase = t1 - t0;

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += EEPROM_PAGE_SIZE)
    {
        for (i = 0; i < EEPROM_PAGE_SIZE; i++)
        {
            buf[i] = (uint8_t)(a + i);
        }
        HW_WriteData(a, buf, EEPROM_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tProg = t1 - t0;

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += 20)
    {
        HW_ReadData(a, &RdHw[a], 20);
    }
    t1 = PLATFORM_GetTick();
    tRead = t1 - t0;

    for (i = 0; i < EEPROM_CAPACITY; i++)
    {
        if (RdHw[i] != (uint8_t)i)
        {
            ok = 0;
        }
    }

    ReportSpeed("Hardware I2C", tErase, tProg, tRead, ok);
}

/*==========================================================================
   Part 2: Software I2C (bit-bang master on PB6/PB7, ~100 kHz)
   ==========================================================================*/
#define EE_SCL_PORT  GPIOB
#define EE_SCL_PIN   GPIO_Pin_6
#define EE_SDA_PORT  GPIOB
#define EE_SDA_PIN   GPIO_Pin_7

static uint8_t RdSw[EEPROM_CAPACITY];

static void SW_I2C_InitPins(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = EE_SCL_PIN | EE_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_SetBits(EE_SCL_PORT, EE_SCL_PIN);
    GPIO_SetBits(EE_SDA_PORT, EE_SDA_PIN);
}

static void SW_Delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 350; i++)
    {
        __NOP();
    }
}

static void SCL_High(void) { GPIO_SetBits(EE_SCL_PORT, EE_SCL_PIN); SW_Delay(); }
static void SCL_Low(void)  { GPIO_ResetBits(EE_SCL_PORT, EE_SCL_PIN); SW_Delay(); }
static void SDA_High(void) { GPIO_SetBits(EE_SDA_PORT, EE_SDA_PIN); }
static void SDA_Low(void)  { GPIO_ResetBits(EE_SDA_PORT, EE_SDA_PIN); }
static uint8_t SDA_Read(void) { return (uint8_t)GPIO_ReadInputDataBit(EE_SDA_PORT, EE_SDA_PIN); }

static void SW_Start(void)
{
    SDA_High(); SCL_High(); SDA_Low(); SCL_Low();
}

static void SW_Stop(void)
{
    SDA_Low(); SCL_High(); SDA_High();
}

static uint8_t SW_WriteByte(uint8_t byte)
{
    uint8_t i, ack;

    for (i = 0; i < 8; i++)
    {
        (byte & 0x80) ? SDA_High() : SDA_Low();
        byte <<= 1;
        SCL_High();
        SCL_Low();
    }

    SDA_High();
    SCL_High();
    ack = (SDA_Read() == Bit_RESET) ? 1 : 0;
    SCL_Low();

    return ack;
}

static uint8_t SW_ReadByte(uint8_t ack)
{
    uint8_t i, byte = 0;

    SDA_High();

    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        SCL_High();
        if (SDA_Read() == Bit_SET)
        {
            byte |= 0x01;
        }
        SCL_Low();
    }

    if (ack)
    {
        SDA_Low();
    }
    else
    {
        SDA_High();
    }
    SCL_High();
    SCL_Low();
    SDA_High();

    return byte;
}

static void SW_WriteData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    while (Length > 0)
    {
        uint8_t avail = EEPROM_PAGE_SIZE - (Address % EEPROM_PAGE_SIZE);
        uint8_t n     = (Length > avail) ? avail : Length;
        uint8_t i;

        SW_Start();
        SW_WriteByte((uint8_t)((EEPROM_7BIT_ADDR << 1) | 0x00));
        SW_WriteByte(Address);
        for (i = 0; i < n; i++)
        {
            SW_WriteByte(Buffer[i]);
        }
        SW_Stop();

        PLATFORM_DelayMS(5);   /* tWR */

        Address += n;
        Buffer  += n;
        Length  -= n;
    }
}

static void SW_ReadData(uint8_t Address, uint8_t *Buffer, uint16_t Length)
{
    uint16_t i;

    SW_Start();
    SW_WriteByte((uint8_t)((EEPROM_7BIT_ADDR << 1) | 0x00));
    SW_WriteByte(Address);
    SW_Stop();

    SW_Start();
    SW_WriteByte((uint8_t)((EEPROM_7BIT_ADDR << 1) | 0x01));

    for (i = 0; i < Length; i++)
    {
        Buffer[i] = SW_ReadByte((i == (Length - 1)) ? 0 : 1);
    }

    SW_Stop();
}

static void SW_SpeedTest(void)
{
    uint8_t  buf[EEPROM_PAGE_SIZE];
    uint16_t a, i;
    uint32_t t0, t1;
    uint32_t tErase, tProg, tRead;
    uint8_t  ok = 1;

    SW_I2C_InitPins();

    memset(buf, 0xFF, sizeof(buf));

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += EEPROM_PAGE_SIZE)
    {
        SW_WriteData(a, buf, EEPROM_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tErase = t1 - t0;

    t0 = PLATFORM_GetTick();
    for (a = 0; a < EEPROM_CAPACITY; a += EEPROM_PAGE_SIZE)
    {
        for (i = 0; i < EEPROM_PAGE_SIZE; i++)
        {
            buf[i] = (uint8_t)(a + i);
        }
        SW_WriteData(a, buf, EEPROM_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tProg = t1 - t0;

    t0 = PLATFORM_GetTick();
    SW_ReadData(0, RdSw, EEPROM_CAPACITY);
    t1 = PLATFORM_GetTick();
    tRead = t1 - t0;

    for (i = 0; i < EEPROM_CAPACITY; i++)
    {
        if (RdSw[i] != (uint8_t)i)
        {
            ok = 0;
        }
    }

    ReportSpeed("Software I2C (bit-bang)", tErase, tProg, tRead, ok);
}

void I2C_EEMERGED_Init(void)
{
    printf("\r\n");
    printf("\r\n=== 24C02 test: hardware I2C + software I2C merged ===");
}

void I2C_EEMERGED_Run(void)
{
    while (1)
    {
        HW_SpeedTest();
        printf("\r\n");

        SW_SpeedTest();
        printf("\r\n==================================================");

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(2000);
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/