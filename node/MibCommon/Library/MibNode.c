/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Node MIB - Implementation
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
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "MibCommon.h"
#include "MibCommonDebug.h"
#include "MibNode.h"

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
PRIVATE tsMibNode	    *psMibNode;		/* Permament stack node mib */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNode_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNode_vInit(tsMibNode	    *psMibNodeInit)
{
	PDM_teStatus   ePdmStatus;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NODE, "\nMibNode_vInit() {%d}", sizeof(tsMibNode));

	/* Take copy of pointer to data */
	psMibNode = psMibNodeInit;

	/* Load Node Status mib data */
	ePdmStatus = PDM_eLoadRecord(&psMibNode->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
								 "MibNode",
#else
								 (uint16)(MIB_ID_NODE & 0xFFFF),
#endif
								 (void *) &psMibNode->sPerm,
								 sizeof(psMibNode->sPerm),
								 FALSE);
}

/****************************************************************************
 *
 * NAME: MibNode_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNode_vRegister(void)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NODE, "\nMibNode_vRegister()");

	/* Make sure permament data is saved */
	psMibNode->bSaveRecord = TRUE;

	/* Tell the stack what its Node mib Name variable should be set to */
	vJIP_SetNodeName(psMibNode->sPerm.acName);
	/* Register a callback when the stack's Node mib Name variable is updated */
	vJIP_RegisterSetNameCallback(MibNode_vUpdateName);
}

/****************************************************************************
 *
 * NAME: MibNode_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNode_vSecond(void)
{
	/* Need to save record ? */
	if (psMibNode->bSaveRecord)
	{
		/* Clear flag */
		psMibNode->bSaveRecord = FALSE;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibNode->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_NODE, "\nMibNode_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_NODE, "\n\tPDM_vSaveRecord(MibNode)");
	}
}

/****************************************************************************/
/***        Callback Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibNode_vCbNameUpdate
 *
 * DESCRIPTION:
 * Callback when stack's Node Mib Name Variable is updated by JIP
 *
 ****************************************************************************/
PUBLIC void MibNode_vUpdateName(char *pcName)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NODE, "\nMibNode_vUpdateName()");

	/* Copy new name */
	strcpy(psMibNode->sPerm.acName, pcName);

	/* Make sure permament data is saved */
	psMibNode->bSaveRecord = TRUE;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
