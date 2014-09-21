/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkSecurity MIB - Implementation
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
#include <mac_pib.h>
#include <JIP.h>
#include <6LP.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "MibCommon.h"
#include "MibCommonDebug.h"
#include "MibNwkSecurity.h"
#include "MibNwkStatus.h"
#include "PATCH.h"

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
extern void           *psJenNetConfigData;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNwkSecurity *psMibNwkSecurity;			/* Nwk Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vInit(thJIP_Mib        hMibNwkSecurityInit,
								tsMibNwkSecurity *psMibNwkSecurityInit,
								bool_t		      bMibNwkSecuritySecurity)
{
	/* Valid data pointer ? */
	if (psMibNwkSecurityInit != (tsMibNwkSecurity *) NULL)
	{
		PDM_teStatus   ePdmStatus;

		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vInit() {%d}", sizeof(tsMibNwkSecurity));

		/* Take copy of pointer to data */
		psMibNwkSecurity = psMibNwkSecurityInit;

		/* Take a copy of the MIB handle */
		psMibNwkSecurity->hMib = hMibNwkSecurityInit;

		/* Note security setting */
		psMibNwkSecurity->bSecurity = bMibNwkSecuritySecurity;

		/* Load NodeStatus mib data */
		ePdmStatus = PDM_eLoadRecord(&psMibNwkSecurity->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
									 "MibNwkSecurity",
#else
									 (uint16)(MIB_ID_NWK_SECURITY & 0xFFFF),
#endif
									 (void *) &psMibNwkSecurity->sPerm,
									 sizeof(psMibNwkSecurity->sPerm),
									 FALSE);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tPDM_eLoadRecord(MibNwkSecurity, %d) = %d", sizeof(psMibNwkSecurity->sPerm), ePdmStatus);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vJipInitData
 *
 * DESCRIPTION:
 * Initialises jip stack data
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vJipInitData(tsJIP_InitData *psJipInitData)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vJipInitData()");

	/* Note the device type we are going to run as */
    psMibNwkSecurity->u8DeviceType = psJipInitData->eDeviceType;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_u8NetworkConfigData
 *
 * DESCRIPTION:
 * Sets network configuration data
 *
 ****************************************************************************/
PUBLIC uint8 MibNwkSecurity_u8NetworkConfigData(tsNetworkConfigData *psNetworkConfigData,
											   uint8 				  u8NwkStatusUpMode,
											   uint32				 u32NwkStatusFrameCounter,
											   uint8 				  u8NwkConfigStackModeInit)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vNetworkConfigData()");

	/* Take a pointer to the configuration data for later editing */
	psMibNwkSecurity->psNetworkConfigData 		 = psNetworkConfigData;
	/* Note ping settings that we need to manipulate according to stack mode */
	psMibNwkSecurity->u32EndDeviceActivityTimeout = psMibNwkSecurity->psNetworkConfigData->u32EndDeviceActivityTimeout;

	/* Security enabled ? */
	if (psMibNwkSecurity->bSecurity)
	{
		/* Set up data structures ready for key retrieval and insertion */
		v6LP_EnableSecurity();
		/* Were we in a gateway network and
		   have a valid network key ? */
		if ((u8NwkStatusUpMode                        						 			    		  == MIB_NWK_STATUS_UP_MODE_GATEWAY) &&
			(psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1 != 0  ||
			 psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2 != 0  ||
			 psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3 != 0  ||
			 psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4 != 0))
		{
			/* Resume gateway operation */
			u8NwkStatusUpMode = MibNwkSecurity_u8ResumeGateway(psNetworkConfigData, u8NwkStatusUpMode, u32NwkStatusFrameCounter);
		}
		/* Are we in a standalone network and
		   had a valid network key and
		   channel and
		   pan id */
		else if ((u8NwkStatusUpMode                        						  				 		   == MIB_NWK_STATUS_UP_MODE_STANDALONE) &&
				 (psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1 != 0   ||
			 	  psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2 != 0   ||
			 	  psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3 != 0   ||
			 	  psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4 != 0)  &&
				  psMibNwkSecurity->sPerm.u8Channel      				 						  >= 11  &&
				  psMibNwkSecurity->sPerm.u8Channel					  	 						  <= 26  &&
				  psMibNwkSecurity->sPerm.u16PanId                       						  >  0   &&
				  psMibNwkSecurity->sPerm.u16PanId                       						  <  0xffff)
		{
			/* Resume standalone operation */
			u8NwkStatusUpMode = MibNwkSecurity_u8ResumeStandalone(psNetworkConfigData, u8NwkStatusUpMode, u32NwkStatusFrameCounter);
		}
		/* Not restoring any security data ? */
		else
		{
			/* Revert to none up mode */
			u8NwkStatusUpMode = MIB_NWK_STATUS_UP_MODE_NONE;
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tu8NwkStatusUpMode=%d", u8NwkStatusUpMode);
			/* Apply initialisation stack mode */
			vApi_SetStackMode(u8NwkConfigStackModeInit);
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetStackMode(%x)", u8NwkConfigStackModeInit);
			/* Set profile for stackmode */
			MibNwkSecurity_vSetProfile((u8NwkConfigStackModeInit & STACK_MODE_STANDALONE) ? TRUE : FALSE);

			/* Not a coordinator ? */
			if (psMibNwkSecurity->u8DeviceType != E_6LP_COORDINATOR)
			{
				/* Trying to join standalone network ? */
				if (u8NwkConfigStackModeInit & STACK_MODE_STANDALONE)
				{
					/* Use standalone commissioning key */
					MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_STANDALONE_COMMISSIONING);
				}
				/* Trying to join gateway network ? */
				else
				{
					/* Use gateway commissioning key */
					MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_GATEWAY_COMMISSIONING);
				}
			}
			/* Coordinator ? */
			else
			{
				/* Use network key */
				MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_NETWORK);
			}
		}
	}

	return u8NwkStatusUpMode;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vResumeGateway
 *
 * DESCRIPTION:
 * REsumes running in a gateway system
 *
 ****************************************************************************/
