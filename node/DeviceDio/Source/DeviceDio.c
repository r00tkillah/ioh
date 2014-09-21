/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DeviceDio - Main Digital I/O Source File
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
#include "MibDio.h"
#include "MibDioStatus.h"
#include "MibDioConfig.h"
#include "MibDioControl.h"
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

/* ADC control */
#define DEVICE_ADC_MASK 		(MIB_ADC_MASK_SRC_VOLT|MIB_ADC_MASK_SRC_TEMP) /* ADCs to be read */
#define DEVICE_ADC_SRC_BUS_VOLTS E_AHI_ADC_SRC_VOLT
#define DEVICE_ADC_PERIOD 		 25											  /* ADC sample period 0-100 in 10ms intervals */

/* DIO pins */
#define DEVICE_DIO_OSC_PULL			16	/* Oscillator push/pull pin */

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

#if MK_BLD_MIB_DIO_STATUS
extern tsMibDioStatus	 sMibDioStatus;
extern thJIP_Mib		 hMibDioStatus;
#endif

#if MK_BLD_MIB_DIO_CONFIG
extern tsMibDioConfig	 sMibDioConfig;
extern thJIP_Mib		 hMibDioConfig;
#endif

#if MK_BLD_MIB_DIO_CONTROL
extern tsMibDioControl	 sMibDioControl;
extern thJIP_Mib		 hMibDioControl;
#endif

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/* Counters */
PRIVATE uint8   u8TickQueue;	/* Tick timer queue */
PRIVATE uint8   u8Tick;	   		/* Tick counter */
PUBLIC  uint32 u32Second;  		/* Second counter */
/* Other data */
PRIVATE bool_t   bInitialised;
PRIVATE bool_t   bSleep;
PRIVATE uint32 u32Awake;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void 			Device_vInit(bool_t bWarmStart);
PUBLIC void 			Device_vPdmInit(void);
PUBLIC void 			Device_vReset(bool_t bFactoryReset);
PUBLIC teJIP_Status 	Device_eJipInit(void);
PUBLIC void 			Device_vMain(void);
PUBLIC void 			Device_vTick(void);
#ifdef MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void 			Device_vSleep(void);
#endif

/****************************************************************************
 *
 * NAME: AppColdStart
 *
 * DESCRIPTION:
 * Entry point for application from boot loader.
 *
 * RETURNS:
 * void, never returns
 *
 ****************************************************************************/
PUBLIC void AppColdStart(void)
{
	/* Initialise device */
	Device_vInit(FALSE);
}

/****************************************************************************
 *
 * NAME: AppWarmStart
 *
 * DESCRIPTION:
 * Entry point for application from boot loader.
 *
 * RETURNS:
 * Never returns.
 *
 ****************************************************************************/
PUBLIC void AppWarmStart(void)
{
	/* Initialise device */
	Device_vInit(TRUE);
}

/****************************************************************************
 *
 * NAME: Device_vInit
 *
 * DESCRIPTION:
 * Entry point for application
 *
 * RETURNS:
 * void, never returns
 *
 ****************************************************************************/
