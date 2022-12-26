#include "main.h"

I2C_HandleTypeDef hi2c0;

void SystemClock_Config(void);
static void I2C0_Init(void);

int main()
{    

    SystemClock_Config();
    
    I2C0_Init();

    // Адрес ведомого
    uint16_t slave_address = 0x2BB; //0x36 0x3FF 0x7F

    uint8_t data[I2C_DATA_BYTES];

    for(int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i;

        #ifdef MIK32_I2C_DEBUG
        xprintf("data[%d] = %d\n", i, data[i]);
        #endif
    }
 
    while (1)
    {    

        /*Запись данных по адресу slave_address = 0x36 без сдвига адреса*/
        HAL_I2C_Master_WriteNBYTE(&hi2c0, 0x36, data, sizeof(data)); 
        if(hi2c0.Init.AutoEnd == AUTOEND_DISABLE)
        {
            /*Формирование события STOP*/
            HAL_I2C_Master_Stop(&hi2c0);
        }
        for (volatile int i = 0; i < 1000000; i++); 


        /*Чтение данных по адресу slave_address = 0x36 без сдвига адреса*/
        HAL_I2C_Master_Read(&hi2c0, slave_address, data, sizeof(data)); 
        if(hi2c0.Init.AutoEnd == AUTOEND_DISABLE)
        {
            /*Формирование события STOP*/
            HAL_I2C_Master_Stop(&hi2c0);
        }
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_I2C_0_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void I2C0_Init(void)
{

    /*Общие настройки*/
    hi2c0.Instance = I2C_0;
    hi2c0.Mode = HAL_I2C_MODE_MASTER;
    hi2c0.ShiftAddress = SHIFT_ADDRESS_DISABLE;
    hi2c0.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE; // При ENABLE в режиме ведущего значение AddressingMode не влияет
    hi2c0.Init.Filter = I2C_FILTER_OFF;

    /*Настройка частоты*/
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 10;
    hi2c0.Clock.SCLH = 16;
    hi2c0.Clock.SCLL = 16;
    //hi2c0.Init.ClockSpeed = 175;

    /*Настройки ведомого*/
    hi2c0.Init.OwnAddress1 = 0;
    hi2c0.Init.OwnAddress2 = 0;
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;

    /*Нстройки ведущего*/
    hi2c0.Init.AutoEnd = AUTOEND_DISABLE;

    HAL_I2C_Init(&hi2c0);

}
