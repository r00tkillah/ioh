/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkCongfig MIB - Declaration
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
#include "Config.h"
#include "MibCommon.h"
#include "MibNwkConfig.h"

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibNwkConfig sMibNwkConfig =
{
	/* Permament data defaults - may be overridden from flash during initialisation */
	.sPerm.u8DeviceType					= 0xFF,
	.sPerm.u32NetworkId					= CONFIG_NETWORK_ID,
	.sPerm.u16PanId						= CONFIG_PAN_ID,
	.sPerm.u32ScanChannels			 	= CONFIG_SCAN_CHANNELS,
	.sPerm.u32EndDeviceActivityTimeout	= CONFIG_END_DEVICE_INACTIVITY_TIMEOUT,
	.sPerm.u16FrameCounterDelta			= CONFIG_FRAME_COUNTER_DELTA
};

/****************************************************************************/
/***        MIB declaration                                                ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_NWK_CONFIG

#define DECLARE_MIB
#include "MibNwkConfigDef.h"

JIP_START_DECLARE_MIB(NwkConfigDef, NwkConfig)
JIP_CALLBACK(DeviceType			  , MibNwkConfig_eSetDeviceType,   		vGetUint8,  &sMibNwkConfig.sPerm.u8DeviceType)
JIP_CALLBACK(NetworkId				  , MibNwkConfig_eSetUint32,    		vGetUint32, &sMibNwkConfig.sPerm.u32NetworkId)
JIP_CALLBACK(ScanChannels			  , MibNwkConfig_eSetScanChannels, 		vGetUint32, &sMibNwkConfig.sPerm.u32ScanChannels)
JIP_CALLBACK(PanId				  	  , MibNwkConfig_eSetUint16,    		vGetUint16, &sMibNwkConfig.sPerm.u16PanId)
JIP_CALLBACK(EndDeviceActivityTimeout, MibNwkConfig_eSetUint32,       		vGetUint32, &sMibNwkConfig.sPerm.u32EndDeviceActivityTimeout)
JIP_CALLBACK(FrameCounterDelta		  , MibNwkConfig_eSetFrameCounterDelta,	vGetUint16, &sMibNwkConfig.sPerm.u16FrameCounterDelta)
JIP_CALLBACK(StackModeInit			  , MibNwkConfig_eSetUint8,   			vGetUint8,  &sMibNwkConfig.sPerm.u8StackModeInit)
JIP_END_DECLARE_MIB(NwkConfig, hNwkConfig)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNwkConfig = &sNwkConfigMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNwkConfig = NULL;

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