PATCH_POINT_PUBLIC(uint8,MibNwkSecurity_u8ResumeGateway)(tsNetworkConfigData *psNetworkConfigData,
											uint8 		   u8NwkStatusUpMode,
											uint32		  u32NwkStatusFrameCounter)
{
	MAC_DeviceDescriptor_s sDeviceDescriptor;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_u8ResumeGateway()");

	/* Use network key */
	MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_NETWORK);
	/* Build security descriptor for this node */
	sDeviceDescriptor.u16PanId  	  = psMibNwkSecurity->sPerm.u16PanId;
	sDeviceDescriptor.u16Short  	  = 0xfffe;
	memcpy(&sDeviceDescriptor.sExt, pvAppApiGetMacAddrLocation(), sizeof(MAC_ExtAddr_s));
	sDeviceDescriptor.u32FrameCounter = u32NwkStatusFrameCounter;
	sDeviceDescriptor.bExempt 		  = FALSE;
	/* Restore security descriptor for this node */
	(void) bSecuritySetDescriptor(0, &sDeviceDescriptor);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tbSecuritySetDescriptor(0, %x:%x, %d)",
		sDeviceDescriptor.sExt.u32H,
		sDeviceDescriptor.sExt.u32L,
		sDeviceDescriptor.u32FrameCounter);
	/* Make three attempts to rejoin gateway with current network key */
	psMibNwkSecurity->u8GatewayRejoin = 3;
	/* Revert to none up mode */
	u8NwkStatusUpMode = MIB_NWK_STATUS_UP_MODE_NONE;
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tu8NwkStatusUpMode=%d", u8NwkStatusUpMode);

	/* Resuming as a coordinator ? */
	if (psMibNwkSecurity->u8DeviceType == E_6LP_COORDINATOR)
	{
		/* Pass in network config data as parameter to do this */
		/* Override scan channels and PAN ID to restore previous network */
		psNetworkConfigData->u32ScanChannels = (1<<psMibNwkSecurity->sPerm.u8Channel);
		psNetworkConfigData->u16PanID		=     psMibNwkSecurity->sPerm.u16PanId;
	}

	return u8NwkStatusUpMode;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vResumeStandalone
 *
 * DESCRIPTION:
 * Resumes running in a standalone system
 *
 ****************************************************************************/
