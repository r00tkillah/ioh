/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         AdcStatus MIB - Patches
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

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
#include <recal.h>
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "Config.h"
#include "MibCommonDebug.h"
#include "Table.h"
#include "MibAdcStatus.h"
/* Optional Application Includes */
//#include "Uart.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_ADC_BITS 	   		  		        (12)
#define MIB_ADC_READ_RANGE	       (1<<MIB_ADC_BITS)
#define MIB_ADC_READ_MAX	  (MIB_ADC_READ_RANGE-1)
#define MIB_ADC_TEMP_CALIBRATE   				200	/* 20C */
#define MIB_ADC_TEMP_OSC_PULL_48 			   1050 /* 105C */
#define MIB_ADC_TEMP_OSC_PUSH_48			   1030	/* 103C */
#define MIB_ADC_TEMP_OSC_PULL_LO 			    950 /*  95C */
#define MIB_ADC_TEMP_OSC_PUSH_LO			    930	/*  93C */
#define MIB_ADC_TEMP_OSC_PULL_HI 			   1100 /* 110C */
#define MIB_ADC_TEMP_OSC_PUSH_HI			   1080	/* 108C */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern uint32 u32Second;  		/* Second counter */

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC uint8			u8MibAdcStatusPatchOscillator;  /* Oscillator state */

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibAdcStatus *psMibAdcStatus;				 /* Nwk Status Mib data */

/* JN5148 or JN5148J01 ? */
#if (defined(JENNIC_CHIP_JN5148) || defined(JENNIC_CHIP_JN5148J01))
/* Others ? */
#else
/* Set up register pointer */
PRIVATE uint32 *pu32Test3V = ((uint32 *) 0x020000C8);
#endif

PRIVATE int16 i16DebugTemp = 0;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibAdcStatusPatch_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibAdcStatusPatch_vInit(thJIP_Mib       hMibAdcStatusInit,
									tsMibAdcStatus *psMibAdcStatusInit,
									uint8 			u8AdcMask,
									uint8 			u8OscPin,
									uint8 			u8Period)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatusPatch_vInit() {%d}", sizeof(tsMibAdcStatus));

	/* Valid data pointer ? */
	if (psMibAdcStatusInit != (tsMibAdcStatus *) NULL)
	{
		/* Inhibit oscillator handling during unpatched initialisation */
		psMibAdcStatusInit->u8OscPin = 0xff;
		/* Pass on to library function */
		MibAdcStatus_vInit(hMibAdcStatusInit,
						   psMibAdcStatusInit,
						   u8AdcMask,
						   u8OscPin,
						   u8Period);
		/* Note correct oscillator pin */
		psMibAdcStatusInit->u8OscPin = u8OscPin;
		/* Override old measurements with minimum possible values so oscillator is pulled is booting at high temperatures */
		psMibAdcStatusInit->i16ChipTemp = CONFIG_INT16_MIN;
		psMibAdcStatusInit->i16CalTemp  = CONFIG_INT16_MIN;
		/* JN5148 or JN5148J01 ? */
		#if (defined(JENNIC_CHIP_JN5148) || defined(JENNIC_CHIP_JN5148J01))
		{
			/* Push oscillator */
			MibAdcStatus_vOscillatorPull(FALSE);
		}
		/* Others ? */
		#else
		{
			/* Fully push oscillator - clear bits 20 and 21 of TEST3V register */
			U32_CLR_BITS(pu32Test3V, (3<<20));
		}
		#endif
		/* Take copy of pointer to data */
		psMibAdcStatus = psMibAdcStatusInit;
	}
}

/****************************************************************************
 *
 * NAME: MibAdcStatusPatch_u8Analogue
 *
 * DESCRIPTION:
 * Called when analogue reading is complete
 *
 ****************************************************************************/
