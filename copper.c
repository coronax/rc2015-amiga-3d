#include <exec/memory.h>
#include "screen.h"

UWORD copper_list_data[] = {
 0x2b01, 0xfffe, /* Wait for vertical position = 43 */
 0x180,  0x31c, /* start of color table */
 0x182,  0xc00,
 0x184,   0xc0,
 0x186,  0x33c,
 0x188,   0xc0,
 0x18a,  0x33c,
 0x18c,  0x272,
 0x18e,  0x333,	/* end of color table */
 0x08e, 0x2c81, /* diwstrt row 44 col 129 */ 
 0x100, 0x3200, /* bplcon0h */
 0x104,   0x24, /* bplcon2h */ 
 0x090, 0xf4c1, /* diwstop row 244 col 449 */
 0x092,   0x30, /* ddfstrt = Wide (to support scroll) */
 0x094,   0x00d0/*0xd8*/, /* ddfstop = Wide */
 0x102,      0, /* bplcon0l */
 0x108,   0x26, /* bpl1mod */
 0x10a,   0x26, /* bpl2mod */
 0x0e0,      0, /* pblpt0 h   bitplane pointers, right? [36]*/
 0x0e2, 0x9810, /* bplpt0 l */
 0x0e4,      0, /* bplpt1 h */
 0x0e6, 0xd690, /* bplpt1 l */ 
 0x0e8,    0x2, /* bplpt2 h */
 0x0ea, 0x8340, /* bplpt2 l */
 0x1e4, 0x2000, /* diwhigh set horiz. stop +256 */
0x2c01, 0xfffe, /* start of my custom list. Wait for 44 */
 0x180,  0x42c, // set color0
 0x092,   0x30, // display data fetch = Wide
 0x108,   0x0026, // bitplane modulo. bitplane is 80 bytes, display is 40.
 0x10a,   0x0026, // But we fetch 1 extra word for scrolling, so mod = 38.
 0x184,  0xddd, // set color2
 0x186,  0xdaa, // set color3
 0x18c,  0xada, // set color6
 0x18e,  0xaad, // set color4
0x3801, 0xfffe, /* Wait for vert pos = 56 */
 0x180,  0x53c, // set color0
0x4401, 0xfffe, /* Wait for vert pos = 68 */
 0x180,  0x64c, // set color0
0x8ae3, 0xfffe, /* Wait for vert pos = 138, hpos = 226 */
#if 0
  0x9c, 0x8010,	/* this was my call interrupt */
#endif
#if 1
                    /* parallax scroll for top (back) section. */
0x00e4, 0,			/* [78] bitplane address high byte */			
0x00e6, 0,			/* [80] bitplane address low byte */
0x0102, 0,			/* [82] bplcon1 - set scroll */
#endif
0x8b01, 0xfffe,
 0x180,  0x75d,
0x8d01, 0xfffe,
 0x184,  0x222,
 0x186,  0x711,
 0x18c,  0x272,
 0x18e,  0x333,
0x8e01, 0xfffe,
/*100*/ 0x180,  0x86d,
0x9001, 0xfffe,
 0x180,  0x97d,
0x9201, 0xfffe,
 0x180,  0xa8d,
0x9401, 0xfffe,
 0x180,  0xb9e,
0x9601, 0xfffe,
 0x180,  0xcae,
0x9801, 0xfffe,
 0x180,  0xdbe,
0x9a01, 0xfffe,
 0x180,  0xece,
0x9c01, 0xfffe,
 0x180,  0xede,
0x9e01, 0xfffe,
 0x180,  0xeee,
0xa301, 0xfffe,
 0x180,  0x443,
 0x182,  0x833,
 0x188,  0x383,
 0x18a,  0x338,
0xb1e3, 0xfffe,
//  0x9c, 0x8010,
#if 1
                    /* parallax scroll for bottom (front) section. */
0x00e4, 0,			/* bitplane address high byte [146]*/			
0x00e6, 0,			/* bitplane address low byte */
0x0102, 0,			/* bplcon1 - set scroll */
#endif

0xb201, 0xfffe,
 0x180,  0x553,
 0x182,  0x933,
 0x188,  0x393,
 0x18a,  0x339,
0xc001, 0xfffe,
 0x180,  0x664,
 0x182,  0xa33,
 0x188,  0x3a3,
 0x18a,  0x33a,
0xce01, 0xfffe,
 0x180,  0x774,
 0x182,  0xb33,
 0x188,  0x3b3,
 0x18a,  0x33b,
0xdc01, 0xfffe,
 0x180,  0x885,
 0x182,  0xc33,
 0x188,  0x3c3,
 0x18a,  0x33c,
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

