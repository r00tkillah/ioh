/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioControl MIB
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
 * Copyright NXP B.V. 2013. All rights reserved
 */
/****************************************************************************/
#ifndef  MIBDIOCONTROL_H_INCLUDED
#define  MIBDIOCONTROL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
#include <JIP.h>
#include <6LP.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Dio permament data */
typedef struct
{
	uint32              	u32Output;
} tsMibDioControlPerm;

/* Bulb status temporary data */
typedef struct
{
	uint32					u32OutputOn;
	uint32					u32OutputOff;
} tsMibDioControlTemp;

/* Network control mib */
typedef struct
{
	/* MIB handles */
	thJIP_Mib 			   	hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor 	sDesc;

	/* Data pointers */
	tsMibDioControlPerm  	sPerm;
	tsMibDioControlTemp  	sTemp;

	/* Other data */
	bool_t   			   	bSaveRecord;
	bool_t					bJoined;

} tsMibDioControl;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
/* Unpatched functions in MibDioControl.c */
PUBLIC void 				MibDioControl_vInit(thJIP_Mib   hMibDioControlInit,
								 		 tsMibDioControl  *psMibDioControlInit);
PUBLIC void 				MibDioControl_vRegister(void);
PUBLIC void 				MibDioControl_vStackEvent(te6LP_StackEvent eEvent);
PUBLIC void 				MibDioControl_vSecond(void);
PUBLIC teJIP_Status 		MibDioControl_eSetOutput          (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status 		MibDioControl_eSetOutputOn   	  (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status 		MibDioControl_eSetOutputOff       (uint32 u32Val, void *pvCbData);

#if defined __cplusplus
}
#endif

#endif  /* MIBDIOCONTROL_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
