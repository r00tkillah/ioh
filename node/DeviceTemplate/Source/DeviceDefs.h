/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         General Configuration Parameters
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
#ifndef  DEVICEDEFS_H_INCLUDED
#define  DEVICEDEFS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
/* SDK includes */
#include <jendefs.h>
/* JenOS includes */
#include <dbg_uart.h>
#include <AppHardwareApi.h>
/* Stack includes */
#include <jip.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Operating defines */
#define DEVICE_COMMISSION_TIMEOUT				  10 /* Factory reset if not fully commissioned after joining standalone network */
#define DEVICE_JOIN_TIMEOUT		 				   0 /* No join timeout */
#define DEVICE_ED_SCAN_SLEEP	 				5000 /* End device scan sleep period milliseconds */
#define DEVICE_ED_SLEEP	 		 				1000 /* End device normal sleep period milliseconds */
#define DEVICE_ED_REQ_AWAKE						  50 /* End device stay awake time after stay awake request (10ms) */

/* Debug flags */
#ifdef DBG_ENABLE
#define DEBUG_UART								E_AHI_UART_0
#define DEBUG_BAUD_RATE							E_AHI_UART_RATE_115200
#define DEBUG_EXCEPTION							TRUE
#define DEBUG_DEVICE_FUNC						TRUE
#define DEBUG_DEVICE_VARS						TRUE
#define DEBUG_NODE_FUNC							TRUE
#define DEBUG_NODE_VARS							TRUE
#else
#define DEBUG_EXCEPTION							FALSE
#define DEBUG_DEVICE_FUNC						FALSE
#define DEBUG_DEVICE_VARS						FALSE
#define DEBUG_NODE_FUNC							FALSE
#define DEBUG_NODE_VARS							FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* DEVICEDEFS_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
