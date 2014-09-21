/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         AdcStatus MIB - Declaration
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
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* Stack includes */
#include <JIP.h>
#include <AccessFunctions.h>
/* Application common includes */
#include "MibCommon.h"
#include "MibAdcStatus.h"
#include "Table.h"

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern uint8 u8MibAdcStatusPatchOscillator;				/* Oscillator state */

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibAdcStatus sMibAdcStatus;

/****************************************************************************/
/***        MIB declaration                                                ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_ADC_STATUS

#define DECLARE_MIB
#include "MibAdcStatusDef.h"

JIP_START_DECLARE_MIB(AdcStatusDef, AdcStatus)
JIP_CALLBACK(Mask,				NULL, 		vGetUint8, 	  	&sMibAdcStatus.sTemp.u8Mask)
JIP_CALLBACK(Read,				NULL, 		Table_vGetData, &sMibAdcStatus.sRead)
JIP_CALLBACK(ChipTemp,  		NULL, 		vGetInt16, 	  	&sMibAdcStatus.i16ChipTemp)
JIP_CALLBACK(CalTemp,   		NULL, 		vGetInt16, 	  	&sMibAdcStatus.i16CalTemp)
JIP_CALLBACK(Oscillator,		NULL, 		vGetUint8, 	  	&u8MibAdcStatusPatchOscillator)
JIP_END_DECLARE_MIB(AdcStatus, hAdcStatus)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibAdcStatus = &sAdcStatusMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibAdcStatus = NULL;

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
