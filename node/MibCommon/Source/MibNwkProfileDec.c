/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NwkProfile MIB - Declaration
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
#include "MibNwkProfile.h"

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibNwkProfile sMibNwkProfile =
{
	/* Permament data defaults - may be overridden from flash during initialisation */
	.sPerm.u8MaxFailedPackets			= CONFIG_MAX_FAILED_PACKETS,
	.sPerm.u8MinBeaconLqi				= CONFIG_MIN_BEACON_LQI,
	.sPerm.u8PrfBeaconLqi				= CONFIG_PRF_BEACON_LQI,
	.sPerm.u16RouterPingPeriod			= CONFIG_ROUTER_PING_PERIOD
};

/****************************************************************************/
/***        MIB declaration                                                ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_NWK_PROFILE

#define DECLARE_MIB
#include "MibNwkProfileDef.h"

JIP_START_DECLARE_MIB(NwkProfileDef, NwkProfile)
JIP_CALLBACK(MaxFailedPackets, MibNwkProfile_eSetUint8,  vGetUint8,  &sMibNwkProfile.sPerm.u8MaxFailedPackets)
JIP_CALLBACK(MinBeaconLqi	  , MibNwkProfile_eSetUint8,  vGetUint8,  &sMibNwkProfile.sPerm.u8MinBeaconLqi)
JIP_CALLBACK(PrfBeaconLqi	  , MibNwkProfile_eSetUint8,  vGetUint8,  &sMibNwkProfile.sPerm.u8PrfBeaconLqi)
JIP_CALLBACK(RouterPingPeriod, MibNwkProfile_eSetUint16, vGetUint16, &sMibNwkProfile.sPerm.u16RouterPingPeriod)
JIP_END_DECLARE_MIB(NwkProfile, hNwkProfile)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNwkProfile = &sNwkProfileMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibNwkProfile = NULL;

#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
