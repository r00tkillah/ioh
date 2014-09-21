/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Node - common functions
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

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
#include <AccessFunctions.h>
#include <OverNetworkDownload.h>
#include <sec2006.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application includes */
#include "Config.h"
#include "DeviceDefs.h"
#include "Exception.h"
#include "Security.h"
#include "Node.h"
#include "MibCommon.h"
#include "MibNode.h"
#include "MibGroup.h"
#include "MibAdcStatus.h"
#include "MibNodeStatus.h"
#include "MibNodeControl.h"
#include "MibNwkStatus.h"
#include "MibNwkConfig.h"
#include "MibNwkProfile.h"
#include "MibNwkSecurity.h"
#include "MibNwkTest.h"
/* Optional Application Includes */
#ifdef  JENNIC_CHIP_FAMILY_JN516x
#include "AHI_EEPROM.h" /* Enables EEPROM factory reset detection, for 6x use only */
#endif
//#include "Uart.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Check for unsupported chip type */
#ifdef  JENNIC_CHIP_FAMILY_JN514x
#ifndef JENNIC_CHIP_JN5142J01
#ifndef JENNIC_CHIP_JN5148J01
#ifndef JENNIC_CHIP_JN5148
#error Unsupported chip!
#endif
#endif
#endif
#endif
#ifdef  JENNIC_CHIP_FAMILY_JN516x
#ifndef JENNIC_CHIP_JN5164
#ifndef JENNIC_CHIP_JN5168
#error Unsupported chip!
#endif
#endif
#endif

/* Apply patches definitions */
#define MIB_NWK_SECURITY_PATCH 		TRUE	/* When true patches in automatic gateway to standalone mode switchover */

/* Factory reset magic number */
#ifdef MK_FACTORY_RESET_MAGIC
#define FACTORY_RESET_MAGIC 		MK_FACTORY_RESET_MAGIC
#else
#define FACTORY_RESET_MAGIC 		0xFA5E13CB
#endif
#define FACTORY_RESET_TICK_TIMER	32000000	/* 16Mhz - 2 seconds */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        External Function Prototypes                                  ***/
/****************************************************************************/
extern PUBLIC void Device_vReset(bool_t);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
/* MIB structures */
#if MK_BLD_MIB_NODE
extern tsMibNode		 sMibNode;
#endif

#if MK_BLD_MIB_GROUP
extern tsMibGroup		 sMibGroup;
#endif

#if MK_BLD_MIB_NODE_STATUS
extern tsMibNodeStatus	 sMibNodeStatus;
extern thJIP_Mib		 hMibNodeStatus;
#endif

#if MK_BLD_MIB_NODE_CONTROL
extern tsMibNodeControl sMibNodeControl;
extern thJIP_Mib		hMibNodeControl;
#endif

#if MK_BLD_MIB_ADC_STATUS
extern tsMibAdcStatus	 sMibAdcStatus;
extern thJIP_Mib		 hMibAdcStatus;
#endif

#if MK_BLD_MIB_NWK_CONFIG
extern tsMibNwkConfig	 sMibNwkConfig;
extern thJIP_Mib		 hMibNwkConfig;
#endif

#if MK_BLD_MIB_NWK_PROFILE
extern tsMibNwkProfile	 sMibNwkProfile;
extern thJIP_Mib		 hMibNwkProfile;
#endif

#if MK_BLD_MIB_NWK_STATUS
extern tsMibNwkStatus	 sMibNwkStatus;
extern thJIP_Mib		 hMibNwkStatus;
#endif

#if MK_BLD_MIB_NWK_SECURITY
extern tsMibNwkSecurity	 sMibNwkSecurity;
extern thJIP_Mib		 hMibNwkSecurity;
#endif

#if MK_BLD_MIB_NWK_TEST
extern tsMibNwkTest	 	 sMibNwkTest;
extern thJIP_Mib		 hMibNwkTest;
#endif

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/* MAC pointers */
PRIVATE void      *pvMac;
PRIVATE MAC_Pib_s *psPib;

/* Flash/EEPROM data */
PRIVATE	uint8 					 u8PdmStartSector	= 0xff;
PRIVATE	uint8					 u8PdmNumSectors	= 0;
PRIVATE	uint32 					u32PdmSectorSize 	= 0;
PRIVATE	uint8					 u8RstSector		= 0xff;

/* JN516x family ? */
#ifdef  JENNIC_CHIP_FAMILY_JN516x
/* PDM device data */
PRIVATE PDM_tsRecordDescriptor   sDeviceDesc;
PRIVATE tsDevicePdm			     sDevicePdm;
#endif

/* Jip initialisation structure */
PRIVATE	tsJIP_InitData sJipInitData;

