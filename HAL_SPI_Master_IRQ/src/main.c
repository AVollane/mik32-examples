#include "mik32_hal_spi.h"
#include "mik32_hal_irq.h"
#include "mik32_hal.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * Данный пример демонстрирует работу с SPI в режиме ведущего с использованием прерываний.
 * Ведущий передает и принимает от ведомого на выводе CS0 12 байт.
 *
 * Результат передачи выводится по UART0.
 * Данный пример можно использовать совместно с ведомым из примера HAL_SPI_Slave.
 */

SPI_HandleTypeDef hspi0;

void SystemClock_Config(void);
static void SPI0_Init(void);

int main()
{
    HAL_Init();

    SystemClock_Config();

    HAL_EPIC_MaskLevelSet(HAL_EPIC_SPI_0_MASK);
    HAL_IRQ_EnableInterrupts();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    SPI0_Init();
    // HAL_SPI_SetDelayBTWN(&hspi0, 1);
    // HAL_SPI_SetDelayAFTER(&hspi0, 0);
    // HAL_SPI_SetDelayINIT(&hspi0, 100);

    uint8_t master_output[] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xB0, 0xB1};
    uint8_t master_input[sizeof(master_output)];
    for (uint32_t i = 0; i < sizeof(master_input); i++)
    {
        master_input[i] = 0;
    }

    while (1)
    {

        if (hspi0.State == HAL_SPI_STATE_READY)
        {
            /* Начало передачи в ручном режиме управления CS */
            if (hspi0.Init.ManualCS == SPI_MANUALCS_ON)
            {
                HAL_SPI_Enable(&hspi0);
                HAL_SPI_CS_Enable(&hspi0, SPI_CS_0);
            }

            /* Передача и прием данных */
            HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange_IT(&hspi0, master_output, master_input, sizeof(master_output));
            if (SPI_Status != HAL_OK)
            {
                HAL_SPI_ClearError(&hspi0);
                hspi0.State = HAL_SPI_STATE_READY;
            }
        }

        if (hspi0.State == HAL_SPI_STATE_END)
        {
            /* Конец передачи в ручном режиме управления CS */
            if (hspi0.Init.ManualCS == SPI_MANUALCS_ON)
            {
                HAL_SPI_CS_Disable(&hspi0);
                HAL_SPI_Disable(&hspi0);
            }

            /* Вывод принятый данных и обнуление массива master_input */
            for (uint32_t i = 0; i < sizeof(master_input); i++)
            {
                xprintf("master_input[%d] = 0x%02x\n", i, master_input[i]);
                master_input[i] = 0;
            }

            xprintf("\n");

            hspi0.State = HAL_SPI_STATE_READY;
        }

        if (hspi0.State == HAL_SPI_STATE_ERROR)
        {
            xprintf("SPI_Error: OVR %d, MODF %d\n", hspi0.Error.RXOVR, hspi0.Error.ModeFail);
            if (hspi0.Init.ManualCS == SPI_MANUALCS_ON)
            {
                HAL_SPI_CS_Disable(&hspi0);
            }
            HAL_SPI_Disable(&hspi0);

            hspi0.State = HAL_SPI_STATE_READY;
        }
    }
}

void SystemClock_Config(void)
{
    PCC_OscInitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}

static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_OFF;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF; /* Настройки ручного режима управления сигналом CS */
    hspi0.Init.ChipSelect = SPI_CS_0;       /* Выбор ведомого устройства в автоматическом режиме управления CS */
    hspi0.Init.ThresholdTX = 1;

    if (HAL_SPI_Init(&hspi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}

void trap_handler()
{
    if (EPIC_CHECK_SPI_0())
    {
        HAL_SPI_IRQHandler(&hspi0);
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}
