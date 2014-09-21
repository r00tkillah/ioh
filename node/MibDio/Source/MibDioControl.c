/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioControl MIB Implementation
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
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>
/* Application device includes */
#include "Node.h"
#include "MibDio.h"
#include "MibDioControl.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Debug */
#define MIB_DIO_CONTROL_DBG TRUE

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
PRIVATE tsMibDioControl *psMibDioControl;  /* MIB data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibDioControl_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vInit(thJIP_Mib          hMibDioControlInit,
                                tsMibDioControl *psMibDioControlInit)
{
    /* Debug */
    DBG_vPrintf(MIB_DIO_CONTROL_DBG, "\nMibDioControl_vInit() {%d}", sizeof(tsMibDioControl));

    /* Valid data pointer ? */
    if (psMibDioControlInit != (tsMibDioControl *) NULL)
    {
        PDM_teStatus   ePdmStatus;

        /* Take copy of pointer to data */
        psMibDioControl = psMibDioControlInit;
        /* Take a copy of the MIB handle */
        psMibDioControl->hMib = hMibDioControlInit;

		/* Read values from registers (to preserve default state if no data is read from PDM) */
		psMibDioControl->sPerm.u32Output           = u32REG_GpioRead(REG_GPIO_DOUT);

        /* Load Dio mib data */
        ePdmStatus = PDM_eLoadRecord(&psMibDioControl->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
									 "MibDioControl",
#else
									 (uint16)(MIB_ID_DIO_CONTROL & 0xFFFF),
#endif
                                     (void *) &psMibDioControl->sPerm,
                                     sizeof(psMibDioControl->sPerm),
                                     FALSE);

		/* Initialise Dio from PDM data */
		vREG_GpioWrite(REG_GPIO_DOUT,  psMibDioControl->sPerm.u32Output);

		/* Read back values from registers (to resolve devices with differing numbers of DIO) */
		psMibDioControl->sPerm.u32Output           = u32REG_GpioRead(REG_GPIO_DOUT);
    }
}

/****************************************************************************
 *
 * NAME: MibDioControl_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vRegister(void)
{
    teJIP_Status eStatus;

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibDioControl->hMib);
    /* Debug */
    DBG_vPrintf(MIB_DIO_CONTROL_DBG, "\nMibDioControl_vRegister()");
    DBG_vPrintf(MIB_DIO_CONTROL_DBG, "\n\teJIP_RegisterMib(DioControl)=%d", eStatus);

    /* Make sure permament data is saved */
	psMibDioControl->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibDioControl_vStackEvent
 *
 * DESCRIPTION:
 * Called when stack events take place
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vStackEvent(te6LP_StackEvent eEvent)
{
	/* Network is up ? */
	if (E_STACK_JOINED  == eEvent ||
		E_STACK_STARTED == eEvent)
	{
		/* Not yet joined a network ? */
		if (psMibDioControl->bJoined == FALSE)
		{
		    /* Group MIB included ? */
		    #if MK_BLD_MIB_GROUP
		    {
				/* Did we not recover the Group MIB's data from the PDM ? */
				if (Node_eGetGroupMibPdmInitState() != PDM_RECOVERY_STATE_RECOVERED)
				{
					/* Place device into global Dio group */
		            (void) bJIP_JoinGroup((uint16)(MIB_ID_DIO_CONTROL & 0xffff));
					/* Debug */
					DBG_vPrintf(MIB_DIO_CONTROL_DBG, "\nbJIP_JoinGroup(%x) {DEFAULT}", (uint16)(MIB_ID_DIO_CONTROL & 0xffff));
				}
			}
			#endif
            /* Note we've now joined */
            psMibDioControl->bJoined = TRUE;
		}
	}
}

/****************************************************************************
 *
 * NAME: MibDioControl_vSecond
 *
 * DESCRIPTION:
 * Called once per second
 *
 ****************************************************************************/
PUBLIC void MibDioControl_vSecond(void)
{
    /* Need to save record ? */
    if (psMibDioControl->bSaveRecord)
    {
        /* Clear flag */
        psMibDioControl->bSaveRecord = FALSE;
        /* Make sure permament data is saved */
        PDM_vSaveRecord(&psMibDioControl->sDesc);
        /* Debug */
        DBG_vPrintf(MIB_DIO_CONTROL_DBG, "\nMibDioControl_vSecond()");
        DBG_vPrintf(MIB_DIO_CONTROL_DBG, "\n\tPDM_vSaveRecord(MibDioControl)");
    }
}

/****************************************************************************
 *
 * NAME: MibDioControl_eSetOutput
 *
 * DESCRIPTION:
 * Sets Output variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioControl_eSetOutput(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vREG_GpioWrite(REG_GPIO_DOUT, u32Val);
	/* Read back final value */
	psMibDioControl->sPerm.u32Output = u32REG_GpioRead(REG_GPIO_DOUT);

   	/* Make sure permament data is saved */
    psMibDioControl->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioControl_eSetOutputOn
 *
 * DESCRIPTION:
 * Sets OutputOn variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioControl_eSetOutputOn(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetOutput(u32Val, 0);
	/* Read back final value */
	psMibDioControl->sPerm.u32Output = u32REG_GpioRead(REG_GPIO_DOUT);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioControl->hMib, VAR_IX_DIO_CONTROL_OUTPUT);

   	/* Make sure permament data is saved */
    psMibDioControl->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioControl_eSetOutputOff
 *
 * DESCRIPTION:
 * Sets OutputOff variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioControl_eSetOutputOff(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetOutput(0, u32Val);
	/* Read back final value */
	psMibDioControl->sPerm.u32Output = u32REG_GpioRead(REG_GPIO_DOUT);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioControl->hMib, VAR_IX_DIO_CONTROL_OUTPUT);

   	/* Make sure permament data is saved */
    psMibDioControl->bSaveRecord = TRUE;

    return eReturn;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
