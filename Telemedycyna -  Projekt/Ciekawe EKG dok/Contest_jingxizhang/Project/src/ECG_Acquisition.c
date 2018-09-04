/******************************************************************************
 *
 * ECG_Acquisition.c
 * Jingxi Zhang
 * 
 * For ECG data acquisition processing
 *
 ******************************************************************************/
#include "circle_api.h"
#include "ECG_Acquisition.h"

#define COMPACT_SIZE
#define ID_IRQ_DMA1  (0x6C)

u16 ECG_Buff[ECG_BUFF_SIZE];
u16 *pDataBuff;
tHandler OldDMA1ISR;

void ECG_NVIC_Configuration(void);
void DMA1_ISR(void);

void ECGInit(void)
{
   // Enable GPIOA, GPIOC,, ADC1 and TIM1 clock
   RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_ADC1 | RCC_APB2Periph_TIM1;

//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
//                         RCC_APB2Periph_ADC1 | RCC_APB2Periph_TIM1, ENABLE);
   pDataBuff = 0;

   ECG_NVIC_Configuration();
   InitGPIO();
   InitDMA1();
   InitADC();
   InitTIM1();
}

void ECGReset(void)
{
   ResetTIM1();
   ResetADC();
   ResetDMA1();
   ResetGPIO();
   DMA1_Interrupts_Stop();
}

void InitTIM1(void)
{
   TIM1_TimeBaseInitTypeDef  TIM1_TimeBaseStructure;
   TIM1_OCInitTypeDef        TIM1_OCInitStructure;

//   TIM1_DeInit();
   enum eSpeed speed = UTIL_GetPll();
   int prescaleValue = 12;
   switch(speed)
   {
      case SPEED_VERY_LOW:
         prescaleValue = 5;         // prescale = 6
      break;
      
      case SPEED_LOW:
         prescaleValue = 7;         // prescale = 8
      break;
      
      case SPEED_MEDIUM:
         prescaleValue = 11;        // prescale = 12
      break;
      
      case SPEED_HIGH:
         prescaleValue = 15;        // prescale = 16
      break;
      
      case SPEED_VERY_HIGH:
         prescaleValue = 23;        // prescale = 24
      break;
   }

   // Time Base configuration
#ifdef COMPACT_SIZE
   TIM1->ARR = 0x186A;
   TIM1->PSC = prescaleValue;
   TIM1->RCR = 0;

   TIM1->CCMR1 = 0x60;
   TIM1->CCER = 0x03;
   TIM1->CCR1 = 0x0C35;

   TIM1->CR1 = 0x01;                // TIM1_Cmd(ENABLE)

   TIM1->BDTR = 0x8000;

   // *(vu32 *) BDTR_MOE_BB = (u16)ENABLE;

#else
   TIM1_TimeBaseStructure.TIM1_Prescaler = prescaleValue;
   TIM1_TimeBaseStructure.TIM1_CounterMode = TIM1_CounterMode_Up;
   TIM1_TimeBaseStructure.TIM1_Period = 0x186A;                   // = 6250 (generates 480/s)
   TIM1_TimeBaseStructure.TIM1_ClockDivision = 0x0;
   TIM1_TimeBaseStructure.TIM1_RepetitionCounter = 0x0;
   TIM1_TimeBaseInit(&TIM1_TimeBaseStructure);

   // Channel1 Configuration in PWM mode
   TIM1_OCInitStructure.TIM1_OCMode = TIM1_OCMode_PWM1; 
   TIM1_OCInitStructure.TIM1_OutputState = TIM1_OutputState_Enable;                
   TIM1_OCInitStructure.TIM1_OutputNState = TIM1_OutputNState_Disable;                  
   TIM1_OCInitStructure.TIM1_Pulse = 0x0C35; 
   TIM1_OCInitStructure.TIM1_OCPolarity = TIM1_OCPolarity_Low;         
   TIM1_OC1Init(&TIM1_OCInitStructure);

   // TIM1 counter enable 
   TIM1_Cmd(ENABLE);

   // TIM1 main Output Enable
   TIM1_CtrlPWMOutputs(ENABLE);  
#endif
}