PUBLIC uint8 MibAdcStatusPatch_u8Analogue(void)
{
	/* Note the reading */
	psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc] = u16AHI_AdcRead();
	/* JN5148 or JN5148J01 ? */
	#if (defined(JENNIC_CHIP_JN5148) || defined(JENNIC_CHIP_JN5148J01))
	{
		/* Do nothing */
	}
	/* JN5142J01 */
	#elif defined(JENNIC_CHIP_JN5142J01)
	{
		/* Left shift the conversion to scale from 8 to 12 bits */
		psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc] <<= 4;
	}
	#else
	{
		/* Left shift the conversion to scale from 10 to 12 bits */
		psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc] <<= 2;
	}
	#endif

	/* Update the table hash value */
	psMibAdcStatus->sRead.u16Hash++;
	/* Debug */
	//DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatusPatch_u8Analogue()", psMibAdcStatus->u8Adc);
	//DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\tu16AHI_AdcRead(%d)=%d", psMibAdcStatus->u8Adc, psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc]);

	/* Temperature reading ? */
	if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_TEMP)
	{
		int16 i16Temp;

		/* Convert reading to temperature */
		i16Temp = MibAdcStatusPatch_i16DeciCentigrade(psMibAdcStatus->u8Adc);
		if (i16DebugTemp - i16Temp >= 50 ||
		    i16DebugTemp - i16Temp <= -50)
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\tMibAdcStatusPatch_i16DeciCentigrade(%d)=%d", psMibAdcStatus->u8Adc, i16Temp);

			/* UART ? */
			#ifdef UART_H_INCLUDED
			{
				/* Debug */
				UART_vNumber(u32Second, 10);
				UART_vString(" TEMP @ ");
				UART_vNumber(i16Temp, 10);
				UART_vString("C\r\n");
			}
			#endif

			/* Note last debugged value */
			i16DebugTemp = i16Temp;
		}

		/* Don't yet have a radio calibration temperature */
		if (psMibAdcStatus->i16CalTemp == CONFIG_INT16_MIN)
		{
			/* Note current value (radio will have calibrated upon booting) */
			psMibAdcStatus->i16CalTemp = i16Temp;
		}
		/* Have a calibration value ? */
		else
		{
			int16 i16Diff;

			/* Calculate difference from last calibration value */
			if (i16Temp > psMibAdcStatus->i16CalTemp)
				i16Diff = i16Temp - psMibAdcStatus->i16CalTemp;
			else
				i16Diff = psMibAdcStatus->i16CalTemp - i16Temp;

			/* Need to recalibrate the radio ? */
			if (i16Diff >= MIB_ADC_TEMP_CALIBRATE)
			{
				teCalStatus	eCalStatus;

			   	/* Attempt calibration */
			   	eCalStatus = eAHI_AttemptCalibration();
			   	/* Success ? */
			   	if (eCalStatus == E_CAL_SUCCESS)
			   	{
			   		/* Note new calibration temperature */
			   		psMibAdcStatus->i16CalTemp = i16Temp;
				}
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\t%dC eAHI_AttemptCalibration()=%d", i16Temp, eCalStatus);

				/* UART ? */
				#ifdef UART_H_INCLUDED
				{
					/* Debug */
					UART_vNumber(u32Second, 10);
					UART_vString(" CALI @ ");
					UART_vNumber(i16Temp, 10);
					UART_vString("C\r\n");
				}
				#endif
			}
		}

		/* JN5148 or JN5148J01 ? */
		#if (defined(JENNIC_CHIP_JN5148) || defined(JENNIC_CHIP_JN5148J01))
		{
			/* Valid oscillator pin ? */
			if (psMibAdcStatus->u8OscPin < 21)
			{
				/* Do we need to pull the oscillator ? */
				if (i16Temp >= MIB_ADC_TEMP_OSC_PULL_48 && u8MibAdcStatusPatchOscillator != 0x1)
				{
					/* Note state */
					u8MibAdcStatusPatchOscillator = 0x1;
					/* Pull oscillator */
					MibAdcStatus_vOscillatorPull(TRUE);
					/* Debug */
					DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\t48 %dC to %dC PULL(%x)", psMibAdcStatus->i16ChipTemp, i16Temp, u8MibAdcStatusPatchOscillator);

					/* UART ? */
					#ifdef UART_H_INCLUDED
					{
						/* Debug */
						UART_vNumber(u32Second, 10);
						UART_vString(" OSCI @ ");
						UART_vNumber(i16Temp, 10);
						UART_vString("C = ");
						UART_vNumber(u8MibAdcStatusPatchOscillator, 2);
						UART_vString("\r\n");
					}
					#endif
				}
				/* Do we need to push the oscillator ? */
				else if (i16Temp <= MIB_ADC_TEMP_OSC_PUSH_48 && u8MibAdcStatusPatchOscillator != 0x0)
				{
					/* Note state */
					u8MibAdcStatusPatchOscillator = 0x0;
					/* Push oscillator */
					MibAdcStatus_vOscillatorPull(FALSE);
					/* Debug */
					DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\t48 %dC to %dC PUSH(%x)", psMibAdcStatus->i16ChipTemp, i16Temp, u8MibAdcStatusPatchOscillator);
					/* UART ? */
					#ifdef UART_H_INCLUDED
					{
						/* Debug */
						UART_vNumber(u32Second, 10);
						UART_vString(" OSCI @ ");
						UART_vNumber(i16Temp, 10);
						UART_vString("C = ");
						UART_vNumber(u8MibAdcStatusPatchOscillator, 2);
						UART_vString("\r\n");
					}
					#endif
				}
			}
		}
		/* Others ? */
		#else
		{
			/* Below low push point ? */
			if (i16Temp <= MIB_ADC_TEMP_OSC_PUSH_LO && u8MibAdcStatusPatchOscillator != 0x0)
			{
				/* Note state */
				u8MibAdcStatusPatchOscillator = 0x0;
				/* Fully push oscillator - clear bits 20 and 21 of TEST3V register */
				U32_CLR_BITS(pu32Test3V, (3<<20));
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\t42 %dC to %dC Pull(%x)", psMibAdcStatus->i16ChipTemp, i16Temp, u8MibAdcStatusPatchOscillator);
				/* UART ? */
				#ifdef UART_H_INCLUDED
				{
					/* Debug */
					UART_vNumber(u32Second, 10);
					UART_vString(" OSCI @ ");
					UART_vNumber(i16Temp, 10);
					UART_vString("C = ");
					UART_vNumber(u8MibAdcStatusPatchOscillator, 2);
					UART_vString("\r\n");
				}
				#endif
			}
			/* Above high pull point ? */
			else if	(i16Temp >= MIB_ADC_TEMP_OSC_PULL_HI && u8MibAdcStatusPatchOscillator != 0x3)
			{
				/* Note state */
				u8MibAdcStatusPatchOscillator = 0x3;
				/* Fully pull oscillator - set bits 20 and 21 of TEST3V register */
				U32_SET_BITS(pu32Test3V, (3<<20));
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\t42 %dC to %dC Pull(%x)", psMibAdcStatus->i16ChipTemp, i16Temp, u8MibAdcStatusPatchOscillator);
				/* UART ? */
				#ifdef UART_H_INCLUDED
				{
					/* Debug */
					UART_vNumber(u32Second, 10);
					UART_vString(" OSCI @ ");
					UART_vNumber(i16Temp, 10);
					UART_vString("C = ");
					UART_vNumber(u8MibAdcStatusPatchOscillator, 2);
					UART_vString("\r\n");
				}
				#endif
			}
			/* Below high push point or above low pull point ? */
			else if (i16Temp <= MIB_ADC_TEMP_OSC_PUSH_HI && i16Temp >= MIB_ADC_TEMP_OSC_PULL_LO && u8MibAdcStatusPatchOscillator != 0x1)
			{
				/* Note state */
				u8MibAdcStatusPatchOscillator = 0x1;
				/* Half pull oscillator - clear bit 20 and set bit 21 of TEST3V register */
				U32_CLR_BITS(pu32Test3V, (1<<20));
				U32_SET_BITS(pu32Test3V, (1<<21));
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\t42 %dC to %dC Pull(%x)", psMibAdcStatus->i16ChipTemp, i16Temp, u8MibAdcStatusPatchOscillator);
				/* UART ? */
				#ifdef UART_H_INCLUDED
				{
					/* Debug */
					UART_vNumber(u32Second, 10);
					UART_vString(" OSCI @ ");
					UART_vNumber(i16Temp, 10);
					UART_vString("C = ");
					UART_vNumber(u8MibAdcStatusPatchOscillator, 2);
					UART_vString("\r\n");
				}
				#endif
			}
		}
		#endif

		/* Note current value */
		psMibAdcStatus->i16ChipTemp = i16Temp;
	}

	/* Return the source we just read */
	return psMibAdcStatus->u8Adc;
}

/****************************************************************************
 *
 * NAME: MibAdcStatusPatch_i16DeciCentigrade
 *
 * DESCRIPTION:
 * Returns most recent specified analogue reading converted to DeciCentigrade
 * for on chip analogue source.
 *
 * dC = Typical dC - (((Reading12 - Typical12) * ScaledResolution) / 100)
 *
 ****************************************************************************/
PUBLIC int16 MibAdcStatusPatch_i16DeciCentigrade(uint8 u8Adc)
{
	int16 i16DeciCentigrade;

	/* JN514x Chip family ? */
	#ifdef JENNIC_CHIP_FAMILY_JN514x
	{
		i16DeciCentigrade = (int16) ((int32) 250 - ((((int32) psMibAdcStatus->sTemp.au16Read[u8Adc] - (int32) 1271) * (int32) 364) / (int32) 100));
	}
	/* Other chip family */
	#else
	{
		i16DeciCentigrade = (int16) ((int32) 250 - ((((int32) psMibAdcStatus->sTemp.au16Read[u8Adc] - (int32) 1244) * (int32) 353) / (int32) 100));
	}
	#endif


	return i16DeciCentigrade;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