/* Other data */
PRIVATE uint8    u8NwkSecurityReset;
PRIVATE bool_t	 bUp;
PRIVATE bool_t	 bJoined;
PRIVATE PDM_teRecoveryState eGroupMibPdmInitState;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Node_vInit
 *
 * DESCRIPTION:
 * Initialises node hardware
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vInit(bool_t bWarmStart)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_vInit(%d)", bWarmStart);

	/* JN514x Chip family ? */
	#ifdef JENNIC_CHIP_FAMILY_JN516x
	{
		/* Enable for high temperature use */
		vAHI_ExtendedTemperatureOperation(TRUE);
	}
	#endif

	/* Work out allocation of Flash/EEPROM sectors (varies per chip and flash device) */
	/* JN514x Chip family ? */
	#ifdef JENNIC_CHIP_FAMILY_JN514x
	{
		extern tSPIflashFncTable *pSPIflashFncTable;

		/* Initialise flash */
		bAHI_FlashInit(E_FL_CHIP_AUTO, NULL);
		/* What is the device type ? */
		switch (pSPIflashFncTable->u16FlashId)
		{
			/* ST M25P40A 8*64Kb */
			case CONFIG_FLASH_ID_STM25P40A:
			{
				/* JN5142J01 ? */
				#ifdef JENNIC_CHIP_JN5142J01
				{
					/* JN5142J01 / M25P40A (8*64Kb) */
					/* Sector 0 - OND Application 1 (Exception dumps) */
					/* Sector 1 - OND Application 2 */
					/* Sector 2 - PDM */
					/* Sector 3 - PDM */
					/* Sector 4 - PDM */
					/* Sector 5 - PDM */
					/* Sector 6 - PDM */
					/* Sector 7 - Factory Reset */

					#ifdef OND_H_INCLUDED
					/* OND configuration */
					u8OND_SectorsAvailable = 2;
					u8OND_SectorSize 	   = 64;
					#endif
					/* PDM configuration */
					u8PdmStartSector	   = 2;
					u8PdmNumSectors        = 5;
					u32PdmSectorSize	   = 64 * 1024;
					/* Reset configuration */
					u8RstSector            = 7;
				}
				#endif

				/* JN5148J01 ? */
				#ifdef JENNIC_CHIP_JN5148J01
				{
					/* JN5148J01 / M25P40A (8*64Kb) */
					/* Sector 0 - OND Application 1 */
					/* Sector 1 - OND Application 1 (Exception dumps) */
					/* Sector 2 - OND Application 2 */
					/* Sector 3 - OND Application 2 */
					/* Sector 4 - PDM */
					/* Sector 5 - PDM */
					/* Sector 6 - PDM */
					/* Sector 7 - Factory Reset */

					/* OND configuration - assumed on JN5148 devices */
					//u8OND_SectorsAvailable = 4;
					//u8OND_SectorSize 	   = u32SectorSizeKb;
					/* PDM configuration */
					u8PdmStartSector	   = 4;
					u8PdmNumSectors        = 3;
					u32PdmSectorSize	   = 64 * 1024;
					/* Reset configuration */
					u8RstSector            = 7;
				}
				#endif

				/* JN5148 ? */
				#ifdef JENNIC_CHIP_JN5148
				{
					/* JN5148 / M25P40A (8*64Kb) */
					/* Sector 0 - Bootloader (Exception dumps) */
					/* Sector 1 - OND Application 1 */
					/* Sector 2 - OND Application 1 */
					/* Sector 3 - OND Application 2 */
					/* Sector 4 - OND Application 2 */
					/* Sector 5 - PDM */
					/* Sector 6 - PDM */
					/* Sector 7 - Factory Reset */

					/* OND configuration - assumed on JN5148 devices */
					//u8OND_SectorsAvailable = 4;
					//u8OND_SectorSize 	   = u32SectorSizeKb;
					/* PDM configuration */
					u8PdmStartSector	   = 5;
					u8PdmNumSectors        = 2;
					u32PdmSectorSize	   = 64 * 1024;
					/* Reset configuration */
					u8RstSector            = 7;
				}
				#endif
			}
			break;

			/* ST M25P10A 4*32Kb */
			case CONFIG_FLASH_ID_STM25P10A:
			{
				/* JN5142J01 ? */
				#ifdef JENNIC_CHIP_JN5142J01
				{
					/* JN5142J01 / M25P10A (4*32Kb) */
					/* Sector 0 - OND Application 1 (Exception dumps) */
					/* Sector 1 - OND Application 2 */
					/* Sector 2 - PDM */
					/* Sector 3 - Factory Reset */

					#ifdef OND_H_INCLUDED
					/* OND configuration */
					u8OND_SectorsAvailable = 2;
					u8OND_SectorSize 	   = 32;
					#endif
					/* PDM configuration */
					u8PdmStartSector	   = 2;
					u8PdmNumSectors        = 1;
					u32PdmSectorSize	   = 32 * 1024;
					/* Reset configuration */
					u8RstSector            = 3;
				}
				#endif
			}
			break;

			default:
			{
				/* Do nothing */
				;
			}
			break;
		}
	}
	/* Other chip families ? */
	#else
	{
		/* Assume 8*32Kb sectors */
		/* Sector 0 - OND Application 1 */
		/* Sector 1 - OND Application 1 */
		/* Sector 2 - OND Application 1 */
		/* Sector 3 - OND Application 1 */
		/* Sector 0 - OND Application 2 */
		/* Sector 1 - OND Application 2 */
		/* Sector 2 - OND Application 2 */
		/* Sector 3 - OND Application 2 (Exception dumps) */

		/* PDM configuration Assume 64*64byte sectors in EEPROM */
		u8PdmStartSector = 0;
		u32PdmSectorSize = 64;
		u8PdmNumSectors  = 62;
		/* Reset configuration */
		u8RstSector      = 62;
	}
	#endif
}

#ifdef  JENNIC_CHIP_FAMILY_JN516x
/****************************************************************************
 *
 * NAME: Node_bTestFactoryResetEeprom (6x EEPROM version)
 *
 * DESCRIPTION:
 * Factory reset detection
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t Node_bTestFactoryResetEeprom(void)
{
	bool_t	 bReturn = FALSE;
	uint32 au32Eeprom[3];

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_bTestFactoryResetEeprom()");

	/* Read wake timer registers */
	bAHI_ReadEEPROM(u8RstSector, 0, sizeof(au32Eeprom), (uint8 *) au32Eeprom);
	/* No history or different device ? */
	if (FACTORY_RESET_MAGIC != au32Eeprom[1] ||
	    MK_JIP_DEVICE_ID         != au32Eeprom[2])
	{
		uint8  u8Sector;
		uint8 au8Junk[64];
		/* Zero junk data */
		memset(au8Junk, 0, sizeof(au8Junk));
		/* Loop through EEPROM sectors */
		for (u8Sector = 0; u8Sector < 63; u8Sector++)
		{
			/* Write junk into the sector to invalidate it */
			bAHI_WriteEEPROM(u8Sector, 0, 64, au8Junk);
		}
		/* Initialise history */
		au32Eeprom[0] = 0xfffffffe;
		au32Eeprom[1] = FACTORY_RESET_MAGIC;
		au32Eeprom[2] = MK_JIP_DEVICE_ID;
		/* Write back to EEPROM */
		bAHI_WriteEEPROM(u8RstSector, 0, sizeof(au32Eeprom), (uint8 *) au32Eeprom);
	}
	/* Have some history ? */
	else
	{
		/* Left shift history and write */
		au32Eeprom[0] <<= 1;
		bAHI_WriteEEPROM(u8RstSector, 0, sizeof(au32Eeprom), (uint8 *) au32Eeprom);
	}
	/* Start tick timer for 1 second */
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
	vAHI_TickTimerWrite(0);
	vAHI_TickTimerInterval(32000000);
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_STOP);
	vAHI_TickTimerIntEnable(TRUE);
	/* Doze until tick timer fires */
	while (u32AHI_TickTimerRead() < 32000000)
	{
		/* Doze until tick timer fires */
		vAHI_CpuDoze();
	}
	/* Update history to flag we've run for a second */
	au32Eeprom[0] |= 0x1;
	bAHI_WriteEEPROM(u8RstSector, 0, sizeof(au32Eeprom), (uint8 *) au32Eeprom);
	/* Disable tick timer */
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);

	/* Has the device been on with the following history long, short, short, short, long ? */
	if ((au32Eeprom[0] & 0x1F) == 0x11) bReturn = TRUE;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, " = %d", bReturn);

	return bReturn;
}
#endif

