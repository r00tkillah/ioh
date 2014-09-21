/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         AdcStatus MIB - Interface
 */
/****************************************************************************/
/*
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2012. All rights reserved
 */
/****************************************************************************/
#ifndef  MIBADCSTATUS_H_INCLUDED
#define  MIBADCSTATUS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_ADC_ADCS 			   6	/* Number of adcs */
#define MIB_ADC_MASK_SRC_1		   0x01	/* ADC1 source mask */
#define MIB_ADC_MASK_SRC_2		   0x02	/* ADC2 source mask */
#define MIB_ADC_MASK_SRC_3		   0x04	/* ADC3 source mask */
#define MIB_ADC_MASK_SRC_4		   0x08	/* ADC4 source mask */
#define MIB_ADC_MASK_SRC_TEMP	   0x10	/* ADC TEMP source mask */
#define MIB_ADC_MASK_SRC_VOLT	   0x20	/* ADC VOLT source mask */

#define MIB_ADC_CONVERT_DC_MIN  4909	/* DeciCentigrade when reading is 0 */
#define MIB_ADC_CONVERT_DC_MAX -9996	/* DeciCentigrade when reading is 4095 */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* ADC status temporary data */
typedef struct
{
	uint8    u8Mask;
	uint16 au16Read[MIB_ADC_ADCS];
}  tsMibAdcStatusTemp;

/* ADC status mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 			hMib;

	/* Variable handles */
	tsTable	    		sRead;

	/* Data structures */
	tsMibAdcStatusTemp 	sTemp;

	/* Other data */
	uint32  			u32Tick;		/* Tick counter */
	int16   			i16ChipTemp;	/* Chip temperature */
	int16   			i16CalTemp;		/* Calibration temperature */
	uint8   			 u8Period;		/* Period for all ADC readings */
	uint8   			 u8Interval;	/* Interval between individual ADC readings */
	uint8   			 u8Adc;			/* Current adc source */
	uint8   			 u8OscPin;		/* Oscillator pin */
} tsMibAdcStatus;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
/* Patched functions - use instead of unpatched functions */
PUBLIC void 			MibAdcStatusPatch_vInit(thJIP_Mib       hMibAdcStatusInit,
												tsMibAdcStatus *psMibAdcStatusInit,
												uint8 			u8AdcMask,
												uint8 			u8OscPin,
												uint8 			u8Period);
PUBLIC uint8 			MibAdcStatusPatch_u8Analogue(void);
PUBLIC int16			MibAdcStatusPatch_i16DeciCentigrade(uint8 u8Adc);

/* Unpatched functions */
PUBLIC void 			MibAdcStatus_vInit( thJIP_Mib       hMibAdcStatusInit,
											tsMibAdcStatus *psMibAdcStatusInit,
											uint8 			u8AdcMask,
											uint8 			u8OscPin,
											uint8 			u8Period);
PUBLIC void 			MibAdcStatus_vRegister(void);
PUBLIC void 			MibAdcStatus_vTick(void);
PUBLIC uint8 			MibAdcStatus_u8Analogue(void);
PUBLIC void 			MibAdcStatus_vOscillatorPull(bool_t bPull);
PUBLIC uint16			MibAdcStatus_u16Read(uint8 u8Adc);
PUBLIC int32 			MibAdcStatus_i32Convert(uint8 u8Adc, int32 i32Min, int32 i32Max);
PUBLIC int16			MibAdcStatus_i16DeciCentigrade(uint8 u8Adc);

#if defined __cplusplus
}
#endif

#endif  /* MIBADCSTATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
