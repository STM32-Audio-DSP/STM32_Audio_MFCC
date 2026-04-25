/**
  ******************************************************************************
  * @file    Templates/TrustZoneDisabled/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

/** @addtogroup STM32U5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FFT_PLOT_SAMPLE_RATE_HZ            (16000U)
#define FFT_PLOT_SIZE                      (256U)
#define FFT_PLOT_PI                        (3.14159265358979323846f)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

static float g_fft_demo_input[FFT_PLOT_SIZE];
static float g_fft_demo_real[FFT_PLOT_SIZE];
static float g_fft_demo_imaginary[FFT_PLOT_SIZE];
/* Private function prototypes -----------------------------------------------*/
#if defined(__ICCARM__)
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __GNUC__) && !defined(__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __ICCARM__ */

static void SystemClock_Config(void);
static void CACHE_Enable(void);
static void MX_USART1_UART_Init(void);
static void FFT_GenerateDemoSignal(float *buffer, uint32_t sample_count, float sample_rate_hz);
static void FFT_Run(float *real, float *imaginary, uint32_t fft_size);
static void FFT_PrintInputSignal(const float *samples, uint32_t sample_count);
static void FFT_PrintSpectrum(const float *samples, uint32_t sample_count);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32U5xx HAL library initialization:
       - Configure the Flash prefetch
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();

  /* This project template calls CACHE_Enable() in order to enable the Instruction
     and Data Cache. This function is provided as template implementation that
     User may integrate in his application in order to enhance performance */

  /* Enable the Instruction Cache */
  CACHE_Enable();

  /* Configure the System clock to have a frequency of 160 MHz */
  SystemClock_Config();

  /* Add your application code here
     */

  BSP_LED_Init(LED_GREEN); /* Green LED */
  MX_USART1_UART_Init();

  FFT_GenerateDemoSignal(g_fft_demo_input, FFT_PLOT_SIZE, (float)FFT_PLOT_SAMPLE_RATE_HZ);

  /* Infinite loop */
  while (1)
  {
    FFT_PrintInputSignal(g_fft_demo_input, FFT_PLOT_SIZE);
//    FFT_PrintSpectrum(g_fft_demo_input, FFT_PLOT_SIZE);
    HAL_Delay(250);
    BSP_LED_Toggle(LED_GREEN);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 160000000
  *            HCLK(Hz)                       = 160000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            APB3 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_MBOOST                     = 1
  *            PLL_M                          = 1
  *            PLL_N                          = 80
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable voltage range 1 for frequency above 100 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Switch to SMPS regulator instead of LDO */
  HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY);

  __HAL_RCC_PWR_CLK_DISABLE();

  /* MSI Oscillator enabled at reset (4Mhz), activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLFRACN= 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_PCLK3);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  Enable ICACHE with 1-way set-associative configuration.
  * @param  None
  * @retval None
  */
static void CACHE_Enable(void)
{
  /* Configure ICACHE associativity mode */
  HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);

  /* Enable ICACHE */
  HAL_ICACHE_Enable();
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void FFT_GenerateDemoSignal(float *buffer, uint32_t sample_count, float sample_rate_hz)
{
  uint32_t sample_index;

  for (sample_index = 0U; sample_index < sample_count; sample_index++)
  {
    float time_s = (float)sample_index / sample_rate_hz;
    buffer[sample_index] =
      (1.7f * sinf(2.0f * FFT_PLOT_PI * 1000.0f * time_s)) +
      (1.3f * sinf(2.0f * FFT_PLOT_PI * 1500.0f * time_s));
  }
}

static void FFT_Run(float *real, float *imaginary, uint32_t fft_size)
{
  uint32_t index;
  uint32_t reversed_index = 0U;
  uint32_t bit_mask;

  for (index = 1U; index < fft_size; index++)
  {
    float temp;

    bit_mask = fft_size >> 1U;
    while ((reversed_index & bit_mask) != 0U)
    {
      reversed_index &= ~bit_mask;
      bit_mask >>= 1U;
    }
    reversed_index |= bit_mask;

    if (index < reversed_index)
    {
      temp = real[index];
      real[index] = real[reversed_index];
      real[reversed_index] = temp;

      temp = imaginary[index];
      imaginary[index] = imaginary[reversed_index];
      imaginary[reversed_index] = temp;
    }
  }

  for (bit_mask = 2U; bit_mask <= fft_size; bit_mask <<= 1U)
  {
    uint32_t half_size = bit_mask >> 1U;
    float phase_step = (-2.0f * FFT_PLOT_PI) / (float)bit_mask;
    uint32_t block_start;

    for (block_start = 0U; block_start < fft_size; block_start += bit_mask)
    {
      uint32_t butterfly_index;

      for (butterfly_index = 0U; butterfly_index < half_size; butterfly_index++)
      {
        uint32_t even_index = block_start + butterfly_index;
        uint32_t odd_index = even_index + half_size;
        float angle = phase_step * (float)butterfly_index;
        float twiddle_real = cosf(angle);
        float twiddle_imaginary = sinf(angle);
        float temp_real = (twiddle_real * real[odd_index]) -
                          (twiddle_imaginary * imaginary[odd_index]);
        float temp_imaginary = (twiddle_real * imaginary[odd_index]) +
                               (twiddle_imaginary * real[odd_index]);

        real[odd_index] = real[even_index] - temp_real;
        imaginary[odd_index] = imaginary[even_index] - temp_imaginary;
        real[even_index] += temp_real;
        imaginary[even_index] += temp_imaginary;
      }
    }
  }
}

static void FFT_PrintInputSignal(const float *samples, uint32_t sample_count)
{
  uint32_t sample_index;

  for (sample_index = 0U; sample_index < sample_count; sample_index++)
  {
    printf("input:%.6f\r\n", samples[sample_index]);
  }
}

static void FFT_PrintSpectrum(const float *samples, uint32_t sample_count)
{
  uint32_t bin_index;
  uint32_t usable_count = sample_count;

  if (usable_count > FFT_PLOT_SIZE)
  {
    usable_count = FFT_PLOT_SIZE;
  }

  memset(g_fft_demo_real, 0, sizeof(g_fft_demo_real));
  memset(g_fft_demo_imaginary, 0, sizeof(g_fft_demo_imaginary));
  memcpy(g_fft_demo_real, samples, usable_count * sizeof(float));

  FFT_Run(g_fft_demo_real, g_fft_demo_imaginary, FFT_PLOT_SIZE);

  for (bin_index = 0U; bin_index <= (FFT_PLOT_SIZE / 2U); bin_index++)
  {
    float magnitude = sqrtf((g_fft_demo_real[bin_index] * g_fft_demo_real[bin_index]) +
                            (g_fft_demo_imaginary[bin_index] * g_fft_demo_imaginary[bin_index])) /
                      (float)FFT_PLOT_SIZE;

    if ((bin_index != 0U) && (bin_index != (FFT_PLOT_SIZE / 2U)))
    {
      magnitude *= 2.0f;
    }

    printf("fft:%.6f\r\n", magnitude);
  }
}

#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void Error_Handler(void)
{
  BSP_LED_On(LED_GREEN);
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