#ifdef  JENNIC_CHIP_FAMILY_JN514x
/****************************************************************************
 *
 * NAME: Node_bTestFactoryResetFlash (4x Flash version)
 *
 * DESCRIPTION:
 * Factory reset detection
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t Node_bTestFactoryResetFlash(void)
{
	bool_t	 bReturn = FALSE;
	uint8   u8Flash;
	uint8   u8Mask = 0;
	uint8   u8Loop;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_bTestFactoryResetFlash()");

	/* Valid factory reset sector ? */
	if (u8RstSector != 0xFF)
	{
		/* Read flags from flash */
		bAHI_FullFlashRead((u8RstSector * u32PdmSectorSize), 1, &u8Flash);
		/* Loop through reset bits */
		for (u8Loop = 0, u8Mask = 0; u8Loop < 8 && u8Mask == 0; u8Loop++)
		{
			/* Bit is set ? */
			if (u8Flash & (1<<u8Loop))
			{
				/* Set mask */
				u8Mask = (1<<u8Loop);
				/* Clear bit */
				U8_CLR_BITS(&u8Flash, u8Mask);
			}
		}
		/* Didn't find a bit set (shouldn't happen) - zero the whole byte */
		if (u8Mask == 0) u8Flash = 0;
		/* Write updated data to flash */
		bAHI_FullFlashProgram((u8RstSector * u32PdmSectorSize), 1, &u8Flash);

		/* Start tick timer for 2 seconds */
		vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
		vAHI_TickTimerWrite(0);
		vAHI_TickTimerInterval(FACTORY_RESET_TICK_TIMER);
		vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_STOP);
		vAHI_TickTimerIntEnable(TRUE);
		/* Doze until tick timer fires */
		while (u32AHI_TickTimerRead() < FACTORY_RESET_TICK_TIMER)
		{
			/* Doze until tick timer fires */
			vAHI_CpuDoze();
		}
		/* Has the device been on with the following history long, short, short, short, long ? */
		if (u8Flash == 0xF0) bReturn = TRUE;
		/* Disable tick timer */
		vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
		/* Write updated data to flash */
		bAHI_FlashEraseSector(u8RstSector);
	}

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, " = %d", bReturn);

	return bReturn;
}
#endif

#if 0
/****************************************************************************
 *
 * NAME: Node_bTestFactoryResetWakeTimer (Wake Timer version)
 *
 * DESCRIPTION:
 * Factory reset detection
 *
 * This variant uses the wake timer and will be very dependent upon the
 * power circuitry as it will need to retain the data while the device is
 * off to work
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t Node_bTestFactoryResetWakeTimer(void)
{
	bool_t	 bReturn = FALSE;
	uint32 u32WakeTimer0;
	uint32 u32WakeTimer1;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_bTestFactoryResetWakeTimer()");

	/* Read wake timer registers */
	u32WakeTimer0 = u32REG_SysRead(REG_SYS_WK_T0);
	u32WakeTimer1 = u32REG_SysRead(REG_SYS_WK_T1);
	/* No history ? */
	if (FACTORY_RESET_MAGIC != u32WakeTimer1)
	{
		/* Initialise history and write */
		u32WakeTimer0 = 0xfffffffe;
		vREG_SysWrite(REG_SYS_WK_T0, u32WakeTimer0);
		/* Initialise signature */
		vREG_SysWrite(REG_SYS_WK_T1, FACTORY_RESET_MAGIC);
	}
	/* Have some history ? */
	else
	{
		/* Left shift history and write */
		u32WakeTimer0 <<= 1;
		vREG_SysWrite(REG_SYS_WK_T0, u32WakeTimer0);
	}
	/* Start tick timer for 1 second */
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);
	vAHI_TickTimerWrite(0);
	vAHI_TickTimerInterval(FACTORY_RESET_TICK_TIMER);
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_STOP);
	vAHI_TickTimerIntEnable(TRUE);
	/* Doze until tick timer fires */
	while (u32AHI_TickTimerRead() < FACTORY_RESET_TICK_TIMER)
	{
		/* Doze until tick timer fires */
		vAHI_CpuDoze();
	}
	/* Update history to flag we've run for a second */
	u32WakeTimer0 |= 0x1;
	vREG_SysWrite(REG_SYS_WK_T0, u32WakeTimer0);
	/* Disable tick timer */
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_DISABLE);

	/* Has the device been on with the following history long, short, short, short, long ? */
	if ((u32WakeTimer0 & 0x1F) == 0x11) bReturn = TRUE;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, " = %d", bReturn);

	return bReturn;
}
#endif

/****************************************************************************
 *
 * NAME: Node_vPdmInit
 *
 * DESCRIPTION:
 * Initialises software
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vPdmInit(uint8 u8AdcMask,
							uint8 u8OscPin,
							uint8 u8AdcPeriod)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_vPdmInit(%02x, %d, %d)", u8AdcMask, u8OscPin, u8AdcPeriod);

	/* Initialise PDM */
	PDM_vInit(u8PdmStartSector,
			  u8PdmNumSectors,
			  u32PdmSectorSize,
			  (OS_thMutex) 1,	/* Mutex */
			  NULL,
			  NULL,
			  NULL);

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nPDM_vInit(%d, %d, %d)", u8PdmStartSector, u8PdmNumSectors, u32PdmSectorSize);

	/* JN516x family ? */
	#ifdef  JENNIC_CHIP_FAMILY_JN516x
	{
		PDM_teStatus   			 ePdmStatus;

		/* Load Device PDM data */
		ePdmStatus = PDM_eLoadRecord(&sDeviceDesc,
									 0xFFFF,
									 (void *) &sDevicePdm,
									 sizeof(sDevicePdm),
									 FALSE);

		/* Record was recovered from flash ? */
		if (PDM_RECOVERY_STATE_RECOVERED == sDeviceDesc.eState)
		{
			/* Is the JIP Device ID unexpected ? */
			if (MK_JIP_DEVICE_ID != sDevicePdm.u32JipDeviceId)
			{
				/* Delete the PDM data and reset to start again */
				PDM_vDelete();
				/* Reset */
				vAHI_SwReset();
			}
		}
		/* Record was not recovered from flash */
		else
		{
			/* Set correct JIP Device ID */
			sDevicePdm.u32JipDeviceId = MK_JIP_DEVICE_ID;
			sDevicePdm.u32Spare       = 0xffffffff;
			/* Make sure permament data is saved */
			PDM_vSaveRecord(&sDeviceDesc);
		}
	}
	#endif

	/* Initialise mibs (which reads PDM data) */
	#if MK_BLD_MIB_NODE
		MibNode_vInit       (&sMibNode);
	#endif
	#if MK_BLD_MIB_GROUP
		MibGroup_vInit      (&sMibGroup);
		/* Make a note of the Group MIB PDM recovery state */
		eGroupMibPdmInitState = sMibGroup.sDesc.eState;
		/* Debug */
		DBG_vPrintf(DEBUG_NODE_VARS, "\n\teGroupMibPdmInitState = %d", sMibGroup.sDesc.eState);
	#endif
	#if MK_BLD_MIB_NODE_STATUS
		MibNodeStatus_vInit (hMibNodeStatus, &sMibNodeStatus);
	#endif
	#if MK_BLD_MIB_NODE_CONTROL
		MibNodeControl_vInit(hMibNodeControl, &sMibNodeControl);
	#endif
	#if MK_BLD_MIB_ADC_STATUS
		MibAdcStatusPatch_vInit  (hMibAdcStatus, &sMibAdcStatus, u8AdcMask, u8OscPin, u8AdcPeriod);
	#endif
	#if MK_BLD_MIB_NWK_CONFIG
		MibNwkConfigPatch_vInit  (hMibNwkConfig, &sMibNwkConfig, (void *) &sMibNwkStatus);
	#endif
	#if MK_BLD_MIB_NWK_PROFILE
		MibNwkProfile_vInit  (hMibNwkProfile, &sMibNwkProfile);
	#endif
	#if MK_BLD_MIB_NWK_STATUS
		MibNwkStatus_vInit  (hMibNwkStatus, &sMibNwkStatus, MK_SECURITY, sMibNwkConfig.sPerm.u16FrameCounterDelta);
	#endif
	#if MK_BLD_MIB_NWK_SECURITY
		#if MIB_NWK_SECURITY_PATCH
			MibNwkSecurityPatch_vInit(hMibNwkSecurity, &sMibNwkSecurity, MK_SECURITY);
		#else
			MibNwkSecurity_vInit(hMibNwkSecurity, &sMibNwkSecurity, MK_SECURITY);
		#endif
	#endif
	#if MK_BLD_MIB_NWK_TEST
		MibNwkTest_vInit(hMibNwkTest, &sMibNwkTest);
	#endif
}