PATCH_POINT_PUBLIC(uint8,MibNwkSecurity_u8ResumeStandalone)(tsNetworkConfigData *psNetworkConfigData,
											   uint8 		   u8NwkStatusUpMode,
											   uint32		  u32NwkStatusFrameCounter)
{
	MAC_DeviceDescriptor_s sDeviceDescriptor;
	uint8 u8SecureAddr;
	uint8 u8Restored = 0;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_u8ResumeStandalone()");

	/* Use network key */
	MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_NETWORK);
	/* Build security descriptor for this node */
	sDeviceDescriptor.u16PanId  	  = psMibNwkSecurity->sPerm.u16PanId;
	sDeviceDescriptor.u16Short  	  = 0xfffe;
	memcpy(&sDeviceDescriptor.sExt, pvAppApiGetMacAddrLocation(), sizeof(MAC_ExtAddr_s));
	sDeviceDescriptor.u32FrameCounter = u32NwkStatusFrameCounter;
	sDeviceDescriptor.bExempt 		  = FALSE;
	/* Restore security descriptor for this node */
	(void) bSecuritySetDescriptor(u8Restored++, &sDeviceDescriptor);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tbSecuritySetDescriptor(%d, %x:%x, %d)",
		u8Restored,
		sDeviceDescriptor.sExt.u32H,
		sDeviceDescriptor.sExt.u32L,
		sDeviceDescriptor.u32FrameCounter);
	/* Zero frame counter for other descriptors we are going to create */
	sDeviceDescriptor.u32FrameCounter = 0;
	/* Loop through stored secure addresses */
	for (u8SecureAddr = 0;
		 u8SecureAddr < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddr++)
	{
		/* Valid secure address ? */
		if (psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddr].u32H != 0 || psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddr].u32L != 0)
		{
			/* Build security descriptor for this node */
			memcpy(&sDeviceDescriptor.sExt, &psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddr], sizeof(MAC_ExtAddr_s));
			/* Restore security descriptor for this node (well reserve a slot anyway as we are zeroing the frame counter) */
			(void) bSecuritySetDescriptor(u8Restored++, &sDeviceDescriptor);
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tbSecuritySetDescriptor(%d, %x:%x, %d)",
				u8Restored,
				sDeviceDescriptor.sExt.u32H,
				sDeviceDescriptor.sExt.u32L,
				sDeviceDescriptor.u32FrameCounter);
		}
	}
	/* Set profile for standalone system */
	MibNwkSecurity_vSetProfile(TRUE);
	/* Start in standalone mode */
	vApi_SetStackMode(STACK_MODE_STANDALONE);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetStackMode(%x)", STACK_MODE_STANDALONE);
	/* Note network is up */
	psMibNwkSecurity->bUp = TRUE;
	/* Skip the normal joining process */
	vApi_SkipJoin(psMibNwkSecurity->sPerm.u16PanId, psMibNwkSecurity->sPerm.u8Channel);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SkipJoin(%x, %d)", psMibNwkSecurity->sPerm.u16PanId, psMibNwkSecurity->sPerm.u8Channel);

	return u8NwkStatusUpMode;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNwkSecurity->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\teJIP_RegisterMib(NwkSecurity)=%d", eStatus);

	/* Make sure permament data is saved */
	psMibNwkSecurity->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vSecond
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vSecond(void)
{
	/* Need to save record ? */
	if (psMibNwkSecurity->bSaveRecord)
	{
		/* Clear flag */
		psMibNwkSecurity->bSaveRecord = FALSE;

		/* Make sure permanent data is saved */
		PDM_vSaveRecord(&psMibNwkSecurity->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tPDM_vSaveRecord(MibNwkSecurity)");
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bStackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen)
{
	bool_t bReturn = FALSE;

	/* Which event ? */
    switch (eEvent)
    {
    	/* Joined network ? */
    	/* Started network ? */
		case E_STACK_JOINED:
		case E_STACK_STARTED:
		{
			tsNwkInfo *psNwkInfo;

			/* Cast data pointer to correct type */
			psNwkInfo = (tsNwkInfo *) pu8Data;

			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vStackEvent(JOINED) {%x}", u16Api_GetStackMode());

			/* Note the channel and PAN ID */
			psMibNwkSecurity->sPerm.u8Channel = psNwkInfo->u8Channel;
			psMibNwkSecurity->sPerm.u16PanId  = psNwkInfo->u16PanID;
			/* Security enabled ? */
			if (psMibNwkSecurity->bSecurity)
			{
				/* Not a coordinator ? */
				if (psMibNwkSecurity->u8DeviceType != E_6LP_COORDINATOR)
				{
					/* Take a copy of the network key */
					memcpy(&psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK], psApi_GetNwkKey(), sizeof(tsSecurityKey));
					/* Invalidate commissioning key */
					vSecurityInvalidateKey(1);
					/* Debug */
					DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvSecurityInvalidateKey(1)");
					/* Have we joined in standalone mode ? */
					if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
					{
						/* Make sure we are not in commissioning mode */
						vApi_SetStackMode(STACK_MODE_STANDALONE);
						/* Debug */
						DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetStackMode(%x)", STACK_MODE_STANDALONE);
						/* Add parent as secure address */
						MibNwkSecurity_bAddSecureAddr(&psNwkInfo->sParentAddr);
					}
				}
			}
			/* Note network is up */
			psMibNwkSecurity->bUp = TRUE;
			/* Save data to flash */
			psMibNwkSecurity->bSaveRecord = TRUE;
		}
		break;

		/* Stack reset ? */
		case E_STACK_RESET:
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vStackEvent(RESET, %d) {%x}", psMibNwkSecurity->bUp, u16Api_GetStackMode());

			/* Security enabled ? */
			if (psMibNwkSecurity->bSecurity)
			{
				/* Not a coordinator ? */
				if (psMibNwkSecurity->u8DeviceType != E_6LP_COORDINATOR)
				{
					/* Not joined ? */
					if (psMibNwkSecurity->bUp == FALSE)
					{
						/* Trying to rejoin a gateway network ? */
						if (psMibNwkSecurity->u8GatewayRejoin > 0)
						{
							/* Decrement rejoin counter */
							psMibNwkSecurity->u8GatewayRejoin--;
							/* Debug */
							DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tpsMibNwkSecurity->u8Rejoin=%d", psMibNwkSecurity->u8GatewayRejoin);
						}

						/* Not trying to rejoin a gateway network ? */
						if (psMibNwkSecurity->u8GatewayRejoin == 0)
						{
							/* Were we trying to join in standalone mode ? */
							if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
							{
								/* Set profile for gateway system */
								MibNwkSecurity_vSetProfile(FALSE);
								/* Use gateway commissioning key */
								MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_GATEWAY_COMMISSIONING);
								/* Swap to gateway mode */
								vApi_SetStackMode(0);
								/* Debug */
								DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetStackMode(%x)", 0);
							}
							/* Trying to join in gateway mode ? */
							else
							{
								/* Set profile for standalone system */
								MibNwkSecurity_vSetProfile(TRUE);
								/* Use standalone commissioning key */
								MibNwkSecurity_vSetSecurityKey(MIB_NWK_SECURITY_SECURITY_KEY_STANDALONE_COMMISSIONING);
								/* Swap to standalone commissioning mode */
								vApi_SetStackMode(STACK_MODE_STANDALONE | STACK_MODE_COMMISSION);
								/* Debug */
								DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetStackMode(%x)", (STACK_MODE_STANDALONE | STACK_MODE_COMMISSION));
							}
						}
					}
					/* Network was up so we've lost it ? */
					else
					{
						/* Lost standalone system (shouldn't happen!!!) ? */
						if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
						{
							/* Return unexpected event - which should force a reset */
							bReturn = TRUE;
						}
						/* Lost gateway system ? */
						else
						{
							/* Make three attempts to rejoin gateway with current network key */
							psMibNwkSecurity->u8GatewayRejoin = 3;
						}
					}
				}
				/* Coordinator (shouldn't happen!!!) ? */
				else
				{
					/* Return unexpected event - which should force a reset */
					bReturn = TRUE;
				}
			}
			/* Note network is down */
			psMibNwkSecurity->bUp = FALSE;
		}
		break;

		/* Others ? */
		default:
		{
			/* Do nothing */
			;
		}
		break;
	}

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bAddSecureAddr
 *
 * DESCRIPTION:
 * Reserve a secure slot for a mac address
 *
 ****************************************************************************/
