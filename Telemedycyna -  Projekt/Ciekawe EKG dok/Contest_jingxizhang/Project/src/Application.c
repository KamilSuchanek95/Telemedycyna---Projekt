/************************* (C) COPYRIGHT 2007 RAISONANCE **********************
* File Name          :  Application.c
* Author             :
* Date First Issued  :
* Description        :  Circle_App CircleOS application template.
* Revision           :
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "circle_api.h"
#include "src\ECG_Acquisition.h"
#include "usb_lib.h"

/* Private defines -----------------------------------------------------------*/

#define NEEDEDVERSION "V 1.7" // Put here the minimal CircleOS version needed by your application
#define ID_IRQ_USB_LP  (0x90)

//#define CHEBYSHEV_2ndOrder_BANDPASS_FILTER
//#define CHEBYSHEV_LOWPASS_FILTER_30
#define CHEBYSHEV_LOWPASS_FILTER_40
//#define FLOATING_POINT_PROCESS

#define UpperThreshold           65                      // there is a historysis to avoid noise
#define LowerThreshold           45

#define ST7637_NORON             0x13
#define ST7637_CASET             0x2A
#define ST7637_RASET             0x2B
#define ST7637_RAMWR             0x2C
#define ST7637_RGBSET            0x2D
#define ST7637_RAMRD             0x2E
#define ST7637_SCRLAR            0x33
#define ST7637_MADCTR            0x36
#define ST7637_VSCSAD            0x37

#if defined CHEBYSHEV_LOWPASS_FILTER_30
// Chebyshev LP filter (4th order, 30Hz at sampling rate of 120 samples/sec) coeffiecients
#ifdef FLOATING_POINT_PROCESS
#define coef_a0   1 
#define coef_a1   4 
#define coef_a2   6
#define coef_b0   -0.1641503452
#define coef_b1   0.4023376691
#define coef_b2   -0.9100943707
#define coef_b3   0.5316388226
#else
#define coef_a0   4096        // a0 = 1   << 12
#define coef_a1   16384       // a1 = 4   << 14
#define coef_a2   24576       // a2 = 6
#define coef_b0   -672        // b0 = -0.1641503452
#define coef_b1   1648        // b1 = 0.4023376691
#define coef_b2   -3728       // b2 = -0.9100943707
#define coef_b3   2178        // b3 = 0.5316388226
#endif // NOT FLOATING_POINT_PROCESS

#elif defined CHEBYSHEV_LOWPASS_FILTER_40
// Chebyshev LP filter (4th order, 40Hz at sampling rate of 120 samples/sec) coeffiecients
#ifdef FLOATING_POINT_PROCESS
#define coef_a0   1 
#define coef_a1   4 
#define coef_a2   6
#define coef_b0   -0.1273076934
#define coef_b1   -0.1849074341
#define coef_b2   -0.9530268929
#define coef_b3   -0.9522761251
#else
#define coef_a0   65536       // a0 = 1   << 16
#define coef_a1   262144      // a1 = 4   << 18
#define coef_a2   393216      // a2 = 6
#define coef_b0   -8343       // b0 = -0.1273076934
#define coef_b1   -12118      // b1 = -0.1849074341
#define coef_b2   -62458      // b2 = -0.9530268929
#define coef_b3   -62408      // b3 = -0.9522761251
#endif // NOT FLOATING_POINT_PROCESS

#elif defined CHEBYSHEV_2ndOrder_BANDPASS_FILTER
// Chebyshev LP filter (2th order, 0.50Hz - 30Hz at sampling rate of 120 samples/sec) coeffiecients
#ifdef FLOATING_POINT_PROCESS
#define coef_a0   1
#define coef_a2   -2
#define coef_b0   -0.2743385344 
#define coef_b1   0.3111179745   
#define coef_b2   -0.7624115633 
#define coef_b3   1.7249341798  
#else
#define coef_a0   65536       // 4096        // 1
#define coef_a2   -131072     // 8192        // -2
#define coef_b0   -17979      // -2087       // -0.2743385344 
#define coef_b1   20389       // -904        // 0.3111179745 
#define coef_b2   -49965      // 5355        // -0.7624115633 
#define coef_b3   113045      // 2087        // 1.7249341798  
#endif
#endif


/* Private variables ---------------------------------------------------------*/

u16 DisplayBuff[DISPLAY_BUFF_SIZE];
u16 prevData;
long xv[5];
long yv[5];
short  scrollOffset;
tHandler OldHandler;
short HRInterval;
short HROffTime;
u8 HROnOffset;
u8 HR;
char strHR[4];
bool bShowThreshold = FALSE;
bool bHROn = FALSE;


/* Private functions ---------------------------------------------------------*/
enum MENU_code MsgVersion(void);
void DrawTrace(short offset);
void Filter( short *pSrc, short len);
void DetectHR();