/****************************************************************************
 *
 * NAME: Node_vReset
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vReset(bool_t bFactoryReset)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_vReset(%d)", bFactoryReset);

	/* Increment reset counter in node status mib */
	#if MK_BLD_MIB_NODE_STATUS
		MibNodeStatus_vIncrementResetCount();
	#endif

	/* FactoryReset ? */
	if (bFactoryReset)
	{
		/* Save or delete records as appropriate for a factory reset */
		#if MK_BLD_MIB_NODE
			PDM_vDeleteRecord(&sMibNode.sDesc);
		#endif
		#if MK_BLD_MIB_GROUP
			PDM_vDeleteRecord(&sMibGroup.sDesc);
		#endif
		#if MK_BLD_MIB_NODE_STATUS
			PDM_vSaveRecord  (&sMibNodeStatus.sDesc);
		#endif
		#if MK_BLD_MIB_NWK_CONFIG
			PDM_vDeleteRecord(&sMibNwkConfig.sDesc);
		#endif
		#if MK_BLD_MIB_NWK_PROFILE
			PDM_vDeleteRecord(&sMibNwkProfile.sDesc);
		#endif
		#if MK_BLD_MIB_NWK_STATUS
			PDM_vSaveRecord  (&sMibNwkStatus.sDesc);
		#endif
		#if MK_BLD_MIB_NWK_SECURITY
			PDM_vDeleteRecord(&sMibNwkSecurity.sDesc);
		#endif
	}
	/* Normal reset */
	else
	{
		/* Save all records to PDM */
		PDM_vSave();
	}

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nvAHI_SwReset(%d)!                ", bFactoryReset);
	/* Reset */
	vAHI_SwReset();
}

/****************************************************************************
 *
 * NAME: Node_eJipInit
 *
 * DESCRIPTION:
 * Initialises software
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC teJIP_Status Node_eJipInit(void)
{
	teJIP_Status     eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_eJipInit()");

	/* Configure JIP */
	sJipInitData.u64AddressPrefix       = CONFIG_ADDRESS_PREFIX; 		/* IPv6 address prefix (C only) */
	sJipInitData.u32Channel				= CONFIG_SCAN_CHANNELS;     	/* Channel 'bitmap' */
	sJipInitData.u16PanId				= 0xffff; /* PAN ID to use or look for (0xffff to search/generate) */
	sJipInitData.u16MaxIpPacketSize		= 0; /*CONFIG_PACKET_BUFFER_LEN-216;*/ /* Max IP packet size, 0 defaults to 1280 */
	sJipInitData.u16NumPacketBuffers	= 2;    						/* Number of IP packet buffers */
	sJipInitData.u8UdpSockets			= 2;           					/* Number of UDP sockets supported */
	sJipInitData.eDeviceType			= MK_NODE_TYPE;             		/* Device type (C, R, or ED) */
	sJipInitData.u32RoutingTableEntries	= CONFIG_ROUTING_TABLE_ENTRIES; /* Routing table size (not ED) */
	sJipInitData.u32DeviceId			= MK_JIP_DEVICE_ID;
	sJipInitData.u8UniqueWatchers		= CONFIG_UNIQUE_WATCHERS;
	sJipInitData.u8MaxTraps				= CONFIG_MAX_TRAPS;
	sJipInitData.u8QueueLength 			= CONFIG_QUEUE_LENGTH;
	sJipInitData.u8MaxNameLength		= CONFIG_MAX_NAME_LEN;
	sJipInitData.u16Port				= JIP_DEFAULT_PORT;
	sJipInitData.pcVersion 				= MK_VERSION;

	/* Building NwkConfig MIB ? */
	#if MK_BLD_MIB_NWK_CONFIG
	{
		/* Device type not retrieved from PDM ? */
		if (sMibNwkConfig.sPerm.u8DeviceType > E_JIP_DEVICE_END_DEVICE)
		{
			/* Use passed in device type */
			sMibNwkConfig.sPerm.u8DeviceType = MK_NODE_TYPE;
		}
		/* Initialise data from network config mib */
		MibNwkConfig_vJipInitData(&sJipInitData);
	}
	#endif

	/* Initialise data from network security mib */
	#if MK_BLD_MIB_NWK_SECURITY
		MibNwkSecurity_vJipInitData(&sJipInitData);
	#endif

	/* Building end device support ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* End device - zero routing table entries */
		if (sJipInitData.eDeviceType == E_JIP_DEVICE_END_DEVICE) sJipInitData.u32RoutingTableEntries = 0;
	}
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsJipInitData.u32Channel             = 0x%08x", sJipInitData.u32Channel);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsJipInitData.u16PanId               = 0x%04x", sJipInitData.u16PanId);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsJipInitData.eDeviceType            = %d",     sJipInitData.eDeviceType);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsJipInitData.u32RoutingTableEntries = %d",     sJipInitData.u32RoutingTableEntries);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsJipInitData.u32DeviceId            = 0x%08x", sJipInitData.u32DeviceId);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsJipInitData.pcVersion              = '%s'",   sJipInitData.pcVersion);

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\neJIP_Init()");
	/* Initialise JIP */
	eStatus = eJIP_Init(&sJipInitData);

	/* Set 1 second defrag timeout */
	v6LP_SetPacketDefragTimeout(1);

	/* Register mib variables (don't bother to register ADC mib) */
	#if MK_BLD_MIB_NODE
		MibNode_vRegister();
	#endif
	#if MK_BLD_MIB_GROUP
		MibGroup_vRegister();
	#endif
	#if MK_REG_MIB_NODE_STATUS
		MibNodeStatus_vRegister();
	#endif
	#if MK_REG_MIB_NODE_CONTROL
		MibNodeControl_vRegister();
	#endif
	#if MK_REG_MIB_NWK_CONFIG
		MibNwkConfig_vRegister();
	#endif
	#if MK_REG_MIB_NWK_PROFILE
		MibNwkProfile_vRegister();
	#endif
	#if MK_REG_MIB_NWK_STATUS
		MibNwkStatus_vRegister();
	#endif
	#if MK_REG_MIB_NWK_SECURITY
		MibNwkSecurity_vRegister();
	#endif
	#if MK_REG_MIB_NWK_TEST
		MibNwkTest_vRegister();
	#endif
	#if MK_REG_MIB_ADC_STATUS
		MibAdcStatus_vRegister();
	#endif

	return eStatus;
}