PUBLIC void Device_vInit(bool_t bWarmStart)
{
	bool_t   				  bFactoryReset;

	/* Initialise stack and hardware interfaces */
	v6LP_InitHardware();

	/* Cold start ? */
	if (FALSE == bWarmStart)
	{
		/* Initialise exception handler */
		Exception_vInit();

		/* Initialise all DIO as outputs and drive low */
		vAHI_DioSetDirection(0, 0xFFFFFFFE);
		vAHI_DioSetOutput(0, 0xFFFFFFFE);
	}

	/* Debug ? */
	#ifdef DBG_ENABLE
	{
		/* Initialise debugging */
		DBG_vUartInit(DEBUG_UART, DEBUG_BAUD_RATE);
		/* Disable the debug port flow control lines to turn off LED2 */
		vAHI_UartSetRTSCTS(DEBUG_UART, FALSE);
	}
	#endif

	/* 6x chip family ? */
	#ifdef  JENNIC_CHIP_FAMILY_JN516x
		/* Wait for clock to stablise */
		while(bAHI_Clock32MHzStable() == FALSE);
	#endif
	/* 42J01 chip ? */
	#ifdef JENNIC_CHIP_JN5142J01
		/* Wait for clock to stablise */
		while(bAHI_Clock32MHzStable() == FALSE);
	#endif

	/* Debug */
	DBG_vPrintf(TRUE, "                ");
	DBG_vPrintf(TRUE, "\n\nDEVICE DIO");
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_vInit(%d)", bWarmStart);

	/* Node initialisation */
	Node_vInit(bWarmStart);

	/* Cold start ? */
	if (FALSE == bWarmStart)
	{
		/* 4x chip family ? */
		#ifdef  JENNIC_CHIP_FAMILY_JN514x
		{
			/* Check for factory reset using flags from flash */
			bFactoryReset = Node_bTestFactoryResetFlash();
		}
		#else
		{
			/* Check for factory reset using flags from EEPROM */
			bFactoryReset = Node_bTestFactoryResetEeprom();
		}
		#endif
		/* Reset the tick queue */
		u8TickQueue = 0;

		/* Initialise PDM and MIB data */
		Device_vPdmInit();

		/* Apply factory reset if required */
		if (bFactoryReset) Device_vReset(TRUE);

		/* Initialise JIP */
		(void) Device_eJipInit();
	}
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	else
	{
		/* Debug */
		DBG_vPrintf(DEBUG_DEVICE_FUNC, "\ni6LP_ResumeStack()");
		/* Resume 6LoWPAN */
		i6LP_ResumeStack();
	}
	#endif

	/* Now initialised */
	bInitialised = TRUE;

	/* Enter main loop */
	Device_vMain();

	/* Allow sleeping ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Go to sleep if we exit main loop */
		Device_vSleep();
	}
	#endif
}

/****************************************************************************
 *
 * NAME: Device_vPdmInit
 *
 * DESCRIPTION:
 * Initialise PDM MIB data
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vPdmInit(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_vPdmInit()");

	/* Initialise PDM and Node MIBs*/
	Node_vPdmInit(DEVICE_ADC_MASK, DEVICE_DIO_OSC_PULL, DEVICE_ADC_PERIOD);

	/* Initialise DIO MIBs */
	#if MK_BLD_MIB_DIO_CONFIG
		MibDioConfig_vInit(hMibDioConfig, &sMibDioConfig);
	#endif
	#if MK_BLD_MIB_DIO_STATUS
		MibDioStatus_vInit(hMibDioStatus, &sMibDioStatus);
	#endif
	#if MK_BLD_MIB_DIO_CONTROL
		MibDioControl_vInit(hMibDioControl, &sMibDioControl);
	#endif
}

/****************************************************************************
 *
 * NAME: Device_vReset
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vReset(bool_t bFactoryReset)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_vReset(%d)", bFactoryReset);

	/* FactoryReset ? */
	if (bFactoryReset)
	{
		#if MK_BLD_MIB_DIO_CONFIG
			PDM_vDeleteRecord(&sMibDioConfig.sDesc);
		#endif
		#if MK_BLD_MIB_DIO_CONTROL
			PDM_vDeleteRecord(&sMibDioControl.sDesc);
		#endif
	}

	/* Perform node level reset */
	Node_vReset(bFactoryReset);
}

/****************************************************************************
 *
 * NAME: Device_eJipInit
 *
 * DESCRIPTION:
 * Initialise JIP layer stack
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC teJIP_Status Device_eJipInit(void)
{
	teJIP_Status     eStatus;

	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_eJipInit()");

	/* Node JIP initialisation */
	eStatus = Node_eJipInit();

	/* Register DIO MIBs */
	#if MK_REG_MIB_DIO_STATUS
		MibDioStatus_vRegister();
	#endif
	#if MK_REG_MIB_DIO_CONFIG
		MibDioConfig_vRegister();
	#endif
	#if MK_REG_MIB_DIO_CONTROL
		MibDioControl_vRegister();
	#endif

	return eStatus;
}