/* Public variables ----------------------------------------------------------*/
extern void USB_Istr ( void ) ;

const char Application_Name[8+1] = {"ECG"};  // max 8 characters for application name

/*******************************************************************************
* Function Name  : Application_Ini
* Description    : Initialization function of Circle_App. This function will
*                  be called only once by CircleOS.
* Input          : None
* Return         : MENU_CONTINUE_COMMAND
*******************************************************************************/
enum MENU_code Application_Ini ( void )
   {
   if(strcmp(UTIL_GetVersion(), NEEDEDVERSION) < 0)
      {
      return MsgVersion();
      }
    
   UTIL_SetPll(SPEED_VERY_HIGH);
   
   // init ECG data acquisition
   ECGInit();
   prevData = 64;
   scrollOffset = 0;
 
   LCD_SetScreenOrientation(V9);

   // initialize the filter buffer
   xv[0] = xv[1] = xv[2] = xv[3] = xv[4] = 0x3F;
   yv[0] = yv[1] = yv[2] = yv[3] = yv[4] = 0x3F;
   
   //Set IRQ Handler
   OldHandler = (tHandler)UTIL_GetIrqHandler( ID_IRQ_USB_LP );
   UTIL_SetIrqHandler(ID_IRQ_USB_LP, (u32)USB_Istr);

   //USB Initialization
//   Set_System();  
   
   USB_Interrupts_Config();
   Set_USBClock();
   USB_Init();

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
   void USB_Interrupts_Stop ( void ) ;

   static int count = 0;

   // TODO: Write your application handling here.
   //       This routine will get called repeatedly by CircleOS, until we
   //       return MENU_LEAVE
    if( pDataBuff )
    {
      // Send the data to USB endpoint buffer
      UserToPMABufferCopy(pDataBuff, GetEPTxAddr(ENDP1), 120);
      // enable endpoint for transmission 
        SetEPTxValid(ENDP1);

      // Filter and copy the data to display buffer
      Filter(pDataBuff, DATA_BUFF_SIZE);
 
      // Erase previous HR string
      LCD_FillRect(scrollOffset, 111, DISPLAY_BUFF_INCREMENT, 15, RGB_WHITE );

      // scroll down
      LCD_SendLCDCmd(ST7637_SCRLAR);                // define scroll area
      LCD_SendLCDData(4);                           // TFA
      LCD_SendLCDData(120);                         // VSA
      LCD_SendLCDData(8);                          // BFA
      
      LCD_SendLCDCmd(ST7637_VSCSAD);                // Vertical Scroll Start Address
      LCD_SendLCDData(scrollOffset+DISPLAY_BUFF_INCREMENT);                // scroll
 
      DrawTrace(scrollOffset);
      
      // detect heart rate
      DetectHR();

      // draw threshold
      if( bShowThreshold )
         LCD_FillRect(scrollOffset, UpperThreshold, DISPLAY_BUFF_INCREMENT, 1, RGB_RED );
      bShowThreshold = !bShowThreshold;
      
      scrollOffset += DISPLAY_BUFF_INCREMENT;
      if( scrollOffset >= 120)
         scrollOffset -= 120;
     
      UTIL_uint2str(strHR, HR, 2, 0);
      DRAW_DisplayString(scrollOffset, 111, strHR, 2);
     
      pDataBuff = 0;
   }
      
   if (  BUTTON_GetState() == BUTTON_PUSHED )                                 // If button  is pushed then exit
   {      
      BUTTON_WaitForRelease();
      BUTTON_SetMode  ( BUTTON_ONOFF_FORMAIN );

      // Reset ECG functions
      ECGReset();
      // Stop scroll
      LCD_SendLCDCmd(ST7637_NORON);
   
      //Stop the USB IRQ
      USB_Interrupts_Stop ( ) ;
      
      //Restore the original IRQ vector
      UTIL_SetIrqHandler( ID_IRQ_USB_LP, OldHandler );
      return MENU_Quit();
   }

   return MENU_CONTINUE;   // Returning MENU_LEAVE will quit to CircleOS
   }

void DrawTrace(short offset)
{
      short i, end;
      short *pWork = &DisplayBuff[0];

      i = offset;
      end = i + DISPLAY_BUFF_INCREMENT;
      
      // Erase the area
      LCD_FillRect(i, 0, DISPLAY_BUFF_INCREMENT, 110, RGB_WHITE );
      
      // draw data
      for(; i<end; ++i)
      {
        DRAW_Line(i, prevData, i, *pWork, RGB_BLUE);
        prevData = *pWork++;
      }
}

