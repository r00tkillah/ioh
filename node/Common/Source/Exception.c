/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         Exception Handlers
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
/* SDK includes */
#include <jendefs.h>
/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
/* JenOS includes */
#include <dbg.h>
/* Application includes */
#include "Config.h"
#include "DeviceDefs.h"
#include "Exception.h"
#include "MibCommon.h"
#ifndef DBG_ENABLE
//#include "Uart.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* Exception vectors */
#define BUS_ERROR                   *((volatile uint32 *)(0x4000000))
#define UNALIGNED_ACCESS            *((volatile uint32 *)(0x4000008))
#define ILLEGAL_INSTRUCTION         *((volatile uint32 *)(0x400000c))
#define STACK_OVERFLOW              *((volatile uint32 *)(0x4000020))
/* Exception macros */
#define MICRO_DISABLE_INTERRUPTS(); asm volatile ("b.di;" : : );

/* Chip dependant RAM size */
#if defined(JENNIC_CHIP_JN5148) || defined(JENNIC_CHIP_JN5148J01)
	#define EXCEPTION_RAM_TOP 0x04020000
#else
	#define EXCEPTION_RAM_TOP 0x04008000
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

extern uint32 heap_location;
extern void *(*prHeap_AllocFunc)(void *, uint32, bool_t);
PRIVATE void *(*prHeap_AllocOrig)(void *, uint32, bool_t);

PRIVATE void *pvHeapAllocOverflowProtect(void *pvPointer, uint32 u32Size, bool_t bClear);

/* JN514x Chip family ? */
#ifdef JENNIC_CHIP_FAMILY_JN514x
PRIVATE void vBusErrorException(uint32 u32HeapAddr, uint32 u32Vector);
PRIVATE void vUnalignedAccessException(uint32 u32HeapAddr, uint32 u32Vector);
PRIVATE void vIllegalInstructionException(uint32 u32HeapAddr, uint32 u32Vector);
PRIVATE void vStackOverflowException(uint32 u32HeapAddr, uint32 u32Vector);
#endif
PRIVATE void vException(uint32 u32HeapAddr, uint32 u32Vector, uint32 u32Code);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC uint32 u32ExceptionHeapMin;
PUBLIC uint32 u32ExceptionHeapMax;
PUBLIC uint32 u32ExceptionStackMin;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: Exception_vInit
 *
 * DESCRIPTION:
 * Exception handler initialisation
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Exception_vInit(void)
{
	/* JN514x Chip family ? */
	#ifdef JENNIC_CHIP_FAMILY_JN514x
	{
		/* Initialise exception handlers */
		BUS_ERROR               =  (uint32) vBusErrorException;
		UNALIGNED_ACCESS        =  (uint32) vUnalignedAccessException;
		ILLEGAL_INSTRUCTION     =  (uint32) vIllegalInstructionException;
		STACK_OVERFLOW          =  (uint32) vStackOverflowException;
	}
	#endif
    /* Note heap start */
    u32ExceptionHeapMin = u32ExceptionHeapMax = ((uint32 *)&heap_location)[0];
    /* Read r1 for bottom of stack */
	uint32 u32r1;
	asm volatile("b.ori %0,r1,0" : "=r"(u32r1) : );
	/* Set memory to know value between heap and stack */
	for (u32ExceptionStackMin = u32ExceptionHeapMin;
	     u32ExceptionStackMin < u32r1;
	     u32ExceptionStackMin += 4)
	{
		/* Set memory to known value */
		*((uint32 *)u32ExceptionStackMin) = 0x00029761;
	}
    /* Initialise heap protection handler */
    prHeap_AllocOrig = prHeap_AllocFunc;
    prHeap_AllocFunc = pvHeapAllocOverflowProtect;
}

/****************************************************************************
 *
 * NAME: Exception_vInit
 *
 * DESCRIPTION:
 * Exception handler initialisation
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void Exception_vUpdateStackMin(void)
{
	/* Loop through memory looking for new minimum stack area */
	while (*((uint32 *)u32ExceptionStackMin) != 0x00029761 &&
	                   u32ExceptionStackMin  >  u32ExceptionHeapMax)
	{
		u32ExceptionStackMin -= 4;
	}
}

/****************************************************************************
 *
 * NAME: pvHeapAllocOverflowProtect
 *
 * DESCRIPTION:
 *
 ****************************************************************************/
PRIVATE void *pvHeapAllocOverflowProtect(void *pvPointer, uint32 u32Size, bool_t bClear)
{
    void *pvAlloc;

    pvAlloc = prHeap_AllocOrig(pvPointer, u32Size, bClear);
    vAHI_SetStackOverflow(TRUE, ((uint32 *)&heap_location)[0]);
    if (((uint32 *)&heap_location)[0] > u32ExceptionHeapMax) u32ExceptionHeapMax = ((uint32 *)&heap_location)[0];

    return pvAlloc;
}

