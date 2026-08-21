#define _I2C_EE_TEST_C_

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "i2c_ee_test.h"

/* 24C01/02 family: 256 bytes, 8-byte pages */
#define EEPROM_7BIT_ADDR     0x50
#define EEPROM_CAPACITY      256
#define EEPROM_PAGE_SIZE     8
#define EEPROM_WR_CYCLE_MS   5

#define EE_SCL_PORT  GPIOB
#define EE_SCL_PIN   GPIO_Pin_6
#define EE_SDA_PORT  GPIOB
#define EE_SDA_PIN   GPIO_Pin_7

static uint8_t Rd[EEPROM_CAPACITY];

/***********************************************************************************************************************
  Software I2C (bit-bang) master on PB6=SCL, PB7=SDA (open-drain + onboard pull-ups)
***********************************************************************************************************************/
static void SWI2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = EE_SCL_PIN | EE_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;      /* open-drain */
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* both released high */
    GPIO_SetBits(EE_SCL_PORT, EE_SCL_PIN);
    GPIO_SetBits(EE_SDA_PORT, EE_SDA_PIN);
}

static void SW_Delay(void)
{
    /* ~100 kHz SCL: each SCL half-period ~5 us at 72 MHz */
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

static void SWI2C_Start(void)
{
    SDA_High();
    SCL_High();
    SDA_Low();
    SCL_Low();
}

static void SWI2C_Stop(void)
{
    SDA_Low();
    SCL_High();
    SDA_High();
}

/***********************************************************************************************************************
  Send one byte (MSB first), return 1 if slave ACKed.
***********************************************************************************************************************/
static uint8_t SWI2C_WriteByte(uint8_t byte)
{
    uint8_t i;
    uint8_t ack;

    for (i = 0; i < 8; i++)
    {
        (byte & 0x80) ? SDA_High() : SDA_Low();
        byte <<= 1;
        SCL_High();
        SCL_Low();
    }

    /* release SDA for ACK */
    SDA_High();
    SCL_High();
    ack = (SDA_Read() == Bit_RESET) ? 1 : 0;
    SCL_Low();

    return ack;
}

/***********************************************************************************************************************
  Read one byte, send ack = 1 on the last desired byte.
***********************************************************************************************************************/
static uint8_t SWI2C_ReadByte(uint8_t ack)
{
    uint8_t i;
    uint8_t byte = 0;

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

/***********************************************************************************************************************
  Write Length bytes starting at Address, page-split.
***********************************************************************************************************************/
static void EEPROM_WriteData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    while (Length > 0)
    {
        uint8_t avail = EEPROM_PAGE_SIZE - (Address % EEPROM_PAGE_SIZE);
        uint8_t n     = (Length > avail) ? avail : Length;
        uint8_t i;

        SWI2C_Start();
        SWI2C_WriteByte((uint8_t)((EEPROM_7BIT_ADDR << 1) | 0x00));  /* write */
        SWI2C_WriteByte(Address);
        for (i = 0; i < n; i++)
        {
            SWI2C_WriteByte(Buffer[i]);
        }
        SWI2C_Stop();

        PLATFORM_DelayMS(EEPROM_WR_CYCLE_MS);   /* tWR */

        Address += n;
        Buffer  += n;
        Length  -= n;
    }
}

/***********************************************************************************************************************
  Sequential read: set address pointer then read Length bytes.
***********************************************************************************************************************/
static void EEPROM_ReadData(uint8_t Address, uint8_t *Buffer, uint16_t Length)
{
    uint16_t i;

    /* address pointer set (dummy write, no data) */
    SWI2C_Start();
    SWI2C_WriteByte((uint8_t)((EEPROM_7BIT_ADDR << 1) | 0x00));
    SWI2C_WriteByte(Address);
    SWI2C_Stop();

    /* sequential read */
    SWI2C_Start();
    SWI2C_WriteByte((uint8_t)((EEPROM_7BIT_ADDR << 1) | 0x01));  /* read */

    for (i = 0; i < Length; i++)
    {
        Buffer[i] = SWI2C_ReadByte((i == (Length - 1)) ? 0 : 1);
    }

    SWI2C_Stop();
}

/***********************************************************************************************************************
  Scan for the EEPROM device address (probe write ACK on 0x50..0x5F).
***********************************************************************************************************************/
static void EEPROM_Scan(void)
{
    uint8_t a;
    uint8_t found = 0;

    printf("\r\nI2C address scan (0x50..0x5F):");

    for (a = 0x50; a < 0x60; a++)
    {
        SWI2C_Start();
        if (SWI2C_WriteByte((uint8_t)((a << 1) | 0x00)))
        {
            printf("\r\n  found EEPROM at 7-bit 0x%02X", (unsigned)a);
            found = 1;
        }
        SWI2C_Stop();
    }

    if (!found)
    {
        printf("\r\n  (none found - check wiring/pull-ups)");
    }
}

static uint32_t EEPROM_EraseAll(uint8_t fill)
{
    uint8_t buf[EEPROM_PAGE_SIZE];
    uint16_t addr;
    uint32_t t0, t1;

    memset(buf, fill, sizeof(buf));

    t0 = PLATFORM_GetTick();

    for (addr = 0; addr < EEPROM_CAPACITY; addr += EEPROM_PAGE_SIZE)
    {
        EEPROM_WriteData(addr, buf, EEPROM_PAGE_SIZE);
    }

    t1 = PLATFORM_GetTick();

    return (t1 - t0);
}

static uint32_t EEPROM_ProgramAll(void)
{
    uint8_t buf[EEPROM_PAGE_SIZE];
    uint16_t addr;
    uint8_t i;
    uint32_t t0, t1;

    t0 = PLATFORM_GetTick();

    for (addr = 0; addr < EEPROM_CAPACITY; addr += EEPROM_PAGE_SIZE)
    {
        for (i = 0; i < EEPROM_PAGE_SIZE; i++)
        {
            buf[i] = (uint8_t)(addr + i);
        }

        EEPROM_WriteData(addr, buf, EEPROM_PAGE_SIZE);
    }

    t1 = PLATFORM_GetTick();

    return (t1 - t0);
}

static uint32_t EEPROM_ReadAll(void)
{
    uint32_t t0, t1;

    t0 = PLATFORM_GetTick();

    EEPROM_ReadData(0, Rd, EEPROM_CAPACITY);

    t1 = PLATFORM_GetTick();

    return (t1 - t0);
}

void I2C_EETEST_Init(void)
{
    SWI2C_Init();
}

void I2C_EETEST_Run(void)
{
    uint32_t tErase, tProg, tRead;
    uint32_t i;
    uint8_t  ok = 1;

    printf("\r\n");
    printf("\r\n=== 24C02 EEPROM test (SW I2C, PB6=SCL PB7=SDA) ===");

    EEPROM_Scan();
    printf("\r\n");

    tErase = EEPROM_EraseAll(0xFF);
    tProg  = EEPROM_ProgramAll();
    tRead  = EEPROM_ReadAll();

    for (i = 0; i < EEPROM_CAPACITY; i++)
    {
        if (Rd[i] != (uint8_t)i)
        {
            ok = 0;
            break;
        }
    }

    printf("\r\n");
    printf("\r\nErase  time: %lu ms", (unsigned long)tErase);
    printf("\r\nProgram time: %lu ms", (unsigned long)tProg);
    printf("\r\nRead   time: %lu ms", (unsigned long)tRead);

    printf("\r\nErase  speed: %lu B/s",
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tErase ? tErase : 1)));
    printf("\r\nProgram speed: %lu B/s",
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tProg ? tProg : 1)));
    printf("\r\nRead   speed: %lu B/s",
           (unsigned long)((EEPROM_CAPACITY * 1000UL) / (tRead ? tRead : 1)));

    printf("\r\nReadback match: %s", ok ? "PASS" : "FAIL");

    printf("\r\n==================================================");

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(500);
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/