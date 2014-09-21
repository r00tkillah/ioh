/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NodeStatus MIB - Implementation
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
#include "MibNodeStatus.h"

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
PRIVATE tsMibNodeStatus *psMibNodeStatus;		/* Node Status Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNodeStatus_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vInit(thJIP_Mib        hMibNodeStatusInit,
								tsMibNodeStatus *psMibNodeStatusInit)
{
	/* Valid data pointer ? */
	if (psMibNodeStatusInit != (tsMibNodeStatus *) NULL)
	{
		PDM_teStatus   ePdmStatus;
		uint32		 u32SystemStatus;

		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\nMibNodeStatus_vInit() {%d}", sizeof(tsMibNodeStatus));

		/* Take copy of pointer to data */
		psMibNodeStatus = psMibNodeStatusInit;

		/* Take a copy of the MIB handle */
		psMibNodeStatus->hMib = hMibNodeStatusInit;

		/* Load NodeStatus mib data */
		ePdmStatus = PDM_eLoadRecord(&psMibNodeStatus->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
									 "MibNodeStatus",
#else
									 (uint16)(MIB_ID_NODE_STATUS & 0xFFFF),
#endif
									 (void *) &psMibNodeStatus->sPerm,
									 sizeof(psMibNodeStatus->sPerm),
									 FALSE);

		/* Read system status */
		u32SystemStatus = u32REG_SysRead(REG_SYS_STAT);
		/* Copy to mib variable */
		psMibNodeStatus->sPerm.u16SystemStatus = (uint16)(u32SystemStatus & 0xffff);

		/* Cold start ? */
		if (0 == (u32SystemStatus & REG_SYSCTRL_STAT_WUS_MASK))
		{
			/* Increment cold start counter */
			psMibNodeStatus->sPerm.u16ColdStartCount++;
		}

		/* Watchdog reset ? */
		if (u32SystemStatus & REG_SYSCTRL_STAT_WD_RES_MASK)
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\n\tWatchdogReset");
			/* Increment watchdog counter */
			psMibNodeStatus->sPerm.u16WatchdogCount++;
		}

		/* Brownout reset ? */
		if (u32SystemStatus & REG_SYSCTRL_STAT_VBO_RES_MASK)
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\n\tBrownoutReset");
			/* Increment brownout counter */
			psMibNodeStatus->sPerm.u16BrownoutCount++;
		}

		/* Initialise other data */
		psMibNodeStatus->bSaveRecord = FALSE;
	}
}

/****************************************************************************
 *
 * NAME: MibNodeStatus_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNodeStatus->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\nMibNodeStatus_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\n\teJIP_RegisterMib(NodeStatus)=%d", eStatus);

	/* Make sure permament data is saved */
	psMibNodeStatus->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibNodeStatus_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vSecond(void)
{
	/* Need to save record ? */
	if (psMibNodeStatus->bSaveRecord)
	{
		/* Clear flag */
		psMibNodeStatus->bSaveRecord = FALSE;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNodeStatus->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\nMibNodeStatus_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_NODE_STATUS, "\n\tPDM_vSaveRecord(MibNodeStatus)");
	}
}

/****************************************************************************
 *
 * NAME: MibNodeStatus_vIncrementResetCount
 *
 * DESCRIPTION:
 * Called to increment reset counter due to a deliberate reset
 *
 ****************************************************************************/
PUBLIC void MibNodeStatus_vIncrementResetCount(void)
{
	/* Increment counter */
	psMibNodeStatus->sPerm.u16ResetCount++;
	/* Make sure permament data is saved */
	psMibNodeStatus->bSaveRecord = TRUE;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
