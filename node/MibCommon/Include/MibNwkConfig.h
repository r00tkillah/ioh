/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkConfig MIB - Interface
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
#ifndef  MIBNWKCONFIG_H_INCLUDED
#define  MIBNWKCONFIG_H_INCLUDED

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
#define MIB_NWK_CONFIG_SCAN_CHANNELS_MIN (0x00000800)
#define MIB_NWK_CONFIG_SCAN_CHANNELS_MAX (0x07fff800)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Network configuration permament data */
typedef struct
{
	/* Config MIB variable data */
	uint8    u8DeviceType;
	uint8    u8StackModeInit;
	uint16	u16PanId;
	uint16  u16FrameCounterDelta;
	uint32  u32NetworkId;
	uint32  u32ScanChannels;
	uint32  u32EndDeviceActivityTimeout;

} tsMibNwkConfigPerm;

/* Network configuration mib */
typedef struct
{
	/* MIB handle */
	thJIP_Mib 					hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor   	sDesc;

	/* Data pointers */
	tsMibNwkConfigPerm 			sPerm;

	/* Other data */
	bool_t						bSaveRecord;

} tsMibNwkConfig;

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
/* Patched functions - call these instead of unpatched ones below */
PUBLIC void 			MibNwkConfigPatch_vInit(thJIP_Mib         hMibNwkConfigInit,
							   					tsMibNwkConfig  *psMibNwkConfigInit,
							   					void   			*pvMibNwkStatusInit);
/* Call after MibNwkConfig_vNetworkConfigData() and MibNwkConfig_vStackEvent(JOINED) */
PUBLIC void 			MibNwkConfigPatch_vSetUserData(void);
/* Patched callback functions */
PUBLIC bool_t 			MibNwkConfigPatch_bBeaconNotifyCallback(tsScanElement *psBeaconInfo,
                                       			 		   		uint16 	      u16ProtocolVersion);
PUBLIC bool_t 			MibNwkConfigPatch_bNwkCallback(MAC_ExtAddr_s *psAddr,
 			                            				uint8 		   u8DataLength,
                            							uint8 		 *pu8Data);
/* Call each time around the main loop */
PUBLIC bool_t MibNwkConfigPatch_bMain(void);

/* Unpatched functions */
PUBLIC void 			MibNwkConfig_vInit( thJIP_Mib        hMibNwkConfigInit,
							   				tsMibNwkConfig  *psMibNwkConfigInit);
PUBLIC void 			MibNwkConfig_vRegister(void);
PUBLIC void 			MibNwkConfig_vSecond(void);
PUBLIC void 			MibNwkConfig_vJipInitData(tsJIP_InitData *psJipInitData);
PUBLIC void 			MibNwkConfig_vNetworkConfigData(tsNetworkConfigData *psNetworkConfigData);
PUBLIC void 			MibNwkConfig_vStackEvent(te6LP_StackEvent eEvent);
PUBLIC teJIP_Status 	MibNwkConfig_eSetUint8(			   uint8   u8Val, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkConfig_eSetUint16(	       uint16 u16Val, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkConfig_eSetUint32(	       uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkConfig_eSetDeviceType(       uint8   u8Val, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkConfig_eSetScanChannels(     uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status 	MibNwkConfig_eSetFrameCounterDelta(uint16 u16Val, void *pvCbData);
PUBLIC void 			MibNwkConfig_vSetNetworkId(void);
PUBLIC bool_t 			MibNwkConfig_bBeaconNotifyCallback(tsScanElement *psBeaconInfo,
                                       			 		   uint16 	   u16ProtocolVersion);
PUBLIC bool_t 			MibNwkConfig_bNwkCallback(		   MAC_ExtAddr_s *psAddr,
 			                            				   uint8 		   u8DataLength,
                            							   uint8 		 *pu8Data);

#if defined __cplusplus
}
#endif

#endif  /* MIBNWKCONFIG_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
