#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include "xprintf.h"

/**
  *   System Clock Configuration
  *   The system Clock is configured as follow :
  *   System Clock source            = PLL (HSE)
  *   SYSCLK(Hz)                     = 168000000
  *   HCLK(Hz)                       = 168000000
  *   AHB Prescaler                  = 1
  *   APB1 Prescaler                 = 4
  *   APB2 Prescaler                 = 2
  *   HSE Frequency(Hz)              = 8000000
  *   PLL_M                          = 8
  *   PLL_N                          = 336
  *   PLL_P                          = 2
  *   VDD(V)                         = 3.3
  *   Main regulator output voltage  = Scale1 mode
  *   Flash Latency(WS)              = 5
  */
static void rcc_config()
{
        /* Enable HSE oscillator */
        LL_RCC_HSE_Enable();
        while(LL_RCC_HSE_IsReady() != 1);

        /* Set FLASH latency */
        LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

        /* Main PLL configuration and activation */
        LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8,
                                    336, LL_RCC_PLLP_DIV_2);
        LL_RCC_PLL_Enable();
        while(LL_RCC_PLL_IsReady() != 1);

        /* Sysclk activation on the main PLL */
        LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
        while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

        /* Set APB1 & APB2 prescaler */
        LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
        LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

        /* Set systick to 1ms */
        SysTick_Config(168000000/1000);

        /* Update CMSIS variable (which can be updated also
         * through SystemCoreClockUpdate function) */
        SystemCoreClock = 168000000;
}

static void uart_config(void)
{
        /*
         * Setting UART pins
         */
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        //USART1_TX
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_1);
        LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
        //USART1_RX
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);
        LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_HIGH);
        /*
         * USART Set clock source
         */
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
        LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
        /*
         * USART Setting
         */
        LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
        LL_USART_SetParity(USART1, LL_USART_PARITY_NONE);
        LL_USART_SetDataWidth(USART1, LL_USART_DATAWIDTH_8B);
        LL_USART_SetStopBitsLength(USART1, LL_USART_STOPBITS_1);
        LL_USART_SetTransferBitOrder(USART1, LL_USART_BITORDER_LSBFIRST);
        LL_USART_SetBaudRate(USART1, SystemCoreClock,
                             LL_USART_OVERSAMPLING_16, 115200);
        /*
         * USART turn on
         */
        LL_USART_Enable(USART1);
        while (!(LL_USART_IsActiveFlag_TEACK(USART1) &&
                 LL_USART_IsActiveFlag_REACK(USART1)));
        return;
}

static char usart_getc(void)
{
        char byte;

        if (LL_USART_IsActiveFlag_RXNE(USART1))
                byte = LL_USART_ReceiveData8(USART1);
        return byte;
}

static void usart_putc(char symbol)
{
        LL_USART_TransmitData8(USART1, symbol);
        while (!LL_USART_IsActiveFlag_TC(USART1));
}

static void printf_config(void)
{
        xdev_out(usart_putc);
        xdev_in(usart_getc);
        return;
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
        while (1) {
        }
}

void MemManage_Handler(void)
{
        while (1) {
        }
}

void BusFault_Handler(void)
{
}

void UsageFault_Handler(void)
{
        while (1) {
        }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

int main(void)
{
        rcc_config();
        uart_config();
        printf_config();

        return 0;
}