void DetectHR()
{
   u8 i = 0;
   short *pWork = &DisplayBuff[0];

   if( !bHROn )                                          // detect QRS wave
   {
      LED_Set(LED_RED, LED_OFF);
      for(; i<DISPLAY_BUFF_INCREMENT; ++i)
      {
         ++HRInterval;
         if( *pWork++ >= UpperThreshold && HRInterval > 48 )   // HR must < 150/min
         {
            BUZZER_SetMode(BUZZER_SHORTBEEP);
            LED_Set(LED_RED, LED_ON);
            HR = 7200 / HRInterval;                     // 1 minute has 480 /4 * 60 = 7200 samples
            bHROn = TRUE;
            HRInterval = 0;
            break;
         }
      }
   }
   if( bHROn )                                           // make sure the QRS wave return below threshold
   {
      HRInterval += DISPLAY_BUFF_INCREMENT-i;            // HR interval must > DISPLAY_BUFF_INCREMENT
      for(; i<DISPLAY_BUFF_INCREMENT; ++i)
      {
         if( *pWork++ < LowerThreshold )
         {
            bHROn = FALSE;
            break;
         }
      }
   }
}


// Chebyshev Notch filter for 60Hz remover
// The filtered result are copied to display data buffer
void Filter( short *pSrc, short len)
{
      short *pDst = &DisplayBuff[0];

      // Average data
      short *pEnd = pSrc + len;
      short value;
      while(pSrc < pEnd)
      {
        // average 4 data
        value = *pSrc++;
        value += *pSrc++;
        value += *pSrc++;
        value += *pSrc++;
        // clamp to maximum
        if( value > 16384)
           value = 16384;
#ifdef CHEBYSHEV_LOWPASS_FILTER_30
        *pDst++ = value>>9;                          // convert data from range of 4096 to range of 128
#else
        *pDst++ = value>>7;                          // convert data from range of 4096 to range of 128
#endif
      }
   
#ifdef CHEBYSHEV_LOWPASS_FILTER_30
      short i;
      short val;
      len /= 4;
      pSrc = pDst = &DisplayBuff[0];
      for(i=0; i<len; ++i)
      {
        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; 
        xv[4] = *pSrc++;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
        yv[4] =  ( coef_a0 * (xv[0] + xv[4]) + coef_a1 * (xv[1] + xv[3]) + coef_a2* xv[2]
                     + ( coef_b0 * yv[0]) + (  coef_b1 * yv[1])
                     + ( coef_b2 * yv[2]) + (  coef_b3 * yv[3])
#ifdef FLOATING_POINT_PROCESS
                  );
#else
                  ) >> 12;
#endif
        val = yv[4] / 3 -10;
        if(val > 109)
           *pDst++ = 109;
        else if(val < 0)
           *pDst++ = 0;
        else
           *pDst++ = val;
      }
   
#elif defined CHEBYSHEV_LOWPASS_FILTER_40
      short i;
      short val;
      len /= 4;
      pSrc = pDst = &DisplayBuff[0];
      for(i=0; i<len; ++i)
      {
        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; 
        xv[4] = *pSrc++;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
        yv[4] =  ( coef_a0 * (xv[0] + xv[4]) + coef_a1 * (xv[1] + xv[3]) + coef_a2* xv[2]
                     + ( coef_b0 * yv[0]) + (  coef_b1 * yv[1])
                     + ( coef_b2 * yv[2]) + (  coef_b3 * yv[3])
#ifdef FLOATING_POINT_PROCESS
                  );
#else
                  ) >> 16;
#endif
        val = yv[4] / 3 -60;
        if(val > 109)
           *pDst++ = 109;
        else if(val < 0)
           *pDst++ = 0;
        else
           *pDst++ = val;
      }
   
   
#elif defined CHEBYSHEV_2ndOrder_BANDPASS_FILTER
      short i;
      short val;
      len /= 4;
      pSrc = pDst = &DisplayBuff[0];
      for(i=0; i<len; ++i)
      {
        xv[0] = xv[1]; 
        xv[1] = xv[2]; 
        xv[2] = xv[3]; 
        xv[3] = xv[4]; 
        xv[4] = *pSrc++;
        yv[0] = yv[1]; 
        yv[1] = yv[2]; 
        yv[2] = yv[3]; 
        yv[3] = yv[4]; 
        yv[4] =  ( coef_a0 *(xv[0] + xv[4]) + coef_a2 * xv[2]
                     + ( coef_b0 * yv[0]) + (  coef_b1 * yv[1])
                     + ( coef_b2 * yv[2]) + (  coef_b3 * yv[3])
#ifdef FLOATING_POINT_PROCESS
                  );
#else
                  ) >> 16; //12;
#endif
        val = (yv[4] /2);
        if(val > 109)
           *pDst++ = 109;
        else if(val < 0)
           *pDst++ = 0;
        else
           *pDst++ = val;
      }

#endif
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
