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

#include "drawing.h"

void Plot (struct BitMap* bp, int plane_number, int x, int y)
{
	// Rows
	// BytesPerRow
	// Planes[8]
	UBYTE* plane = bp->Planes[plane_number];
	int byte_index = y * bp->BytesPerRow + (x>>3);
	plane[byte_index] = plane[byte_index] | (1<<(7-(x&0x7)));
}

void DrawTest (struct BitMap* bp, int plane_number)
{
	int x;
	for (x = 20; x < 300; ++x)
	{
		Plot (bp, plane_number, x, 50);
	}
}

// Line-drawing routine using Bresenham's algorithm.
// Not used in the current version.
// x0, y0, x1, y1 are ints and not FIXEDs
void DrawLine (struct BitMap* bp, int plane_number, int x0, int y0, int x1, int y1)
{
	short deltax = x1-x0;
	short deltay = y1-y0;
	short steep;
	short xstep, ystep;
	UBYTE* plane;
	UBYTE* byte_index;
	short bit_index;
	UBYTE pattern;
	short error;
	short i;
	
	xstep = (deltax > 0)?1:((deltax == 0)?0:-1);
	ystep = (deltay > 0)?bp->BytesPerRow:((deltay == 0)?0:-bp->BytesPerRow);
	deltax = (deltax>=0)?deltax:-deltax;
	deltay = (deltay>=0)?deltay:-deltay;
	steep = (deltay > deltax)?1:0;
	/*
	printf ("coords: (%d, %d) to (%d, %d)\n", x0, y0, x1, y1);
	printf ("deltax is %d\n", deltax);
	printf ("deltay is %d\n", deltay);
	printf ("xstep is %d\n", xstep);
	printf ("ystep is %d\n", ystep);
	exit(0);
	*/
	
	// Calculate which byte and which bit of that byte 
	// correspond to (x0, y0).
	// Remember that bit 7 (0x80) is on the left, and bit 0
	// (0x01) is on the right.
	
	plane = bp->Planes[plane_number];
	byte_index = plane + (y0 * bp->BytesPerRow) + (x0>>3);
	bit_index = 7 - (x0&0x7);
	pattern = 1 << bit_index;
	*byte_index |=  pattern;
	
	if (!steep)
	{
		// for the shallow version, iterate over the x
		// coordinate, from 0 to deltax
		error = deltax >> 1;

		if (xstep == -1)
		{
			for (i = 0; i < deltax; ++i)
			{
				if (pattern == 0x80)
				{
					pattern = 0x01;
					--byte_index;
				}
				else
					pattern = pattern << 1;

				error = error - deltay;
				if (error < 0)
				{
					byte_index += ystep;
					error = error + deltax;
				}
				*byte_index |=  pattern;
			}
		}
		else if (xstep == 1)
		{
			for (i = 0; i < deltax; ++i)
			{
				if (pattern == 0x01)
				{
					pattern = 0x80;
					++byte_index;
				}
				else
					pattern = pattern >> 1;

				error = error - deltay;
				if (error < 0)
				{
					byte_index += ystep;
					error = error + deltax;
				}
				*byte_index |=  pattern;
			}		
		}
		else // xstep == 0
		{
			// can't actually happen in a shallow line
		}
	}
	else
	{
		// for the steep version, iterate over the y
		// coordinate, from 0 to deltay
		error = deltay >> 1;
		
		if (xstep == -1)
		{
			for (i = 0; i < deltay; ++i)
			{
				byte_index += ystep;
	
				error = error - deltax;
				if (error < 0)
				{
					if (pattern == 0x80)
					{
						pattern = 0x01;
						--byte_index;
					}
					else
						pattern = pattern << 1;

					error = error + deltay;
				
				}
				*byte_index |= pattern;
			}
		}
		else if (xstep == 1)
		{
			for (i = 0; i < deltay; ++i)
			{
				byte_index += ystep;

				error = error - deltax;
				if (error < 0)
				{
					if (pattern == 0x01)
					{
						pattern = 0x80;
						++byte_index;
					}
					else
						pattern = pattern >> 1;
	
					error = error + deltay;
				
				}
				*byte_index |= pattern;
			}			
		}
		else // xstep == 0
		{
			// can happen on a vertical line - special case
			for (i = 0; i < deltay; ++i)
			{
				byte_index += ystep;
				*byte_index |= pattern;
			}			
		}
	}
}
		
		

	
