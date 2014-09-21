/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioConfig MIB Implementation
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
#include "MibDio.h"
#include "MibDioConfig.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Debug */
#define MIB_DIO_CONFIG_DBG TRUE

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
PRIVATE tsMibDioConfig *psMibDioConfig;  /* MIB data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibDioConfig_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibDioConfig_vInit(thJIP_Mib        hMibDioConfigInit,
                         	   tsMibDioConfig *psMibDioConfigInit)
{
    /* Debug */
    DBG_vPrintf(MIB_DIO_CONFIG_DBG, "\nMibDioConfig_vInit() {%d}", sizeof(tsMibDioConfig));

    /* Valid data pointer ? */
    if (psMibDioConfigInit != (tsMibDioConfig *) NULL)
    {
        PDM_teStatus   ePdmStatus;

        /* Take copy of pointer to data */
        psMibDioConfig = psMibDioConfigInit;
        /* Take a copy of the MIB handle */
        psMibDioConfig->hMib = hMibDioConfigInit;

		/* Read values from registers (to preserve default state if no data is read from PDM) */
		psMibDioConfig->sPerm.u32Direction        =  u32REG_GpioRead(REG_GPIO_DIR);
		psMibDioConfig->sPerm.u32Pullup           = (u32REG_SysRead(REG_SYS_PULLUP) & MIB_DIO_MASK);
		psMibDioConfig->sPerm.u32InterruptEnabled = (u32REG_SysRead(REG_SYS_WK_EM)  & MIB_DIO_MASK);
		psMibDioConfig->sPerm.u32InterruptEdge    = (u32REG_SysRead(REG_SYS_WK_ET)  & MIB_DIO_MASK);

        /* Load Dio mib data */
        ePdmStatus = PDM_eLoadRecord(&psMibDioConfig->sDesc,
#if defined(JENNIC_CHIP_FAMILY_JN514x)
									 "MibDioConfig",
#else
									 (uint16)(MIB_ID_DIO_CONFIG & 0xFFFF),
#endif
                                     (void *) &psMibDioConfig->sPerm,
                                     sizeof(psMibDioConfig->sPerm),
                                     FALSE);

		/* Initialise Dio from PDM data */
		vREG_GpioWrite(REG_GPIO_DIR,    psMibDioConfig->sPerm.u32Direction                       );
		vREG_SysWrite (REG_SYS_PULLUP, (psMibDioConfig->sPerm.u32Pullup           & MIB_DIO_MASK));
		vREG_SysWrite (REG_SYS_WK_EM,  (psMibDioConfig->sPerm.u32InterruptEnabled & MIB_DIO_MASK));
		vREG_SysWrite (REG_SYS_WK_ET,  (psMibDioConfig->sPerm.u32InterruptEdge    & MIB_DIO_MASK));

		/* Read back values from registers (to resolve devices with differing numbers of DIO) */
		psMibDioConfig->sPerm.u32Direction        =  u32REG_GpioRead(REG_GPIO_DIR);
		psMibDioConfig->sPerm.u32Pullup           = (u32REG_SysRead(REG_SYS_PULLUP) & MIB_DIO_MASK);
		psMibDioConfig->sPerm.u32InterruptEnabled = (u32REG_SysRead(REG_SYS_WK_EM)  & MIB_DIO_MASK);
		psMibDioConfig->sPerm.u32InterruptEdge    = (u32REG_SysRead(REG_SYS_WK_ET)  & MIB_DIO_MASK);
    }
}

/****************************************************************************
 *
 * NAME: MibDioConfig_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibDioConfig_vRegister(void)
{
    teJIP_Status eStatus;

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibDioConfig->hMib);
    /* Debug */
    DBG_vPrintf(MIB_DIO_CONFIG_DBG, "\nMibDioConfig_vRegister()");
    DBG_vPrintf(MIB_DIO_CONFIG_DBG, "\n\teJIP_RegisterMib(DioConfig)=%d", eStatus);

    /* Make sure permament data is saved */
	psMibDioConfig->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_vSecond
 *
 * DESCRIPTION:
 * Called once per second
 *
 ****************************************************************************/
