/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkConfig MIB - Implementation
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
#include "MibNwkConfig.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_NWK_CONFIG_FRAME_COUNTER_DELTA_MIN 10

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
PRIVATE tsMibNwkConfig *psMibNwkConfig;			/* Nwk Config Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkConfig_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vInit(thJIP_Mib        hMibNwkConfigInit,
							   tsMibNwkConfig  *psMibNwkConfigInit)
{
	/* Valid data pointer ? */
	if (psMibNwkConfigInit != (tsMibNwkConfig *) NULL)
	{
		PDM_teStatus   ePdmStatus;

		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_vInit() {%d}", sizeof(tsMibNwkConfig));

		/* Take copy of pointer to data */
		psMibNwkConfig = psMibNwkConfigInit;

		/* Take a copy of the MIB handle */
		psMibNwkConfig->hMib = hMibNwkConfigInit;

		/* Load NodeStatus mib data */
		ePdmStatus = PDM_eLoadRecord(&psMibNwkConfig->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
									 "MibNwkConfig",
#else
									 (uint16)(MIB_ID_NWK_CONFIG & 0xFFFF),
#endif
									 (void *) &psMibNwkConfig->sPerm,
									 sizeof(psMibNwkConfig->sPerm),
									 FALSE);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNwkConfig->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\teJIP_RegisterMib(NwkConfig)=%d", eStatus);

	/* Make sure permament data is saved */
	psMibNwkConfig->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vSecond(void)
{
	/* Need to save record ? */
	if (psMibNwkConfig->bSaveRecord)
	{
		/* Clear flag */
		psMibNwkConfig->bSaveRecord = FALSE;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNwkConfig->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\tPDM_vSaveRecord(MibNwkConfig)");
	}
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_vJipInitData
 *
 * DESCRIPTION:
 * Initialises jip stack data
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vJipInitData(tsJIP_InitData *psJipInitData)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_vJipInitData()");

	/* Initialise stack settings from permanent data */
    psJipInitData->eDeviceType = psMibNwkConfig->sPerm.u8DeviceType;
	psJipInitData->u32Channel  = psMibNwkConfig->sPerm.u32ScanChannels;
    psJipInitData->u16PanId    = psMibNwkConfig->sPerm.u16PanId;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_vNetworkConfigData
 *
 * DESCRIPTION:
 * Sets network configuration data
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vNetworkConfigData(tsNetworkConfigData *psNetworkConfigData)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_vNetworkConfigData()");

	/* Initialse network settings */
	psNetworkConfigData->u32EndDeviceActivityTimeout	= psMibNwkConfig->sPerm.u32EndDeviceActivityTimeout;

	/* Set network ID */
	MibNwkConfig_vSetNetworkId();
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vStackEvent(te6LP_StackEvent eEvent)
{
	/* Which event ? */
    switch (eEvent)
    {
    	/* Joined network ? */
    	/* Started network ? */
		case E_STACK_JOINED:
		case E_STACK_STARTED:
		{
			/* Set network ID */
			MibNwkConfig_vSetNetworkId();
		}

		/* Others ? */
		default:
		{
			/* Do nothing */
			;
		}
		break;
	}
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_eSetUint8
 *
 * DESCRIPTION:
 * Generic set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkConfig_eSetUint8(uint8 u8Val, void *pvCbData)
{
	teJIP_Status eReturn;

	/* Call standard function */
	eReturn = eSetUint8(u8Val, pvCbData);

	/* Make sure permament data is saved */
	psMibNwkConfig->bSaveRecord = TRUE;

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_eSetUint16
 *
 * DESCRIPTION:
 * Generic set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkConfig_eSetUint16(uint16 u16Val, void *pvCbData)
{
	teJIP_Status eReturn;

	/* Call standard function */
	eReturn = eSetUint16(u16Val, pvCbData);

	/* Make sure permament data is saved */
	psMibNwkConfig->bSaveRecord = TRUE;

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_eSetUint32
 *
 * DESCRIPTION:
 * Generic set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkConfig_eSetUint32(uint32 u32Val, void *pvCbData)
{
	teJIP_Status eReturn;

	/* Call standard function */
	eReturn = eSetUint32(u32Val, pvCbData);

	/* Make sure permament data is saved */
	psMibNwkConfig->bSaveRecord = TRUE;

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_eSetDeviceType
 *
 * DESCRIPTION:
 * Custom set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkConfig_eSetDeviceType(uint8 u8Val, void *pvCbData)
{
	teJIP_Status eReturn = E_JIP_ERROR_BAD_VALUE;

	/* Valid device type ? */
	if (u8Val >= E_6LP_COORDINATOR && u8Val <= E_6LP_END_DEVICE)
	{
		/* Call standard function */
		eReturn = eSetUint8(u8Val, pvCbData);

        /* Make sure permament data is saved */
		psMibNwkConfig->bSaveRecord = TRUE;
	}

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_eSetScanChannels
 *
 * DESCRIPTION:
 * Custom set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkConfig_eSetScanChannels(uint32 u32Val, void *pvCbData)
{
	teJIP_Status eReturn = E_JIP_ERROR_BAD_VALUE;

	/* Valid device type ? */
	if (u32Val >= MIB_NWK_CONFIG_SCAN_CHANNELS_MIN && u32Val <= MIB_NWK_CONFIG_SCAN_CHANNELS_MAX)
	{
		/* Call standard function */
		eReturn = eSetUint32(u32Val, pvCbData);

        /* Make sure permament data is saved */
		psMibNwkConfig->bSaveRecord = TRUE;
	}

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_eSetFrameCounterDelta
 *
 * DESCRIPTION:
 * Custom set data callback
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkConfig_eSetFrameCounterDelta(uint16 u16Val, void *pvCbData)
{
	teJIP_Status eReturn = E_JIP_ERROR_BAD_VALUE;

	/* Valid value ? */
	if (u16Val >= MIB_NWK_CONFIG_FRAME_COUNTER_DELTA_MIN)
	{
		/* Call standard function */
		eReturn = eSetUint16(u16Val, pvCbData);

        /* Make sure permament data is saved */
		psMibNwkConfig->bSaveRecord = TRUE;
	}

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_vSetNetworkId
 *
 * DESCRIPTION:
 * Puts wanted network id into establish route requests and beacon responses
 *
 ****************************************************************************/
PUBLIC void MibNwkConfig_vSetNetworkId(void)
{
	uint8 au8NwkId[MAX_BEACON_USER_DATA];

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_vSetNetworkId()");

	/* Create array containing network ID */
	memset(au8NwkId, 0, MAX_BEACON_USER_DATA);
	memcpy(au8NwkId, (uint8 *)&psMibNwkConfig->sPerm.u32NetworkId, sizeof(uint32));

	/* Set beacon payload */
	vApi_SetUserBeaconBits(au8NwkId);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\tvApi_SetUserBeaconBits(%x)", psMibNwkConfig->sPerm.u32NetworkId);
	/* Set up beacon response callback */
	vApi_RegBeaconNotifyCallback(MibNwkConfig_bBeaconNotifyCallback);

	/* Set establish route payload */
	v6LP_SetUserData(sizeof(uint32), au8NwkId);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\n\tv6LP_SetUserData(%x)", psMibNwkConfig->sPerm.u32NetworkId);
	/* Set up establish route callback */
	v6LP_SetNwkCallback(MibNwkConfig_bNwkCallback);
}

/****************************************************************************
 *
 * NAME: MibNwkConfig_bBeaconNotifyCallback
 *
 * DESCRIPTION:
 * Callback when processing beacon responses
 * Ignores responses from nodes using a different network ID
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkConfig_bBeaconNotifyCallback(tsScanElement *psBeaconInfo,
                                       			 uint16 	   u16ProtocolVersion)
{
	bool_t bReturn 		= TRUE;
    uint32 u32NetworkId = 0;

	/* Not a coordinator (don't filter results for a coordinator) ? */
	if (psMibNwkConfig->sPerm.u8DeviceType != E_6LP_COORDINATOR)
	{
	    /* Extract network id from request */
        memcpy((uint8 *)&u32NetworkId, psBeaconInfo->au8UserDefined, sizeof(uint32));
        /* Does it not match the network ID we are using ? */
        if (u32NetworkId != psMibNwkConfig->sPerm.u32NetworkId)
        {
			/* Discard the beacon */
            bReturn = FALSE;
        }
	}

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_bBeaconNotifyCallback(%x)=%d", u32NetworkId, bReturn);

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
PUBLIC bool_t MibNwkConfig_bNwkCallback(MAC_ExtAddr_s *psAddr,
 			                            uint8 		   u8DataLength,
                            			uint8 		 *pu8Data)
{
	bool_t bReturn 	    = FALSE;
    uint32 u32NetworkId = 0;

	/* Data matches a uint32 ? */
    if (u8DataLength == sizeof(uint32))
    {
	    /* Extract network id from request */
        memcpy((uint8 *)&u32NetworkId, pu8Data, sizeof(uint32));
        /* Does it match the network ID we are using ? */
        if (u32NetworkId == psMibNwkConfig->sPerm.u32NetworkId)
        {
			/* Allow the establish route */
            bReturn = TRUE;
        }
    }

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_CONFIG, "\nMibNwkConfig_bNwkCallback(%d, %x)=%d", u8DataLength, u32NetworkId, bReturn);

    return bReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
