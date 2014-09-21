/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkSecurity MIB - Interface
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
#ifndef  MIBNWKSECURITY_H_INCLUDED
#define  MIBNWKSECURITY_H_INCLUDED

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
/* Stack includes */
#include <Sec2006.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MIB_NWK_SECURITY_SECURE_ADDR_COUNT						8

#define MIB_NWK_SECURITY_SECURITY_KEY_NETWORK					0
#define MIB_NWK_SECURITY_SECURITY_KEY_GATEWAY_COMMISSIONING		1
#define MIB_NWK_SECURITY_SECURITY_KEY_STANDALONE_COMMISSIONING	2

#define MIB_NWK_SECURITY_RESET_NONE					0
#define MIB_NWK_SECURITY_RESET_FACTORY				1
#define MIB_NWK_SECURITY_RESET_CHIP	    			2
#define MIB_NWK_SECURITY_RESET_STACK_TO_GATEWAY 	3
#define MIB_NWK_SECURITY_RESET_STACK_TO_STANDALONE 	4

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Network control permament data */
typedef struct
{
	uint8 				   u8Channel;
	uint16				  u16PanId;
	tsSecurityKey	  	  asSecurityKey[3];
	MAC_ExtAddr_s		   asSecureAddr[MIB_NWK_SECURITY_SECURE_ADDR_COUNT];

} tsMibNwkSecurityPerm;

/* Network status mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 				 hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor   sDesc;

	/* Data structures */
	tsMibNwkSecurityPerm 	 sPerm;

	/* Other data */
	bool_t 				  bUp;
	uint8 				 u8GatewayRejoin;
	uint8                u8DeviceType;
	uint32 			    u32EndDeviceActivityTimeout;
	tsNetworkConfigData *psNetworkConfigData;
	bool_t			      bSecurity;
	bool_t         	      bSaveRecord;

} tsMibNwkSecurity;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
/* Patched functions in MibNwkSecurityPatch.c - CALL THESE INSTEAD OF UNPATCHED FUNCTIONS BELOW */
PUBLIC void 			MibNwkSecurityPatch_vInit(thJIP_Mib        hMibNwkSecurityInit,
											 	  tsMibNwkSecurity *psMibNwkSecurityInit,
											 	  bool_t            bMibNwkSecuritySecurity);
PUBLIC uint8 			MibNwkSecurityPatch_u8StackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen);
PUBLIC void 			MibNwkSecurityPatch_vSecond(void);

/* Unpatched functions in ROM / Libraries */
PUBLIC void 			MibNwkSecurity_vInit(thJIP_Mib        hMibNwkSecurityInit,
											 tsMibNwkSecurity *psMibNwkSecurityInit,
											 bool_t            bMibNwkSecuritySecurity);
PUBLIC void 			MibNwkSecurity_vJipInitData(tsJIP_InitData *psJipInitData);
PUBLIC uint8 			MibNwkSecurity_u8NetworkConfigData(tsNetworkConfigData *psNetworkConfigData,
															uint8 u8NwkStatusUpMode,
															uint32 u32FrameCounter,
	 			  											  uint8 		   u8NwkConfigStackModeInit);
PUBLIC uint8 			MibNwkSecurity_u8ResumeGateway(   tsNetworkConfigData *psNetworkConfigData,
														  uint8 		   u8NwkStatusUpMode,
													   	  uint32		  u32NwkStatusFrameCounter);
PUBLIC uint8 			MibNwkSecurity_u8ResumeStandalone(tsNetworkConfigData *psNetworkConfigData,
														  uint8 		   u8NwkStatusUpMode,
											   			  uint32		  u32NwkStatusFrameCounter);
PUBLIC void 			MibNwkSecurity_vRegister(void);
PUBLIC bool_t 			MibNwkSecurity_bStackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen);
PUBLIC void 			MibNwkSecurity_vSecond(void);
PUBLIC void 			MibNwkSecurity_vNetworkConfigSecurity(void);
PUBLIC bool_t			MibNwkSecurity_bAddSecureAddr(MAC_ExtAddr_s *psMacAddr);
PUBLIC bool_t			MibNwkSecurity_bDelSecureAddr(MAC_ExtAddr_s *psMacAddr);
PUBLIC void 			MibNwkSecurity_vResetSecureAddr(void);
PUBLIC void 			MibNwkSecurity_vSetSecurityKey(uint8 u8Key);
PUBLIC void 			MibNwkSecurity_vSetProfile(bool_t bStandalone);
PUBLIC teJIP_Status 	MibNwkSecurity_eSetKey(const uint8 *pu8Val, uint8 u8Len, void *pvCbData);
PUBLIC void 			MibNwkSecurity_vGetKey(thJIP_Packet hPacket, void *pvCbData);

#if defined __cplusplus
}
#endif

#endif  /* MIBNWKSECURITY_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
