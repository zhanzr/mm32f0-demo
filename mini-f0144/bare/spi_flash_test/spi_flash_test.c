#define _SPI_FLASH_TEST_C_

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "spi_flash_test.h"

/* ZD25WQ80: 1 MB SPI NOR flash */
#define FLASH_CAPACITY       (1024UL * 1024UL)   /* 1 MB */
#define FLASH_PAGE_SIZE      256
#define FLASH_SECTOR_SIZE    (4 * 1024)

/* Test region: 16 KB (4 sectors) - balances test time and coverage */
#define TEST_REGION_SIZE     (16 * 1024)
#define TEST_SECTORS         (TEST_REGION_SIZE / FLASH_SECTOR_SIZE)
#define TEST_PAGES           (TEST_REGION_SIZE / FLASH_PAGE_SIZE)

#define SPI_FLASH_CS_H()  SPI_CSInternalSelected(SPI2, DISABLE)
#define SPI_FLASH_CS_L()  SPI_CSInternalSelected(SPI2, ENABLE)

static uint8_t WrBuf[FLASH_PAGE_SIZE];
static uint8_t RdBuf[FLASH_PAGE_SIZE];

/***********************************************************************************************************************
  SPI2 master (PB12=CS, PB13=SCK, PB14=MISO, PB15=MOSI)
***********************************************************************************************************************/
static void SPI_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef  SPI_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SPI2, ENABLE);

    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStruct.SPI_DataWidth = 8;
    SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &SPI_InitStruct);

    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Rx);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Tx);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    SPI_Cmd(SPI2, ENABLE);
}

/***********************************************************************************************************************
  Polling byte transfer
***********************************************************************************************************************/
static void FLASH_RxBytes(uint8_t *Buffer, uint32_t Length)
{
    uint32_t i;

    for (i = 0; i < Length; i++)
    {
        SPI_SendData(SPI2, 0xFF);

        while (RESET == SPI_GetFlagStatus(SPI2, SPI_FLAG_TXEPT))
        {
        }

        while (RESET == SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL))
        {
        }

        Buffer[i] = (uint8_t)SPI_ReceiveData(SPI2);
    }
}

static void FLASH_TxBytes(uint8_t *Buffer, uint32_t Length)
{
    uint32_t i;

    for (i = 0; i < Length; i++)
    {
        SPI_SendData(SPI2, Buffer[i]);

        while (RESET == SPI_GetFlagStatus(SPI2, SPI_FLAG_TXEPT))
        {
        }

        while (RESET == SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL))
        {
        }

        (void)SPI_ReceiveData(SPI2);
    }
}

/***********************************************************************************************************************
  ZD25WQ80 commands
***********************************************************************************************************************/
static void FLASH_WriteEnable(void)
{
    uint8_t cmd = 0x06;

    SPI_FLASH_CS_L();
    FLASH_TxBytes(&cmd, 1);
    SPI_FLASH_CS_H();
}

static void FLASH_WaitBusy(void)
{
    uint8_t  cmd[2] = { 0x05, 0xFF };
    uint8_t  st = 0;
    uint32_t timeout = 0;

    do
    {
        SPI_FLASH_CS_L();
        FLASH_TxBytes(cmd, 2);
        FLASH_RxBytes(&st, 1);
        SPI_FLASH_CS_H();

        if (timeout++ > 0xFFFFFF)
        {
            break;
        }
    }
    while (st & 0x01);
}

static uint32_t FLASH_ReadJEDEC(void)
{
    uint8_t  cmd = 0x9F;
    uint8_t  buf[3];
    uint32_t id = 0;

    SPI_FLASH_CS_L();
    FLASH_TxBytes(&cmd, 1);
    FLASH_RxBytes(buf, 3);
    SPI_FLASH_CS_H();

    id  = buf[0];
    id <<= 8;
    id |= buf[1];
    id <<= 8;
    id |= buf[2];

    return id;
}

static void FLASH_SectorErase(uint32_t Address)
{
    uint8_t cmd[4] = { 0x20, 0x00, 0x00, 0x00 };

    cmd[1] = (uint8_t)((Address >> 16) & 0xFF);
    cmd[2] = (uint8_t)((Address >> 8)  & 0xFF);
    cmd[3] = (uint8_t)((Address >> 0)  & 0xFF);

    FLASH_WriteEnable();

    SPI_FLASH_CS_L();
    FLASH_TxBytes(cmd, 4);
    SPI_FLASH_CS_H();

    FLASH_WaitBusy();
}

