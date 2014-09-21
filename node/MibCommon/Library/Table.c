/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Table - Implementation Generic Access Functions
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
/***        Include files                                                 ***/
/****************************************************************************/
/* Standard includes */
#include <string.h>
/* SDK includes */
#include <jendefs.h>
#include <JIP.h>
/* Application includes */
#include "Table.h"

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

/****************************************************************************/
/***        Imported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Table_vGetData
 *
 * DESCRIPTION:
 * Responds to remote table get data requests
 *
 ****************************************************************************/
PUBLIC void Table_vGetData(thJIP_Packet      hPacket,
						   void             *pvCbData,
						   tsJIP_TableData *psTableData)
{
	uint16 u16AddedCount = 0;
    uint16 u16AddedEntry = 0;
    uint16 u16Remaining = 0;
	uint16 u16Entry;

	tsTable *psTable;
	uint8  *pu8Data;

	/* Get pointer to table structure */
	psTable = (tsTable *) pvCbData;
	pu8Data = (uint8 *) psTable->pvData;

	/* Loop through entire table */
	for (u16Entry = 0; u16Entry < psTable->u16Entries; u16Entry++)
	{
		/* Is this entry in the range requested ? */
		if ((u16Entry >= psTableData->u16FirstEntry) && (u16AddedCount < psTableData->u8EntryCount))
		{
			/* Can we add this group to packet ? */
			if (eJIP_PacketAddData(hPacket, &pu8Data[u16Entry*psTable->u32Size], psTable->u32Size, u16Entry) == E_JIP_OK)
			{
				/* Increment count of entries added to this packet */
				u16AddedCount++;
				/* Note added entry */
				u16AddedEntry = u16Entry;
			}
		}
	}

	/* Didn't send any entries ? */
	if (u16AddedCount == 0)
	{
		/* No entries requested ? */
		if (psTableData->u8EntryCount == 0)
		{
			/* All entries remain */
			u16Remaining = psTable->u16Entries;
		}
		/* No entries to send ? */
		else
		{
			/* No entries remain */
			u16Remaining = 0;
		}
	}
	else
	{
		/* Calculate remaining entries */
		u16Remaining = psTable->u16Entries - u16AddedEntry - 1;
	}

	/* Update table data with hash value and remaining entries */
	psTableData->u16TableVersion = psTable->u16Hash;
    psTableData->u16RemainingEntries = u16Remaining;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
