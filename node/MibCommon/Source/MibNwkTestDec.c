/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkTest MIB - Declaration
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
#include "MibNwkTest.h"


/****************************************************************************/
/***        External data                                                 ***/
/****************************************************************************/
extern PUBLIC uint8  u8LastPktLqi;

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibNwkTest sMibNwkTest;

/****************************************************************************/
/***        MIB declaration                                                ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_NWK_TEST

#define DECLARE_MIB
#include "MibNwkTestDef.h"
JIP_START_DECLARE_MIB(NwkTestDef, NwkTest)
JIP_CALLBACK(Tests, 	MibNwkTest_eSetTests, 		vGetUint8,		&sMibNwkTest.sTemp.u8Tests)
JIP_CALLBACK(TxReq, 	NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8TxReq)
JIP_CALLBACK(TxOk, 		NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8TxOk)
JIP_CALLBACK(RxOk, 		NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8RxOk)
JIP_CALLBACK(RxLqiMin, 	NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8RxLqiMin)
JIP_CALLBACK(RxLqiMax, 	NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8RxLqiMax)
JIP_CALLBACK(RxLqiMean, NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8RxLqiMean)
JIP_CALLBACK(CwChannel, eSetUint8,  				vGetUint8,  	&sMibNwkTest.sTemp.u8CwChannel)
JIP_CALLBACK(MacRetries,eSetUint8,  				vGetUint8,  	&sMibNwkTest.sTemp.u8MacRetries)
JIP_CALLBACK(TxLqiMin, 	NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8TxLqiMin)
JIP_CALLBACK(TxLqiMax, 	NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8TxLqiMax)
JIP_CALLBACK(TxLqiMean, NULL, 				  		vGetUint8,		&sMibNwkTest.sTemp.u8TxLqiMean)
JIP_CALLBACK(RxLqi, 	NULL, 				  		vGetUint8,		&u8LastPktLqi)
JIP_END_DECLARE_MIB(NwkTest, hNwkTest)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNwkTest = &sNwkTestMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNwkTest = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