static void FLASH_PageProgram(uint32_t Address, uint8_t *Buffer, uint32_t Length)
{
    uint8_t cmd[4] = { 0x02, 0x00, 0x00, 0x00 };

    cmd[1] = (uint8_t)((Address >> 16) & 0xFF);
    cmd[2] = (uint8_t)((Address >> 8)  & 0xFF);
    cmd[3] = (uint8_t)((Address >> 0)  & 0xFF);

    FLASH_WriteEnable();

    SPI_FLASH_CS_L();
    FLASH_TxBytes(cmd, 4);
    FLASH_TxBytes(Buffer, Length);
    SPI_FLASH_CS_H();

    FLASH_WaitBusy();
}

static void FLASH_FastRead(uint32_t Address, uint8_t *Buffer, uint32_t Length)
{
    uint8_t cmd[5] = { 0x0B, 0x00, 0x00, 0x00, 0xFF };

    cmd[1] = (uint8_t)((Address >> 16) & 0xFF);
    cmd[2] = (uint8_t)((Address >> 8)  & 0xFF);
    cmd[3] = (uint8_t)((Address >> 0)  & 0xFF);

    SPI_FLASH_CS_L();
    FLASH_TxBytes(cmd, 5);
    FLASH_RxBytes(Buffer, Length);
    SPI_FLASH_CS_H();
}

/***********************************************************************************************************************
  Speed tests over TEST_REGION_SIZE
***********************************************************************************************************************/
static void SpeedTest(void)
{
    uint32_t addr;
    uint32_t t0, t1;
    uint32_t tErase, tProg, tRead;
    uint8_t  ok = 1;

    /* Erase: sector erase over the region */
    t0 = PLATFORM_GetTick();
    for (addr = 0; addr < TEST_REGION_SIZE; addr += FLASH_SECTOR_SIZE)
    {
        FLASH_SectorErase(addr);
    }
    t1 = PLATFORM_GetTick();
    tErase = t1 - t0;

    /* Program: page program with a pattern */
    t0 = PLATFORM_GetTick();
    for (addr = 0; addr < TEST_REGION_SIZE; addr += FLASH_PAGE_SIZE)
    {
        uint32_t i;
        for (i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            WrBuf[i] = (uint8_t)(addr + i);
        }
        FLASH_PageProgram(addr, WrBuf, FLASH_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tProg = t1 - t0;

    /* Read: fast read over the region */
    t0 = PLATFORM_GetTick();
    for (addr = 0; addr < TEST_REGION_SIZE; addr += FLASH_PAGE_SIZE)
    {
        FLASH_FastRead(addr, RdBuf, FLASH_PAGE_SIZE);
    }
    t1 = PLATFORM_GetTick();
    tRead = t1 - t0;

    /* Verify */
    for (addr = 0; addr < TEST_REGION_SIZE; addr += FLASH_PAGE_SIZE)
    {
        FLASH_FastRead(addr, RdBuf, FLASH_PAGE_SIZE);
        {
            uint32_t i;
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                if (RdBuf[i] != (uint8_t)(addr + i))
                {
                    ok = 0;
                }
            }
        }
    }

    printf("\r\n");
    printf("\r\n--- SPI Flash speed (region %u KB, SPI2 prescaler /2) ---", TEST_REGION_SIZE / 1024);
    printf("\r\nErase  time: %lu ms    speed: %lu B/s",
           (unsigned long)tErase,
           (unsigned long)((TEST_REGION_SIZE * 1000UL) / (tErase ? tErase : 1)));
    printf("\r\nProgram time: %lu ms    speed: %lu B/s",
           (unsigned long)tProg,
           (unsigned long)((TEST_REGION_SIZE * 1000UL) / (tProg ? tProg : 1)));
    printf("\r\nRead   time: %lu ms    speed: %lu B/s",
           (unsigned long)tRead,
           (unsigned long)((TEST_REGION_SIZE * 1000UL) / (tRead ? tRead : 1)));
    printf("\r\nReadback match: %s", ok ? "PASS" : "FAIL");
}

void SPI_FLASHTEST_Init(void)
{
    printf("\r\n");
    printf("\r\n=== ZD25WQ80 SPI Flash test (SPI2, PB12-15) ===");
}

void SPI_FLASHTEST_Run(void)
{
    while (1)
    {
        SPI_Configure();

        printf("\r\nJEDEC ID: 0x%06lX", (unsigned long)FLASH_ReadJEDEC());

        SpeedTest();

        printf("\r\n==================================================");

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(2000);
    }
}

/********************************************** (C) Copyright MindMotion **********************************************/