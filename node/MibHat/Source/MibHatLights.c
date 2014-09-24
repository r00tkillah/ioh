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
#include "MibHat.h"
#include "MibHatlights.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Debug */
#define MIB_HAT_LIGHTS_DBG TRUE

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
PRIVATE tsMibHatLights *psMibHatLights;  /* MIB data */

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: MibHatLights_vInit
 *
 * DESCRIPTION:
 * Initialises data
 *
 ****************************************************************************/
PUBLIC void MibHatLights_vInit(thJIP_Mib        hMibHatLightsInit,
			       tsMibHatLights *psMibHatLightsInit)
{
    /* Debug */
    DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\nMibHatLights_vInit() {%d}", sizeof(tsMibHatLights));

    /* Valid data pointer ? */
    if (psMibHatLightsInit != (tsMibHatLights *) NULL)
    {
        PDM_teStatus   ePdmStatus;

        /* Take copy of pointer to data */
        psMibHatLights = psMibHatLightsInit;
        /* Take a copy of the MIB handle */
        psMibHatLights->hMib = hMibHatLightsInit;

	/* default state if no data is read from PDM */
	psMibHatLights->sPerm.u32Light0 = 0;
	psMibHatLights->sPerm.u32Light1 = 0;
	psMibHatLights->sPerm.u32Light2 = 0;
	psMibHatLights->sPerm.u32Light3 = 0;

        /* Load Hat mib data */
        ePdmStatus = PDM_eLoadRecord(&psMibHatLights->sDesc,
				     (uint16)(MIB_ID_HAT_LIGHTS & 0xFFFF),
                                     (void *) &psMibHatLights->sPerm,
                                     sizeof(psMibHatLights->sPerm),
                                     FALSE);
	
	/* Initialise Lights from PDM data */
	/* FIXME! Implement */
    }
}

/****************************************************************************
 *
 * NAME: MibHatLights_vRegister
 *
 * DESCRIPTION:
 * Registers MIB
 *
 ****************************************************************************/
PUBLIC void MibHatLights_vRegister(void)
{
    teJIP_Status eStatus;

    /* Register MIB */
    eStatus = eJIP_RegisterMib(psMibHatLights->hMib);
    /* Debug */
    DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\nMibHatLights_vRegister()");
    DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\n\teJIP_RegisterMib(HatLights)=%d", eStatus);

    /* Make sure permament data is saved */
	psMibHatLights->bSaveRecord = TRUE;
}

/****************************************************************************
 *
 * NAME: MibHatLights_vSecond
 *
 * DESCRIPTION:
 * Called once per second
 *
 ****************************************************************************/
PUBLIC void MibHatLights_vSecond(void)
{
    /* Need to save record ? */
    if (psMibHatLights->bSaveRecord)
    {
        /* Clear flag */
        psMibHatLights->bSaveRecord = FALSE;
        /* Make sure permament data is saved */
        PDM_vSaveRecord(&psMibHatLights->sDesc);
        /* Debug */
        DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\nMibHatLights_vSecond()");
        DBG_vPrintf(MIB_HAT_LIGHTS_DBG, "\n\tPDM_vSaveRecord(MibHatLights)");
    }
}

PRIVATE teJIP_Status MibHatLights_eSetLight(uint32 *pu32Light, uint32 u32Val, void *pvCbData)
{
    teJIP_Status   eReturn = E_JIP_OK;

    /* Write new value */
    /* FIXME: implement */
    /* Read back final value */
    *pu32Light = u32Val;

    /* Make sure permament data is saved */
    psMibHatLights->bSaveRecord = TRUE;

    return eReturn;
}

/****************************************************************************
 *
 * NAME: MibHatLights_eSetLight0
 *
 * DESCRIPTION:
 * Sets Direction variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatLights_eSetLight0(uint32 u32Val, void *pvCbData)
{
  return MibHatLights_eSetLight(&(psMibHatLights->sPerm.u32Light0), u32Val, pvCbData);
}

/****************************************************************************
 *
 * NAME: MibHatLights_eSetLight1
 *
 * DESCRIPTION:
 * Sets Direction variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatLights_eSetLight1(uint32 u32Val, void *pvCbData)
{
  return MibHatLights_eSetLight(&(psMibHatLights->sPerm.u32Light1), u32Val, pvCbData);
}

/****************************************************************************
 *
 * NAME: MibHatLights_eSetLight2
 *
 * DESCRIPTION:
 * Sets Direction variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatLights_eSetLight2(uint32 u32Val, void *pvCbData)
{
  return MibHatLights_eSetLight(&(psMibHatLights->sPerm.u32Light2), u32Val, pvCbData);
}

/****************************************************************************
 *
 * NAME: MibHatLights_eSetLight3
 *
 * DESCRIPTION:
 * Sets Direction variable
 *
 ****************************************************************************/
PUBLIC teJIP_Status MibHatLights_eSetLight3(uint32 u32Val, void *pvCbData)
{
  return MibHatLights_eSetLight(&(psMibHatLights->sPerm.u32Light3), u32Val, pvCbData);
}



/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
