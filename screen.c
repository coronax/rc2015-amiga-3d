#include <exec/memory.h>
#include <graphics/videocontrol.h>
#include <graphics/gfxmacros.h>
#include "screen.h"

#include <stdio.h>

extern UWORD background_data[];

void CopyImage (struct BitMap* bitmap, int bitplane, int x_offset, UWORD image[], int width, int height)
{
	UWORD* raster = (UWORD*)bitmap->Planes[bitplane] + x_offset/16;
	int num_words = (width * height) / 16;
	int words_per_row = width / 16;
	int x,y;
	UWORD* row = raster;
	for (y = 0; y < height; ++y)
	{
		row = raster;
		for (x = 0; x < words_per_row; ++x)
		{
			*row++ = *image++;
		}
		raster += bitmap->BytesPerRow/2;
	}
//	for (i = 0; i < 4000; ++i)
//	{
//		raster[i] = image[i];
//	}
	//SetWrMsk (s->RastPort, 0xff ^ (1<<bitplane));
		
}

void ClearPlane (struct BitMap* bitmap, int bitplane)
{
	//struct BitMap* bitmap = s->RasInfo->BitMap;
	int num_bytes = bitmap->BytesPerRow * bitmap->Rows;
	void* raster = (void*)bitmap->Planes[bitplane];
	// i've turned off the wait in this bitclear.  I think
	// there's enough calculation happening before draws 
	// for this to be safe, but we'll see...
	BltClear (raster, num_bytes, 0);
}

void InitMyScreen (struct MyScreen* s, ULONG mode_id, int width, int height, int depth)
{
    s->ModeID = mode_id;
    s->Width = width;
    s->Height = height;
    s->Depth = depth;

    s->View = NULL;
    s->ViewExtra = NULL;
    s->MonitorSpec = NULL;
    s->ViewPort = NULL;
    s->ViewPortExtra = NULL;
    s->ColorMap = NULL;
    s->RasInfo1 = NULL;
	s->RasInfo2 = NULL;
    s->RastPort = NULL;
}

UWORD colortable[32] = { 0x031c, 0xc00, 0x0c0, 0x33c, 0x0c0, 0x33c, 0x272, 0x333,
0x999, 0x000, 0x888, 0x33f, 0x3f3, 0xf33, 0xfff, 0x333,
0x999, 0x000, 0x888, 0x33f, 0x3f3, 0xf33, 0xfff, 0x333,
0x999, 0x000, 0x888, 0x33f, 0x3f3, 0xf33, 0xfff, 0x333 };