PATCH_POINT_PUBLIC(bool_t,MibNwkSecurity_bAddSecureAddr)(MAC_ExtAddr_s *psMacAddr)
{
	uint8 u8Found = 0xff;
	uint8 u8Free  = 0xff;
	uint8 u8SecureAddress;
	bool_t bReturn = TRUE;

	/* Loop through secure addresses looking for the passed in address and an unused slot */
	for (u8SecureAddress = 0;
		 u8SecureAddress < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddress++)
	{
		/* Is this the device we are looking for ? */
		if (memcmp(&psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress], psMacAddr, sizeof(MAC_ExtAddr_s)) == 0)
		{
			/* Note we've found it */
			u8Found = u8SecureAddress;
		}
		/* Is this a blank address and we've not found one already ? */
		if (psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress].u32H == 0 &&
			psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress].u32L == 0 &&
			u8Free == 0xff)
		{
			/* Note the free slot */
			u8Free = u8SecureAddress;
		}
	}

	/* Did we not find the address already and we have an unused slot ? */
	if (u8Found == 0xff && u8Free != 0xff)
	{
		/* Store the address in the free slot */
		memcpy(&psMibNwkSecurity->sPerm.asSecureAddr[u8Free], psMacAddr, sizeof(MAC_ExtAddr_s));
		/* Save to flash */
		psMibNwkSecurity->bSaveRecord = TRUE;
	}
	/* We wanted to add the address but didnt have space ? */
	else if (u8Found == 0xff && u8Free == 0xff)
	{
		/* Failed */
		bReturn = FALSE;
	}

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_bAddSecureAddr(%x:%x)=%d",
		psMacAddr->u32H, psMacAddr->u32L, bReturn);

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_bDelSecureAddr
 *
 * DESCRIPTION:
 * Unreserve a secure slot for a mac address
 *
 ****************************************************************************/
