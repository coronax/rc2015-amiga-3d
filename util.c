/* Copyright (c) 2015, Christopher Just
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above 
 *    copyright notice, this list of conditions and the following 
 *    disclaimer in the documentation and/or other materials 
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <stdio.h>
#include <devices/timer.h>

struct Library* IntuitionBase = NULL;
struct Library* GfxBase = NULL;
struct Library* MathBase = NULL;
struct Library* MathTransBase = NULL;


BOOL OpenLibraries ()
{
    IntuitionBase = OpenLibrary("intuition.library", 37);
    GfxBase = OpenLibrary("graphics.library", 37);
    MathBase = OpenLibrary ("mathffp.library", 37L);
    MathTransBase = OpenLibrary ("mathtrans.library", 37L);

    if (IntuitionBase == NULL)
    {
        printf ("Error opening intuition.library\n");
        return FALSE;
    }
    if (GfxBase == NULL)
    {
        printf ("Error opening graphics.library\n");
        return FALSE;
    }
    if (MathBase == NULL)
    {
        printf ("Error opening mathffp.library\n");
		return FALSE;
    }
    if (MathTransBase == NULL)
    {
        printf ("Error opening mathtrans.library\n");
		return FALSE;
    }
    return TRUE;
}


void CloseLibraries ()
{
    if (MathTransBase)
        CloseLibrary (MathTransBase);
    if (MathBase)
        CloseLibrary (MathBase);
    if (GfxBase)
        CloseLibrary (GfxBase);
    if (IntuitionBase)
        CloseLibrary (IntuitionBase);
}


struct Library* TimerBase = NULL;
struct timerequest* TimerIO;
struct timeval time1;

BOOL OpenTimer ()
{
    int error;
    TimerIO = (struct timerequest*)AllocMem(sizeof(struct timerequest), 
				   MEMF_PUBLIC | MEMF_CLEAR);
    error = OpenDevice (TIMERNAME, UNIT_MICROHZ, (struct IORequest*)TimerIO, 0);
    if (error)
    {
        printf ("Error: Can't open timer.device\n");
		return FALSE;
    }
    else
    {
        TimerBase = (struct Library*)TimerIO->tr_node.io_Device;
		return TRUE;
    }
}


void CloseTimer()
{
    if (TimerBase)
    {
        /*
        if (!CheckIO((struct IORequest*)TimerIO))
		{
			AbortIO ((struct IORequest*)TimerIO);
		}
		WaitIO ((struct IORequest*)TimerIO);
		*/
		CloseDevice ((struct IORequest*)TimerIO);
    }
    if (TimerIO)
        FreeMem (TimerIO, sizeof (struct timerequest));
}
