/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NodeStatus MIB - Interface
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
#ifndef  MIBNODESTATUS_H_INCLUDED
#define  MIBNODESTATUS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Node status permament data */
typedef struct
{
	uint16  u16SystemStatus;
	uint16  u16ColdStartCount;
	uint16  u16ResetCount;
	uint16  u16WatchdogCount;
	uint16  u16BrownoutCount;

} tsMibNodeStatusPerm;


/* Network status mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 				 hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor   sDesc;

	/* Data structures */
	tsMibNodeStatusPerm 	 sPerm;

	/* Other data */
	bool_t 					 bSaveRecord;

} tsMibNodeStatus;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
PUBLIC void 			MibNodeStatus_vInit(thJIP_Mib        hMibNodeStatusInit,
											tsMibNodeStatus *psMibNodeStatusInit);
PUBLIC void 			MibNodeStatus_vRegister(void);
PUBLIC void 			MibNodeStatus_vSecond(void);
PUBLIC void 			MibNodeStatus_vIncrementResetCount(void);

#if defined __cplusplus
}
#endif

#endif  /* MIBNODESTATUS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