PUBLIC bool_t MibNwkSecurity_bDelSecureAddr(MAC_ExtAddr_s *psMacAddr)
{
	uint8 u8SecureAddress;

	/* Loop through secure addresses looking for the passed in address and an unused slot */
	for (u8SecureAddress = 0;
		 u8SecureAddress < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddress++)
	{
		/* Is this the device we are looking for ? */
		if (memcmp(&psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress], psMacAddr, sizeof(MAC_ExtAddr_s)) == 0)
		{
			/* Blank the address */
			psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress].u32H = 0;
			psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress].u32L = 0;
			/* Save to flash */
			psMibNwkSecurity->bSaveRecord = TRUE;
		}
	}

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_bDelSecureAddr(%x:%x)=%d",
		psMacAddr->u32H, psMacAddr->u32L, 1);

	return TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vResetSecureAddr
 *
 * DESCRIPTION:
 * Reset all secure address slots
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vResetSecureAddr(void)
{
	uint8 u8SecureAddress;

	/* Loop through secure addresses looking for the passed in address and an unused slot */
	for (u8SecureAddress = 0;
		 u8SecureAddress < MIB_NWK_SECURITY_SECURE_ADDR_COUNT;
		 u8SecureAddress++)
	{
		/* Blank the address */
		psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress].u32H = 0;
		psMibNwkSecurity->sPerm.asSecureAddr[u8SecureAddress].u32L = 0;
	}
	/* Save to flash */
	psMibNwkSecurity->bSaveRecord = TRUE;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vResetSecureAddr()");
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vSetSecurityKey
 *
 * DESCRIPTION:
 * Set security key to apply
 *
 ****************************************************************************/
