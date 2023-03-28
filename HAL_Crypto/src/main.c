#include "uart_lib.h"
#include "xprintf.h"

#include "mik32_hal_rcc.h"
#include "mik32_hal_crypto.h"


Crypto_HandleTypeDef hcrypto;

void SystemClock_Config(void);
static void Crypto_Init(void);

uint32_t crypto_key[CRYPTO_KEY_KUZNECHIK] = {0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0xfedcba98, 0x76543210, 0x01234567, 0x89abcdef};

                           

void kuznechik_ECB_code()
{
    uint32_t plain_text[] =     {            
                                    0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                    0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                    0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                    0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                };
    
    uint32_t cipher_text[] = {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 

    uint32_t expect_cipher_text[] = {
                                        0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                        0xb429912c, 0x6e0032f9, 0x285452d7, 0x6718d08b, 
                                        0xf0ca3354, 0x9d247cee, 0xf3f5a531, 0x3bd4b157, 
                                        0xd0b09ccd, 0xe830b9eb, 0x3a02c4c5, 0xaa8ada98
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_ECB);
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);


    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length); 


    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("plain: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");   

    xprintf("cipher: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", expect_cipher_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        if(expect_cipher_text[i] != cipher_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == plain_text_length)
        {
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_ECB_decode()
{
    uint32_t plain_text[] =     {            
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0
                                };
    
    uint32_t cipher_text[] = {  
                                0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                0xb429912c, 0x6e0032f9, 0x285452d7, 0x6718d08b, 
                                0xf0ca3354, 0x9d247cee, 0xf3f5a531, 0x3bd4b157, 
                                0xd0b09ccd, 0xe830b9eb, 0x3a02c4c5, 0xaa8ada98
                             }; 
    
    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_ECB);
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);


    /* Расшифровать данные */
    HAL_Crypto_Decode(&hcrypto, cipher_text, plain_text, cipher_text_length); 


    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("cipher: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");   

    xprintf("plain: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", expect_plain_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        if(expect_plain_text[i] != plain_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == cipher_text_length)
        {
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CBC_code()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CBC] = {0x12341234, 0x11114444, 0xABCDABCD, 0xAAAABBBB};
    
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };

    uint32_t cipher_text[] = {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 

    uint32_t expect_cipher_text[] = {
                                        0x50796e7f, 0x4094ce10, 0xbab7374c, 0x981047e3, 
                                        0x1ee4f83b, 0x334948ed, 0x86a0873c, 0x86bff9a2, 
                                        0xa084f5fa, 0x965481e4, 0xb64be9bd, 0x32ef21e3, 
                                        0xa6e376cf, 0x95e8a097, 0x9a46ba33, 0x152b1843
                                    };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CBC);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);


    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length);     

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("plain: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");   

    xprintf("cipher: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", expect_cipher_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        if(expect_cipher_text[i] != cipher_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == plain_text_length)
        {
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CBC_decode()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CBC] = {0x12341234, 0x11114444, 0xABCDABCD, 0xAAAABBBB};
    
    uint32_t plain_text[] =     {            
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0
                                };
    
    uint32_t cipher_text[] = {  
                                0x50796e7f, 0x4094ce10, 0xbab7374c, 0x981047e3, 
                                0x1ee4f83b, 0x334948ed, 0x86a0873c, 0x86bff9a2, 
                                0xa084f5fa, 0x965481e4, 0xb64be9bd, 0x32ef21e3, 
                                0xa6e376cf, 0x95e8a097, 0x9a46ba33, 0x152b1843
                             }; 

    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };
    

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);

    

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CBC);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);
  

    /* Расшифровать данные */
    HAL_Crypto_Decode(&hcrypto, cipher_text, plain_text, cipher_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("cipher: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");   

    xprintf("plain: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", expect_plain_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        if(expect_plain_text[i] != plain_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == cipher_text_length)
        {
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CTR_code()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CTR] = {0x12345678, 0x90ABCEF0};
    
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };
    
    uint32_t cipher_text[] = {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 

    uint32_t expect_cipher_text[] = {
                                        0xf195d8be, 0xc10ed1db, 0xd57b5fa2, 0x40bda1b8,
                                        0x85eee733, 0xf6a13e5d, 0xf33ce4b3, 0x3c45dee4,
                                        0xa5eae88b, 0xe6356ed3, 0xd5e877f1, 0x3564a3a5,
                                        0xcb91fab1, 0xf20cbab6, 0xd1c6d158, 0x20bdba73
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CTR);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);

    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("plain: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");   

    xprintf("cipher: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", expect_cipher_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        if(expect_cipher_text[i] != cipher_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == plain_text_length)
        {
            xprintf("Success\n"); 
        }
    }
    xprintf("\n");
}

void kuznechik_CTR_decode()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CTR] = {0x12345678, 0x90ABCEF0};
    
    uint32_t plain_text[] =  {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 
    
    uint32_t cipher_text[] = {
                                0xf195d8be, 0xc10ed1db, 0xd57b5fa2, 0x40bda1b8, 
                                0x85eee733, 0xf6a13e5d, 0xf33ce4b3, 0x3c45dee4,
                                0xa5eae88b, 0xe6356ed3, 0xd5e877f1, 0x3564a3a5
                             };

    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text); 


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CTR);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);


    /* Расшифровать данные */
    HAL_Crypto_Decode(&hcrypto, cipher_text, plain_text, cipher_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("cipher: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");   

    xprintf("plain: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", expect_plain_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        if(expect_plain_text[i] != plain_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == cipher_text_length)
        {
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

int main()
{    
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Crypto_Init();

    xprintf("\nkuznechik_ECB_code\n");
    kuznechik_ECB_code();
    xprintf("\nkuznechik_ECB_decode\n");   
    kuznechik_ECB_decode();

    xprintf("\nkuznechik_CBC_code\n");
    kuznechik_CBC_code();
    xprintf("\nkuznechik_CBC_decode\n");   
    kuznechik_CBC_decode();


    xprintf("\nkuznechik_CTR_code\n");
    kuznechik_CTR_code();
    xprintf("\nkuznechik_CTR_decode\n");   
    kuznechik_CTR_decode();


    while (1)
    {    
        
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

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT | PM_CLOCK_CRYPTO_M;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M; 
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Crypto_Init(void)
{
    hcrypto.Instance = CRYPTO;

    hcrypto.Algorithm = CRYPTO_ALG_KUZNECHIK;
    hcrypto.CipherMode = CRYPTO_CIPHER_MODE_ECB;
    hcrypto.SwapMode = CRYPTO_SWAP_MODE_NONE; 
    hcrypto.OrderMode = CRYPTO_ORDER_MODE_MSW;

    HAL_Crypto_Init(&hcrypto);
}