/*
UWORD colortable[32] = { 0x031c, 0xc00, 0x222, 0x711, 0x0c0, 0x33c, 0x272, 0x333,
0x999, 0x000, 0x888, 0x33f, 0x3f3, 0xf33, 0xfff, 0x333,
0x999, 0x000, 0x888, 0x33f, 0x3f3, 0xf33, 0xfff, 0x333,
0x999, 0x000, 0x888, 0x33f, 0x3f3, 0xf33, 0xfff, 0x333 };
*/
BOOL OpenMyScreen (struct MyScreen* s)
{
	int i;
    int num_colors = 1<<s->Depth;
    struct TagItem vc_tags[] = 
      {{VTAG_ATTACH_CM_SET, 0}, {VTAG_VIEWPORTEXTRA_SET, 0},
       {VTAG_NORMAL_DISP_SET, 0}, {VTAG_END_CM, 0}};
    struct DimensionInfo query = { 0 };


    s->View = AllocMem (sizeof (struct View), MEMF_PUBLIC | MEMF_CLEAR);
    s->ViewExtra = GfxNew (VIEW_EXTRA_TYPE);
    s->ViewPort = AllocMem (sizeof (struct ViewPort), MEMF_PUBLIC | MEMF_CLEAR);
    s->ViewPortExtra = (struct ViewPortExtra*)GfxNew(VIEWPORT_EXTRA_TYPE);
    s->ColorMap = GetColorMap(num_colors);
    s->RasInfo1 = AllocMem (sizeof (struct RasInfo), MEMF_PUBLIC | MEMF_CLEAR);
    s->RasInfo2 = AllocMem (sizeof (struct RasInfo), MEMF_PUBLIC | MEMF_CLEAR);
    s->RastPort = AllocMem (sizeof (struct RastPort), MEMF_PUBLIC | MEMF_CLEAR);

    if (!(s->View && s->ViewExtra && s->ViewPort && s->ViewPortExtra && s->ColorMap && s->RasInfo1 && s->RasInfo2 && s->RastPort))
    {
        puts ("Couldn't allocate objects for MyScreen\n");
        return FALSE;
    }

    s->MonitorSpec = OpenMonitor (NULL, s->ModeID);
    if (!s->MonitorSpec)
    {
		puts ("Couldn't open default monitor\n");
		return FALSE;
    }

    InitView (s->View);
    GfxAssociate (s->View, (struct ExtendedNode*)s->ViewExtra);
    s->View->Modes |= EXTEND_VSTRUCT;
    s->ViewExtra->Monitor = s->MonitorSpec;

    s->RasInfo1->RxOffset = 0;
    s->RasInfo1->RyOffset = 0;
    s->RasInfo1->Next = NULL;
    s->RasInfo1->BitMap = CreateBitMap (s->Width * 2, s->Height, 3/*s->Depth*/);
	printf("Create bitmap size is %d\n", s->Width * 2);
	//s->RasInfo1->Next = s->RasInfo2;
    if (!s->RasInfo1->BitMap)
    {
		puts ("Failed to allocate bitmaps\n");
		return FALSE;
    }
/*
    s->RasInfo2->RxOffset = 0;
    s->RasInfo2->RyOffset = 0;
    s->RasInfo2->Next = NULL;
    s->RasInfo2->BitMap = CreateBitMap (s->Width * 2, s->Height, 1);
    if (!s->RasInfo2->BitMap)
    {
		puts ("Failed to allocate bitmaps\n");
		return FALSE;
    }
*/
    if (!GetDisplayInfoData (NULL, (APTR)&query, sizeof(query),
				DTAG_DIMS, s->ModeID))
    {
		puts ("Failed to GetDisplayInfoData\n");
		return FALSE;
    }

    if (! (vc_tags[2].ti_Data = (ULONG)FindDisplayInfo(s->ModeID)))
    {
        puts ("Failed to FindDisplayInfo\n");
        return FALSE;
    }

    InitVPort (s->ViewPort);
    s->View->ViewPort = s->ViewPort;
    s->ViewPort->RasInfo = s->RasInfo1;
    s->ViewPort->DWidth = s->Width;
    s->ViewPort->DHeight = s->Height;
    s->ViewPort->DxOffset = 0;
    s->ViewPort->DyOffset = 0;
    s->ViewPort->Modes = (UWORD)s->ModeID & 0x0000ffff;

    s->ViewPortExtra->DisplayClip = query.TxtOScan;
	
	InitRastPort (s->RastPort);
	s->RastPort->Flags = DBUFFER;
	s->RastPort->BitMap = s->ViewPort->RasInfo->BitMap;

	// RastPort Area info
	for (i = 0; i < 100; ++i)
		s->AreaBuffer[i] = 0;
	s->AreaInfo = (struct AreaInfo*)AllocMem (sizeof (struct AreaInfo), MEMF_PUBLIC | MEMF_CLEAR);
	InitArea (s->AreaInfo, s->AreaBuffer, 200/5);
	s->RastPort->AreaInfo = s->AreaInfo;
	
	s->TmpRas = (struct TmpRas*)AllocMem (sizeof (struct TmpRas), MEMF_PUBLIC | MEMF_CLEAR);
	s->TmpRasBuffer = AllocMem (320*200/8, MEMF_CHIP | MEMF_CLEAR);
	InitTmpRas (s->TmpRas, s->TmpRasBuffer, 320*200/8);
	s->RastPort->TmpRas = s->TmpRas;
	
	
	// setup background image
	CopyImage (s->RasInfo1->BitMap, 1, 0, background_data, 320, 200);
	CopyImage (s->RasInfo1->BitMap, 1, 320, background_data, 320, 200);
	SetWrMsk (s->RastPort, 0xff ^ (1<<1));
	    
   // s->ColorMap = GetColorMap(32L);
    vc_tags[0].ti_Data = (ULONG)s->ViewPort;
    if (VideoControl (s->ColorMap, vc_tags))
    {
		puts ("VideoControl call failed\n");
		return FALSE;
    }

    LoadRGB4 (s->ViewPort, colortable, 32);

    MakeVPort (s->View, s->ViewPort);
    MrgCop (s->View);
    return TRUE;
}