PUBLIC void MibDioConfig_vSecond(void)
{
    /* Need to save record ? */
    if (psMibDioConfig->bSaveRecord)
    {
        /* Clear flag */
        psMibDioConfig->bSaveRecord = FALSE;
        /* Make sure permament data is saved */
        PDM_vSaveRecord(&psMibDioConfig->sDesc);
        /* Debug */
        DBG_vPrintf(MIB_DIO_CONFIG_DBG, "\nMibDioConfig_vSecond()");
        DBG_vPrintf(MIB_DIO_CONFIG_DBG, "\n\tPDM_vSaveRecord(MibDioConfig)");
    }
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetDirection
 *
 * DESCRIPTION:
 * Sets Direction variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetDirection(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vREG_GpioWrite(REG_GPIO_DIR, u32Val);
	/* Read back final value */
	psMibDioConfig->sPerm.u32Direction = u32REG_GpioRead(REG_GPIO_DIR);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetPullup
 *
 * DESCRIPTION:
 * Sets Pullup variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetPullup(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vREG_SysWrite(REG_SYS_PULLUP, (u32Val & MIB_DIO_MASK));
	/* Read back final value */
	psMibDioConfig->sPerm.u32Pullup = (u32REG_SysRead(REG_SYS_PULLUP) & MIB_DIO_MASK);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetInterruptEnabled
 *
 * DESCRIPTION:
 * Sets InterruptEnabled variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetInterruptEnabled(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vREG_SysWrite (REG_SYS_WK_EM, (u32Val & MIB_DIO_MASK));
	/* Read back final value */
	psMibDioConfig->sPerm.u32InterruptEnabled = (u32REG_SysRead(REG_SYS_WK_EM) & MIB_DIO_MASK);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetInterruptEdge
 *
 * DESCRIPTION:
 * Sets InterruptEdge variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetInterruptEdge(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vREG_SysWrite (REG_SYS_WK_ET, (u32Val & MIB_DIO_MASK));
	/* Read back final value */
	psMibDioConfig->sPerm.u32InterruptEdge = (u32REG_SysRead(REG_SYS_WK_ET) & MIB_DIO_MASK);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetDirectionInput
 *
 * DESCRIPTION:
 * Sets DirectionInput variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetDirectionInput(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetDirection(u32Val, 0);
	/* Read back final value */
	psMibDioConfig->sPerm.u32Direction = u32REG_GpioRead(REG_GPIO_DIR);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_DIRECTION);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetDirectionOutput
 *
 * DESCRIPTION:
 * Sets DirectionOutput variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetDirectionOutput(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetDirection(0, u32Val);
	/* Read back final value */
	psMibDioConfig->sPerm.u32Direction = u32REG_GpioRead(REG_GPIO_DIR);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_DIRECTION);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetPullupEnable
 *
 * DESCRIPTION:
 * Sets PullupEnable variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetPullupEnable(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetPullup((u32Val & MIB_DIO_MASK), 0);
	/* Read back final value */
	psMibDioConfig->sPerm.u32Pullup = (u32REG_SysRead(REG_SYS_PULLUP) & MIB_DIO_MASK);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_PULLUP);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetPullupDisable
 *
 * DESCRIPTION:
 * Sets PullupEnable variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetPullupDisable(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioSetPullup(0, (u32Val & MIB_DIO_MASK));
	/* Read back final value */
	psMibDioConfig->sPerm.u32Pullup = (u32REG_SysRead(REG_SYS_PULLUP) & MIB_DIO_MASK);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_PULLUP);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetInterruptEnable
 *
 * DESCRIPTION:
 * Sets InterruptEnable variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetInterruptEnable(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioInterruptEnable((u32Val & MIB_DIO_MASK), 0);
	/* Read back final value */
	psMibDioConfig->sPerm.u32InterruptEnabled = (u32REG_SysRead(REG_SYS_WK_EM) & MIB_DIO_MASK);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_INTERRUPT_ENABLED);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetInterruptDisable
 *
 * DESCRIPTION:
 * Sets InterruptEnable variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetInterruptDisable(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioInterruptEnable(0, (u32Val & MIB_DIO_MASK));
	/* Read back final value */
	psMibDioConfig->sPerm.u32InterruptEnabled = (u32REG_SysRead(REG_SYS_WK_EM) & MIB_DIO_MASK);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_INTERRUPT_ENABLED);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetInterruptRising
 *
 * DESCRIPTION:
 * Sets InterruptRising variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetInterruptRising(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioInterruptEdge((u32Val & MIB_DIO_MASK), 0);
	/* Read back final value */
	psMibDioConfig->sPerm.u32InterruptEdge = (u32REG_SysRead(REG_SYS_WK_ET) & MIB_DIO_MASK);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_INTERRUPT_EDGE);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibDioConfig_eSetInterruptFalling
 *
 * DESCRIPTION:
 * Sets InterruptRising variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibDioConfig_eSetInterruptFalling(uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

	/* Write new value */
	vAHI_DioInterruptEdge(0, (u32Val & MIB_DIO_MASK));
	/* Read back final value */
	psMibDioConfig->sPerm.u32InterruptEdge = (u32REG_SysRead(REG_SYS_WK_ET) & MIB_DIO_MASK);
	/* Notify any traps */
	vJIP_NotifyChanged(psMibDioConfig->hMib, VAR_IX_DIO_CONFIG_INTERRUPT_EDGE);

   	/* Make sure permament data is saved */
    psMibDioConfig->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
