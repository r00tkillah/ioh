/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Common MIBs - IDs, Indicies and Values
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
#ifndef  MIBCOMMON_H_INCLUDED
#define  MIBCOMMON_H_INCLUDED

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
/* Stack defines */
#define MIB_COMMON_STACK_MODE_STANDALONE  0x0001
#define MIB_COMMON_STACK_MODE_COMMISSION  0x0002


/* Stack MIBs *******************************************/
/* Node MIB */
#define MIB_IDX_NODE									 0
#define MIB_ID_NODE 							0xffffff00

/* JenNet MIB */
#define MIB_IDX_JENNET									 1
#define MIB_ID_JENNET 							0xffffff01

/* Groups MIB */
#define MIB_IDX_GROUPS									 2
#define MIB_ID_GROUPS 							0xffffff02
#define VAR_IX_GROUPS_GROUPS        					 0
#define VAR_IX_GROUPS_ADD_GROUP    						 1
#define VAR_IX_GROUPS_REMOVE_GROUP 						 2
#define VAR_IX_GROUPS_REMOVE_ALL_GROUPS   				 3

#define VAR_VAL_GROUPS_GROUP_A                      0xa00a
#define VAR_VAL_GROUPS_GROUP_B                      0xb00b
#define VAR_VAL_GROUPS_GROUP_C                      0xc00c
#define VAR_VAL_GROUPS_GROUP_D                      0xd00d
#define VAR_VAL_GROUPS_GROUP_ALL                    0xf00f

/* OND MIB */
#define MIB_IDX_OND									     3
#define MIB_ID_OND 								0xffffff03

/* Node MIBs ********************************************/
/* NodeStatus MIB */
#define MIB_ID_NODE_STATUS						0xfffffe80
#define VAR_IX_NODE_STATUS_SYSTEM_STATUS				 0
#define VAR_IX_NODE_STATUS_COLD_START_COUNT				 1
#define VAR_IX_NODE_STATUS_RESET_COUNT					 2
#define VAR_IX_NODE_STATUS_WATCHDOG_COUNT				 3
#define VAR_IX_NODE_STATUS_BROWNOUT_COUNT				 4
#define VAR_IX_NODE_STATUS_HEAP_MIN						 5
#define VAR_IX_NODE_STATUS_HEAP_MAX						 6
#define VAR_IX_NODE_STATUS_STACK_MIN					 7

/* NodeConfig MIB */
#define MIB_ID_NODE_CONFIG						0xfffffe81

/* NodeControl MIB */
#define MIB_ID_NODE_CONTROL						0xfffffe82
#define VAR_IX_NODE_CONTROL_RESET						 0
#define VAR_IX_NODE_CONTROL_FACTORY_RESET				 1

/* Nwk MIBs *********************************************/
/* NwkStatus MIB */
#define MIB_ID_NWK_STATUS						0xfffffe88
#define VAR_IX_NWK_STATUS_RUN_TIME						 0
#define VAR_IX_NWK_STATUS_UP_COUNT						 1
#define VAR_IX_NWK_STATUS_UP_TIME						 2
#define VAR_IX_NWK_STATUS_DOWN_TIME						 3

/* NwkConfig MIB */
#define MIB_ID_NWK_CONFIG						0xfffffe89
#define VAR_IX_NWK_CONFIG_DEVICE_TYPE					 0
#define VAR_IX_NWK_CONFIG_NETWORK_ID					 1
#define VAR_IX_NWK_CONFIG_SCAN_CHANNELS					 2
#define VAR_IX_NWK_CONFIG_PAN_ID						 3
#define VAR_IX_NWK_CONFIG_END_DEVICE_ACTIVITY_TIMEOUT	 4
#define VAR_IX_NWK_CONFIG_FRAME_COUNTER_DELTA			 5
#define VAR_IX_NWK_CONFIG_STACK_MODE_INIT				 6

/* NwkControl MIB */
#define MIB_ID_NWK_CONTROL						0xfffffe8a

/* NwkSecurity MIB */
#define MIB_ID_NWK_SECURITY						0xfffffe8b
#define VAR_IX_NWK_SECURITY_KEY_NETWORK		 			 0
#define VAR_IX_NWK_SECURITY_KEY_GATEWAY		 			 1
#define VAR_IX_NWK_SECURITY_KEY_STANDALONE			  	 2
#define VAR_IX_NWK_SECURITY_CHANNEL						 3
#define VAR_IX_NWK_SECURITY_PAN_ID                       4
#define VAR_IX_NWK_SECURITY_REJOIN                       5

/* NwkTest MIB */
#define MIB_ID_NWK_TEST							0xfffffe8c
#define VAR_IX_NWK_TEST_TESTS  							 0
#define VAR_IX_NWK_TEST_TX_REQ 							 1
#define VAR_IX_NWK_TEST_TX_OK  							 2
#define VAR_IX_NWK_TEST_RX_OK  							 3
#define VAR_IX_NWK_TEST_RX_LQI_MIN						 4
#define	VAR_IX_NWK_TEST_RX_LQI_MAX						 5
#define	VAR_IX_NWK_TEST_RX_LQI_MEAN						 6
#define	VAR_IX_NWK_TEST_CW_CHANNEL						 7
#define	VAR_IX_NWK_TEST_MAC_RETRIES						 8
#define VAR_IX_NWK_TEST_TX_LQI_MIN						 9
#define	VAR_IX_NWK_TEST_TX_LQI_MAX						10
#define	VAR_IX_NWK_TEST_TX_LQI_MEAN						11
#define	VAR_IX_NWK_TEST_RX_LQI							12

/* NwkProfile MIB */
#define MIB_ID_NWK_PROFILE						0xfffffe8d
#define VAR_IX_NWK_PROFILE_MAX_FAILED_PACKETS			 0
#define VAR_IX_NWK_PROFILE_MIN_BEACON_LQI				 1
#define VAR_IX_NWK_PROFILE_PRF_BEACON_LQI				 2
#define VAR_IX_NWK_PROFILE_ROUTER_PING_PERIOD			 3

/* Peripheral MIBs **************************************/
/* AdcStatus MIB */
#define MIB_ID_ADC_STATUS						0xfffffe90
#define VAR_IX_ADC_STATUS_MASK							 0
#define VAR_IX_ADC_STATUS_READ							 1
#define VAR_IX_ADC_STATUS_CHIP_TEMP						 2
#define VAR_IX_ADC_STATUS_CAL_TEMP						 3
#define VAR_IX_ADC_STATUS_OSCILLATOR					 4

/* Device MIBs ******************************************/
/* DeviceStatus MIB */
#define MIB_ID_DEVICE_STATUS  					0xfffffea0

/* DeviceConfig MIB */
#define MIB_ID_DEVICE_CONFIG  					0xfffffea1

/* DeviceControl MIB */
#define MIB_ID_DEVICE_CONTROL 					0xfffffea2
#define VAR_IX_DEVICE_CONTROL_MODE						 0
#define VAR_IX_DEVICE_CONTROL_SCENE_ID					 1

#define VAR_VAL_DEVICE_CONTROL_MODE_OFF			  		 0	/* Device off mode */
#define VAR_VAL_DEVICE_CONTROL_MODE_ON		  			 1	/* Device on mode */
#define VAR_VAL_DEVICE_CONTROL_MODE_TOGGLE  		  	 2	/* Device toggle between on and off */
#define VAR_VAL_DEVICE_CONTROL_MODE_COUNT	  		     3	/* Number of modes */

#if defined __cplusplus
}
#endif

#endif  /* MIBCOMMON_H_INCLUDED */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
