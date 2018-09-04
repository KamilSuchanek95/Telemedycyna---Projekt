
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "circle_api.h"

u16 testBuff[60];

/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
   {
   GPIO_InitTypeDef GPIO_InitStructure;

   /* Select PLL as system clock source */
   RCC_SYSCLKConfig( RCC_SYSCLKSource_HSI );

   /* Enable PLL */
   RCC_PLLCmd( DISABLE );
   /* PLLCLK = 12MHz * 6 = 72 MHz */
   RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_6 );

   /* Enable PLL */
   RCC_PLLCmd( ENABLE );

   /* Wait till PLL is ready */
   while( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET )
      { ; }

   /* Select PLL as system clock source */
   RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

   /* Wait till PLL is used as system clock source */
   while( RCC_GetSYSCLKSource() != 0x08 )
      { ; }

   /* Enable GPIOE clock */
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);
   
   // Test
   u8 i=0;
   for(i=0; i<60; i++)
      {
      testBuff[i] = 69 *i;
      }
   }

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

  /* Enable USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;


  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //FL071018 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

   // Systick priority
   NVIC_SystemHandlerPriorityConfig( SystemHandler_SysTick, 6, 6 );
}


/*******************************************************************************
* Function Name  : USB_Interrupts_Stop.
* Description    : Stop the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Stop(void)
   {
   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //FL071018 0
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
   NVIC_Init(&NVIC_InitStructure);

   //Stop USB
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);  
   }


