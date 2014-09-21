/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkTest MIB - Interface
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
#ifndef  MIBNWKTEST_H_INCLUDED
#define  MIBNWKTEST_H_INCLUDED

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
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Network status temporary data */
typedef struct
{
	uint8   u8Tests;
	uint8   u8TxReq;
	uint8   u8TxOk;
	uint8   u8RxOk;
	uint8   u8RxLqiMin;
	uint8   u8RxLqiMax;
	uint8   u8RxLqiMean;
	uint8   u8CwChannel;			   /* Constant wave channel */
	uint8   u8MacRetries;
	uint8   u8TxLqiMin;
	uint8   u8TxLqiMax;
	uint8   u8TxLqiMean;
} tsMibNwkTestTemp;

/* Network status mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 				 hMib;

	/* Data structures */
	tsMibNwkTestTemp 		 sTemp;

	/* Other data */
	ts6LP_SockAddr 			 s6LP_SockAddr;
	uint16 				   u16RxLqiTotal;
	uint16 				   u16TxLqiTotal;
	uint8 					u8MaxFailedPkts;
	uint8                   u8DefaultMacRetries;
	uint8 					u8Pending;
	bool_t					 bUp;
	bool_t					 bRun;

} tsMibNwkTest;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
PUBLIC void 			MibNwkTest_vInit( thJIP_Mib       hMibNwkTestInit,
							   				tsMibNwkTest *psMibNwkTestInit);
PUBLIC void 			MibNwkTest_vRegister(void);
PUBLIC void 			MibNwkTest_vTick(void);
PUBLIC void 			MibNwkTest_vStackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen);

PUBLIC teJIP_Status 	MibNwkTest_eSetTests(uint8 u8Val, void *pvCbData);

#if defined __cplusplus
}
#endif

#endif  /* MIBNWKTEST_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