void DestroyMyScreen (struct MyScreen* s)
{
	DestroyBitMap (s->RasInfo1->BitMap, s->Width*2, s->Height, 3);
//	DestroyBitMap (s->RasInfo2->BitMap, s->Width*2, s->Height, 1);
	DestroyViewPort (s->ViewPort);
	DestroyView (s->View);
	
	if (s->ColorMap)
		FreeColorMap (s->ColorMap);
	if (s->ViewPortExtra)
		GfxFree (s->ViewPortExtra);
	
	if (s->RastPort)
		FreeMem (s->RastPort, sizeof (struct RastPort));
	if (s->RasInfo1)
		FreeMem (s->RasInfo1, sizeof (struct RasInfo));
	if (s->RasInfo2)
		FreeMem (s->RasInfo2, sizeof (struct RasInfo));
	if (s->AreaInfo)
		FreeMem (s->AreaInfo, sizeof (struct AreaInfo));
	if (s->TmpRasBuffer)
		FreeMem (s->TmpRasBuffer, 320*200/8);
	if (s->TmpRas)
		FreeMem (s->TmpRas, sizeof (struct TmpRas));
}


struct BitMap* CreateBitMap (int width, int height, int depth)
{
    struct BitMap* bitmap;
    int i;

    if (!(bitmap = (struct BitMap*)AllocMem(sizeof(struct BitMap), MEMF_CLEAR)))
    {
        puts ("Failed to allocate bitmaps.\n");
        return NULL;
    }

    InitBitMap (bitmap, depth, width, height);
    for (i = 0; i < depth; ++i)
    {
        bitmap->Planes[i] = AllocRaster (width, height);
        if (bitmap->Planes[i])
        {
            BltClear (bitmap->Planes[i], (width/8)*height, 1);
        }
        else
		{
			DestroyBitMap (bitmap, width, height, depth);
			return NULL;
		}
    }

    return bitmap;
}        


void DestroyBitMap (struct BitMap* bitmap, int width, int height, int depth)
{
    int i;

    if (bitmap)
    {
        for (i = 0; i < depth; ++i)
		{
			if (bitmap->Planes[i])
				FreeRaster (bitmap->Planes[i], width, height);
		}

		FreeMem (bitmap, sizeof (struct BitMap));
    }
}




void DestroyView (struct View* view)
{
    struct ViewExtra* view_extra;
    if (view)
    {
        if (view_extra = (struct ViewExtra*)GfxLookUp(view))
		{
			if (view_extra->Monitor)
				CloseMonitor (view_extra->Monitor);
			GfxFree ((struct ExtendedNode*)view_extra);
		}
		if (view->LOFCprList)
			FreeCprList (view->LOFCprList);
		if (view->SHFCprList)
			FreeCprList (view->SHFCprList);

		FreeMem (view, sizeof (struct View));
	}
}



void DestroyViewPort (struct ViewPort* viewport)
{
    if (viewport)
    {
	//	struct ViewPortExtra* viewport_extra =
		//	(struct ViewPortExtra*)GfxLookUp(viewport);
		FreeVPortCopLists (viewport);
		FreeMem (viewport, sizeof (struct ViewPort));
//		if (viewport_extra);
	//		GfxFree ((struct ExtendedNode*)viewport_extra);
    }
}