/****************************************************************************
 *
 * NAME: Node_v6lpConfigureNetwork
 *
 * DESCRIPTION:
 * Configures network
 *
 ****************************************************************************/
PUBLIC void Node_v6lpConfigureNetwork(tsNetworkConfigData *psNetworkConfigData)
{
	uint8   u8UpMode 	    = 0;
	uint32 u32FrameCounter  = 0;
	uint8   u8StackModeInit = 0;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_v6lpConfigureNetwork()");

	/* JN516x family ? */
	#ifdef  JENNIC_CHIP_FAMILY_JN516x
	{
		tsSecurityKey sFastKey;
		/* Set up fast commissioning key */
		sFastKey.u32KeyVal_1 = CONFIG_FAST_COMMISSIONING_KEY_1;
		sFastKey.u32KeyVal_2 = CONFIG_FAST_COMMISSIONING_KEY_2;
		sFastKey.u32KeyVal_3 = CONFIG_FAST_COMMISSIONING_KEY_3;
		sFastKey.u32KeyVal_4 = CONFIG_FAST_COMMISSIONING_KEY_4;
		/* Configure fast commissioning */
		vApi_ConfigureFastCommission(CONFIG_FAST_COMMISSIONING_CHANNEL, CONFIG_FAST_COMMISSIONING_PAN_ID);
		vSecurityUpdateKey(2, &sFastKey);
	}
	#endif

	/* Get MAC and PIB pointers */
	pvMac = pvAppApiGetMacHandle();
	psPib = MAC_psPibGetHandle(pvMac);

	/* Using Node MIB ? */
	#if MK_BLD_MIB_NODE
	{
		/* Did we not load a name from flash ? */
		if (sMibNode.sPerm.acName[0] == '\0')
		{
			uint8 		  u8NameLen;
			char          acHex[17]="0123456789ABCDEF";
			uint64      *pu64MacAddr;

			/* Get pointer to MAC address */
			pu64MacAddr = (uint64 *) pvAppApiGetMacAddrLocation();
			#ifdef MK_JIP_NODE_NAME
			/* Copy base device name */
			strncpy(sMibNode.sPerm.acName, MK_JIP_NODE_NAME, 8);
			#else
			/* Use JIP Device ID */
			sMibNode.sPerm.acName[0] = acHex[((MK_JIP_DEVICE_ID >> 28) & 0xf)];
			sMibNode.sPerm.acName[1] = acHex[((MK_JIP_DEVICE_ID >> 24) & 0xf)];
			sMibNode.sPerm.acName[2] = acHex[((MK_JIP_DEVICE_ID >> 20) & 0xf)];
			sMibNode.sPerm.acName[3] = acHex[((MK_JIP_DEVICE_ID >> 16) & 0xf)];
			sMibNode.sPerm.acName[4] = acHex[((MK_JIP_DEVICE_ID >> 12) & 0xf)];
			sMibNode.sPerm.acName[5] = acHex[((MK_JIP_DEVICE_ID >>  8) & 0xf)];
			sMibNode.sPerm.acName[6] = acHex[((MK_JIP_DEVICE_ID >>  4) & 0xf)];
			sMibNode.sPerm.acName[7] = acHex[((MK_JIP_DEVICE_ID      ) & 0xf)];
			#endif
			/* Terminate early if required */
			sMibNode.sPerm.acName[8] = '\0';
			/* Note length */
			u8NameLen = strlen(sMibNode.sPerm.acName);
			/* Append least significant 6 digits of mac address */
			sMibNode.sPerm.acName[u8NameLen    ] = ' ';
			sMibNode.sPerm.acName[u8NameLen + 1] = acHex[((*pu64MacAddr >> 20) & 0xf)];
			sMibNode.sPerm.acName[u8NameLen + 2] = acHex[((*pu64MacAddr >> 16) & 0xf)];
			sMibNode.sPerm.acName[u8NameLen + 3] = acHex[((*pu64MacAddr >> 12) & 0xf)];
			sMibNode.sPerm.acName[u8NameLen + 4] = acHex[((*pu64MacAddr >>  8) & 0xf)];
			sMibNode.sPerm.acName[u8NameLen + 5] = acHex[((*pu64MacAddr >>  4) & 0xf)];
			sMibNode.sPerm.acName[u8NameLen + 6] = acHex[((*pu64MacAddr      ) & 0xf)];
		}

	}
	#endif

	/* Using network control MIB ? */
	#if MK_BLD_MIB_NWK_SECURITY
	{
		uint8 u8Key;
		/* Loop through commissioning keys */
		for (u8Key = 1; u8Key < 3; u8Key++)
		{
			/* Did we not load a valid commissioning key from flash ? */
			if (sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_1 == 0 &&
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_2 == 0 &&
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_3 == 0 &&
				sMibNwkSecurity.sPerm.asSecurityKey[u8Key].u32KeyVal_4 == 0)
			{
				/* Generate commissioning key from MAC address */
				Security_vBuildCommissioningKey((uint8 *) pvAppApiGetMacAddrLocation(),
												(uint8 *) &sMibNwkSecurity.sPerm.asSecurityKey[u8Key]);

				/* Save to flash */
				sMibNwkSecurity.bSaveRecord = TRUE;
			}
		}
	}
	#endif

	#if MK_BLD_MIB_NWK_CONFIG
		/* Allow network config mib to set its settings and register callbacks */
		MibNwkConfig_vNetworkConfigData(psNetworkConfigData);
		/* Update user data */
		MibNwkConfigPatch_vSetUserData();
	#endif

	/* NwkStatus MIB included ? */
	#if MK_BLD_MIB_NWK_STATUS
	{
		/* Set UpMode and FrameCounter from netwrok status mib */
		u8UpMode = sMibNwkStatus.sPerm.u8UpMode;
		u32FrameCounter = sMibNwkStatus.sPerm.u32FrameCounter;
	}
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tu8UpMode = %d (START)", u8UpMode);

	/* Network config MIB included ? */
	#if MK_BLD_MIB_NWK_CONFIG
	{
		/* Set UpMode and FrameCounter from netwrok status mib */
		u8StackModeInit = sMibNwkConfig.sPerm.u8StackModeInit;
	}
	#endif

	/* Running patched network security */
	#if (MIB_NWK_SECURITY_PATCH)
	{
		/* Up mode is gateway and we are not an end device - start in standalone mode until the announce picks us up */
		if (u8UpMode                 == MIB_NWK_STATUS_UP_MODE_GATEWAY &&
		    sJipInitData.eDeviceType != E_JIP_DEVICE_END_DEVICE)
		{
			u8UpMode = MIB_NWK_STATUS_UP_MODE_STANDALONE;
		}
	}
	#endif

	/* Network contorl MIB included ? */
	#if MK_BLD_MIB_NWK_SECURITY
	{
		/* Get network security set up correctly */
		u8UpMode = MibNwkSecurity_u8NetworkConfigData(psNetworkConfigData, u8UpMode, u32FrameCounter, u8StackModeInit);
	}
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tu8UpMode = %d (FINAL)", u8UpMode);

	/* NwkStatus MIB included ? */
	#if MK_BLD_MIB_NWK_STATUS
	{
		/* Copy the final up mode value */
		sMibNwkStatus.sPerm.u8UpMode = u8UpMode;
	}
	#endif

	#if MK_BLD_MIB_NWK_PROFILE
	{
		/* Override scan sort handler with patched beacon response handler */
		vApi_RegScanSortCallback(MibNwkProfile_bScanSortCallback);
		/* Ensure the configured profile is applied */
		MibNwkProfile_vApply();
	}
	#else
	{
		/* Set to default join profile */
		(void) bJnc_SetJoinProfile(CONFIG_JOIN_PROFILE, NULL);
	}
	#endif

	/* Building end device support ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Want to sleep between scans ? */
		#if DEVICE_ED_SCAN_SLEEP
		{
			/* End device ? */
			if (sJipInitData.eDeviceType == E_JIP_DEVICE_END_DEVICE)
			{
				/* Sleep during backoff for end device */
				psNetworkConfigData->bSleepDuringBackoff   = TRUE;
				psNetworkConfigData->u32EndDeviceScanSleep = DEVICE_ED_SCAN_SLEEP;
			}
		}
		#endif
	}
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tpsNetworkConfigData->bSleepDuringBackoff     = %d", psNetworkConfigData->bSleepDuringBackoff);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tpsNetworkConfigData->u8EndDevicePingInterval = %d", psNetworkConfigData->u8EndDevicePingInterval);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tpsNetworkConfigData->u32EndDeviceScanTimeout = %d", psNetworkConfigData->u32EndDeviceScanTimeout);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tpsNetworkConfigData->u32EndDeviceScanSleep   = %d", psNetworkConfigData->u32EndDeviceScanSleep);
	DBG_vPrintf(DEBUG_NODE_VARS, "\n\tpsNetworkConfigData->u32EndDevicePollPeriod  = %d", psNetworkConfigData->u32EndDevicePollPeriod);
}

