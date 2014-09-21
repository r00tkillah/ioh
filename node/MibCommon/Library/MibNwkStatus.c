/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkStatus MIB - Implementation
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
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "MibCommon.h"
#include "MibCommonDebug.h"
#include "MibNwkStatus.h"
#include "MibNwkConfig.h"

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
PRIVATE tsMibNwkStatus *psMibNwkStatus;			/* Nwk Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNwkStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vInit(thJIP_Mib        hMibNwkStatusInit,
							   tsMibNwkStatus  *psMibNwkStatusInit,
								bool_t           bMibNwkStatusSecurity,
								uint16 		   u16MibNwkConfigFrameCounterDelta)
{
	/* Valid data pointer ? */
	if (psMibNwkStatusInit != (tsMibNwkStatus *) NULL)
	{
		PDM_teStatus   ePdmStatus;

		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\nMibNwkStatus_vInit(%d, %d) {%d}",
			bMibNwkStatusSecurity,
			u16MibNwkConfigFrameCounterDelta,
			sizeof(tsMibNwkStatus));

		/* Take copy of pointer to data */
		psMibNwkStatus = psMibNwkStatusInit;

		/* Take a copy of the MIB handle */
		psMibNwkStatus->hMib = hMibNwkStatusInit;

		/* Note security setting */
		psMibNwkStatus->bSecurity = bMibNwkStatusSecurity;

		/* Use delta provided by network config MIB */
		psMibNwkStatus->u16FrameCounterDelta = u16MibNwkConfigFrameCounterDelta;

		/* Load NwkStatus mib data */
		ePdmStatus = PDM_eLoadRecord(&psMibNwkStatus->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
									 "MibNwkStatus",
#else
									 (uint16)(MIB_ID_NWK_STATUS & 0xFFFF),
#endif
									 (void *) &psMibNwkStatus->sPerm,
									 sizeof(psMibNwkStatus->sPerm),
									 FALSE);

		/* Default temporary status data */
		psMibNwkStatus->sTemp.u32RunTime = psMibNwkStatus->sPerm.u32DownTime + psMibNwkStatus->sPerm.u32UpTime;

		/* Restarting in standalone up mode ? */
		if (psMibNwkStatus->sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_STANDALONE)
		{
			/* Advance our frame counter */
			psMibNwkStatus->sPerm.u32FrameCounter += (uint32) psMibNwkStatus->u16FrameCounterDelta;
		}
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tpsMibNwkStatus->sPerm.u8UpMode=%d", 	   psMibNwkStatus->sPerm.u8UpMode);
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tpsMibNwkStatus->sPerm.u32FrameCounter=%d", psMibNwkStatus->sPerm.u32FrameCounter);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNwkStatus->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\nMibNwkStatus_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\neJIP_RegisterMib(NwkStatus)=%d", eStatus);

	/* Make sure permament data is saved */
	psMibNwkStatus->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vSecond
 *
 * DESCRIPTION:
 * Timer function
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vSecond(void)
{
	MAC_Pib_s *psPib;

	/* Get pointer to pib */
	psPib = MAC_psPibGetHandle(pvAppApiGetMacHandle());

	/* Security enabled ? */
	if (psMibNwkStatus->bSecurity)
	{
		/* Has our frame counter advanced since we last saved it ? */
		if (psPib->u32MacFrameCounter > psMibNwkStatus->u32SavedFrameCounter)
		{
			/* Has it advanced far enough to be worth saving ? */
			if (psPib->u32MacFrameCounter - psMibNwkStatus->u32SavedFrameCounter >= (uint32) psMibNwkStatus->u16FrameCounterDelta)
			{
				/* Save data to flash */
				psMibNwkStatus->bSaveRecord = TRUE;
			}
		}
	}

	/* Increment run time */
	psMibNwkStatus->sTemp.u32RunTime++;
	/* Been running an hour ? */
	if ((psMibNwkStatus->sTemp.u32RunTime % 3600) == 0)
	{
		/* Save data to flash */
		psMibNwkStatus->bSaveRecord = TRUE;
		/* Need to notify for the run time variable */
		vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_RUN_TIME);
	}

	/* Network is up ? */
	if (psMibNwkStatus->bUp)
	{
		/* Increment up timer */
		psMibNwkStatus->sPerm.u32UpTime++;
		/* Been up an hour ? */
		if ((psMibNwkStatus->sPerm.u32UpTime % 3600) == 0)
		{
			/* Need to notify for the up time variable */
			vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_UP_TIME);
		}
	}
	/* Network is down - increment down timer */
	else
	{
		/* Increment down timer */
		psMibNwkStatus->sPerm.u32DownTime++;
		/* Been down an hour ? */
		if ((psMibNwkStatus->sPerm.u32DownTime % 3600) == 0)
		{
			/* Need to notify for the down time variable */
			vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_DOWN_TIME);
		}
	}

	/* Need to save record ? */
	if (psMibNwkStatus->bSaveRecord)
	{
		/* Clear flag */
		psMibNwkStatus->bSaveRecord = FALSE;
		/* Take a copy of the frame counter */
		psMibNwkStatus->sPerm.u32FrameCounter = psPib->u32MacFrameCounter;
		/* Note saved frame counters */
		psMibNwkStatus->u32SavedFrameCounter = psMibNwkStatus->sPerm.u32FrameCounter;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNwkStatus->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\nMibNwkStatus_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tPDM_vSaveRecord(MibNwkStatus)");
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tpsMibNwkStatus->sPerm.u8UpMode=%d", 	   psMibNwkStatus->sPerm.u8UpMode);
		DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tpsMibNwkStatus->sPerm.u32FrameCounter=%d", psMibNwkStatus->sPerm.u32FrameCounter);
	}
}