void ResetTIM1(void)
{
   TIM1->CR1 = 0x00;                // TIM1_Cmd(DISABLE)
}

void InitDMA1(void)
{
#ifdef COMPACT_SIZE
   // Orig: DMA1 CCR1 = 0x5a1, CNDTR1=0x2, CPAR1=0x4001244C, CMAR1=0x200045fc
   DMA_Channel1->CCR &= 0xFFFFFFFE;
   DMA->IFCR |= 0x0000000F;

   //Set IRQ Handler
   OldDMA1ISR = (tHandler)UTIL_GetIrqHandler( ID_IRQ_DMA1 );
   UTIL_SetIrqHandler(ID_IRQ_DMA1, (u32)DMA1_ISR);  

   DMA_Channel1->CCR = 0x5a6;
   DMA_Channel1->CNDTR = ECG_BUFF_SIZE;
   DMA_Channel1->CPAR = ADC1_DR_Address;
   DMA_Channel1->CMAR = (u32)&ECG_Buff[0];
   DMA_Channel1->CCR |= 0x00000001;

#else
   DMA_InitTypeDef   DMA_InitStructure;

   // Reconfig the DMA channel 1
   DMA_DeInit( DMA_Channel1 );

   DMA_InitStructure.DMA_PeripheralBaseAddr  = ADC1_DR_Address;
   DMA_InitStructure.DMA_MemoryBaseAddr      = (u32)&ECG_Buff[0];
   DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize          = ECG_BUFF_SIZE;
   DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;
   DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;
   DMA_InitStructure.DMA_Mode                = DMA_Mode_Circular;
   DMA_InitStructure.DMA_Priority            = DMA_Priority_Low;
   DMA_InitStructure.DMA_M2M                 = DMA_M2M_Disable;

   DMA_Init( DMA_Channel1, &DMA_InitStructure );

   // Enable DMA Channel1 Transfer half and Complete interrupt
   DMA_ITConfig(DMA_Channel1, DMA_IT_TC | DMA_IT_HT, ENABLE);
   
   // Enable DMA Channel1
   DMA_Cmd( DMA_Channel1, ENABLE );
#endif

}

// reset the DMA1 back to original
void ResetDMA1(void)
{
   DMA_Channel1->CCR &= 0xFFFFFFFE;
   DMA->IFCR |= 0x0000000F;

   //Reset IRQ Handler
   UTIL_SetIrqHandler(ID_IRQ_DMA1, (u32)OldDMA1ISR);  

   DMA_Channel1->CCR = 0x5a1;
   DMA_Channel1->CNDTR = 8+1;
   DMA_Channel1->CPAR = 0x4001244c;
   DMA_Channel1->CMAR = 0x200045fc;
   DMA_Channel1->CCR |= 0x00000001;    // restart DMA1
}


void InitADC(void)
{
#ifdef COMPACT_SIZE
   ADC1->CR1 =    0x0;        //(0x100)
   ADC1->CR2 =    0x900101;   //(0x9e0103)
   ADC1->SMPR1 =  0x140000;
   ADC1->SMPR2 =  0x28000140; //(0x28000000)
   ADC1->SQR1 = 0x00;
   ADC1->SQR2 = 0x00;
   ADC1->SQR3 = 0x02;         //(0x800000, 0x4210, 0x21084209)

#else
   ADC_InitTypeDef   ADC_InitStructure;

   ADC_InitStructure.ADC_Mode                = ADC_Mode_Independent;
   ADC_InitStructure.ADC_ScanConvMode        = DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode  = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_T1_CC1;
   ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfChannel        = 1;

   ADC_Init( ADC1, &ADC_InitStructure );

   // ADC1 Regular Channel 2 Configuration 
   ADC_RegularChannelConfig( ADC1, ADC_Channel_2,  1,  ADC_SampleTime_55Cycles5);

   // Enable ADC1 DMA
   ADC_DMACmd(ADC1, ENABLE);

   // Enable ADC1 external trigger
   ADC_ExternalTrigConvCmd(ADC1, ENABLE);

   // Enable ADC1 
   ADC_Cmd(ADC1, ENABLE);
#endif
}