/****************************************************************************
 *
 * NAME: Node_v6lpDataEvent
 *
 * DESCRIPTION:
 * As this app uses JIP for all communication we are not interested in the
 * 6LP data events. For any receive events we simply discard the packet to
 * free the packet buffer.
 *
 * PARAMETERS: Name        RW  Usage
 *             iSocket     R   Socket on which packet received
 *             eEvent      R   Data event
 *             psAddr      R   Source address (for RX) or destination (for TX)
 *             u8AddrLen   R   Length of address
 *
 ****************************************************************************/
PUBLIC void Node_v6lpDataEvent(  int 			  iSocket,
							  	 te6LP_DataEvent  eEvent,
                              	 ts6LP_SockAddr *psAddr,
                              	 uint8           u8AddrLen)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_v6lpDataEvent(%d)", eEvent);

	/* Which event ? */
	switch (eEvent)
	{
		/* Data received ? */
		/* IP data received ? */
		/* 6LP ICMP message ? */
		case E_DATA_RECEIVED:
		case E_IP_DATA_RECEIVED:
		case E_6LP_ICMP_MESSAGE:
		{

			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, "\ni6LP_RecvFrom()");
			/* Discard 6LP packets as only interested in JIP communication  */
			i6LP_RecvFrom(iSocket, NULL, 0, 0, NULL, NULL);
		}
		break;

		/* Others ? */
		default:
		{
			/* Do nothing */
			;
		}
		break;
	}
}

/****************************************************************************
 *
 * NAME: Node_vJipStackEvent
 *
 * DESCRIPTION:
 * Processes any incoming stack events.
 * Once a join indication has been received, we initialise JIP and register
 * the various MIBs.
 *
 * PARAMETERS: Name          RW Usage
 *             eEvent        R  Stack event
 *             pu8Data       R  Additional information associated with event
 *             u8DataLen     R  Length of additional information
 *
 ****************************************************************************/
