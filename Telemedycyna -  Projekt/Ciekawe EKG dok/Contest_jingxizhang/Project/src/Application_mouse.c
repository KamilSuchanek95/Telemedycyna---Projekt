/************************* (C) COPYRIGHT 2007 RAISONANCE **********************
* File Name          :  Application.c
* Author             :
* Date First Issued  :
* Description        :  Circle_App CircleOS application template.
* Revision           :
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "circle_api.h"
#include "hw_config.h"
#include "stm32f10x_lib.h"

/* Private defines -----------------------------------------------------------*/

#define NEEDEDVERSION "V 1.6" // Put here the minimal CircleOS version needed by your application

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
enum MENU_code MsgVersion(void);

/* Public variables ----------------------------------------------------------*/

const char Application_Name[8+1] = {"MOUSE"};  // max 8 characters for application name

extern void USB_Istr ( void ) ;
enum MENU_code Application_Handler ( void );
unsigned counter = 0;
void DummyFct ( void ) 
   { 
   counter++;
   }

/*******************************************************************************
* Function Name  : Application_Ini
* Description    : Initialization function of Circle_App. This function will
*                  be called only once by CircleOS.
* Input          : None
* Return         : MENU_CONTINUE_COMMAND
*******************************************************************************/
tHandler OldHandler;
enum MENU_code Application_Ini ( void )
   {
   NVIC_InitTypeDef NVIC_InitStructure;

   if(strcmp(UTIL_GetVersion(), NEEDEDVERSION) < 0)
      {
      return MsgVersion();
      }

#define ID_IRQ_USB_LP  (0x90)
      
   //Set IRQ Handler
   OldHandler = (tHandler)UTIL_GetIrqHandler( ID_IRQ_USB_LP );
   UTIL_SetIrqHandler(ID_IRQ_USB_LP, (u32)USB_Istr);

   //USB Initialization
   Set_System();  
   USB_Interrupts_Config();
   Set_USBClock();
   USB_Init();
   
   //Unfortunately, there is no way to disconnect the cable on 
   //the STM32 Primer. Therefore, it will be necessary to 
   //physically disconnect-reconnect the cable.
   DRAW_SetCharMagniCoeff(2);
   DRAW_DisplayString(0,90," *MOUSE*",5);  
   DRAW_SetCharMagniCoeff(1);
   DRAW_DisplayString(5,70," Disconnect and  ",17);
   DRAW_DisplayString(5,55," reconnect the   ",17);
   DRAW_DisplayString(5,40," STM32 USB Cable ",17);
   DRAW_DisplayString(5,25,"(near the button)",17); 
      
   return MENU_CONTINUE_COMMAND;
   }

/*******************************************************************************
* Function Name  : Application_Handler
* Description    : Management of the Circle_App.
*
* Input          : None
* Return         : MENU_CONTINUE
*******************************************************************************/
enum MENU_code Application_Handler ( void )
   {
   void Orientation_Send ( void ) ;
   void USB_Interrupts_Stop ( void ) ;
      
   if ( BUTTON_GetState() == BUTTON_PUSHED )
      {
      BUTTON_WaitForRelease();
      BUTTON_SetMode  ( BUTTON_ONOFF_FORMAIN );

      //Stop the USB IRQ
      USB_Interrupts_Stop ( ) ;
      
      //Restore the original IRQ vector
      UTIL_SetIrqHandler( ID_IRQ_USB_LP, OldHandler );
      return MENU_Quit();
      }
   
   Orientation_Send();

   return MENU_CONTINUE;   // Returning MENU_LEAVE will quit to CircleOS
   }

/*******************************************************************************
* Function Name  : MsgVersion
* Description    : Display the current CircleOS version and the version needed
*                  exit to main menu after 4 secondes
*
* Input          : None
* Return         : MENU_CONTINUE
*******************************************************************************/
enum MENU_code MsgVersion(void)
   {
   int hh,mm,ss,ss2;
   
   DRAW_DisplayString(5,60,"CircleOS",17);
   DRAW_DisplayString(80,60,UTIL_GetVersion(),3);
   DRAW_DisplayString(5,34,NEEDEDVERSION,3);
   DRAW_DisplayString(40,34," required",12);
   
   RTC_GetTime(&hh,&mm,&ss);
   ss = ss + 4;                  // 4 secondes 
   ss = ss%60;
   
   do
      {
      RTC_GetTime(&hh,&mm,&ss2);
      }while (ss2 != ss);           // do while < 4 secondes
   
   return MENU_REFRESH;
   }

