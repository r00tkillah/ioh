/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         AdcStatus MIB - Implementation
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
#include "MibCommonDebug.h"
#include "Table.h"
#include "MibAdcStatus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_ADC_BITS 	   		  		        (12)
#define MIB_ADC_READ_RANGE	       (1<<MIB_ADC_BITS)
#define MIB_ADC_READ_MAX	  (MIB_ADC_READ_RANGE-1)
#define MIB_ADC_TEMP_CALIBRATE   				200	/* 20C */
#define MIB_ADC_TEMP_OSC_PULL   			   1050 /* 105C */
#define MIB_ADC_TEMP_OSC_PUSH				   1030	/* 103C */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibAdcStatus *psMibAdcStatus;				 /* Nwk Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibAdcStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vInit( thJIP_Mib       hMibAdcStatusInit,
								tsMibAdcStatus *psMibAdcStatusInit,
								uint8 			u8AdcMask,
								uint8 			u8OscPin,
								uint8 			u8Period)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatus_vInit() {%d}", sizeof(tsMibAdcStatus));

	/* Valid data pointer ? */
	if (psMibAdcStatusInit != (tsMibAdcStatus *) NULL)
	{
		/* Take copy of pointer to data */
		psMibAdcStatus = psMibAdcStatusInit;

		/* Take a copy of the MIB handle */
		psMibAdcStatus->hMib = hMibAdcStatusInit;

		/* Default temporary status data */
		psMibAdcStatus->sTemp.u8Mask = u8AdcMask;
		psMibAdcStatus->i16ChipTemp = 32767;
		psMibAdcStatus->i16CalTemp  = 32767;

		/* Initialise other data */
		if (u8Period < MIB_ADC_ADCS) u8Period = MIB_ADC_ADCS;
		psMibAdcStatus->u8Period 	= u8Period;
		psMibAdcStatus->u8Interval  = u8Period / MIB_ADC_ADCS;

		/* Initialise the Analogue Peripherals */
		vAHI_ApConfigure(E_AHI_AP_REGULATOR_ENABLE,
						 E_AHI_AP_INT_ENABLE,
						 E_AHI_AP_SAMPLE_8,
						 E_AHI_AP_CLOCKDIV_500KHZ,
						 E_AHI_AP_INTREF);

		/* Valid oscillator pin ? */
		if (psMibAdcStatus->u8OscPin < 21)
		{
			/* Push oscillator */
			MibAdcStatus_vOscillatorPull(FALSE);
		}
	}
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibAdcStatus->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatus_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\teJIP_RegisterMib(AdcStatus)=%d", eStatus);

	/* Configure table structure */
	psMibAdcStatus->sRead.pvData		= (void *) psMibAdcStatus->sTemp.au16Read;
	psMibAdcStatus->sRead.u32Size		= sizeof(uint16);
	psMibAdcStatus->sRead.u16Entries 	= MIB_ADC_ADCS;
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vTick
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vTick(void)
{
	/* Time to take a reading */
	if ((psMibAdcStatus->u32Tick % psMibAdcStatus->u8Interval) == 0)
	{
		/* Analogue domain is up ? */
		if (bAHI_APRegulatorEnabled())
		{
			/* Calculate which adc */
			psMibAdcStatus->u8Adc = (psMibAdcStatus->u32Tick % psMibAdcStatus->u8Period) / psMibAdcStatus->u8Interval;
			/* Valid adc source ? */
			if (psMibAdcStatus->u8Adc < MIB_ADC_ADCS)
			{
				/* Adc is enabled ? */
				if (psMibAdcStatus->sTemp.u8Mask & (1<<psMibAdcStatus->u8Adc))
				{
					/* Start reading */
					 vAHI_AdcEnable(E_AHI_ADC_SINGLE_SHOT, E_AHI_AP_INPUT_RANGE_2, psMibAdcStatus->u8Adc);
					 vAHI_AdcStartSample();
					/* Debug */
					DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatus_vTick()");
					DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\tvAHI_AdcStartSample(%d)", psMibAdcStatus->u8Adc);
				}
			}
		}
	}
	/* Increment tick counter */
	psMibAdcStatus->u32Tick++;
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_u8Analogue
 *
 * DESCRIPTION:
 * Called when analogue reading is complete
 *
 ****************************************************************************/
PUBLIC uint8 MibAdcStatus_u8Analogue(void)
{
	/* Note the reading */
	psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc] = u16AHI_AdcRead();
	/* Update the table hash value */
	psMibAdcStatus->sRead.u16Hash++;
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatus_u8Analogue()", psMibAdcStatus->u8Adc);
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\tu16AHI_AdcRead(%d)=%d", psMibAdcStatus->u8Adc, psMibAdcStatus->sTemp.au16Read[psMibAdcStatus->u8Adc]);

	/* Temperature reading ? */
	if (psMibAdcStatus->u8Adc == E_AHI_ADC_SRC_TEMP)
	{
		int16 i16Temp;

		/* Convert reading to temperature */
		i16Temp = MibAdcStatus_i16DeciCentigrade(psMibAdcStatus->u8Adc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\tMibAdcStatus_i16DeciCentigrade(%d)=%d", psMibAdcStatus->u8Adc, i16Temp);

		/* Don't yet have a radio calibration temperature */
		if (psMibAdcStatus->i16CalTemp == 32767)
		{
			/* Note current value */
			psMibAdcStatus->i16CalTemp = i16Temp;
		}
		/* Have a calibration value ? */
		else
		{
			int16 i16Diff;

			/* Calculate difference from last calibration value */
			if (i16Temp > i16Temp - psMibAdcStatus->i16CalTemp)
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
				DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\n\teAHI_AttemptCalibration()=%d", eCalStatus);
			}
		}

		/* Valid oscillator pin ? */
		if (psMibAdcStatus->u8OscPin < 21)
		{
			/* Got a pervious temperature reading ? */
			if (psMibAdcStatus->i16ChipTemp != 32767)
			{
				/* Do we need to pull the oscillator ? */
				if 		(psMibAdcStatus->i16ChipTemp <  MIB_ADC_TEMP_OSC_PULL &&
						 i16Temp 					 >= MIB_ADC_TEMP_OSC_PULL)
				{
					/* Pull oscillator */
					MibAdcStatus_vOscillatorPull(TRUE);
				}
				/* Do we need to push the oscillator ? */
				else if (psMibAdcStatus->i16ChipTemp >  MIB_ADC_TEMP_OSC_PUSH &&
						 i16Temp 					 <= MIB_ADC_TEMP_OSC_PUSH)
				{
					/* Push oscillator */
					MibAdcStatus_vOscillatorPull(FALSE);
				}
			}
		}

		/* Note current value */
		psMibAdcStatus->i16ChipTemp = i16Temp;
	}

	/* Return the source we just read */
	return psMibAdcStatus->u8Adc;
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_vOscillatorPull
 *
 * DESCRIPTION:
 * Handles oscillator pulling to cope with temperature changes
 *
 ****************************************************************************/
PUBLIC void MibAdcStatus_vOscillatorPull(bool_t bPull)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_ADC_STATUS, "\nMibAdcStatus_vOscillatorPull(%d)", bPull);

	/* Valid oscillator pin ? */
	if (psMibAdcStatus->u8OscPin < 21)
	{
		/* Pull ? */
		if (bPull)
		{
			/* Pull oscillator */
			vAHI_DioSetDirection(0, (1<<psMibAdcStatus->u8OscPin));
			vAHI_DioSetOutput	(0, (1<<psMibAdcStatus->u8OscPin));
		}
		/* Push ? */
		else
		{
			/* Push oscillator */
			vAHI_DioSetPullup   (0, (1<<psMibAdcStatus->u8OscPin));
			vAHI_DioSetDirection((1<<psMibAdcStatus->u8OscPin), 0);
		}
	}
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_u16Read
 *
 * DESCRIPTION:
 * Returns most recent specified analogue raw reading
 *
 ****************************************************************************/
