/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkProfile MIB - Implementation
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
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
#include <AccessFunctions.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "MibCommon.h"
#include "MibCommonDebug.h"
#include "MibNwkProfile.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

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
PRIVATE tsMibNwkProfile *psMibNwkProfile;			/* Nwk Config Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkProfile_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkProfile_vInit(thJIP_Mib        hMibNwkProfileInit,
							    tsMibNwkProfile  *psMibNwkProfileInit)
{
	/* Valid data pointer ? */
	if (psMibNwkProfileInit != (tsMibNwkProfile *) NULL)
	{
		PDM_teStatus   ePdmStatus;

		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\nMibNwkProfile_vInit() {%d}", sizeof(tsMibNwkProfile));

		/* Take copy of pointer to data */
		psMibNwkProfile = psMibNwkProfileInit;

		/* Take a copy of the MIB handle */
		psMibNwkProfile->hMib = hMibNwkProfileInit;

		/* Load NodeStatus mib data */
		ePdmStatus = PDM_eLoadRecord(&psMibNwkProfile->sDesc,
									 "MibNwkProfile",
									 (void *) &psMibNwkProfile->sPerm,
									 sizeof(psMibNwkProfile->sPerm),
									 FALSE);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkProfile_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNwkProfile->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\nMibNwkProfile_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\n\teJIP_RegisterMib(NwkProfile)=%d", eStatus);

	/* Make sure permament data is saved */
	psMibNwkProfile->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNwkProfile_vSecond(void)
{
	/* Need to save record ? */
	if (psMibNwkProfile->bSaveRecord)
	{
		/* Clear flag */
		psMibNwkProfile->bSaveRecord = FALSE;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNwkProfile->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\nMibNwkProfile_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\n\tPDM_vSaveRecord(MibNwkProfile)");
		/* Apply potential changes */
		MibNwkProfile_vApply();
	}
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_vApply
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNwkProfile_vApply(void)
{
	tsNwkProfile sNwkProfile;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\nMibNwkProfile_vApply()");
	/* Read network profile */
	vJnc_GetNwkProfile(&sNwkProfile);
	/* Update min beacon LQI */
	sNwkProfile.u8MinBeaconLQI 		= psMibNwkProfile->sPerm.u8MinBeaconLqi;
	/* In standalone mode ? */
	if (u16Api_GetStackMode() & MIB_COMMON_STACK_MODE_STANDALONE)
	{
		/* Zero profile values */
		sNwkProfile.u8MaxFailedPkts 	= 0;
		sNwkProfile.u16RouterPingPeriod = 0;
	}
	/* In gateway mode ? */
	else
	{
		/* Apply profile values */
		sNwkProfile.u8MaxFailedPkts 	= psMibNwkProfile->sPerm.u8MaxFailedPackets;
		sNwkProfile.u16RouterPingPeriod = psMibNwkProfile->sPerm.u16RouterPingPeriod;
	}
	/* Apply as user profiles */
	(void) bJnc_SetJoinProfile(PROFILE_USER, &sNwkProfile);
	(void) bJnc_SetRunProfile(PROFILE_USER, &sNwkProfile);
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_bCbScanSort
 *
 * DESCRIPTION:
 * Sorts incoming beacon repsonses into preferred order
 *
 ****************************************************************************/
 PUBLIC bool_t MibNwkProfile_bScanSortCallback(tsScanElement *pasScanResult,
                            			 uint8 			 u8ScanListSize,
                            			 uint8 		  *pau8ScanListOrder)
{
	bool_t bReturn = FALSE;

    uint8   i,n,tmp;
    bool_t  bSwapped;

	DBG_vPrintf(TRUE, "\nMibNwkProfile_bCbScanSortCallback(%d)", u8ScanListSize);

    if (u8ScanListSize > 0)
    {
		n = u8ScanListSize;

		/* simple bubblesort for the time being, since it's only a small list */
		do
		{
			bSwapped = FALSE;
			n = n - 1;

			for (i=0;i<n;i++)
			{
				/* sort order is depth, loading and link quality in that order */
				if (MibNwkProfile_bScanSortCheckSwap(pasScanResult, i, pau8ScanListOrder))
				{
					/* to make sort quicker we'll find the final order first,
					   then copy all the data later */
					tmp = pau8ScanListOrder[i];
					pau8ScanListOrder[i] = pau8ScanListOrder[i+1];
					pau8ScanListOrder[i+1] = tmp;

					bSwapped = TRUE;
				}
			}
		} while (bSwapped);

		/* Loop through entries */
		for (i = 0; i < u8ScanListSize; i++)
		{
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_PROFILE, "\nBcnSrt {%x:%x %x %d %d %d %d %x %x %x%x%x%x%x%x %d} [%x]",
				pasScanResult[pau8ScanListOrder[i]].sExtAddr.u32H,
				pasScanResult[pau8ScanListOrder[i]].sExtAddr.u32L,
				pasScanResult[pau8ScanListOrder[i]].u16PanId,
				pasScanResult[pau8ScanListOrder[i]].u16Depth,
				pasScanResult[pau8ScanListOrder[i]].u8Channel,
				pasScanResult[pau8ScanListOrder[i]].u8LinkQuality,
				pasScanResult[pau8ScanListOrder[i]].u8NumChildren,
				pasScanResult[pau8ScanListOrder[i]].u16StackMode,
				pasScanResult[pau8ScanListOrder[i]].u32DevType,
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[0],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[1],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[2],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[3],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[4],
				pasScanResult[pau8ScanListOrder[i]].au8UserDefined[5],
				pasScanResult[pau8ScanListOrder[i]].u8JoinProfile,
				u16Api_GetStackMode());
		}
	}

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_bScanSortCheckSwap
 *
 * DESCRIPTION:
 * Applies scan sort algorithm by checking if two beacons need to be reordered
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkProfile_bScanSortCheckSwap(tsScanElement *pasScanResult,
                                  				uint8 		    u8ScanListItem,
                                  				uint8 	     *pau8ScanListOrder)
{
    uint8 j,k;

    j = pau8ScanListOrder[u8ScanListItem];   // GNATS PR438
    k = pau8ScanListOrder[u8ScanListItem+1];

    /* LQI of both items is above preferred LQI ? */
    if (pasScanResult[j].u8LinkQuality >= psMibNwkProfile->sPerm.u8PrfBeaconLqi &&
        pasScanResult[k].u8LinkQuality >= psMibNwkProfile->sPerm.u8PrfBeaconLqi)
    {
		/* Sort by depth, children, LQI */
		if (pasScanResult[j].u16Depth > pasScanResult[k].u16Depth)
		{
			return TRUE;
		}
		else if ( (pasScanResult[j].u16Depth == pasScanResult[k].u16Depth) &&
				  (pasScanResult[j].u8NumChildren > pasScanResult[k].u8NumChildren) )
		{
			return TRUE;
		}
		else if ( (pasScanResult[j].u16Depth == pasScanResult[k].u16Depth) &&
				  (pasScanResult[j].u8NumChildren == pasScanResult[k].u8NumChildren) &&
				  (pasScanResult[j].u8LinkQuality < pasScanResult[k].u8LinkQuality) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
    /* LQI of both items is below preferred LQI ? */
    else if (pasScanResult[j].u8LinkQuality < (uint8) psMibNwkProfile->sPerm.u8PrfBeaconLqi &&
        	 pasScanResult[k].u8LinkQuality < (uint8) psMibNwkProfile->sPerm.u8PrfBeaconLqi)
    {
		/* Sort by LQI, children, depth */
		if (pasScanResult[j].u8LinkQuality < pasScanResult[k].u8LinkQuality)
		{
			return TRUE;
		}
		else if ( (pasScanResult[j].u8LinkQuality == pasScanResult[k].u8LinkQuality) &&
				  (pasScanResult[j].u8NumChildren > pasScanResult[k].u8NumChildren) )
		{
			return TRUE;
		}
		else if ( (pasScanResult[j].u8LinkQuality == pasScanResult[k].u8LinkQuality) &&
				  (pasScanResult[j].u8NumChildren == pasScanResult[k].u8NumChildren) &&
				  (pasScanResult[j].u16Depth > pasScanResult[k].u16Depth) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	/* LQI of first item is below preferred LQI while second is above ? */
	else if (pasScanResult[j].u8LinkQuality <  (uint8) psMibNwkProfile->sPerm.u8PrfBeaconLqi &&
        	 pasScanResult[k].u8LinkQuality >= (uint8) psMibNwkProfile->sPerm.u8PrfBeaconLqi)
    {
		return TRUE;
    }
    /* items are ok */
    else
    {
    	return FALSE;
    }
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_eSetUint8
 *
 * DESCRIPTION:
 * Generic set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkProfile_eSetUint8(uint8 u8Val, void *pvCbData)
{
	teJIP_Status eReturn;

	/* Call standard function */
	eReturn = eSetUint8(u8Val, pvCbData);

	/* Make sure permament data is saved */
	psMibNwkProfile->bSaveRecord = TRUE;

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkProfile_eSetUint16
 *
 * DESCRIPTION:
 * Generic set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkProfile_eSetUint16(uint16 u16Val, void *pvCbData)
{
	teJIP_Status eReturn;

	/* Call standard function */
	eReturn = eSetUint16(u16Val, pvCbData);

	/* Make sure permament data is saved */
	psMibNwkProfile->bSaveRecord = TRUE;

	return eReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
