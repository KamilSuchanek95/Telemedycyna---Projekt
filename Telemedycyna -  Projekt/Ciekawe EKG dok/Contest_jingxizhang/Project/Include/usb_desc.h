/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Date First Issued  : 02/05/2007
* Description        : Descriptor Header for Joystick Mouse Demo
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
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define HID_DESCRIPTOR_TYPE                     0x21
#define ECG_SIZ_HID_DESC                        0x09
#define ECG_OFF_HID_DESC                   0x12

#define ECG_SIZ_DEVICE_DESC                     18
#define ECG_SIZ_CONFIG_DESC                     34
#define ECG_SIZ_REPORT_DESC                     21   //47 //74
#define ECG_SIZ_STRING_LANGID                   4
#define ECG_SIZ_STRING_VENDOR                   38
#define ECG_SIZ_STRING_PRODUCT                  32
#define ECG_SIZ_STRING_SERIAL                   26

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const u8 ECG_DeviceDescriptor[ECG_SIZ_DEVICE_DESC];
extern const u8 ECG_ConfigDescriptor[ECG_SIZ_CONFIG_DESC];
extern const u8 ECG_ReportDescriptor[ECG_SIZ_REPORT_DESC];
extern const u8 ECG_StringLangID[ECG_SIZ_STRING_LANGID];
extern const u8 ECG_StringVendor[ECG_SIZ_STRING_VENDOR];
extern const u8 ECG_StringProduct[ECG_SIZ_STRING_PRODUCT];
extern const u8 ECG_StringSerial[ECG_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