PATCH_POINT_PUBLIC(void,MibNwkSecurity_vSetSecurityKey)(uint8 u8Key)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vSetSecurityKey(%d)", u8Key);
	/* Valid key ? */
	if (u8Key < 3)
	{
		/* Setting network key ? */
		if (u8Key == MIB_NWK_SECURITY_SECURITY_KEY_NETWORK)
		{
			/* Invalidate commissioning key */
			vSecurityInvalidateKey(1);
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvSecurityInvalidateKey(1)");
			/* Apply network key */
			vApi_SetNwkKey(0, &psMibNwkSecurity->sPerm.asSecurityKey[u8Key]);
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetNwkKey(0, %x:%x:%x:%x)",
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_1,
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_2,
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_3,
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_4);
		}
		/* Setting a commissioning key ? */
		else
		{
			/* Invalidate unused key y */
			vSecurityInvalidateKey(0);
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvSecurityInvalidateKey(0)");
			/* Apply commissioning key */
			vApi_SetNwkKey(1, &psMibNwkSecurity->sPerm.asSecurityKey[u8Key]);
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tvApi_SetNwkKey(1, %x:%x:%x:%x)",
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_1,
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_2,
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_3,
				psMibNwkSecurity->sPerm.asSecurityKey[u8Key].u32KeyVal_4);
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vSetProfile
 *
 * DESCRIPTION:
 * Set network operating profile according to network mode
 *
 ****************************************************************************/
PATCH_POINT_PUBLIC(void,MibNwkSecurity_vSetProfile)(bool_t bStandalone)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurity_vSetProfile(%d)", bStandalone);

	/* Standalone system ? */
	if (bStandalone)
	{
		tsNwkProfile sNwkProfile;

		/* Read network profile */
		vJnc_GetNwkProfile(&sNwkProfile);
		/* Inhibit pings */
		sNwkProfile.u8MaxFailedPkts = 0;
		sNwkProfile.u16RouterPingPeriod = 0;
		/* Apply as user profile */
		(void) bJnc_SetRunProfile(PROFILE_USER, &sNwkProfile);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tbJnc_SetRunProfile(USER)");
		/* Inhibit End Device activity timeout */
		psMibNwkSecurity->psNetworkConfigData->u32EndDeviceActivityTimeout	= 0;
	}
	/* Gateway system ? */
	else
	{
		/* Apply default run profile for joining gateway system - will be updated if necessary upon joining */
		(void) bJnc_SetRunProfile(0, NULL);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tbJnc_SetRunProfile(0)");
		/* Apply gateway end device activity timeout settings */
		psMibNwkSecurity->psNetworkConfigData->u32EndDeviceActivityTimeout	= psMibNwkSecurity->u32EndDeviceActivityTimeout;
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_eSetKey
 *
 * DESCRIPTION:
 * Handle remote set of a key
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibNwkSecurity_eSetKey(const uint8 *pu8Val, uint8 u8Len, void *pvCbData)
{
	bool_t bReturn = FALSE;

	/* Has the correct amount of data being passed in ? */
	if (u8Len == sizeof(tsSecurityKey))
	{
		/* Copy data */
		memcpy(pvCbData, pu8Val, u8Len);
		/* Success */
		bReturn = TRUE;
	}

	return bReturn;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurity_vGetKey
 *
 * DESCRIPTION:
 * Handle remote get of a key
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurity_vGetKey(thJIP_Packet hPacket, void *pvCbData)
{
	eJIP_PacketAddData(hPacket, pvCbData, sizeof(tsSecurityKey), 0);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
