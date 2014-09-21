/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkSecurity MIB - Patches
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
#include "MibCommonDebug.h"
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
/***        External Function Prototypes                                  ***/
/****************************************************************************/
extern PUBLIC void 			Device_vReset(bool_t bFactoryReset);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern        void    *psJenNetConfigData;
extern PUBLIC uint8    u8LastPktLqi;

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC uint16 u16MibNwkSecurityPatchRejoin;					/* Rejoin timer */

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNwkSecurity *psMibNwkSecurity;			/* Nwk Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkSecurityPatch_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurityPatch_vInit(thJIP_Mib        hMibNwkSecurityInit,
								tsMibNwkSecurity *psMibNwkSecurityInit,
								bool_t		      bMibNwkSecuritySecurity)
{
	/* Run unpatched initialisation */
	MibNwkSecurity_vInit(hMibNwkSecurityInit,
						 psMibNwkSecurityInit,
						 bMibNwkSecuritySecurity);

	/* Take copy of pointer to data to use with patches */
	psMibNwkSecurity = psMibNwkSecurityInit;
}

/****************************************************************************
 *
 * NAME: MibNwkSecurityPatch_vSecond
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibNwkSecurityPatch_vSecond(void)
{
	/* Call unpatched function */
	MibNwkSecurity_vSecond();
	/* Counting down to rejoin ? */
	if (u16MibNwkSecurityPatchRejoin > 0)
	{
		/* Decrement */
		u16MibNwkSecurityPatchRejoin--;
		/* Time to reset ? */
		if (u16MibNwkSecurityPatchRejoin == 0)
		{
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nDevice_vReset() N");
			/* Clear out current network information */
			psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1 = 0;
			psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2 = 0;
			psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3 = 0;
			psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4 = 0;
			psMibNwkSecurity->sPerm.u8Channel = 0;
			psMibNwkSecurity->sPerm.u16PanId  = 0;
			/* Perform reset (will save PDMs and force rejoin) */
			Device_vReset(FALSE);
		}
	}
}

/****************************************************************************
 *
 * NAME: MibNwkSecurityPatch_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC uint8 MibNwkSecurityPatch_u8StackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen)
{
	uint8 u8Return = MIB_NWK_SECURITY_RESET_NONE;

	/* Which event ? */
    switch (eEvent)
    {
    	/* Joined network ? */
    	/* Started network ? */
		case E_STACK_JOINED:
		case E_STACK_STARTED:
		{
			/* Pass to default handler */
			if (MibNwkSecurity_bStackEvent(eEvent, pu8Data, u8DataLen))
			{
				/* Return reset chip */
				u8Return = MIB_NWK_SECURITY_RESET_CHIP;
			}
			else
			{
				/* Limit rejoins to this channel and PAN ID */
				psMibNwkSecurity->psNetworkConfigData->u32ScanChannels = (1<<psMibNwkSecurity->sPerm.u8Channel);
				psMibNwkSecurity->psNetworkConfigData->u16PanID		   =     psMibNwkSecurity->sPerm.u16PanId;
			}
		}
		break;

		/* Stack reset ? */
		case E_STACK_RESET:
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurityPatch_vStackEvent(RESET, %d) {%x}", psMibNwkSecurity->bUp, u16Api_GetStackMode());

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
							/* Got a network key ? */
							if (psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_1 != 0   ||
			 	  				psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_2 != 0   ||
			 	  				psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_3 != 0   ||
			 	  				psMibNwkSecurity->sPerm.asSecurityKey[MIB_NWK_SECURITY_SECURITY_KEY_NETWORK].u32KeyVal_4 != 0)
							{
								/* Router ? */
								if (psMibNwkSecurity->u8DeviceType == E_6LP_ROUTER)
								{
									/* Reset the stack to standalone mode */
									u8Return = MIB_NWK_SECURITY_RESET_STACK_TO_STANDALONE;
									/* Debug */
									DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tu8Return=TO_STANDALONE 1");
								}
							}
							/* Not got a network key ? */
							else
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
							}
						}
					}
					/* JN514x family ? */
					#ifdef  JENNIC_CHIP_FAMILY_JN514x
					/* Network was up so we've lost it ? */
					else
					{
						/* Lost standalone system (shouldn't happen but might do while we are chenging modes) ? */
						if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
						{
							/* Do nothing - might happen during a swtich from one mode to another) */
							;
						}
						/* Lost gateway system ? */
						else
						{
							/* Reset the stack to standalone mode (4x series cannot scan during backoff) */
							u8Return = MIB_NWK_SECURITY_RESET_STACK_TO_STANDALONE;
							/* Debug */
							DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tu8Return=TO_STANDALONE 2");
						}
					}
					#endif
				}
				/* Coordinator (shouldn't happen!!!) ? */
				else
				{
					/* Unexpected event - force a chip reset */
					u8Return = MIB_NWK_SECURITY_RESET_CHIP;
					/* Debug */
					DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tu8Return=RESET_CHIP 2");
				}
			}
			/* Note network is down */
			psMibNwkSecurity->bUp = FALSE;
		}
		break;

		/* Gateway present ? */
		case E_STACK_NETWORK_ANNOUNCE:
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\nMibNwkSecurityPatch_vStackEvent(NETWORK_ANNOUNCE, %d) {%x}", psMibNwkSecurity->bUp, u16Api_GetStackMode());
			/* In standalone mode ? */
			if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
			{
				/* Make one attempt to rejoin gateway with current network key */
				psMibNwkSecurity->u8GatewayRejoin = 1;
				/* Reset stack to gateway mode */
				u8Return = MIB_NWK_SECURITY_RESET_STACK_TO_GATEWAY;
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_NWK_SECURITY, "\n\tu8Return=TO_GATEWAY");
			}
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

	return u8Return;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
