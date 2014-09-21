/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioConfig MIB Declaration
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
#include "MibDio.h"
#include "MibDioConfig.h"

/****************************************************************************/
/***        MIB structure                                                 ***/
/****************************************************************************/
PUBLIC tsMibDioConfig sMibDioConfig;

/****************************************************************************/
/***        MIB declaration                                               ***/
/****************************************************************************/
/* Registering MIB ? */
#if MK_REG_MIB_DIO_CONFIG

#define DECLARE_MIB
#include "MibDioConfigDef.h"
JIP_START_DECLARE_MIB(DioConfigDef, DioConfig)
JIP_CALLBACK(Direction,        MibDioConfig_eSetDirection,        vGetUint32, &sMibDioConfig.sPerm.u32Direction       )
JIP_CALLBACK(Pullup,           MibDioConfig_eSetPullup,           vGetUint32, &sMibDioConfig.sPerm.u32Pullup          )
JIP_CALLBACK(InterruptEnabled, MibDioConfig_eSetInterruptEnabled, vGetUint32, &sMibDioConfig.sPerm.u32InterruptEnabled)
JIP_CALLBACK(InterruptEdge,    MibDioConfig_eSetInterruptEdge,    vGetUint32, &sMibDioConfig.sPerm.u32InterruptEdge   )
JIP_CALLBACK(DirectionInput,   MibDioConfig_eSetDirectionInput,   vGetUint32, &sMibDioConfig.sTemp.u32DirectionInput  )
JIP_CALLBACK(DirectionOutput,  MibDioConfig_eSetDirectionOutput,  vGetUint32, &sMibDioConfig.sTemp.u32DirectionOutput )
JIP_CALLBACK(PullupEnable,     MibDioConfig_eSetPullupEnable,     vGetUint32, &sMibDioConfig.sTemp.u32PullupEnable    )
JIP_CALLBACK(PullupDisable,    MibDioConfig_eSetPullupDisable,    vGetUint32, &sMibDioConfig.sTemp.u32PullupDisable   )
JIP_CALLBACK(InterruptEnable,  MibDioConfig_eSetInterruptEnable,  vGetUint32, &sMibDioConfig.sTemp.u32InterruptEnable )
JIP_CALLBACK(InterruptDisable, MibDioConfig_eSetInterruptDisable, vGetUint32, &sMibDioConfig.sTemp.u32InterruptDisable)
JIP_CALLBACK(InterruptRising,  MibDioConfig_eSetInterruptRising,  vGetUint32, &sMibDioConfig.sTemp.u32InterruptRising )
JIP_CALLBACK(InterruptFalling, MibDioConfig_eSetInterruptFalling, vGetUint32, &sMibDioConfig.sTemp.u32InterruptFalling)
JIP_END_DECLARE_MIB(DioConfig, hDioConfig)

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibDioConfig = &sDioConfigMib.sMib;

#else

/* Public MIB handle */
PUBLIC const thJIP_Mib hMibDioConfig = NULL;

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