/* JN514x Chip family ? */
#ifdef JENNIC_CHIP_FAMILY_JN514x
PRIVATE void vBusErrorException(uint32 u32HeapAddr, uint32 u32Vector)
{
    vException(u32HeapAddr, u32Vector, 1);
}

PRIVATE void vUnalignedAccessException(uint32 u32HeapAddr, uint32 u32Vector)
{
    vException(u32HeapAddr, u32Vector, 2);
}

PRIVATE void vIllegalInstructionException(uint32 u32HeapAddr, uint32 u32Vector)
{
    vException(u32HeapAddr, u32Vector, 3);
}

PRIVATE void vStackOverflowException(uint32 u32HeapAddr, uint32 u32Vector)
{
    vException(u32HeapAddr, u32Vector, 4);
}
/* Other chip family ? */
#else
PUBLIC void vException_BusError(uint32 u32StackPointer, uint32 u32Vector)
{
    vException(u32StackPointer, u32Vector, 1);
}

PUBLIC void vException_UnalignedAccess(uint32 u32StackPointer, uint32 u32Vector)
{
    vException(u32StackPointer, u32Vector, 2);
}

PUBLIC void vException_IllegalInstruction(uint32 u32StackPointer, uint32 u32Vector)
{
    vException(u32StackPointer, u32Vector, 3);
}

PUBLIC void vException_StackOverflow(uint32 u32StackPointer, uint32 u32Vector)
{
    vException(u32StackPointer, u32Vector, 4);
}

PUBLIC void vException_Trap(uint32 u32StackPointer, uint32 u32Vector)
{
    vException(u32StackPointer, u32Vector, 5);
}
#endif
/****************************************************************************
 *
 * NAME: vException
 *
 * DESCRIPTION:
 *
 ****************************************************************************/
