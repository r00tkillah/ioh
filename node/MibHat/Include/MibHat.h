#ifndef  MIBHAT_H_INCLUDED
#define  MIBHAT_H_INCLUDED

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
/* HatLights MIB */
#define MIB_ID_HAT_LIGHTS						0xFFFFFE74
#define VAR_IX_HAT_LIGHTS_0	        				0
#define VAR_IX_HAT_LIGHTS_1	        				1
#define VAR_IX_HAT_LIGHTS_2	        				2
#define VAR_IX_HAT_LIGHTS_3	        				3

#if defined __cplusplus
}
#endif

#endif  /* MIBHAT_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
