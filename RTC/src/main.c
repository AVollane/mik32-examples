#include "common.h"
#include "rtc.h"
#include "wakeup.h"
#include "power_manager.h"
#include "epic.h"

void rtc_wait_flag()
{
    uint32_t retry_limit = 10000;
    for (uint32_t i = 0; i < retry_limit; i++)
    {
        if ((RTC->CTRL & RTC_CTRL_FLAG_M) == 0)
        {
            return;
        }
    }
    
    TEST_ERROR("CTRL.FLAG is still 1 after %d retries", retry_limit);
}

void rts_set_time(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint32_t RTC_time = (1 << RTC_TIME_DOW_S) | // Пн
                        (1 << RTC_TIME_TH_S)  | // 
                        (9 << RTC_TIME_H_S)   | // 19ч 
                        (1 << RTC_TIME_TM_S)  | //
                        (6 << RTC_TIME_M_S)   | // 10м 
                        (1 << RTC_TIME_TS_S)  | //
                        (2 << RTC_TIME_S_S);    // 12с

    xprintf("Установка времени RTC\n");
    RTC->TIME = RTC_time;
    rtc_wait_flag();
}

void rtc_check_date()
{
    uint8_t TC, C, TY, Y, TM, M, TD, D;
    uint32_t rtc_date_read = RTC->DATE;
    TC = (rtc_date_read & RTC_DATE_TC_M) >> RTC_DATE_TC_S;
    C = (rtc_date_read & RTC_DATE_C_M) >> RTC_DATE_C_S;
    TY = (rtc_date_read & RTC_DATE_TY_M) >> RTC_DATE_TY_S;
    Y = (rtc_date_read & RTC_DATE_Y_M) >> RTC_DATE_Y_S;
    TM = (rtc_date_read & RTC_DATE_TM_M) >> RTC_DATE_TM_S;
    M = (rtc_date_read & RTC_DATE_M_M) >> RTC_DATE_M_S;
    TD = (rtc_date_read & RTC_DATE_TD_M) >> RTC_DATE_TD_S;
    D = (rtc_date_read & RTC_DATE_D_M) >> RTC_DATE_D_S;

    xprintf("\n%d%d век\n", TC, C);
    xprintf("%d%d.%d%d.%d%d\n", TD, D, TM, M, TY, Y);
}

void rtc_check_time()
{
    switch (RTC->DOW)
    {
    case 1:
        xprintf("Понедельник\n");
        break;
    case 2:
        xprintf("Вторник\n");
        break;
    case 3:
        xprintf("Среда\n");
        break;
    case 4:
        xprintf("Четверг\n");
        break;
    case 5:
        xprintf("Пятница\n");
        break;
    case 6:
        xprintf("Суббота\n");
        break;
    case 7:
        xprintf("Воскресенье\n");
        break;
    }
    xprintf("%d%d:%d%d:%d%d.%d\n", RTC->TH, RTC->H, RTC->TM, RTC->M, RTC->TS, RTC->S, RTC->TOS);

}

void rtc_check()
{
    rtc_check_date();
    rtc_check_time();
}

int main()
{

    //Включаем тактирование необходимых блоков и модуля выбора режима GPIO 
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_WDT_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M | PM_CLOCK_RTC_M;
    for (volatile int i = 0; i < 10; i++);

    /*
    * CLOCKS_BU - Регистр управления тактированием батарейного домена
    * 
    * OCS32K_en - Включение/отключение внешнего осцилятора на 32 KГц - разряд 0
    * RC32K_en - Включение/отключение LSI32К - разряд 1
    * Adj_RC32К - Поправочные коэффициенты LSI32К - разряд 6-9
    * RTC_Clk_Mux - Выбор источника тактирования часов реального времени: - разряд 10
    *               0 – внутренний LSI32К;
    *               1 – внешний осциллятор OSC32K
    * OSC32K_sm - Режим повышенного потребления, активный уровень “0” для OSC32K - разряд 14
    */
    WU->CLOCKS_BU = WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
    for (volatile int i = 0; i < 100; i++);
    WU->CLOCKS_BU = 0<<WU_CLOCKS_BU_OSC32K_PD_S | WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
    xprintf("Запуск с внешним осцилятором OSC32K\n");

    // Сброс RTC
    WU->RTC_CONRTOL = WU_RTC_RESET_CLEAR_M;

    // Для записи даты нужно сбросить бит EN в регистре CTRL
    //xprintf("Disable RTC\n");
    RTC->CTRL = 0;
    rtc_wait_flag();
    


    uint32_t RTC_data = (2 << RTC_DATE_TC_S)  | // 
                        (1 << RTC_DATE_C_S)   | // 21 век
                        (2 << RTC_DATE_TY_S)  | //
                        (2 << RTC_DATE_Y_S)   | // 22 год
                        (0 << RTC_DATE_TM_S)  | // 
                        (7 << RTC_DATE_M_S)   | // 07 месяц - июль
                        (1 << RTC_DATE_TD_S)  | //
                        (8 << RTC_DATE_D_S);    // 18 число

    xprintf("Установка даты RTC\n");
    RTC->DATE = RTC_data;
    rtc_wait_flag();

    xprintf("Включнеие RTC\n");
    RTC->CTRL = RTC_CTRL_EN_M;
    rtc_wait_flag();

    while (1)
    {
        rtc_check();
        for (volatile int i = 0; i < 1000000; i++); 
    }
    
}