PRIVATE void vException(uint32 u32HeapAddr, uint32 u32Vector, uint32 u32Code)
{
    /* u32HeapAddr is the address of the processor heap now
       u32Vector is the exception source */

	/* JN514x Chip family ? */
	#ifdef JENNIC_CHIP_FAMILY_JN514x
	extern tSPIflashFncTable *pSPIflashFncTable;
	#endif
	uint32 					u32WriteMax = 0;
	uint32 					u32WriteMin = 0;
	uint32 					u32Write;
	uint32  				u32Read;
	uint32				   au32Read[8];
	uint32 					u32Loop;
    uint32 				   au32Value[4];

	/* Stop watchdog */
    vAHI_WatchdogStop();

	/* Disable interrupts */
    MICRO_DISABLE_INTERRUPTS();

	/* Debug enabled ? */
	#if defined(DBG_ENABLE)
	{
		/* Debug basic data */
		DBG_vPrintf(DEBUG_EXCEPTION, "Exception=%08x Code=%x HeapMax=%x StackMin=%x", u32Vector, u32Code, u32ExceptionHeapMax, u32ExceptionStackMin);
		DBG_vPrintf(DEBUG_EXCEPTION, "Dump:");
	}
	/* Basic UART enabled ? */
	#elif defined(UART_H_INCLUDED)
	{
		/* Initialise UART */
		UART_vInit();
		/* Debug basic data */
		UART_vString("\r\nEXCEPTION");
		UART_vString("\r\nException=");
		UART_vNumber(u32Vector, 16);
		UART_vString(" Code=");
		UART_vNumber(u32Code, 16);
		UART_vString(" HeapMax=");
		UART_vNumber(u32ExceptionHeapMax, 16);
		UART_vString(" StackMin=");
		UART_vNumber(u32ExceptionStackMin, 16);
		UART_vString("\r\nDump:");
	}
	#endif
	/* JN514x Chip family ? */
	#ifdef JENNIC_CHIP_FAMILY_JN514x
	{
		/* Initialise flash */
		bAHI_FlashInit(E_FL_CHIP_AUTO, NULL);
		/* Calculate sector size (minus 1kb for OND footers) */
		if (pSPIflashFncTable->u16FlashId == CONFIG_FLASH_ID_STM25P40A)
		{
			/* JN5148J01 ? */
			#ifdef JENNIC_CHIP_JN5148J01
			{
				/* Allow dump into second sector only  (minus 1kb)
				  (software is likely to be larger than a single sector here plus there is no second stage bootloader) */
				u32WriteMin =  64*1024;
				u32WriteMax = 127*1024; /* ST M25P40A 8*64Kb */
			}
			#else
			{
				/* Allow dump into first sector only (minus 1kb) */
				u32WriteMax = 63*1024; /* ST M25P40A 8*64Kb */
			}
			#endif
		}
		if (pSPIflashFncTable->u16FlashId == CONFIG_FLASH_ID_STM25P10A)
		{
			/* Allow dump into first sector only (minus 1kb) */
			u32WriteMax = 31*1024; /* ST M25P10A 4*32Kb */
		}
	}
	/* Other chip families ? */
	#else
	{
		/* Assume 8*32Kb sectors */
		/* Sector 0 (  0- 31) - OND Application 1 */
		/* Sector 1 ( 32- 63) - OND Application 1 */
		/* Sector 2 ( 64- 95) - OND Application 1 */
		/* Sector 3 ( 96-127) - OND Application 1 (Exception dumps) */
		/* Sector 4 (128-159) - OND Application 2 */
		/* Sector 5 (160-191) - OND Application 2 */
		/* Sector 6 (192-223) - OND Application 2 */
		/* Sector 7 (224-255) - OND Application 2 */

		/* Initialise flash */
		bAHI_FlashInit(E_FL_CHIP_INTERNAL, NULL);

		/* Allow dump into fourth sector only */
		u32WriteMin = 224*1024;
		u32WriteMax = 256*1024;
	}
	#endif

	/* Loop through flash looking for blank area to dump exception data into */
	for (u32Read = u32WriteMin, u32Write = 0xffffffff; u32Read < u32WriteMax && u32Write == 0xffffffff; u32Read += sizeof(au32Read))
	{
		/* Read in 64 bytes */
		bAHI_FullFlashRead(u32Read, sizeof(au32Read), (uint8 *) &au32Read);
		/* Loop through data */
		for (u32Loop = 1; u32Loop < 8; u32Loop++)
		{
			/* Collect lowest value into entry 0 */
			if (au32Read[0] > au32Read[u32Loop]) au32Read[0] = au32Read[u32Loop];
		}
		/* Data is blank - use read address as write address */
		if (au32Read[0] == 0xffffffff) u32Write = u32Read;
	}
	/* Valid flash write address ? */
	if (u32Write < u32WriteMax - 16)
	{
		/* Write exception text, code and vector */
		au32Value[0] = 0x45584345;
		au32Value[1] = 0x5054494f;
		au32Value[2] = 0x4e202000 + (u32Code & 0xff);
		au32Value[3] = u32Vector;
		bAHI_FullFlashProgram(u32Write, sizeof(au32Value), (uint8 *) au32Value);
		u32Write += sizeof(au32Value);
	}

	/* Loop dumping stack */
	u32Loop = 0;
	while (u32HeapAddr < EXCEPTION_RAM_TOP)
	{
		/* Note heap address */
		au32Value[u32Loop] = *(uint32 *)u32HeapAddr;
		/* Debug enabled ? */
		#if defined(DBG_ENABLE)
		{
			/* Time for a new line - output one */
			if (0 == u32Loop) DBG_vPrintf(DEBUG_EXCEPTION, "\n\r");
			/* Output data */
			DBG_vPrintf(DEBUG_EXCEPTION, "%04x:%08x ", u32Loop, *(uint32 *)u32HeapAddr);
		}
		/* Basic UART enabled ? */
		#elif defined(UART_H_INCLUDED)
		{
			/* Time for a new line - output one */
			if (0 == u32Loop) UART_vString("\r\n");
			/* Output data */
			UART_vNumber(u32Loop, 16);
			UART_vChar(':');
			UART_vNumber(*(uint32 *)u32HeapAddr, 16);
			UART_vChar(' ');
		}
		#endif
		/* Increment heap address */
		u32HeapAddr += 4;
		/* Increment loop counter */
		u32Loop++;
		/* Output 4 values ? */
		if (4 == u32Loop)
		{
			/* Valid flash write address ? */
			if (u32Write < u32WriteMax - 16)
			{
				/* Write to flash */
				bAHI_FullFlashProgram(u32Write, sizeof(au32Value), (uint8 *) au32Value);
				/* Increment location for next time */
				u32Write += sizeof(au32Value);
			}
			/* Revert to 0 */
			u32Loop = 0;
		}
	}
	/* Got some data left to write ? */
	if (0 != u32Loop)
	{
		while (u32Loop < 4)
		{
			au32Value[u32Loop++] = 0xffffffff;
		}
		/* Valid flash write address ? */
		if (u32Write < u32WriteMax - 16)
		{
			/* Write to flash */
			bAHI_FullFlashProgram(u32Write, sizeof(au32Value), (uint8 *) au32Value);
		}
	}

    /* Run device specific exception handler */
    Device_vException(u32HeapAddr, u32Vector, u32Code);

	/* Production build ? */
	#if MK_PRODUCTION
	{
	    /* Reset */
	    vAHI_SwReset();
	}
	#else
	{
	    /* Loop indefinitely */
	    while(1);
	}
	#endif
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
