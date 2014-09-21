/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NodeControl MIB - Implementation
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
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application common includes */
#include "MibCommonDebug.h"
#include "MibNodeControl.h"
#include "MibNodeStatus.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

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
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsMibNodeControl 	*psMibNodeControl;	   /* Node Control Mib data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/
PUBLIC void Device_vReset(bool_t bFactoryReset);

/****************************************************************************
 *
 * NAME: MibNodeControl_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibNodeControl_vInit(thJIP_Mib           hMibNodeControlInit,
								 tsMibNodeControl 	*psMibNodeControlInit)
{
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NODE_CONTROL, "\nMibNodeControl_vInit() {%d}", sizeof(tsMibNodeControl));

	/* Valid data pointer ? */
	if (psMibNodeControlInit != (tsMibNodeControl *) NULL)
	{
		/* Take copy of pointer to data */
		psMibNodeControl = psMibNodeControlInit;

		/* Take a copy of the MIB handle */
		psMibNodeControl->hMib = hMibNodeControlInit;
	}
}

/****************************************************************************
 *
 * NAME: MibNodeControl_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibNodeControl_vRegister(void)
{
	teJIP_Status eStatus;

	/* Register MIB */
	eStatus = eJIP_RegisterMib(psMibNodeControl->hMib);
	/* Debug */
	DBG_vPrintf(CONFIG_DBG_MIB_NODE_CONTROL, "\nMibNodeControl_vRegister()");
	DBG_vPrintf(CONFIG_DBG_MIB_NODE_CONTROL, "\n\teJIP_RegisterMib(NodeControl)=%d", eStatus);
}

/****************************************************************************
 *
 * NAME: MibNodeControl_vSecond
 *
 * DESCRIPTION:
 * Timing function
 *
 ****************************************************************************/
PUBLIC void MibNodeControl_vSecond(void)
{
	/* Counting down to factory reset ? */
	if (psMibNodeControl->sTemp.u16FactoryReset > 0)
	{
		/* Decrement */
		psMibNodeControl->sTemp.u16FactoryReset--;
		/* Time to reset ? */
		if (psMibNodeControl->sTemp.u16FactoryReset == 0)
		{
			DBG_vPrintf(CONFIG_DBG_MIB_NODE_CONTROL, "\nDevice_vReset() Y");
			/* Perform factory reset */
			Device_vReset(TRUE);
		}
	}

	/* Counting down to reset ? */
	if (psMibNodeControl->sTemp.u16Reset > 0)
	{
		/* Decrement */
		psMibNodeControl->sTemp.u16Reset--;
		/* Time to reset ? */
		if (psMibNodeControl->sTemp.u16Reset == 0)
		{
			DBG_vPrintf(CONFIG_DBG_MIB_NODE_CONTROL, "\nDevice_vReset() Z");
			/* Perform normal reset */
			Device_vReset(FALSE);
		}
	}

	/* Need to force an exception ? */
	if (psMibNodeControl->sTemp.u8RaiseException != 0)
	{
		/* Save all PDM records */
		PDM_vSave();
		/* Force an exception */
		*((uint32*)0xffffffff) = 0x12345678;
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
