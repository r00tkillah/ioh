/****************************************************************************/
/*
 * MODULE              JN-AN-1162 JenNet-IP Smart Home
 *
 * DESCRIPTION         UART Basic Handler
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
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>
#include "Uart.h"

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
PRIVATE bool_t bOpen = FALSE;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void UART_vInit(void)
{
	/* UART0 not already enabled ? */
	if ((u32REG_SysRead(REG_SYS_PWR_CTRL) & REG_SYSCTRL_PWRCTRL_UEN0_MASK) == 0)
	{
		/* Disable use of CTS/RTS */
		vAHI_UartSetRTSCTS(E_AHI_UART_0, FALSE);
		/* Enable UART */
		vAHI_UartEnable(E_AHI_UART_0);
		/* Set settings */
		vAHI_UartSetControl(E_AHI_UART_0, E_AHI_UART_EVEN_PARITY, E_AHI_UART_PARITY_DISABLE, E_AHI_UART_WORD_LEN_8, E_AHI_UART_1_STOP_BIT, E_AHI_UART_RTS_HIGH);
		/* Set baud rate */
		vAHI_UartSetClockDivisor(E_AHI_UART_0, E_AHI_UART_RATE_115200);
		/* Note we opened the UART */
		bOpen = TRUE;
	}
}

PUBLIC void UART_vClose(void)
{
	/* UART is open ? */
	if (bOpen)
	{
		/* Wait for buffers to empty */
		while ((u8AHI_UartReadLineStatus(E_AHI_UART_0) & E_AHI_UART_LS_THRE) == 0);
		while ((u8AHI_UartReadLineStatus(E_AHI_UART_0) & E_AHI_UART_LS_TEMT) == 0);
		/* Disable UART */
		vAHI_UartDisable(E_AHI_UART_0);
	}
}

PUBLIC void UART_vChar(char cChar)
{
#if	UART_TRACE_ENABLE
	/* UART is open ? */
	if (bOpen)
	{
		/* Write character */
		vAHI_UartWriteData(E_AHI_UART_0, cChar);
		/* Wait for buffers to empty */
		while ((u8AHI_UartReadLineStatus(E_AHI_UART_0) & E_AHI_UART_LS_THRE) == 0);
		while ((u8AHI_UartReadLineStatus(E_AHI_UART_0) & E_AHI_UART_LS_TEMT) == 0);
	}
#endif
}

PUBLIC void UART_vString(char *pcMessage)
{
#if UART_TRACE_ENABLE
	/* UART is open ? */
	if (bOpen)
	{
		while (*pcMessage)
		{
			UART_vChar(*pcMessage);
			pcMessage++;
		}
	}
#endif
}

PUBLIC void UART_vNumber (uint32 u32Number, uint8 u8Base)
{
#if	UART_TRACE_ENABLE
	/* UART is open ? */
	if (bOpen)
	{
		char buf[33];
		char *p = buf + 33;
		uint32 c, n;

		/* Convert number to string */
		*--p = '\0';
		do {
			n = u32Number / u8Base;
			c = u32Number - (n * u8Base);
			if (c < 10) {
				*--p = '0' + c;
			} else {
				*--p = 'a' + (c - 10);
			}
			u32Number /= u8Base;
		} while (u32Number != 0);

		/* Output string */
		UART_vString(p);
	}
#endif
}

PUBLIC void UART_vPTSChar(char cChar)
{
	/* UART is open ? */
	if (bOpen)
	{
		vAHI_UartWriteData(E_AHI_UART_0, cChar);
		while ((u8AHI_UartReadLineStatus(E_AHI_UART_0) & E_AHI_UART_LS_THRE) == 0);
		while ((u8AHI_UartReadLineStatus(E_AHI_UART_0) & E_AHI_UART_LS_TEMT) == 0);
	}
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
