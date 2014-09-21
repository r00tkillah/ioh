/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Address Utilities Implementation
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
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
/* Stack includes */
#include <Api.h>
#include <AppApi.h>
#include <JIP.h>
#include <6LP.h>
#include <AccessFunctions.h>
/* JenOS includes */
#include <dbg.h>
#include <dbg_uart.h>
#include <os.h>
#include <pdm.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Address_vBuildGroup
 *
 * DESCRIPTION:
 * Constructs an IPv6 group address from a MAC Address and Group number
 *
 ****************************************************************************/
PUBLIC void Address_vBuildGroup(in6_addr      *psAddr,
								MAC_ExtAddr_s *psMacAddr,
								uint16        u16Group)
{
	/* Zero the address */
	memset(psAddr, 0, sizeof(in6_addr));

	/* Set multicast portion of address */
	psAddr->s6_addr[0] = 0xFF;
	psAddr->s6_addr[1] = (1 << 4) | (5);

	/* Set group part of group address */
	psAddr->s6_addr16[7] = u16Group;

	/* Got a MAC address ? */
	if (psMacAddr != (MAC_ExtAddr_s *) NULL)
	{
		/* Cast MAC address to uint16 pointer for easier manipulation */
		uint16 *pu16ExtAddr = (uint16 *) psMacAddr;

		/* Set MAC address portion of address */
		psAddr->s6_addr16[6] = pu16ExtAddr[3];
		psAddr->s6_addr16[5] = pu16ExtAddr[2];
		psAddr->s6_addr16[4] = pu16ExtAddr[1];
		psAddr->s6_addr16[3] = pu16ExtAddr[0];
	}
}


/****************************************************************************
 *
 * NAME: Address_eSet
 *
 * DESCRIPTION:
 * Handle remote set of a group address
 *
 ****************************************************************************/
PUBLIC teJIP_Status Address_eSet(const uint8 *pu8Val, uint8 u8Len, void *pvCbData)

{
	teJIP_Status eReturn = E_JIP_ERROR_BAD_BUFFER_SIZE;

	/* Has the correct amount of data being passed in ? */
	if (u8Len == sizeof(in6_addr))
	{
		/* Copy data */
		memcpy(pvCbData, pu8Val, u8Len);

		/* Success */
		eReturn = E_JIP_OK;
	}

	return eReturn;
}

/****************************************************************************
 *
 * NAME: MibOccIllBulbConfig_vGetAddress
 *
 * DESCRIPTION:
 * Handle remote get of an address
 *
 ****************************************************************************/
PUBLIC void Address_vGet(thJIP_Packet hPacket, void *pvCbData)
{
	/* Add address to returned data */
	eJIP_PacketAddData(hPacket, pvCbData, sizeof(in6_addr), 0);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
