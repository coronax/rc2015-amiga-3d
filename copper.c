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
#include "screen.h"

#define COLOR0 0x0180
#define COLOR1 0x0182
#define COLOR2 0x0184
#define COLOR3 0x0186
#define COLOR4 0x0188
#define COLOR5 0x018a
#define COLOR6 0x018c
#define COLOR7 0x018e
#define COLOR8 0x0190
#define COLOR9 0x0192
//#define INT_TRIGGER 0x009c
#define DIWHIGH 0x1e4

#define BPL1PTH	0x0e0
#define BPL1PTL 0x0e2
#define BPL2PTH	0x0e4
#define BPL2PTL 0x0e6
#define BPL3PTH	0x0e8
#define BPL3PTL 0x0ea
#define BPL4PTH	0x0ec
#define BPL4PTL 0x0ee
#define BPL5PTH	0x0f0
#define BPL5PTL 0x0f2
#define BPL6PTH	0x0f4
#define BPL6PTL 0x0f6

#define DIWSTRT 0x08e
#define DIWSTOP 0x090
#define DDFSTRT 0x092
#define DDFSTOP 0x094
#define BPLCON0 0x100
#define BPLCON1 0x102
#define BPLCON2 0x104
#define BPLCON3 0x106
#define BPL1MOD 0x108
#define BPL2MOD 0x10a


UWORD copper_list_data[] = {
 0x2b01,  0xfffe, /* Wait for vertical position = 43 */
 COLOR0,   0x31c, /* start of color table */
 COLOR1,   0xc00,
 COLOR2,   0x0c0,
 COLOR3,   0x33c,
 COLOR4,   0x0c0,
 COLOR5,   0x33c,
 COLOR6,   0x272,
 COLOR7,   0x333,	/* end of color table */
 DIWSTRT, 0x2c81, /* diwstrt row 44 col 129 */ 
 BPLCON0, 0x3200, /* bplcon0h */
 BPLCON2,   0x24, /* bplcon2h */ 
 DIWSTOP, 0xf4c1, /* diwstop row 244 col 449 */
 DDFSTRT,   0x30, /* ddfstrt = Wide (to support scroll) */
 DDFSTOP, 0x00d0/*0xd8*/, /* ddfstop = Wide */
 BPLCON1,      0, /* bplcon0l */
 BPL1MOD,   0x26, /* bpl1mod */
 BPL2MOD,   0x26, /* bpl2mod */
 BPL1PTH,      0, /* pblpt0 h   bitplane pointers, right? [36]*/
 BPL1PTL, 0x9810, /* bplpt0 l */
 BPL2PTH,      0, /* bplpt1 h */
 BPL2PTL, 0xd690, /* bplpt1 l */ 
 BPL3PTH,    0x2, /* bplpt2 h */
 BPL3PTL, 0x8340, /* bplpt2 l */
 DIWHIGH, 0x2000, /* diwhigh set horiz. stop +256 */
 0x2c01,  0xfffe, /* start of my custom list. Wait for 44 */
 COLOR0,   0x42c, // set color0
 DDFSTRT,   0x30, // display data fetch = Wide
 BPL1MOD, 0x0026, // bitplane modulo. bitplane is 80 bytes, display is 40.
 BPL2MOD, 0x0026, // But we fetch 1 extra word for scrolling, so mod = 38.
 COLOR2,   0xddd, // set color2
 COLOR3,   0xdaa, // set color3
 COLOR6,   0xada, // set color6
 COLOR7,   0xaad, // set color7
 0x3801,  0xfffe, /* Wait for vert pos = 56 */
 COLOR0,   0x53c, // set color0
 0x4401,  0xfffe, /* Wait for vert pos = 68 */
 COLOR0,   0x64c, // set color0
 0x8ae3,  0xfffe, /* Wait for vert pos = 138, hpos = 226 */
#if 0
  0x9c, 0x8010,	/* this was my call interrupt */
#endif
                   /* parallax scroll for top (back) section. */
 BPL2PTH, 0,			/* [78] bitplane address high byte */			
 BPL2PTL, 0,			/* [80] bitplane address low byte */
 BPLCON1, 0,			/* [82] bplcon1 - set scroll */
 0x8b01, 0xfffe,
 COLOR0,  0x75d,
 0x8d01, 0xfffe,
 COLOR2,  0x222,
 COLOR3,  0x711,
 COLOR6,  0x272,
 COLOR7,  0x333,
 0x8e01, 0xfffe,
 COLOR0,  0x86d,
 0x9001, 0xfffe,
 COLOR0,  0x97d,
 0x9201, 0xfffe,
 COLOR0,  0xa8d,
 0x9401, 0xfffe,
 COLOR0,  0xb9e,
 0x9601, 0xfffe,
 COLOR0,  0xcae,
 0x9801, 0xfffe,
 COLOR0,  0xdbe,
 0x9a01, 0xfffe,
 COLOR0,  0xece,
 0x9c01, 0xfffe,
 COLOR0,  0xede,
 0x9e01, 0xfffe,
 COLOR0,  0xeee,
 0xa301, 0xfffe,
 COLOR0,  0x443,
 COLOR1,  0x833,
 COLOR4,  0x383,
 COLOR5,  0x338,
 0xb1e3, 0xfffe,
                    /* parallax scroll for bottom (front) section. */
 BPL2PTH, 0,			/* bitplane address high byte [146]*/			
 BPL2PTL, 0,			/* bitplane address low byte */
 BPLCON1, 0,			/* bplcon1 - set scroll */

 0xb201, 0xfffe,
 COLOR0,  0x553,
 COLOR1,  0x933,
 COLOR4,  0x393,
 COLOR5,  0x339,
 0xc001, 0xfffe,
 COLOR0,  0x664,
 COLOR1,  0xa33,
 COLOR4,  0x3a3,
 COLOR5,  0x33a,
 0xce01, 0xfffe,
 COLOR0,  0x774,
 COLOR1,  0xb33,
 COLOR4,  0x3b3,
 COLOR5,  0x33b,
 0xdc01, 0xfffe,
 COLOR0,  0x885,
 COLOR1,  0xc33,
 COLOR4,  0x3c3,
 COLOR5,  0x33c,
 0xffff, 0xfffe,
     0,      0,
     0,      0,
     0,      0
};

