/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkProfile MIB - Interface
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
#ifndef  MIBNWKPROFILE_H_INCLUDED
#define  MIBNWKPROFILE_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* Stack includes */
#include <Api.h>
#include <JIP.h>
#include <6LP.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "MibCommonDebug.h"
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Network configuration permament data */
typedef struct
{
	/* Config MIB variable data */
	uint8    u8MaxFailedPackets;
	uint8    u8MinBeaconLqi;
	uint8    u8PrfBeaconLqi;
	uint16	u16RouterPingPeriod;
} tsMibNwkProfilePerm;

/* Network configuration mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 					hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor   	sDesc;

	/* Data pointers */
	tsMibNwkProfilePerm			sPerm;

	/* Other data */
	bool_t						bSaveRecord;

} tsMibNwkProfile;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/

/* Unpatched functions */
PUBLIC void 			MibNwkProfile_vInit(thJIP_Mib        hMibNwkProfileInit,
							   				tsMibNwkProfile  *psMibNwkProfileInit);
PUBLIC void 			MibNwkProfile_vRegister(void);
PUBLIC void 			MibNwkProfile_vSecond(void);
PUBLIC bool_t 			MibNwkProfile_bScanSortCallback(tsScanElement *pasScanResult,
              			              			  uint8 			 u8ScanListSize,
                            			 		  uint8 		  *pau8ScanListOrder);
PUBLIC bool_t 			MibNwkProfile_bScanSortCheckSwap(tsScanElement *pasScanResult,
                                  						 uint8 		    u8ScanListItem,
                                  						 uint8 	     *pau8ScanListOrder);
PUBLIC void 			MibNwkProfile_vApply(void);
PUBLIC teJIP_Status 	MibNwkProfile_eSetUint8(uint8    u8Val, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkProfile_eSetUint16(uint16 u16Val, void *pvCbData);

#if defined __cplusplus
}
#endif

#endif  /* MIBNWKPROFILE_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
