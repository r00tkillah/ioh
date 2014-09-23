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
#include "MibHat.h"
#include "MibHatLights.h"

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibHatLights sMibHatLights;

/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_HAT_CONFIG

#define DECLARE_MIB
#include "MibHatLightsDef.h"
JIP_START_DECLARE_MIB(HatlightsDef, Hatlights)
JIP_CALLBACK(Direction, MibHatlights_eSetLight0, vGetUint32, &sMibHatlights.sPerm.u32Light0)
JIP_CALLBACK(Direction, MibHatlights_eSetLight1, vGetUint32, &sMibHatlights.sPerm.u32Light1)
JIP_CALLBACK(Direction, MibHatlights_eSetLight2, vGetUint32, &sMibHatlights.sPerm.u32Light2)
JIP_CALLBACK(Direction, MibHatlights_eSetLight3, vGetUint32, &sMibHatlights.sPerm.u32Light3)
JIP_END_DECLARE_MIB(Hatlights, hHatlights)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibHatlights = &sHatlightsMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibHatlights = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
