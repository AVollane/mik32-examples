#include "mik32_hal_rcc.h"
#include "mik32_hal_adc.h"

#include "uart_lib.h"
#include "xprintf.h"


ADC_HandleTypeDef hadc;

void SystemClock_Config(void);
static void ADC_Init(void);

int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    
    ADC_Init();

    uint16_t adc_value = 0;
    // HAL_ADC_ContinuousEnable(&hadc); /* Запуск преобразования в непрерывном режиме */


    while (1)
    {    
        HAL_ADC_Single(&hadc); /* Запуск однократного преобразования */
        adc_value = HAL_ADC_WaitAndGetValue(&hadc); /* Ожидание и чтение актуальных данных (режим одиночного преобразования) */
        //adc_value = HAL_ADC_GetValue(&hadc); /* Получить текущий результат преобразования (режим непрерывного преобразования) */

        xprintf("ADC: %d (V = %d,%03d)\n", adc_value, ((adc_value*1200)/4095)/1000, ((adc_value*1200)/4095)%1000);

        for (volatile int i = 0; i < 1000000; i++);
    }
       
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;   
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 0;                  
    RCC_OscInit.LSI32KCalibrationValue = 0;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_ANALOG_REG_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void ADC_Init(void)
{
    hadc.Instance = ANALOG_REG;

    hadc.Init.Sel = ADC_CHANNEL4;
    hadc.Init.EXTRef = ADC_EXTREF_OFF; /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hadc.Init.EXTClb = ADC_EXTCLB_CLBREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_ADC_Init(&hadc);
}
