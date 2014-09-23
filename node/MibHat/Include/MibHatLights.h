#ifndef  MIBHATLIGHTS_H_INCLUDED
#define  MIBHATLIGHTS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <os.h>
#include <pdm.h>
#include <JIP.h>
#include <6LP.h>
/* Application common includes */
#include "Table.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* Dio permament data */
typedef struct
{
	uint32			u32Light0;
	uint32			u32Light1;
	uint32			u32Light2;
	uint32			u32Light3;
} tsMibHatLightsPerm;

/* Bulb status temporary data */
typedef struct
{
	uint32					u32Light0;
	uint32					u32Light1;
	uint32					u32Light2;
	uint32					u32Light3;
} tsMibHatLightsTemp;

/* Network control mib */
typedef struct
{
	/* MIB handles */
	thJIP_Mib			hMib;

	/* PDM record descriptor */
	PDM_tsRecordDescriptor		sDesc;

	/* Data pointers */
	tsMibHatLightsPerm		sPerm;
	tsMibHatLightsTemp		sTemp;

	/* Other data */
	bool_t					bSaveRecord;

} tsMibHatLights;

/****************************************************************************/
/***        Public Data                                     			  ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                    ***/
/****************************************************************************/
/* Unpatched functions in MibHat.c */
PUBLIC void		MibHatLights_vInit(thJIP_Mib hMibHatLightsInit,
					   tsMibHatLights *psMibHatLightsInit);
PUBLIC void 		MibHatLights_vRegister(void);
PUBLIC void 		MibHatLights_vSecond(void);
PUBLIC teJIP_Status MibHatLights_eSetLight0	     (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status MibHatLights_eSetLight1	     (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status MibHatLights_eSetLight2	     (uint32 u32Val, void *pvCbData);
PUBLIC teJIP_Status MibHatLights_eSetLight3	     (uint32 u32Val, void *pvCbData);

#if defined __cplusplus
}
#endif

#endif  /* MIBDIOCONFIG_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