const int copper_list_count = sizeof(copper_list_data)/4;

void CramCopperList (struct MyScreen* s)
{
	UWORD* new_list = AllocMem (sizeof (copper_list_data), MEMF_CHIP);
	int i;
	// Copy the copper list template
	for (i = 0; i < 100*2; ++i)
		new_list[i] = copper_list_data[i];
	// Fill in bitplane pointers
	new_list[37] = (UWORD)((ULONG)s->RasInfo1->BitMap->Planes[0] >> 16);
	new_list[39] = (UWORD)((ULONG)s->RasInfo1->BitMap->Planes[0] & 0x0000ffff);
	new_list[41] = (UWORD)((ULONG)s->RasInfo1->BitMap->Planes[1] >> 16);
	new_list[43] = (UWORD)((ULONG)s->RasInfo1->BitMap->Planes[1] & 0x0000ffff);
	new_list[45] = (UWORD)((ULONG)s->RasInfo1->BitMap->Planes[2] >> 16);
	new_list[47] = (UWORD)((ULONG)s->RasInfo1->BitMap->Planes[2] & 0x0000ffff);

	s->View->LOFCprList->start = new_list;
	s->View->LOFCprList->MaxCount = 100;
}

void UpdateCopperList (struct MyScreen* s, UWORD* ptr1, UWORD scroll1, UWORD* ptr2, UWORD scroll2)
{
	s->View->LOFCprList->start[79] = (UWORD)((ULONG)ptr1 >> 16);
	s->View->LOFCprList->start[81] = (UWORD)((ULONG)ptr1 & 0x0000ffff);
	s->View->LOFCprList->start[83] = scroll1;

	s->View->LOFCprList->start[147] = (UWORD)((ULONG)ptr2 >> 16);
	s->View->LOFCprList->start[149] = (UWORD)((ULONG)ptr2 & 0x0000ffff);
	s->View->LOFCprList->start[151] = scroll2;
}