PUBLIC uint16 MibAdcStatus_u16Read(uint8 u8Adc)
{
	return psMibAdcStatus->sTemp.au16Read[u8Adc];
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_i32Convert
 *
 * DESCRIPTION:
 * Returns most recent specified analogue reading converting it to specified
 * range.
 *
 ****************************************************************************/
PUBLIC int32 MibAdcStatus_i32Convert(uint8 u8Adc, int32 i32Min, int32 i32Max)
{
	int32 i32Return;

	/* Inverted ? */
	if (i32Min > i32Max)
	{
		/* Calculate conversion */
		i32Return = (((MIB_ADC_READ_MAX-(int32)psMibAdcStatus->sTemp.au16Read[u8Adc])*((i32Min-i32Max)+1))/MIB_ADC_READ_RANGE)+i32Max;
	}
	/* Not inverted ? */
	else
	{
		/* Calculate conversion */
		i32Return = (((int32)psMibAdcStatus->sTemp.au16Read[u8Adc]*((i32Max-i32Min)+1))/MIB_ADC_READ_RANGE)+i32Min;
	}

	/* Return reading converted to millivolts */
	return i32Return;
}

/****************************************************************************
 *
 * NAME: MibAdcStatus_i16DeciCentigrade
 *
 * DESCRIPTION:
 * Returns most recent specified analogue reading converted to DeciCentigrade
 * for on chip analogue source.
 *
 ****************************************************************************/
PUBLIC int16 MibAdcStatus_i16DeciCentigrade(uint8 u8Adc)
{
	return (int16) MibAdcStatus_i32Convert(u8Adc, MIB_ADC_CONVERT_DC_MIN, MIB_ADC_CONVERT_DC_MAX);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
