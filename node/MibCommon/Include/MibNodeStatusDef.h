/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         NodeStatus MIB - Definition
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
/***        Macro Definitions                                             ***/
/****************************************************************************/
#undef MIB_HEADER
#define MIB_HEADER "MibNodeStatusDef.h"

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Stack includes */
#include <jip_define_mib.h>
/* Application common includes */
#include "MibCommon.h"

/****************************************************************************/
/***        MIB definition                                                ***/
/****************************************************************************/
START_DEFINE_MIB(MIB_ID_NODE_STATUS, NodeStatusDef)
/*         ID 					   				Type    Name  			Disp  Flags Access 			Cache Security */
DEFINE_VAR(VAR_IX_NODE_STATUS_SYSTEM_STATUS	  , UINT16,	SystemStatus,	NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_COLD_START_COUNT, UINT16,	ColdStartCount, NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_RESET_COUNT	  , UINT16,	ResetCount, 	NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_WATCHDOG_COUNT  , UINT16,	WatchdogCount,  NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_BROWNOUT_COUNT  , UINT16,	BrownoutCount,  NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_HEAP_MIN  	  , UINT32,	HeapMin,  		NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_HEAP_MAX  	  , UINT32,	HeapMax,  		NULL, 0,    (READ | TRAP),  NONE, NONE)
DEFINE_VAR(VAR_IX_NODE_STATUS_STACK_MIN  	  , UINT32,	StackMin,  		NULL, 0,    (READ | TRAP),  NONE, NONE)
END_DEFINE_MIB(NodeStatusDef)


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
