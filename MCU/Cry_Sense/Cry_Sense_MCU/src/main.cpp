#include <mbed.h>
#define I2S_WS_PIN GPIO_PIN_15 // PA15
#define I2S_CK_PIN GPIO_PIN_10 // PC10
#define I2S_SD_PIN GPIO_PIN_12 // PC12

// #define I2S_WS_PIN PA_15// PA15
// #define I2S_CK_PIN PC_10 // PC10
// #define I2S_SD_PIN PC_12 // PC12

#define BUFFER_SIZE 4096

uint32_t audio_buffer[BUFFER_SIZE];
bool dma_complete = false;

GPIO_InitTypeDef LED_TypeDef;
GPIO_InitTypeDef I2S_SEL_TypeDef;
GPIO_InitTypeDef gpios_init;

I2S_InitTypeDef I2S_TypeDef;
__I2S_HandleTypeDef I2S_Handle;

DMA_InitTypeDef DMA1_TypeDef;
__DMA_HandleTypeDef DMA_Handle;
// Initialize GPIO for SEL signal of the microphone,
// Set to LOW (Left Channel) PC11

// DMA ISR

void XferCpltCallback(DMA_HandleTypeDef *DMA_Handle)
{
  dma_complete = true;
}

extern "C" __attribute__((weak)) void DMA1_Stream0_IRQHandler(void)
{
  dma_complete = true;

  HAL_DMA_IRQHandler(&DMA_Handle);
}

void I2S_Init()
{
  // Configure the clock
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    printf("RCCEX_CLKCONFIG FAILE\n\r");
  }
  // a. Enable SPI3 Clock
  __HAL_RCC_SPI3_CLK_ENABLE();

  // b.1 Enable Clock for I2S GPIOs
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  // set GPIO Pins for I2S interface

  // Set WS pin (PortA)
  gpios_init.Pin = I2S_WS_PIN;
  gpios_init.Mode = GPIO_MODE_AF_PP;
  gpios_init.Pull = GPIO_NOPULL;
  gpios_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpios_init.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOA, &gpios_init);
  // Set clk and sd (PortC)
  gpios_init.Pin = I2S_CK_PIN | I2S_SD_PIN;
  gpios_init.Mode = GPIO_MODE_AF_PP;
  gpios_init.Pull = GPIO_NOPULL;
  gpios_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpios_init.Alternate = GPIO_AF6_SPI3; //*****diff*****//
  HAL_GPIO_Init(GPIOC, &gpios_init);

  // Set I2S settings
  I2S_TypeDef.Mode = I2S_MODE_MASTER_RX;
  I2S_TypeDef.Standard = I2S_STANDARD_MSB; // CHANGE PHIL TO MSB
  I2S_TypeDef.DataFormat = I2S_DATAFORMAT_24B;
  I2S_TypeDef.AudioFreq = I2S_AUDIOFREQ_48K;
  I2S_TypeDef.ClockSource = I2S_CLOCK_PLL;
  I2S_TypeDef.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  I2S_TypeDef.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  I2S_TypeDef.CPOL = I2S_CPOL_LOW;

  I2S_Handle.Instance = SPI3;
  I2S_Handle.Init = I2S_TypeDef;
  HAL_I2S_MspInit(&I2S_Handle);
  if (HAL_I2S_Init(&I2S_Handle) != HAL_OK)
  {
    printf("\n\rI2S Init Failed\n\r");
  }
  else
  {
    printf("I2S Init Successful\n\r");
  }

  __HAL_I2S_ENABLE(&I2S_Handle);
}

// DMA1 Initialize
void DMA_Init()
{
  // enable DMA clock
  __HAL_RCC_DMA1_CLK_ENABLE();

  DMA1_TypeDef.Channel = DMA_CHANNEL_0;
  DMA1_TypeDef.Direction = DMA_PERIPH_TO_MEMORY;
  DMA1_TypeDef.PeriphInc = DMA_PINC_DISABLE;
  DMA1_TypeDef.MemInc = DMA_MINC_ENABLE;
  DMA1_TypeDef.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  DMA1_TypeDef.MemDataAlignment = DMA_MDATAALIGN_WORD;
  DMA1_TypeDef.Mode = DMA_CIRCULAR;
  DMA1_TypeDef.Priority = DMA_PRIORITY_VERY_HIGH;
  DMA1_TypeDef.FIFOMode = DMA_FIFOMODE_DISABLE;
  DMA_Handle.Instance = DMA1_Stream0;
  DMA_Handle.Init = DMA1_TypeDef;

  // Enable NVIC

  if (HAL_DMA_Init(&DMA_Handle) != HAL_OK)
  {
    printf("DMA Init Failed\n\r");
  }
  else
  {
    printf("DMA Init Successful\n\r");
  }

  __HAL_LINKDMA(&I2S_Handle, hdmarx, DMA_Handle);
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  HAL_DMA_RegisterCallback(&DMA_Handle, HAL_DMA_XFER_CPLT_CB_ID, XferCpltCallback);
  __HAL_DMA_ENABLE(&DMA_Handle);
  // put your setup code here, to run once:

  if (HAL_I2S_Receive_DMA(&I2S_Handle, (uint16_t *)audio_buffer, BUFFER_SIZE) != HAL_OK)
  {
    printf("I2S DMA Receive Failed\n\r");
  }
  else
  {
    printf("I2S DMA Receive Successful\n\r");
  }
}

int main()
{
  HAL_Init();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  __HAL_RCC_GPIOG_CLK_ENABLE();

  I2S_Init();
  // DMA_Init();

  while (1)
  {

    // put your main code here, to run repeatedly:
    if (HAL_I2S_Receive(&I2S_Handle, (uint16_t *)audio_buffer, BUFFER_SIZE, 200) == HAL_OK)
    {
      ThisThread::sleep_for(200ms);
      // printf("Received data:\r\n");
      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        printf("%d\n\r ", audio_buffer[i]);
      }
    }
    else
    {
      printf("I2S Receive Failed\r\n");
    }
    // ThisThread::sleep_for(200ms);

    /*
if(dma_complete==true){
  printf("dma_ready: %u",dma_complete);
  dma_complete = false;
  for(int i=0;i<(sizeof(audio_buffer)/sizeof(uint32_t));i++){
    printf("%u",audio_buffer[i]);
  }

}
*/
  }
}
