/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         MIB Variable - Defines
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
#ifndef  MIBVAR_H_INCLUDED
#define  MIBVAR_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Plug MIBs ********************************************/
/* PlugStatus MIB */
#define MIB_ID_PLUG_STATUS						0xfffffe10
#define VAR_IX_PLUG_STATUS_ON_COUNT						0
#define VAR_IX_PLUG_STATUS_ON_TIME						1
#define VAR_IX_PLUG_STATUS_OFF_TIME						2
#define VAR_IX_PLUG_STATUS_CHIP_TEMP					3
#define VAR_IX_PLUG_STATUS_BUS_VOLTS					4
#define VAR_IX_PLUG_STATUS_V							5
#define VAR_IX_PLUG_STATUS_I							6
#define VAR_IX_PLUG_STATUS_P							7
#define VAR_IX_PLUG_STATUS_Q1							8
#define VAR_IX_PLUG_STATUS_S							9
#define VAR_IX_PLUG_STATUS_S1							10
#define VAR_IX_PLUG_STATUS_PF							11
#define VAR_IX_PLUG_STATUS_PF1							12
#define VAR_IX_PLUG_STATUS_SN							13
#define VAR_IX_PLUG_STATUS_N							14
#define VAR_IX_PLUG_STATUS_THDI							15

/* PlugConfig MIB */
#define MIB_ID_PLUG_CONFIG						0xfffffe11
#define VAR_IX_PLUG_CONFIG_INIT_MODE					0
#define VAR_IX_PLUG_CONFIG_VPP							1
#define VAR_IX_PLUG_CONFIG_I1PP							2
#define VAR_IX_PLUG_CONFIG_I2PP 						3
#define VAR_IX_PLUG_CONFIG_DELTA_PHI1					4
#define VAR_IX_PLUG_CONFIG_DELTA_PHI2					5
#define VAR_IX_PLUG_CONFIG_TEST_FLOAT					6

/* PlugGroup MIB */
#define MIB_ID_PLUG_GROUP						0xfffffe12

/* PlugScene MIB */
#define MIB_ID_PLUG_SCENE						0xfffffe13

/* PlugControl MIB */
#define MIB_ID_PLUG_CONTROL						0xfffffe14
#define VAR_IX_PLUG_CONTROL_MODE						0
#define VAR_IX_PLUG_CONTROL_SCENE_ID				  	1

#define VAR_VAL_PLUG_CONTROL_MODE_OFF			  		0	/* Plug off mode */
#define VAR_VAL_PLUG_CONTROL_MODE_ON		  			1	/* Plug on mode */
#define VAR_VAL_PLUG_CONTROL_MODE_TOGGLE  		  		2	/* Plug toggle between on and off */
#define VAR_VAL_PLUG_CONTROL_MODE_TEST		  			3	/* Bulb test mode */
#define VAR_VAL_PLUG_CONTROL_MODE_COUNT	  		    	4	/* Number of modes */

#if defined __cplusplus
}
#endif

#endif  /* MIBVAR_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
