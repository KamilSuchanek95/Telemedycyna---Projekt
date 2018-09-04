/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Date First Issued  : 02/05/2007
* Description        : All processings related to Joystick Mouse Demo
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

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 ProtocolValue;

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    ECG_init,
    ECG_Reset,
    ECG_Status_In,
    ECG_Status_Out,
    ECG_Data_Setup,
    ECG_NoData_Setup,
    ECG_Get_Interface_Setting,
    ECG_GetDeviceDescriptor,
    ECG_GetConfigDescriptor,
    ECG_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    ECG_GetConfiguration,
    ECG_SetConfiguration,
    ECG_GetInterface,
    ECG_SetInterface,
    ECG_GetStatus,
    ECG_ClearFeature,
    ECG_SetEndPointFeature,
    ECG_SetDeviceFeature,
    ECG_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (u8*)ECG_DeviceDescriptor,
    ECG_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (u8*)ECG_ConfigDescriptor,
    ECG_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR ECG_Report_Descriptor =
  {
    (u8 *)ECG_ReportDescriptor,
    ECG_SIZ_REPORT_DESC
  };

ONE_DESCRIPTOR Mouse_Hid_Descriptor =
  {
    (u8*)ECG_ConfigDescriptor + ECG_OFF_HID_DESC,
    ECG_SIZ_HID_DESC
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(u8*)ECG_StringLangID, ECG_SIZ_STRING_LANGID},
    {(u8*)ECG_StringVendor, ECG_SIZ_STRING_VENDOR},
    {(u8*)ECG_StringProduct, ECG_SIZ_STRING_PRODUCT},
    {(u8*)ECG_StringSerial, ECG_SIZ_STRING_SERIAL}
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ECG_init.
* Description    : Joystick Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ECG_init(void)
   {

  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  _SetISTR(0);               /* clear pending interrupts */
  wInterrupt_Mask = IMR_MSK;
  _SetCNTR(wInterrupt_Mask); /* set interrupts mask */
  pInformation->Current_Feature = ECG_ConfigDescriptor[7];
  /* Wait until device is configured */
  //FL0710 while (pInformation->Current_Configuration == 0)
  {
    NOP_Process();
  }

  bDeviceState = CONFIGURED;
}

/*******************************************************************************
* Function Name  : ECG_Reset.
* Description    : Joystick Mouse reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ECG_Reset(void)
{
  /* Set ECG_DEVICE as not configured */
  pInformation->Current_Configuration = 0;
  pInformation->Current_Interface = 0;/*the default Interface*/
  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_INTERRUPT);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  //SetEPTxCount(ENDP1, 4);
  SetEPTxCount(ENDP1, 120);
  SetEPRxStatus(ENDP1, EP_RX_DIS);
  SetEPTxStatus(ENDP1, EP_TX_NAK);

  /* Set this device to response on default address */
  SetDeviceAddress(0);
}

/*******************************************************************************
* Function Name  : ECG_Status_In.
* Description    : Joystick status IN routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ECG_Status_In(void)
{
}

/*******************************************************************************
* Function Name  : ECG_Status_Out
* Description    : Joystick status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ECG_Status_Out (void)
{
}

/*******************************************************************************
* Function Name  : ECG_Data_Setup
* Description    : Handle the data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT ECG_Data_Setup(u8 RequestNo)
{
  u8 *(*CopyRoutine)(u16);

  CopyRoutine = NULL;

  if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
      && (pInformation->USBwIndex0 == 0))
  {

    if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
    {
      CopyRoutine = ECG_GetReportDescriptor;
    }
    else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
    {
      CopyRoutine = ECG_GetHIDDescriptor;
    }

  } /* End of GET_DESCRIPTOR */

  /*** GET_PROTOCOL ***/
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
           && RequestNo == GET_PROTOCOL)
  {
    CopyRoutine = ECG_GetProtocolValue;
  }

  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : ECG_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT ECG_NoData_Setup(u8 RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_PROTOCOL))
  {
    return ECG_SetProtocol();
  }

  else
  {
    return USB_UNSUPPORT;
  }
}

/*******************************************************************************
* Function Name  : ECG_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *ECG_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : ECG_GetConfigDescriptor.
* Description    : Gets the configuration descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *ECG_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : ECG_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *ECG_GetStringDescriptor(u16 Length)
{
  u8 wValue0 = pInformation->USBwValue0;
  return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
}

/*******************************************************************************
* Function Name  : ECG_GetReportDescriptor.
* Description    : Gets the HID report descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *ECG_GetReportDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &ECG_Report_Descriptor);
}

/*******************************************************************************
* Function Name  : ECG_GetHIDDescriptor.
* Description    : Gets the HID descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *ECG_GetHIDDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Mouse_Hid_Descriptor);
}

/*******************************************************************************
* Function Name  : ECG_Get_Interface_Setting.
* Description    : tests the interface and the alternate setting according to the
*                  supported one.
* Input          : - Interface : interface number.
*                  - AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : USB_SUCCESS or USB_UNSUPPORT.
*******************************************************************************/
RESULT ECG_Get_Interface_Setting(u8 Interface, u8 AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 0)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : ECG_SetProtocol
* Description    : Joystick Set Protocol request routine.
* Input          : None.
* Output         : None.
* Return         : USB SUCCESS.
*******************************************************************************/
RESULT ECG_SetProtocol(void)
{
  u8 wValue0 = pInformation->USBwValue0;
  ProtocolValue = wValue0;
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : ECG_GetProtocolValue
* Description    : get the protocol value
* Input          : Length.
* Output         : None.
* Return         : address of the protcol value.
*******************************************************************************/
u8 *ECG_GetProtocolValue(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (u8 *)(&ProtocolValue);
  }
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