/****************************************************************************
 *
 * NAME: MibNwkStatus_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibNwkStatus_vStackEvent(te6LP_StackEvent eEvent)
{
	/* Network is up and we think it is down ? */
	if (E_STACK_JOINED  == eEvent ||
	    E_STACK_STARTED == eEvent)
	{
		/* Was network down previously ? */
		if (FALSE == psMibNwkStatus->bUp)
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\nMibNwkStatus_vStackEvent(JOINED)");
			/* Set up flag */
			psMibNwkStatus->bUp = TRUE;
			/* Increment up counters */
			if (psMibNwkStatus->sPerm.u16UpCount < 0xffff) psMibNwkStatus->sPerm.u16UpCount++;
			/* Have we joined in standalone mode ? */
			if (u16Api_GetStackMode() & STACK_MODE_STANDALONE)
			{
				/* Note that we are in a standalone network */
				psMibNwkStatus->sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_STANDALONE;
			}
			/* Must be in gateway mode ? */
			else
			{
				/* Note that we are in a standalone network */
				psMibNwkStatus->sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_GATEWAY;
			}
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tpsMibNwkStatus->sPerm.u8UpMode=%d", 	   psMibNwkStatus->sPerm.u8UpMode);
			/* Save data to flash */
			psMibNwkStatus->bSaveRecord = TRUE;
			/* Need to notify for the run time variable */
			vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_RUN_TIME);
			/* Need to notify for the up count variable */
			vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_UP_COUNT);
			/* Need to notify for the up time variable */
			vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_UP_TIME);
			/* Need to notify for the down time variable */
			vJIP_NotifyChanged(psMibNwkStatus->hMib, VAR_IX_NWK_STATUS_DOWN_TIME);
		}
	}
	/* Network is down and we think it is up ? */
	if (E_STACK_RESET == eEvent)
	{
		/* Was network up previously ? */
 		if (TRUE == psMibNwkStatus->bUp)
 		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\nMibNwkStatus_vStackEvent(STACK_RESET)");
			/* Clear up flag */
			psMibNwkStatus->bUp = FALSE;
			/* Revert to none up mode */
			psMibNwkStatus->sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_NONE;
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\n\tpsMibNwkStatus->sPerm.u8UpMode=%d", 	   psMibNwkStatus->sPerm.u8UpMode);
			/* Save data to flash */
			psMibNwkStatus->bSaveRecord = TRUE;
		}
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
