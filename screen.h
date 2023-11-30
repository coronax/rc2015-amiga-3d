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

#include <graphics/gfx.h>
#include <intuition/screens.h>
#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>

struct MyScreen
{
    ULONG ModeID;
    int Width;
    int Height;
    int Depth;

    struct View *View;
    struct ViewExtra *ViewExtra;
    struct MonitorSpec *MonitorSpec;
    struct ViewPort* ViewPort;
    struct ViewPortExtra* ViewPortExtra;
    struct ColorMap *ColorMap;
    struct RasInfo *RasInfo1;
	struct RasInfo *RasInfo2;
    struct RastPort *RastPort;
	struct AreaInfo *AreaInfo;
	WORD AreaBuffer[100];
	struct TmpRas *TmpRas;
	void* TmpRasBuffer;
};


void InitMyScreen (struct MyScreen* s, ULONG mode_id, int width, int height, int depth);

BOOL OpenMyScreen (struct MyScreen* s);

struct BitMap* CreateBitMap (int width, int height, int depth);

void DestroyBitMap (struct BitMap* bitmap, int width, int height, int depth);



void DestroyView (struct View* view);


void DestroyViewPort (struct ViewPort* viewport);

void ClearPlane (struct BitMap* bitmap, int bitplane);


