/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Dio MIB IDs, indicies and values
 *
 * DioStatus MIB
 * UINT32, Input,            Function: u32AHI_DioReadInput();
 * UINT32, Interrupt,        Function: u32AHI_DioInterruptStatus();
 *
 * DioConfig MIB
 * UINT32, Direction,        Register: REG_GPIO_DIR
 * UINT32, Pullup,           Register: REG_SYS_PULLUP
 * UINT32, InterruptEnabled, Register: REG_SYS_WK_EM
 * UINT32, InterruptEdge,    Register: REG_SYS_WK_ET
 * UINT32, DirectionInput,   Function: vAHI_DioSetDirection(u32Inputs, 0);
 * UINT32, DirectionOutput,  Function: vAHI_DioSetDirection(0, u32Outputs);
 * UINT32, PullupEnable,     Function: vAHI_DioSetPullup(u32On, 0);
 * UINT32, PullupDisable,    Function: vAHI_DioSetPullup(0, u32Off);
 * UINT32, InterruptEnable,  Function: vAHI_DioInterruptEnable(u32Enable, 0);
 * UINT32, InterruptDisable, Function: vAHI_DioInterruptEnable(0, u32Disable);
 * UINT32, InterruptRising,  Function: vAHI_DioInterruptEdge(u32Rising, 0);
 * UINT32, InterruptFalling, Function: vAHI_DioInterruptEdge(0, u32Falling);
 *
 * DioControl MIB
 * UINT32, Output,           Register: REG_GPIO_DOUT
 * UINT32, OutputOn,         Function: vAHI_DioSetOutput(u32On,  0)
 * UINT32, OutputOff,        Function: vAHI_DioSetOutput(u32Off, 0)
 */
/*****************************************************************************/
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
#ifndef  MIBDIO_H_INCLUDED
#define  MIBDIO_H_INCLUDED

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
/* Check for unsupported chip type */
#ifdef  JENNIC_CHIP_FAMILY_JN514x
#ifdef JENNIC_CHIP_JN5142J01
#define MIB_DIO_MASK 0x03FFFF /* 18 DIO on JN5142J01 */
#else
#define MIB_DIO_MASK 0x1FFFFF /* 21 DIO on JN5148 */
#endif
#endif
#ifdef  JENNIC_CHIP_FAMILY_JN516x
#define MIB_DIO_MASK 0x0FFFFF /* 20 DIO on JN516x */
#endif


/* DioStatus MIB */
#define MIB_ID_DIO_STATUS						0xFFFFFE70
#define VAR_IX_DIO_STATUS_INPUT            				 0
#define VAR_IX_DIO_STATUS_INTERRUPT        				 1

/* DioConfig MIB */
#define MIB_ID_DIO_CONFIG						0xFFFFFE71
#define VAR_IX_DIO_CONFIG_DIRECTION        				 0
#define VAR_IX_DIO_CONFIG_PULLUP           				 1
#define VAR_IX_DIO_CONFIG_INTERRUPT_ENABLED				 2
#define VAR_IX_DIO_CONFIG_INTERRUPT_EDGE   				 3
#define VAR_IX_DIO_CONFIG_DIRECTION_INPUT  				 4
#define VAR_IX_DIO_CONFIG_DIRECTION_OUTPUT 				 5
#define VAR_IX_DIO_CONFIG_PULLUP_ENABLE    				 6
#define VAR_IX_DIO_CONFIG_PULLUP_DISABLE   				 7
#define VAR_IX_DIO_CONFIG_INTERRUPT_ENABLE 				 8
#define VAR_IX_DIO_CONFIG_INTERRUPT_DISABLE				 9
#define VAR_IX_DIO_CONFIG_INTERRUPT_RISING 				10
#define VAR_IX_DIO_CONFIG_INTERRUPT_FALLING				11

/* DioControl MIB */
#define MIB_ID_DIO_CONTROL						0xFFFFFE73
#define VAR_IX_DIO_CONTROL_OUTPUT    					 0
#define VAR_IX_DIO_CONTROL_OUTPUT_ON 					 1
#define VAR_IX_DIO_CONTROL_OUTPUT_OFF					 2

#if defined __cplusplus
}
#endif

#endif  /* MIBDIO_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
