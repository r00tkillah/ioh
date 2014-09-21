/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkTest MIB - Definition
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
#undef MIB_HEADER
#define MIB_HEADER "MibNwkTestDef.h"

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Stack includes */
#include <jip_define_mib.h>
/* Application common includes */
#include "MibCommon.h"

/****************************************************************************/
/***        MIB definition                                                ***/
/****************************************************************************/
START_DEFINE_MIB(MIB_ID_NWK_TEST, NwkTestDef)
/*         ID 					  	   Type     Name   		Disp  Flags Access                  Cache Security */
DEFINE_VAR(VAR_IX_NWK_TEST_TESTS,       UINT8,	Tests, 		NULL, 0,    (READ | WRITE | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_TX_REQ,      UINT8,	TxReq, 	    NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_TX_OK,       UINT8,	TxOk, 	    NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_RX_OK,       UINT8,	RxOk, 	    NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_RX_LQI_MIN,  UINT8,	RxLqiMin,   NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_RX_LQI_MAX,  UINT8,	RxLqiMax,   NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_RX_LQI_MEAN, UINT8,	RxLqiMean,  NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_CW_CHANNEL, 	UINT8,	CwChannel,  NULL, 0,    (READ | WRITE | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_MAC_RETRIES, UINT8,	MacRetries, NULL, 0,    (READ | WRITE | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_TX_LQI_MIN,  UINT8,	TxLqiMin,   NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_TX_LQI_MAX,  UINT8,	TxLqiMax,   NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_TX_LQI_MEAN, UINT8,	TxLqiMean,  NULL, 0,    (READ | TRAP),          NONE, NONE)
DEFINE_VAR(VAR_IX_NWK_TEST_RX_LQI,      UINT8,	RxLqi,      NULL, 0,    (READ),                 NONE, NONE)
END_DEFINE_MIB(NwkTestDef)

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
