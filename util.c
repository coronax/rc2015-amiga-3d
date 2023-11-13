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
