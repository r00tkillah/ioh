/****************************************************************************/
/*
 * MODULE              Application Hardware Interface
 *
 * DESCRIPTION         EEPROM Direct Access Routines
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
#include "jendefs.h"
#include "AppHardwareApi.h"
#include "PeripheralRegs.h"
#include "PDM.h"

/* These were previously internal PDM functions */
extern PUBLIC bool_t bPDM_InitialiseEEPROM(bool_t bEEPROMreadRoutine);
extern PUBLIC void vPDM_WriteEEPROMfixSegmentData(uint8 u8SegmentIndex);

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define EEPROM_SEGMENT_SIZE  (64)
#define EEPROM_USER_SEGMENTS (63)

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
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:      bAHI_ReadEEPROM
 **
 ** DESCRIPTION:
 ** Read bytes from the EEPROM into a user-supplied buffer. It is possible to
 ** read into the subsequent sector.
 **
 ** PARAMETERS: Name       Usage
 ** uint8       u8Segment  EEPROM Segment Number
 ** uint8       u8Offset   Byte offset into segment
 ** uint8       u8Bytes    Bytes to read
 ** uint8 *     pu8Buffer  Buffer for read data
 **
 ** RETURN:
 ** TRUE if success, FALSE if parameter error
 **
 ****************************************************************************/
PUBLIC bool_t bAHI_ReadEEPROM(uint8  u8Segment,
                              uint8  u8Offset,
                              uint8  u8Bytes,
                              uint8 *pu8Buffer)
{
    uint32 u32Address = u8Segment * EEPROM_SEGMENT_SIZE + u8Offset;
    int i;

    /* Checks on input parameters: ensure that read doesn't go past end of
       last user segment */
    if ((u32Address + u8Bytes) > (EEPROM_USER_SEGMENTS * EEPROM_SEGMENT_SIZE))
    {
        return FALSE;
    }

    /* Set EEPROM for Read */
    (void)bPDM_InitialiseEEPROM(TRUE);

    /* Set EEPROM start read address */
    vREG_FECWrite(REG_FEC_EEADDR, u32Address);

    /* Set command to 8-bit reads with read-prefetch */
    vREG_FECWrite(REG_FEC_EECMD, 0x8);

    for (i = 0; i < u8Bytes; i++)
    {
        pu8Buffer[i] = (uint8)u32REG_FECRead(REG_FEC_EERDATA);
    }

    return TRUE;
}

/****************************************************************************
 **
 ** NAME:      bAHI_WriteEEPROM
 **
 ** DESCRIPTION:
 ** Write bytes to the specified segment and offset within that segment. Only
 ** writes within one segment per call are allowed.
 **
 ** PARAMETERS: Name       Usage
 ** uint8       u8Segment  EEPROM Segment Number
 ** uint8       u8Offset   Byte offset into segment
 ** uint8       u8Bytes    Bytes to write
 ** uint8 *     pu8Buffer  Buffer for write data
 **
 ** RETURN:
 ** TRUE if success, FALSE if parameter error
 **
 ****************************************************************************/
PUBLIC bool_t bAHI_WriteEEPROM(uint8  u8Segment,
                               uint8  u8Offset,
                               uint8  u8Bytes,
                               uint8 *pu8Buffer)
{
    int i;

    /* Checks on input parameters: ensure that write doesn't go beyond one
       segment */
    if (   (u8Segment >= EEPROM_USER_SEGMENTS)
        || ((u8Offset + u8Bytes) > EEPROM_SEGMENT_SIZE)
       )
    {
        return FALSE;
    }

    /* Set EEPROM for write */
    (void)bPDM_InitialiseEEPROM(FALSE);

    /* Set EEPROM start write address */
    vREG_FECWrite(REG_FEC_EEADDR, u8Segment * EEPROM_SEGMENT_SIZE + u8Offset);

    /* Set command to 8-bit writes (always get auto address increment) */
    vREG_FECWrite(REG_FEC_EECMD, 0x3);

    for (i = 0; i < u8Bytes; i++)
    {
        vREG_FECWrite(REG_FEC_EEWDATA, pu8Buffer[i]);
    }

    /* Fix Data Into EEPROM */
    vPDM_WriteEEPROMfixSegmentData(u8Segment);

    return TRUE;
}


/****************************************************************************/
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