/****************************************************************************
 *
 * NAME: v6LP_ConfigureNetwork
 *
 * DESCRIPTION:
 * Configures network
 *
 ****************************************************************************/
PUBLIC void v6LP_ConfigureNetwork(tsNetworkConfigData *psNetworkConfigData)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nv6LP_ConfigureNetwork()");

	/* Call node configure network */
	Node_v6lpConfigureNetwork(psNetworkConfigData);
}

/****************************************************************************
 *
 * NAME: Device_vMain
 *
 * DESCRIPTION:
 * Main device loop
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vMain(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_vMain()");
	/* Make sure sleep flag is clear */
	bSleep = FALSE;
	/* Main loop */
	while(FALSE == bSleep || u32Awake > 0)
	{
	   	/* Main processing for network config changing mode ? */
	   	if (MibNwkConfigPatch_bMain())
	   	{
			#if MK_BLD_MIB_NWK_PROFILE
				/* Ensure the configured profile is applied */
				MibNwkProfile_vApply();
			#endif
		}
		/* Restart watchdog */
	   	vAHI_WatchdogRestart();
	   	/* Deal with device tick timer events ? */
	   	Device_vTick();
		/* Doze */
		vAHI_CpuDoze();
	}
}

/****************************************************************************
 *
 * NAME: v6LP_DataEvent
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
PUBLIC void v6LP_DataEvent(int iSocket, te6LP_DataEvent eEvent,
                           ts6LP_SockAddr *psAddr, uint8 u8AddrLen)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nv6LP_DataEvent(%d)", eEvent);

	/* Call node data event handler */
	Node_v6lpDataEvent(iSocket, eEvent, psAddr, u8AddrLen);
}

/****************************************************************************
 *
 * NAME: vJIP_StackEvent
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
PUBLIC void vJIP_StackEvent(te6LP_StackEvent eEvent, uint8 *pu8Data, uint8 u8DataLen)
{
	bool_t bPollNoData;

	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nvJIP_StackEvent(%d)", eEvent);

	/* Node handling */
	bPollNoData = Node_bJipStackEvent(eEvent, pu8Data, u8DataLen);
	/* Allowing sleep ? */
	#ifdef MK_BLD_NODE_TYPE_END_DEVICE
	{
		/* Did we get a poll response but no data ? */
		if (bPollNoData)
		{
			/* Set flag for sleep */
			bSleep = TRUE;
		}
	}
	#endif

	/* MIB handling */
	MibDioControl_vStackEvent(eEvent);

	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nvJIP_StackEvent(%d) RETURN", eEvent);
}

/****************************************************************************
 *
 * NAME: v6LP_PeripheralEvent
 *
 * DESCRIPTION:
 * Processes any incoming peripheral events. The end device is completely
 * event driven (the idle loop does nothing) and events here are simply used
 * to update the appropriate JIP module.
 *
 * PARAMETERS: Name          RW Usage
 *             u32Device     R  Device that caused peripheral event
 *             u32ItemBitmap R  Events within that peripheral
 *
 ****************************************************************************/
PUBLIC void v6LP_PeripheralEvent(uint32 u32Device, uint32 u32ItemBitmap)
{
	/* Tick Timer is run by stack at 10ms intervals */
	if (u32Device == E_AHI_DEVICE_TICK_TIMER)
	{
		/* Initialised ? */
		if (bInitialised == TRUE)
		{
			/* Increment pending ticks */
			if (u8TickQueue < 100) u8TickQueue++;
		}
	}

	/* ADC reading ? */
	if (u32Device == E_AHI_DEVICE_ANALOGUE)
	{
		uint8 u8Adc;

		/* Perform node handling (which includes obtaining the ADC source that completed) */
		u8Adc = Node_u8Analogue();
	}

	/* Building MIB DioStatus ? */
	#if MK_BLD_MIB_DIO_STATUS
	{
		/* System Control interrupts are raised for DIO events */
		if (u32Device == E_AHI_DEVICE_SYSCTRL)
		{
			/* Pass into DIO MIB */
			MibDioStatus_vSysCtrl(u32Device, u32ItemBitmap);
		}
	}
	#endif
}