PUBLIC bool_t Node_bJipStackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen)
{
	bool_t bReturn   = FALSE;
	bool_t bSuppress = FALSE;

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_bJipStackEvent(%d)", eEvent);

	/* Which event ? */
    switch (eEvent)
    {
    	/* Joined network ? */
		case E_STACK_JOINED:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " JOINED");

			/* Network is now up */
			bUp     = TRUE;

			/* OND ? */
			#ifdef OND_H_INCLUDED
				#ifdef MK_BLD_NODE_TYPE_END_DEVICE
				/* End device ? */
				if (sJipInitData.eDeviceType == E_JIP_DEVICE_END_DEVICE)
				{
					/* Debug */
					DBG_vPrintf(DEBUG_NODE_FUNC, "\neOND_SleepingDevInit()");
					/* Initialise OND */
					eOND_SleepingDevInit();
				}
				else
				#endif
				{
					/* Debug */
					DBG_vPrintf(DEBUG_NODE_FUNC, "\neOND_DevInit()");
					/* Initialise OND */
					eOND_DevInit();
				}
		    #else
		    	#warning OND IS DISABLED!!!
		    #endif

			/* Not yet joined a network ? */
			if (bJoined == FALSE)
			{
			    /* Group MIB included ? */
			    #if MK_BLD_MIB_GROUP
			    {
					/* Did we not recover the Group MIB's data from the PDM ? */
					if (eGroupMibPdmInitState != PDM_RECOVERY_STATE_RECOVERED)
					{
						/* Place device into global group */
						(void) bJIP_JoinGroup(0xf00f);
						/* Debug */
						DBG_vPrintf(DEBUG_NODE_VARS, "\n\tbJIP_JoinGroup(0xf00f) {DEFAULT}");
					}
				}
				#endif
				/* We have now joined a network */
				bJoined = TRUE;
			}

			/* MibNwkConfig included ? */
			#if MK_BLD_MIB_NWK_CONFIG
			{
				/* Call set user data function */
				MibNwkConfigPatch_vSetUserData();
			}
			#endif

		    /* Commissioning timeout set ? */
		    #if (DEVICE_COMMISSION_TIMEOUT > 0)
			#if MK_BLD_MIB_NODE_CONTROL && MK_BLD_MIB_NWK_STATUS
		    {
				/* Have we joined in standalone mode (rather than resumed) ? */
				if ((u16Api_GetStackMode() & NONE_GATEWAY_MODE) != 0 &&
					sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
				{
					/* Start the factory reset timer */
					sMibNodeControl.sTemp.u16FactoryReset = DEVICE_COMMISSION_TIMEOUT;
				}
			}
			#endif
			#endif
		}
		break;

		/* Lost network ? */
		case E_STACK_RESET:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " RESET");

			/* Network is now down */
			bUp = FALSE;

			/* Were we deliberately resetting the stack to gateway mode ? */
			if (u8NwkSecurityReset == MIB_NWK_SECURITY_RESET_STACK_TO_GATEWAY)
			{
				/* Debug */
				DBG_vPrintf(DEBUG_NODE_VARS, "\n\tu8NwkSecurityReset = RESET_STACK_TO_GATEWAY");
				/* Suppress passing this event to other modules */
				bSuppress = TRUE;
				/* Stack should now be idle -  clear reset state */
				u8NwkSecurityReset = MIB_NWK_SECURITY_RESET_NONE;
				/* Set up stack to resume running in gateway mode (IGNORE RETURNED UP MODE) */
				(void) MibNwkSecurity_u8ResumeGateway(sMibNwkSecurity.psNetworkConfigData,
												 	  sMibNwkStatus.sPerm.u8UpMode,
												 	  psPib->u32MacFrameCounter);
				/* Ensure we are in gateway mode */
				vApi_SetStackMode(0);
				/* Ensure we are using correct profile for gateway system */
				MibNwkSecurity_vSetProfile(FALSE);
				/* Don't skip joining procedure */
				vApi_EnableJoin();
				/* Restart the stack */
				vApi_ReStartCR(FALSE);
			}
			/* Are we deliberately resetting the stack to standalone mode ? */
			else if (u8NwkSecurityReset == MIB_NWK_SECURITY_RESET_STACK_TO_STANDALONE)
			{
				/* Debug */
				DBG_vPrintf(DEBUG_NODE_VARS, "\n\tu8NwkSecurityReset = RESET_STACK_TO_STANDALONE");
				/* Suppress passing this event to other modules */
				bSuppress = TRUE;
				/* Stack should now be idle -  clear reset state */
				u8NwkSecurityReset = MIB_NWK_SECURITY_RESET_NONE;
				/* Update network status */
				sMibNwkStatus.sPerm.u8UpMode = MIB_NWK_STATUS_UP_MODE_STANDALONE;
				sMibNwkStatus.bSaveRecord    = TRUE;
				/* Set up stack to resume running in standalone mode */
				(void)	MibNwkSecurity_u8ResumeStandalone(sMibNwkSecurity.psNetworkConfigData,
				 		 							  	  sMibNwkStatus.sPerm.u8UpMode,
												 		  psPib->u32MacFrameCounter);
				/* Restart the stack */
				vApi_ReStartCR(FALSE);
			}
			/* Never joined a network ? */
			else if (sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
			{
				/* Debug */
				DBG_vPrintf(DEBUG_NODE_VARS, "\n\tsMibNwkStatus.sPerm.u8UpMode = UP_MODE_NONE");
				/* Re-apply default join profile */
				(void) bJnc_SetJoinProfile(CONFIG_JOIN_PROFILE, NULL);
			}
		}
		break;

		/* Gateway present ? */
		case E_STACK_NETWORK_ANNOUNCE:
		{
			/* Debug */
			DBG_vPrintf(DEBUG_NODE_FUNC, " NETWORK_ANNOUNCE");
		}
		break;

		/* Allowing sleep ? */
		#ifdef MK_BLD_NODE_TYPE_END_DEVICE
		/* Poll event ? */
		case E_STACK_POLL:
		{
			te6LP_PollResponse ePollResponse;

			/* Cast response */
			ePollResponse = *((te6LP_PollResponse *) pu8Data);
			/* Debug */
			DBG_vPrintf(DEBUG_DEVICE_FUNC, " POLL %d", ePollResponse);

			/* Which response ? */
			switch (ePollResponse)
			{
				/* Got some data ? */
				case E_6LP_POLL_DATA_READY:
				{

					/* Debug */
					DBG_vPrintf(DEBUG_DEVICE_FUNC, " DATA_READY");
					/* Poll again in case there is more */
					ePollResponse = e6LP_Poll();
					/* Debug */
					DBG_vPrintf(DEBUG_DEVICE_FUNC, "\ne6LP_Poll() = %d", ePollResponse);
				}
				break;

				/* Others ? */
				default:
				{
					/* Network up - set flag to indicate polled but no data */
					if (bUp) bReturn = TRUE;
				}
				break;
			}
		}
		break;
		#endif

		/* Others ? */
	    default:
		{
			/* Do nothing */
			;
		}
        break;
    }

	/* Pass on to mibs */
	#if MK_BLD_MIB_GROUP
		MibGroup_vStackEvent(eEvent);
	#endif
	#if MK_BLD_MIB_NWK_STATUS
		/* Not stack reset ? */
		if (eEvent != E_STACK_RESET)
		{
			/* Pass on to MIB */
			MibNwkStatus_vStackEvent(eEvent);
		}
		/* Stack reset (override standard handling to avoid up mode being changed) ? */
		else
		{
			/* Was network up previously ? */
			if (TRUE == sMibNwkStatus.bUp)
			{
				/* Debug */
				DBG_vPrintf(CONFIG_DBG_MIB_NWK_STATUS, "\nMibNwkStatus_vStackEvent(STACK_RESET)");
				/* Clear up flag */
				sMibNwkStatus.bUp = FALSE;
			}
		}
	#endif
	#if MK_BLD_MIB_NWK_SECURITY
	{
		/* Patch network security MIB ? */
		#if MIB_NWK_SECURITY_PATCH
		{
			/* Not suppressing ? */
			if (FALSE == bSuppress)
			{
				/* Pass to patched NwkSecurity MIB  */
				u8NwkSecurityReset = MibNwkSecurityPatch_u8StackEvent(eEvent, pu8Data, u8DataLen);
				/* Need to reset ? */
				switch (u8NwkSecurityReset)
				{
					/* Factory reset ? */
					case MIB_NWK_SECURITY_RESET_FACTORY:
					{
						/* Clear reset type */
						u8NwkSecurityReset = MIB_NWK_SECURITY_RESET_NONE;
						/* Perform a factory reset */
						Device_vReset(TRUE);
					}
					break;
					/* Chip reset ? */
					case MIB_NWK_SECURITY_RESET_CHIP:
					{
						/* Clear reset type */
						u8NwkSecurityReset = MIB_NWK_SECURITY_RESET_NONE;
						/* Perform a factory reset */
						Device_vReset(FALSE);
					}
					break;
					/* Reset stack ? */
					case MIB_NWK_SECURITY_RESET_STACK_TO_GATEWAY:
					case MIB_NWK_SECURITY_RESET_STACK_TO_STANDALONE:
					{
						/* Reset the stack to idle mode - which will cause a re-entrant call with another stack reset (handled above) */
						while(FALSE == bApi_ResetStack(FALSE, 0x00));
					}
					break;
				}
			}
		}
		/* Unpatched network security MIB ? */
		#else
		{
			/* Pass to NwkSecurity MIB - need to reset ? */
			if (MibNwkSecurity_bStackEvent(eEvent, pu8Data, u8DataLen) == TRUE)
			{
				/* Perform a reset */
				Device_vReset(FALSE);
			}
		}
		#endif
	}
	#endif
	#if MK_BLD_MIB_NWK_TEST
		MibNwkTest_vStackEvent(eEvent, pu8Data, u8DataLen);
	#endif
	#if MK_BLD_MIB_NWK_PROFILE
		/* Ensure the configured profile is applied */
		MibNwkProfile_vApply();
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_bJipStackEvent(%d) RETURN=%d", eEvent, bReturn);

	return bReturn;
}

