/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         DioConfig MIB Definition
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
#undef MIB_HEADER
#define MIB_HEADER "MibDioConfigDef.h"

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Stack includes */
#include <jip_define_mib.h>
/* Application common includes */
#include "MibDio.h"

/****************************************************************************/
/***        MIB definition                                                ***/
/****************************************************************************/
/* DioConfig MIB */
START_DEFINE_MIB(MIB_ID_DIO_CONFIG, DioConfigDef)
/*         ID 					   	            Type    Name              Disp  Flags Access                 Cache Security */
DEFINE_VAR(VAR_IX_DIO_CONFIG_DIRECTION,         UINT32, Direction,        NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Register: REG_GPIO_DIR */
DEFINE_VAR(VAR_IX_DIO_CONFIG_PULLUP,            UINT32, Pullup,           NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Register: REG_SYS_PULLUP */
DEFINE_VAR(VAR_IX_DIO_CONFIG_INTERRUPT_ENABLED, UINT32, InterruptEnabled, NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Register: REG_SYS_WK_EM */
DEFINE_VAR(VAR_IX_DIO_CONFIG_INTERRUPT_EDGE,    UINT32, InterruptEdge,    NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Register: REG_SYS_WK_ET */
DEFINE_VAR(VAR_IX_DIO_CONFIG_DIRECTION_INPUT,   UINT32, DirectionInput,   NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioSetDirection(u32Inputs, 0); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_DIRECTION_OUTPUT,  UINT32, DirectionOutput,  NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioSetDirection(0, u32Outputs); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_PULLUP_ENABLE,     UINT32, PullupEnable,     NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioSetPullup(u32On, 0); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_PULLUP_DISABLE,    UINT32, PullupDisable,    NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioSetPullup(0, u32Off); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_INTERRUPT_ENABLE,  UINT32, InterruptEnable,  NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioInterruptEnable(u32Enable, 0); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_INTERRUPT_DISABLE, UINT32, InterruptDisable, NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioInterruptEnable(0, u32Disable); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_INTERRUPT_RISING,  UINT32, InterruptRising,  NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioInterruptEdge(u32Rising, 0); */
DEFINE_VAR(VAR_IX_DIO_CONFIG_INTERRUPT_FALLING, UINT32, InterruptFalling, NULL, 0,    (READ | WRITE | TRAP), NONE, NONE) /* Function: vAHI_DioInterruptEdge(0, u32Falling); */
END_DEFINE_MIB(DioConfigDef)

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