// Reset ADC back to original
void ResetADC(void)
{

   ADC1->CR1 =    0x100;
   ADC1->SMPR1 =  0x140000;
   ADC1->SMPR2 =  0x28000000;
   ADC1->SQR1 = 0x800000;
   ADC1->SQR2 = 0x4210;
   ADC1->SQR3 = 0x21084209; 
   ADC1->CR2 =    0x9e0103;
}


void InitGPIO(void)
{
#ifdef COMPACT_SIZE
  // Configure TIM1_CH1 (PA8) as alternate function push-pull
   GPIOA->CRH &= 0xFFFFFFF0;           // 0x88844449; (orig: 0x88844444)
   GPIOA->CRH |= 0x09;
  // Configure PA.02 (ADC Channel-2) as analog input
   GPIOA->CRL &= 0xFFFFF0FF;           // = 44444044; (orig: 0x44444444)
#else
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure PA.02 (ADC Channel-2) as analog input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

void ResetGPIO(void)
{
   GPIOA->CRH &= 0xFFFFFFF0;
   GPIOA->CRH |= 0x04;
   GPIOA->CRL |= 0x00000400;
}


void ECG_NVIC_Configuration(void)
{
#ifdef COMPACT_SIZE
   u32 tmppriority = 0x00, tmpreg = 0x00, tmpmask = 0x00;
   u32 tmppre = 0, tmpsub = 0x0F;

   NVIC_InitTypeDef NVIC_InitStruct;

   // Enable DMA1 IRQChannel 
   NVIC_InitStruct.NVIC_IRQChannel = DMAChannel1_IRQChannel;
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

   // Compute the Corresponding IRQ Priority    
    tmppriority = (0x700 - (SCB->AIRC & (u32)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;
    
    tmppriority = (u32)NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  NVIC_InitStruct.NVIC_IRQChannelSubPriority & tmpsub;

    tmppriority = tmppriority << 0x04;
    tmppriority = ((u32)tmppriority) << ((NVIC_InitStruct.NVIC_IRQChannel & (u8)0x03) * 0x08);
    
    tmpreg = NVIC->Priority[(NVIC_InitStruct.NVIC_IRQChannel >> 0x02)];
    tmpmask = (u32)0xFF << ((NVIC_InitStruct.NVIC_IRQChannel & (u8)0x03) * 0x08);
    tmpreg &= ~tmpmask;
    tmppriority &= tmpmask;  
    tmpreg |= tmppriority;

    NVIC->Priority[(NVIC_InitStruct.NVIC_IRQChannel >> 0x02)] = tmpreg;
    
    // Enable the Selected IRQ Channels --------------------------------------
    NVIC->Enable[(NVIC_InitStruct.NVIC_IRQChannel >> 0x05)] =
      (u32)0x01 << (NVIC_InitStruct.NVIC_IRQChannel & (u8)0x1F);

#else
//   NVIC_Init(&NVIC_InitStruct);
   NVIC_InitTypeDef NVIC_InitStructure;

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   NVIC_InitStructure.NVIC_IRQChannel = DMAChannel1_IRQChannel;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
#endif
}

void DMA1_Interrupts_Stop(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   NVIC_InitStructure.NVIC_IRQChannel = DMAChannel1_IRQChannel;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
   NVIC_Init(&NVIC_InitStructure);

   //Stop TIM1
   RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);  
}

void DMA1_ISR(void)
{
   if ((DMA->ISR & DMA_FLAG_HT1) != 0)             // half transfer
   //if(DMA_GetFlagStatus(DMA_FLAG_HT1))
   {
      pDataBuff = &ECG_Buff[0];
   }
   else if ((DMA->ISR & DMA_FLAG_TC1) != 0)        // complete transfer
   //else if(DMA_GetFlagStatus(DMA_FLAG_TC1))
   {
      pDataBuff = &ECG_Buff[DATA_BUFF_SIZE];
   }

   DMA->IFCR = DMA_FLAG_GL1;
   //DMA_ClearFlag(DMA_FLAG_GL1);
}