/****************************************************************************
 *
 * NAME: Node_u8Analogue
 *
 * DESCRIPTION:
 * Called when an ADC conversion is completed
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC uint8 Node_u8Analogue(void)
{
	/* AdcStatus MIB has been built ? */
	#if MK_BLD_MIB_ADC_STATUS
		/* Just return value from ADC MIB */
		return MibAdcStatusPatch_u8Analogue();
	#else
		/* Return invalid value */
		return 0xFF;
	#endif
}

/****************************************************************************
 *
 * NAME: Node_vTick
 *
 * DESCRIPTION:
 * Called every 10ms
 *
 * u8Tick should loop from 0 to 99 each call
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vTick(uint8 u8Tick, uint32 u32Second)
{
		/* A second has passed ? */
		if (u8Tick == 0)
		{
			/* Debug ? */
			#ifdef DBG_ENABLE
			{
				/* A minute has passed ? */
				if (u32Second % 60 == 0)
				{
					/* Debug */
					DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_vTick(%d, %d)", u8Tick, u32Second);
				}
			}
			#endif

			/* Update stack min address */
			Exception_vUpdateStackMin();

			/* Join timeout configured ? */
			#if DEVICE_JOIN_TIMEOUT
			{
				/* Should we stop trying to join now ? */
				if (u32Second == DEVICE_JOIN_TIMEOUT &&
					sMibNwkStatus.sPerm.u8UpMode == MIB_NWK_STATUS_UP_MODE_NONE)
				{
					/* Debug */
					DBG_vPrintf(DEBUG_NODE_FUNC, "\nbApi_ResetStack(IDLE)");
					/* Reset to idle */
					while(FALSE == bApi_ResetStack(FALSE, 0x00));
				}
			}
			#endif
		}

		/* Pass seconds onto mibs (spread across a second) */
		#if MK_BLD_MIB_NODE
			if (u8Tick == 0 ) MibNode_vSecond();
		#endif
		#if MK_BLD_MIB_GROUP
			if (u8Tick == 8 ) MibGroup_vSecond();
		#endif
		#if MK_BLD_MIB_NODE_STATUS
			if (u8Tick == 16) MibNodeStatus_vSecond();
		#endif
		#if MK_BLD_MIB_NODE_STATUS
			if (u8Tick == 24) MibNodeControl_vSecond();
		#endif
		#if MK_BLD_MIB_NWK_CONFIG
			if (u8Tick == 32) MibNwkConfig_vSecond();
		#endif
		#if MK_BLD_MIB_NWK_PROFILE
			if (u8Tick == 40) MibNwkProfile_vSecond();
		#endif
		#if MK_BLD_MIB_NWK_STATUS
			if (u8Tick == 48) MibNwkStatus_vSecond();
		#endif
		#if MK_BLD_MIB_NWK_SECURITY
			if (u8Tick == 56) MibNwkSecurityPatch_vSecond();
		#endif

		/* Pass tick on to mibs */
		#if MK_BLD_MIB_ADC_STATUS
			MibAdcStatus_vTick();
		#endif
		#if MK_BLD_MIB_NWK_TEST
			MibNwkTest_vTick();
		#endif
}

/****************************************************************************
 *
 * NAME: Node_bUp
 *
 * DESCRIPTION:
 * Returns if network is up
 *
 * RETURNS:
 * Network state
 *
 ****************************************************************************/
PUBLIC bool_t Node_bUp(void)
{
	return bUp;
}

/****************************************************************************
 *
 * NAME: Node_eGetGroupMibPdmInitState
 *
 * DESCRIPTION:
 * Returns initial state of Group MIB PDM load record request
 *
 * RETURNS:
 * Network state
 *
 ****************************************************************************/
PUBLIC PDM_teRecoveryState Node_eGetGroupMibPdmInitState(void)
{
	return eGroupMibPdmInitState;
}

#ifdef MK_BLD_NODE_TYPE_END_DEVICE
/****************************************************************************
 *
 * NAME: Node_vSleep
 *
 * DESCRIPTION:
 * Called prior to sleeping
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Node_vSleep(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_NODE_FUNC, "\nNode_vSleep()");

	/* Pass fake second calls into mibs (these save PDM data) */
	#if MK_BLD_MIB_NODE
		MibNode_vSecond();
	#endif
	#if MK_BLD_MIB_GROUP
		MibGroup_vSecond();
	#endif
	#if MK_BLD_MIB_NODE_STATUS
		MibNodeStatus_vSecond();
	#endif
	#if MK_BLD_MIB_NODE_STATUS
		MibNodeControl_vSecond();
	#endif
	#if MK_BLD_MIB_NWK_CONFIG
		MibNwkConfig_vSecond();
	#endif
	#if MK_BLD_MIB_NWK_PROFILE
		MibNwkProfile_vSecond();
	#endif
	#if MK_BLD_MIB_NWK_STATUS
		MibNwkStatus_vSecond();
	#endif
	#if MK_BLD_MIB_NWK_SECURITY
		MibNwkSecurityPatch_vSecond();
	#endif
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
