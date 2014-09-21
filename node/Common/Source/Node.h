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
#ifndef  NODE_H_INCLUDED
#define  NODE_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* Stack includes */
#include <6LP.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Function Prototypes                                     ***/
/****************************************************************************/
PUBLIC void   				Node_vInit(bool_t);
#ifdef  JENNIC_CHIP_FAMILY_JN516x
PUBLIC bool_t 				Node_bTestFactoryResetEeprom(void);
#endif
#ifdef  JENNIC_CHIP_FAMILY_JN514x
PUBLIC bool_t 				Node_bTestFactoryResetFlash(void);
#endif
PUBLIC bool_t 				Node_bTestFactoryResetWakeTimer(void);
PUBLIC void   				Node_vPdmInit(uint8, uint8, uint8);
PUBLIC void   				Node_vReset(bool_t);
PUBLIC teJIP_Status 		Node_eJipInit(void);
PUBLIC void 				Node_v6lpConfigureNetwork(tsNetworkConfigData *);
PUBLIC void 				Node_v6lpDataEvent(int, te6LP_DataEvent, ts6LP_SockAddr *, uint8);
PUBLIC bool_t 				Node_bJipStackEvent(te6LP_StackEvent, uint8 *, uint8);
PUBLIC uint8 				Node_u8Analogue(void);
PUBLIC void 				Node_vTick(uint8, uint32);
PUBLIC bool_t 				Node_bUp(void);
PUBLIC PDM_teRecoveryState	Node_eGetGroupMibPdmInitState(void);
#if MK_BLD_NODE_TYPE_END_DEVICE
PUBLIC void 				Node_vSleep(void);
#endif

#if defined __cplusplus
}
#endif

#endif  /* NODE_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
