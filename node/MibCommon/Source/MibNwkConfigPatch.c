/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkConfig MIB - Patches
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
#include "Config.h"
#include "MibCommonDebug.h"
#include "MibNwkConfig.h"
#include "MibNwkSecurity.h"
#include "MibNwkStatus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define STACK_MODE_STANDALONE  0x0001
#define STACK_MODE_COMMISSION  0x0002

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
PRIVATE tsMibNwkConfig *psMibNwkConfig;				 /* Nwk Config Mib data */
PRIVATE tsMibNwkStatus *psMibNwkStatus;				 /* Nwk Status Mib data */
PRIVATE bool_t 			 bStandaloneBeacon;
PRIVATE uint16         u16DeviceType = MK_JIP_DEVICE_TYPE;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkConfigPatch_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkConfigPatch_vInit(thJIP_Mib    	  hMibNwkConfigInit,
							   		tsMibNwkConfig  *psMibNwkConfigInit,
							   		void            *pvMibNwkStatusInit)
{
	/* Valid data pointer ? */
	if (psMibNwkConfigInit != (tsMibNwkConfig *) NULL)
	{
		/* Take copy of pointer to data */
		psMibNwkConfig = psMibNwkConfigInit;
		psMibNwkStatus = (tsMibNwkStatus *) pvMibNwkStatusInit;

		/* Call unpatched function */
		MibNwkConfig_vInit(hMibNwkConfigInit, psMibNwkConfigInit);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkConfigPatch_vMain
 *
 * DESCRIPTION:
 * Callback when processing beacon responses
 * Ignores responses from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkConfigPatch_bMain(void)
{
	bool_t bReturn;

	/* Note return value */
	bReturn = bStandaloneBeacon;

	/* Swapped to standalone mode due to a beacon ? */
	if (bStandaloneBeacon == TRUE)
	{
		/* Set profile for standalone system */
		MibNwkSecurity_vSetProfile(TRUE);
		/* Use standalone commissioning key */
		MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_STANDALONE_COMMISSIONING);
		/* Clear flag */
		bStandaloneBeacon = FALSE;
	}

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfigPatch_vSetUserData
 *
 * DESCRIPTION:
 * Puts wanted network id into establish route requests and beacon responses
 *
 ****************************************************************************/
PUBLIC void MibNwkConfigPatch_vSetUserData(void)
{
	tsBeaconUserData sBeaconUserData;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfigPatch_vSetUserData()");

	/* Set up user data */
	sBeaconUserData.u32NetworkId  = psMibNwkConfig->sPerm.u32NetworkId;
	sBeaconUserData.u16DeviceType = MK_JIP_DEVICE_TYPE;

	/* Set beacon payload */
	vApi_SetUserBeaconBits((uint8 *) &sBeaconUserData);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\tvApi_SetUserBeaconBits(%x, %x)", sBeaconUserData.u32NetworkId, sBeaconUserData.u16DeviceType);
	/* Set up beacon response callback */
	vApi_RegBeaconNotifyCallback(MibNwkConfigPatch_bBeaconNotifyCallback);

	/* Set establish route payload */
	v6LP_SetUserData(sizeof(tsBeaconUserData), (uint8 *) &sBeaconUserData);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\tv6LP_SetUserData(%x, %x)", sBeaconUserData.u32NetworkId, sBeaconUserData.u16DeviceType);
	/* Set up establish route callback */
	v6LP_SetNwkCallback(MibNwkConfigPatch_bNwkCallback);

	/* Set device types */
	vJIP_SetDeviceTypes(1, &u16DeviceType);
}

/****************************************************************************
 *
 * NAME: MibNwkConfigPatch_bBeaconNotifyCallback
 *
 * DESCRIPTION:
 * Callback when processing beacon responses
 * Ignores responses from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkConfigPatch_bBeaconNotifyCallback(tsScanElement *psBeaconInfo,
                                       			 	  uint16 	   u16ProtocolVersion)
{
	bool_t bReturn;

	/* Call unpatched function */
	bReturn = MibNwkConfig_bBeaconNotifyCallback(psBeaconInfo, u16ProtocolVersion);

	/* Going to accept this response ? */
	if (bReturn)
	{
		/* Got a valid nwk status data pointer ? */
		if (psMibNwkStatus != NULL)
		{
			/* Joining for first time ? */
			if (psMibNwkStatus->sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
			{
				/* LQI below preferred level ? */
				if (psBeaconInfo->u8LinkQuality < CONFIG_PRF_BEACON_LQI)
				{
					/* Reject beacon */
					bReturn = FALSE;
				}
			}
			/* Re-joining ? */
			else
			{
				/* LQI below minimum level ? */
				if (psBeaconInfo->u8LinkQuality < CONFIG_MIN_BEACON_LQI)
				{
					/* Reject beacon */
					bReturn = FALSE;
				}
			}
		}
	}

	/* Still going to accept this repsonse ? */
	if (bReturn)
	{
		/* Is this a response from a device in standalone commissioning mode ? */
		if ((psBeaconInfo->u16StackMode & (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION)) == (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION))
		{
			/* Are we currently in gateway mode ? */
			if ((u16Api_GetStackMode() & STACK_MODE_STANDALONE) == 0)
			{
				/* Flag that we need to complete swapping to standalone mode */
				bStandaloneBeacon = TRUE;
				/* Swap to standalone commissioning mode */
				vApi_SetStackMode(STACK_MODE_STANDALONE | STACK_MODE_COMMISSION);
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\tvApi_SetStackMode(%x)", (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION));
			}
		}
	}

	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nBeacon {%x:%x %x %d %d %d %d %x %x %02x%02x%02x%02x %02x%02x %d} [%x] = %d",
		psBeaconInfo->sExtAddr.u32H,
		psBeaconInfo->sExtAddr.u32L,
		psBeaconInfo->u16PanId,
		psBeaconInfo->u16Depth,
		psBeaconInfo->u8Channel,
		psBeaconInfo->u8LinkQuality,
		psBeaconInfo->u8NumChildren,
		psBeaconInfo->u16StackMode,
		psBeaconInfo->u32DevType,
		psBeaconInfo->au8UserDefined[0],
		psBeaconInfo->au8UserDefined[1],
		psBeaconInfo->au8UserDefined[2],
		psBeaconInfo->au8UserDefined[3],
		psBeaconInfo->au8UserDefined[4],
		psBeaconInfo->au8UserDefined[5],
		psBeaconInfo->u8JoinProfile,
		u16Api_GetStackMode(),
		bReturn);

    return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_bNwkCallback
 *
 * DESCRIPTION:
 * Callback when processing establish route request
 * Rejects requests from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkConfigPatch_bNwkCallback(MAC_ExtAddr_s *psAddr,
 			                                 uint8 		   u8DataLength,
                            				 uint8 		 *pu8Data)
{
	bool_t             bReturn		  = FALSE;
	tsEstablishRouteUserData *psEstablishRouteUserData;

	/* Take a user data pointer to the data */
	psEstablishRouteUserData = (tsEstablishRouteUserData *) pu8Data;

	/* There is at least a uint32 in the data ? */
    if (u8DataLength >= sizeof(uint32))
    {
        /* Does it match the network ID we are using ? */
        if (psEstablishRouteUserData->u32NetworkId == psMibNwkConfig->sPerm.u32NetworkId)
        {
			/* Allow the establish route */
            bReturn = TRUE;
        }
    }

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfigPatch_bNwkCallback(%d, %x)=%d", u8DataLength, psEstablishRouteUserData->u32NetworkId, bReturn);

    return bReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