/****************************************************************************
 *
 * NAME: Device_vTick
 *
 * DESCRIPTION:
 * Reset device
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vTick(void)
{
	/* Tick timer fired ? */
	while (u8TickQueue > 0)
	{
		/* Increment tick counter */
		u8Tick++;

		/* A second has passed ? */
		if (u8Tick >= 100)
		{
			/* Zero tick counter */
			u8Tick = 0;
			/* Increment second counter */
			if (u32Second < 0xFFFFFFFF) u32Second++;
			/* Debug ? */
			#ifdef DBG_ENABLE
			{
				/* A minute has passed ? */
				if (u32Second % 60 == 0)
				{
					/* Debug */
					DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_vTick() u32Second = %d", u32Second);
				}
			}
			#endif
		}

		/* Node tick handling */
		Node_vTick(u8Tick, u32Second);

		/* Pass seconds onto DIO mibs */
		#if MK_BLD_MIB_DIO_CONFIG
			if (u8Tick == 80) MibDioConfig_vSecond();
		#endif
		#if MK_BLD_MIB_DIO_CONTROL
			if (u8Tick == 88) MibDioControl_vSecond();
		#endif

		/* Pass tick on to DIO mibs */
		#if MK_BLD_MIB_DIO_STATUS
			MibDioStatus_vTick();
		#endif

		/* Call JIP tick for last tick in queue */
		if (u8TickQueue == 1)
		{
			/* JN514x family ? */
			#ifdef  JENNIC_CHIP_FAMILY_JN514x
			{
				/* Call 6LP tick for last entry in queue */
				v6LP_Tick();
			}
			/* Other chip family ? */
			#else
			{
				/* Call JIP tick for last entry in queue */
				vJIP_Tick();
			}
			#endif
		}
		/* Decrement pending ticks */
		if (u8TickQueue > 0) u8TickQueue--;

		/* Decrement stay awake timer */
		if (u32Awake > 0) u32Awake--;
	}
}

/****************************************************************************
 *
 * NAME: Device_vException
 *
 * DESCRIPTION:
 *
 ****************************************************************************/
PUBLIC WEAK void Device_vException(uint32 u32HeapAddr, uint32 u32Vector, uint32 u32Code)
{
	/* Reset */
	vAHI_SwReset();
}

#ifdef MK_BLD_NODE_TYPE_END_DEVICE
/****************************************************************************
 *
 * NAME: vJIP_StayAwakeRequest()
 *
 * DESCRIPTION:
 * Request from JIP to stay awake a bit longer
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC WEAK void vJIP_StayAwakeRequest(void)
{
	/* Set awake timer */
	u32Awake = DEVICE_ED_REQ_AWAKE;
}

/****************************************************************************
 *
 * NAME: Device_vSleep
 *
 * DESCRIPTION:
 * Go to sleep
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Device_vSleep(void)
{
	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nDevice_vSleep()");


	/* Debug */
	//DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nPDM_vSave()");
	/* Save all records to PDM */
	//PDM_vSave();

	/* Get node to do pre-sleep preparation */
	Node_vSleep();

	/* Pass fake second calls into mibs (these save PDM data) */
	#if MK_BLD_MIB_DIO_CONFIG
		MibDioConfig_vSecond();
	#endif
	#if MK_BLD_MIB_DIO_CONTROL
		MibDioControl_vSecond();
	#endif

	/* Debug */
	DBG_vPrintf(DEBUG_DEVICE_FUNC, "\nv6LP_Sleep(TRUE, %d)        ", DEVICE_ED_SLEEP);
	/* Request stack to put us to sleep */
	v6LP_Sleep(TRUE, DEVICE_ED_SLEEP);

	/* Sleep loop */
	while(1)
	{
	   	/* Deal with device tick timer events ? */
	   	Device_vTick();
		/* Doze */
		vAHI_CpuDoze();
	}
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
