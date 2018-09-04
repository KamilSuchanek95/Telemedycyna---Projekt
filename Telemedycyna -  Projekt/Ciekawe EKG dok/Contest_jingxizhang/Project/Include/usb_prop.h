/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Date First Issued  : 02/05/2007
* Description        : All processings related to Joystick Mouse demo
********************************************************************************
* History:
* 05/21/2007: V0.3
* 04/02/2007: V0.2
* 02/05/2007: V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _HID_REQUESTS
{
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,

  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
} HID_REQUESTS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void ECG_init(void);
void ECG_Reset(void);
void ECG_Status_In (void);
void ECG_Status_Out (void);
RESULT ECG_Data_Setup(u8);
RESULT ECG_NoData_Setup(u8);
RESULT ECG_Get_Interface_Setting(u8 Interface, u8 AlternateSetting);
u8 *ECG_GetDeviceDescriptor(u16 );
u8 *ECG_GetConfigDescriptor(u16);
u8 *ECG_GetStringDescriptor(u16);
RESULT ECG_SetProtocol(void);
u8 *ECG_GetProtocolValue(u16 Length);
RESULT ECG_SetProtocol(void);
u8 *ECG_GetReportDescriptor(u16 Length);
u8 *ECG_GetHIDDescriptor(u16 Length);

/* Exported define -----------------------------------------------------------*/
#define ECG_GetConfiguration          NOP_Process
#define ECG_SetConfiguration          NOP_Process
#define ECG_GetInterface              NOP_Process
#define ECG_SetInterface              NOP_Process
#define ECG_GetStatus                 NOP_Process
#define ECG_ClearFeature              NOP_Process
#define ECG_SetEndPointFeature        NOP_Process
#define ECG_SetDeviceFeature          NOP_Process
#define ECG_SetDeviceAddress          NOP_Process

#define REPORT_DESCRIPTOR                  0x22

#endif /* __USB_PROP_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
