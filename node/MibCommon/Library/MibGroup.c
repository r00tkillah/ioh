/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Group MIB - Implementation
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
#include "MibGroup.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define IN6_ADDR_MULTICAST_FLAGS                     (1) // Transient address
#define IN6_ADDR_MULTICAST_SCOPE                     (5) // Site local scope

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Imported Variables                                            ***/
/****************************************************************************/
extern uint8 u8SocketMaxGroupAddrs;	   /* Maximum groups variable for stack */

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibGroup	    *psMibGroup;		    /* Permament stack node mib */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/
PUBLIC bool_t bJIP_AddGroupAddr(in6_addr *psAddr);
PUBLIC bool_t bJIP_RemoveGroupAddr(in6_addr *psAddr);

/****************************************************************************
 *
 * NAME: MibGroup_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibGroup_vInit(tsMibGroup	    *psMibGroupInit)
{
	PDM_teStatus   ePdmStatus;

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\nMibGroup_vInit() {%d}", sizeof(tsMibGroup));

	/* Take copy of pointer to data */
	psMibGroup = psMibGroupInit;

	/* Load Node Status mib data */
	ePdmStatus = PDM_eLoadRecord(&psMibGroup->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
								 "MibGroup",
#else
								 (uint16)(MIB_ID_GROUPS & 0xFFFF),
#endif
								 (void *) &psMibGroup->sPerm,
								 sizeof(psMibGroup->sPerm),
								 FALSE);

	#if CONFIG_DBG_MIB_GROUP
	{
		uint8 u8Group;
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\n\tPDM_eLoadRecord(MibGroup, %d) = %d", sizeof(psMibGroup->sPerm), ePdmStatus);
		/* Loop through current groups */
		for (u8Group = 0; u8Group < MIB_GROUP_MAX; u8Group++)
		{
			/* Debug */
			DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\n\tasGroupAddr[%d] = %x:%x:%x:%x:%x:%x:%x:%x)",
				u8Group,
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[0],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[1],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[2],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[3],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[4],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[5],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[6],
				psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[7]);
		}
	}
	#endif

	/* Tell stack maximum number of groups we want to support */
	u8SocketMaxGroupAddrs = MIB_GROUP_MAX;
}

/****************************************************************************
 *
 * NAME: MibGroup_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibGroup_vRegister(void)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\nMibGroup_vRegister()");

	/* Make sure permament data is saved */
	psMibGroup->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibGroup_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibGroup_vSecond(void)
{
	/* Need to save record ? */
	if (psMibGroup->bSaveRecord)
	{
		/* Clear flag */
		psMibGroup->bSaveRecord = FALSE;
		/* Make sure permament data is saved */
		PDM_vSaveRecord(&psMibGroup->sDesc);
		/* Debug */
		DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\nMibGroup_vSecond()");
		DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\n\tPDM_vSaveRecord(MibGroup)");
		#if CONFIG_DBG_MIB_GROUP
		{
			uint8 u8Group;
			/* Loop through current groups */
			for (u8Group = 0; u8Group < MIB_GROUP_MAX; u8Group++)
			{
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\n\tasGroupAddr[%d] = %x:%x:%x:%x:%x:%x:%x:%x)",
					u8Group,
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[0],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[1],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[2],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[3],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[4],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[5],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[6],
					psMibGroup->sPerm.asGroupAddr[u8Group].s6_addr16[7]);
			}
		}
		#endif
	}
}

/****************************************************************************
 *
 * NAME: MibGroup_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibGroup_vStackEvent(te6LP_StackEvent eEvent)
{
	/* Network is up ? */
	if (E_STACK_JOINED  == eEvent ||
		E_STACK_STARTED == eEvent)
	{
		/* First time joined ? */
		if (psMibGroup->bJoined == FALSE)
		{
			uint8 u8Group;

			/* Loop through current groups */
			for (u8Group = 0; u8Group < MIB_GROUP_MAX; u8Group++)
			{
				/* Valid group ? */
				if (memcmp(&psMibGroup->sPerm.asGroupAddr[u8Group], &psMibGroup->sIn6AddrBlank, sizeof(in6_addr)) != 0)
				{
					/* Join the group */
					bJIP_AddGroupAddr(&psMibGroup->sPerm.asGroupAddr[u8Group]);
				}
			}
			/* Note we've now joined */
			psMibGroup->bJoined = TRUE;
		}
	}
}

/****************************************************************************/
/***        Callback Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: bJIP_GroupCallback
 *
 * DESCRIPTION:
 * Callback when stack's Node Mib Name Variable is updated by JIP
 *
 ****************************************************************************/
PUBLIC WEAK bool_t bJIP_GroupCallback(teJIP_GroupEvent eEvent, in6_addr *psAddr)
{
	uint8 u8Group;
	uint8 u8Unused = 0xff;
	uint8 u8Found  = 0xff;
	bool_t bReturn = FALSE;

	/* Valid group */
	if (memcmp(psAddr, &psMibGroup->sIn6AddrBlank, sizeof(in6_addr)) != 0)
	{
		/* Loop through current groups */
		for (u8Group = 0; u8Group < MIB_GROUP_MAX; u8Group++)
		{
			/* Is this the group we are looking for ? */
			if (memcmp(&psMibGroup->sPerm.asGroupAddr[u8Group], psAddr, sizeof(in6_addr)) == 0)
			{
				/* Note the index */
				if (u8Found == 0xff) u8Found = u8Group;
			}
			/* Is this an unused group ? */
			if (memcmp(&psMibGroup->sPerm.asGroupAddr[u8Group], &psMibGroup->sIn6AddrBlank, sizeof(in6_addr)) == 0)
			{
				/* Note the index */
				if (u8Unused == 0xff) u8Unused = u8Group;
			}
		}

		/* Joining a group ? */
		if (E_JIP_GROUP_JOIN == eEvent)
		{
			/* Not already in the group and we have an unused slot to store it ? */
			if (u8Found == 0xff && u8Unused < MIB_GROUP_MAX)
			{
				/* Note group ID */
				memcpy(&psMibGroup->sPerm.asGroupAddr[u8Unused], psAddr, sizeof(in6_addr));
				/* Make sure permament data is saved */
				psMibGroup->bSaveRecord = TRUE;
				/* Allow join */
				bReturn = TRUE;
			}
			/* Already in the group ? */
			else if (u8Found < MIB_GROUP_MAX)
			{
				/* Allow join */
				bReturn = TRUE;
			}
		}
		/* Leaving a group ? */
		else if (E_JIP_GROUP_LEAVE)
		{
			/* Already in the group ? */
			if (u8Found < MIB_GROUP_MAX)
			{
				/* Clear group ID */
				memset(&psMibGroup->sPerm.asGroupAddr[u8Found], 0, sizeof(in6_addr));
				/* Make sure permament data is saved */
				psMibGroup->bSaveRecord = TRUE;
				/* Allow leave */
				bReturn = TRUE;
			}
			/* Not in group ? */
			else
			{
				/* Allow leave */
				bReturn = TRUE;
			}
		}
	}

	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_GROUP, "\nbJIP_GroupCallback(%d, %x:%x:%x:%x:%x:%x:%x:%x)=%d",
		eEvent,
		psAddr->s6_addr16[0],
		psAddr->s6_addr16[1],
		psAddr->s6_addr16[2],
		psAddr->s6_addr16[3],
		psAddr->s6_addr16[4],
		psAddr->s6_addr16[5],
		psAddr->s6_addr16[6],
		psAddr->s6_addr16[7],
		bReturn);

	return bReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
