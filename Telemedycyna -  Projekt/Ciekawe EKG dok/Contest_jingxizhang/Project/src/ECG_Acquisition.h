/******************************************************************************
 *
 * ECG_Acquisition.h
 * Jingxi Zhang
 * 
 * Header file for ECG data acquisition processing
 *
 ******************************************************************************/

#ifndef _ECG_ACQUISITION_H_
#define _ECG_ACQUISITION_H_


#define ADC1_DR_Address    ((u32)0x4001244C)

#define ECG_BUFF_SIZE             120
#define DATA_BUFF_SIZE            (ECG_BUFF_SIZE/2)
#define AVERAGE_BIN               4
#define DISPLAY_BUFF_SIZE         120
#define DISPLAY_BUFF_INCREMENT    (DATA_BUFF_SIZE/AVERAGE_BIN)

extern u16 *pDataBuff;

// Function declare
void ECGInit(void);
void ECGReset(void);
void InitTIM1(void);
void ResetTIM1(void);
void InitDMA1(void);
void ResetDMA1(void);
void InitADC(void);
void ResetADC(void);
void InitGPIO(void);
void ResetGPIO(void);
void DMA1_Interrupts_Stop(void);


#endif //_ECG_ACQUISITION_H_
