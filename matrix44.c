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

#include "matrix44.h"

#include <stdio.h>
//#include <math.h>

void PrintMatrix (Matrix44* m)
{
    int i;
    for (i = 0; i < 4; ++i)
    {
        printf ("| %f %f %f %f |\n", 
		FIXTOFLOAT(m->val[i][0]), FIXTOFLOAT(m->val[i][1]), 
                FIXTOFLOAT(m->val[i][2]), FIXTOFLOAT(m->val[i][3]));
    }
}

void PrintPoint (Point4* p)
{
    printf ("[ %f %f %f ]", FIXTOFLOAT(p->val[0]), FIXTOFLOAT(p->val[1]), FIXTOFLOAT(p->val[2]));
}

void SetPoint (Point4* p, int x, int y, int z)
{
    p->val[0] = x;
    p->val[1] = y;
    p->val[2] = z;
    p->val[3] = FIX1;
}

/* b = a. */
void CopyMatrix (Matrix44* a, Matrix44* b)
{
    b->val[0][0] = a->val[0][0]; b->val[0][1] = a->val[0][1];
    b->val[0][2] = a->val[0][2]; b->val[0][3] = a->val[0][3];
    b->val[1][0] = a->val[1][0]; b->val[1][1] = a->val[1][1];
    b->val[1][2] = a->val[1][2]; b->val[1][3] = a->val[1][3];
    b->val[2][0] = a->val[2][0]; b->val[2][1] = a->val[2][1];
    b->val[2][2] = a->val[2][2]; b->val[2][3] = a->val[2][3];
    b->val[3][0] = a->val[3][0]; b->val[3][1] = a->val[3][1];
    b->val[3][2] = a->val[3][2]; b->val[3][3] = a->val[3][3];
}

/* Orthogonal projection matrix. */
void MakeOrthoMatrix (Matrix44* m, int min_x, int max_x, int min_y, 
                      int max_y, int min_z, int max_z)
{
    m->val[0][0] = FIXDIV (FIX2 ,(max_x - min_x));
    m->val[0][1] = 0;
    m->val[0][2] = 0;
    m->val[0][3] = - FIXDIV ((max_x + min_x) , (max_x - min_x));
    m->val[1][0] = 0;
    m->val[1][1] = FIXDIV (FIX2, (max_y - min_y));
    m->val[1][2] = 0;
    m->val[1][3] = - FIXDIV ((max_y + min_y) , (max_y - min_y));
    m->val[2][0] = 0;
    m->val[2][1] = 0;
    m->val[2][2] = FIXDIV (FIX2, (max_z - min_z));
    m->val[2][3] = - FIXDIV ((max_z + min_z) , (max_z - min_z));
    m->val[3][0] = 0;
    m->val[3][1] = 0;
    m->val[3][2] = 0;
    m->val[3][3] = FIX1;
}


/* Perspective projection matrix. */
void MakePerspectiveMatrix (Matrix44* m, int x_min, int x_max, 
			    int y_min, int y_max, int z_min, 
			    int z_max)
{
    m->val[0][0] = FIXDIV (FIXMULT(FIX2 , z_min) , (x_max - x_min));
    m->val[0][1] = 0;
    m->val[0][2] = FIXDIV ((x_max + x_min) , (x_max - x_min));
    m->val[0][3] = 0;
    m->val[1][0] = 0;
    m->val[1][1] = FIXDIV (FIXMULT(FIX2, z_min) , (y_max - y_min));
    m->val[1][2] = FIXDIV ((y_max + y_min) , (y_max - y_min));
    m->val[1][3] = 0;
    m->val[2][0] = 0;
    m->val[2][1] = 0;
    m->val[2][2] = - FIXDIV((z_max + z_min) , (z_max - z_min));
    m->val[2][3] = -FIXDIV(FIXMULT(FIX2 ,FIXMULT(z_max , z_min)) , (z_max - z_min));
    m->val[3][0] = 0;
    m->val[3][1] = 0;
    m->val[3][2] = -FIX1;
    m->val[3][3] = 0;
}

/* Transform from unit cube to screen coords */
void MakeScreenMatrix (Matrix44* m, int min_x, int max_x, int min_y, 
                      int max_y)
{
    m->val[0][0] = FIXDIV ((max_x - min_x) , FIX2);
    m->val[0][1] = 0;
    m->val[0][2] = 0;
    m->val[0][3] = min_x + FIXDIV ((max_x-min_x), FIX2);
    m->val[1][0] = 0;
    m->val[1][1] = -FIXDIV((max_y-min_y) , FIX2);
    m->val[1][2] = 0;
    m->val[1][3] = min_y + FIXDIV((max_y-min_y), FIX2);
    m->val[2][0] = 0;
    m->val[2][1] = 0;
    m->val[2][2] = FIX1;
    m->val[2][3] = 0;
    m->val[3][0] = 0;
    m->val[3][1] = 0;
    m->val[3][2] = 0;
    m->val[3][3] = FIX1;
}


void MakeTransMatrix (Matrix44* m, int x, int y, int z)
{
    m->val[0][0] = m->val[1][1] = m->val[2][2] = m->val[3][3] = FIX1;
    m->val[0][1] = m->val[0][2] = m->val[1][0] = m->val[1][2] = 0;
    m->val[2][0] = m->val[2][1] = m->val[3][0] = m->val[3][1] = m->val[3][2] = 0;
    m->val[0][3] = x;
    m->val[1][3] = y;
    m->val[2][3] = z;
}

/*
void MakeXYZEulerMatrix (Matrix44* m, 
*/
void MakeXRot (Matrix44* m, int sin_val, int cos_val)
{
    m->val[1][1] = m->val[2][2] = cos_val;
    m->val[2][1] = sin_val;
    m->val[1][2] = -sin_val;
    m->val[0][1] = m->val[0][2] = 0;
    m->val[0][3] = m->val[1][0] = 0;
    m->val[1][3] = m->val[2][0] = 0;
    m->val[2][3] = m->val[3][0] = 0;
    m->val[3][1] = m->val[3][2] = 0;
    m->val[0][0] = m->val[3][3] = FIX1;
}
void MakeYRot (Matrix44* m, int sin_val, int cos_val)
{
    m->val[0][0] = m->val[2][2] = cos_val;
    m->val[0][2] = sin_val;
    m->val[2][0] = -sin_val;
    m->val[0][1] = m->val[0][3] = 0;
    m->val[1][0] = m->val[1][2] = 0;
    m->val[1][3] = m->val[2][1] = 0;
    m->val[2][3] = m->val[3][0] = 0;
    m->val[3][1] = m->val[3][2] = 0;
    m->val[1][1] = m->val[3][3] = FIX1;
}
void MakeZRot (Matrix44* m, int sin_val, int cos_val)
{
    m->val[0][0] = m->val[1][1] = cos_val;
    m->val[1][0] = sin_val;
    m->val[0][1] = -sin_val;
    m->val[0][2] = m->val[0][3] = 0;
    m->val[1][2] = m->val[1][3] = 0;
    m->val[2][0] = m->val[2][1] = 0;
    m->val[3][0] = m->val[3][1] = 0;
    m->val[2][3] = m->val[3][2] = 0;
    m->val[2][2] = m->val[3][3] = FIX1;
}

void MakeIdentityMatrix (struct Matrix44* m)
{
    int i,j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            m->val[i][j] = (i==j)?FIX1:0;
}

#define FIXMULL(x,y) (((x)>>HALFSHIFT)*(y))

/* q = m*p */
void TransformPoint (Matrix44* m, Point4* p, Point4* q)
{
	// It turns out to be slightly faster to cache these
	// values so I don't have to compute them multiple
	// times.
	int a = p->val[0]>>8;
	int b = p->val[1]>>8;
	int c = p->val[2]>>8;
	int d = p->val[3]>>8;
    q->val[0] =  (m->val[0][0]* a) +
                 (m->val[0][1]* b) +
                (m->val[0][2]* c) +
                (m->val[0][3]* d);
    q->val[1] = (m->val[1][0]* a) +
                (m->val[1][1]* b) +
                (m->val[1][2]* c) +
                (m->val[1][3]* d);
    q->val[2] = (m->val[2][0]* a) +
                (m->val[2][1]* b) +
                (m->val[2][2]* c) +
                (m->val[2][3]* d);
    q->val[3] = (m->val[3][0]* a) +
                (m->val[3][1]* b) +
                (m->val[3][2]* c) +
                (m->val[3][3]* d);
	
//    q->val[0] = FIXDIV(q->val[0], q->val[3]);
//    q->val[1] = FIXDIV(q->val[1], q->val[3]);
//    q->val[2] = FIXDIV(q->val[2], q->val[3]);
//	q->val[3] = FIX1;
}



void TransformPoint2d (Matrix44* m, Point4* p, Point4* q)
{
	// m is already shifted to multiply, so we make shifted
	// values of p that we can just integer multiply.
	int a = p->val[0]>>8;
	int b = p->val[1]>>8;
	int c = p->val[2]>>8;
	int d = p->val[3]>>8;
    q->val[0] =  (m->val[0][0]* a) +
                 (m->val[0][1]* b) +
                 (m->val[0][2]* c) +
                 (m->val[0][3]* d);
    q->val[1] =  (m->val[1][0]* a) +
                 (m->val[1][1]* b) +
                 (m->val[1][2]* c) +
                 (m->val[1][3]* d);
	q->val[3] =  (m->val[3][0]* a) +
                 (m->val[3][1]* b) +
                 (m->val[3][2]* c) +
                 (m->val[3][3]* d);
	
/*	
	if (q->val[3] == 0)
	{
		puts ("val3 is zero!!!");
		exit(0);
	}
	*/
    q->val[0] = FIXDIV(q->val[0], q->val[3]);
    q->val[1] = FIXDIV(q->val[1], q->val[3]);
}


void SubtractVec (Point4* out, Point4* u, Point4* v)
{
//	printf ("calculating u(%f, %f, %f) - v(%f, %f, %f)\n",
//	FIXTOFLOAT (u->val[0]),FIXTOFLOAT (u->val[1]),FIXTOFLOAT (u->val[2]),
//	FIXTOFLOAT (v->val[0]),FIXTOFLOAT (v->val[1]),FIXTOFLOAT (v->val[2]));
	out->val[0] = u->val[0] - v->val[0];
	out->val[1] = u->val[1] - v->val[1];
	out->val[2] = u->val[2] - v->val[2];
	out->val[3] = FIX1;
//	printf ("result is (%f, %f, %f)\n",
//	FIXTOFLOAT (out->val[0]),FIXTOFLOAT (out->val[1]),FIXTOFLOAT (out->val[2]));
}


void CrossProduct (Point4* out, Point4* u, Point4* v)
{
	out->val[0] = FIXMULT(u->val[1],v->val[2]) - FIXMULT(u->val[2],v->val[1]);
	out->val[1] = FIXMULT(u->val[2],v->val[0]) - FIXMULT(u->val[0],v->val[2]);
	out->val[2] = FIXMULT(u->val[0],v->val[1]) - FIXMULT(u->val[1],v->val[0]);
	out->val[3] = FIX1;
}

int DotProduct (Point4* u, Point4* v)
{
	//printf ("computing dot of (%f, %f, %f) and (%f, %f, %f)\n",
	//FIXTOFLOAT (u->val[0]),FIXTOFLOAT (u->val[1]),FIXTOFLOAT (u->val[2]),
	//FIXTOFLOAT (v->val[0]),FIXTOFLOAT (v->val[1]),FIXTOFLOAT (v->val[2]));
	return FIXMULT(u->val[0],v->val[0]) + FIXMULT (u->val[1],v->val[1]) +
		FIXMULT (u->val[2],v->val[2]);
}

/* m3 = m1 * m2 */
void MultMatrix (Matrix44* m1, Matrix44* m2, Matrix44* m3)
{
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
        {
			m3->val[i][j] = FIXMULT (m1->val[i][0] , m2->val[0][j])
				+ FIXMULT (m1->val[i][1] , m2->val[1][j])
				+ FIXMULT (m1->val[i][2] , m2->val[2][j])
				+ FIXMULT (m1->val[i][3] , m2->val[3][j]);
        }
} 


void MultAffine (Matrix44* m1, Matrix44* m2, Matrix44* m3)
{
    int i;
    for (i = 0; i < 3; ++i)
	{
		m3->val[i][0] = FIXMULT (m1->val[i][0] , m2->val[0][0])
			+ FIXMULT (m1->val[i][1] , m2->val[1][0])
			+ FIXMULT (m1->val[i][2] , m2->val[2][0]);
		m3->val[i][1] = FIXMULT (m1->val[i][0] , m2->val[0][1])
			+ FIXMULT (m1->val[i][1] , m2->val[1][1])
			+ FIXMULT (m1->val[i][2] , m2->val[2][1]);
		m3->val[i][2] = FIXMULT (m1->val[i][0] , m2->val[0][2])
			+ FIXMULT (m1->val[i][1] , m2->val[1][2])
			+ FIXMULT (m1->val[i][2] , m2->val[2][2]);
		m3->val[i][3] = FIXMULT (m1->val[i][0] , m2->val[0][3])
			+ FIXMULT (m1->val[i][1] , m2->val[1][3])
			+ FIXMULT (m1->val[i][2] , m2->val[2][3]);
	}
    m3->val[0][3] += m1->val[0][3];
    m3->val[1][3] += m1->val[1][3];
    m3->val[2][3] += m1->val[2][3];


    m3->val[3][0] = m3->val[3][1] = m3->val[3][2] = 0;
    m3->val[3][3] = FIX1;
}

/* Right-shift each element of the matrix by 8 bits.  
 * This is needed before the matrix can be used for 
 * TransformPoint or TransformPoint2d
 */
void PreShiftMatrix (Matrix44* m)
{
    m->val[0][0] = m->val[0][0]>>HALFSHIFT; 
	m->val[0][1] = m->val[0][1]>>HALFSHIFT;
    m->val[0][2] = m->val[0][2]>>HALFSHIFT; 
	m->val[0][3] = m->val[0][3]>>HALFSHIFT;
    m->val[1][0] = m->val[1][0]>>HALFSHIFT; 
	m->val[1][1] = m->val[1][1]>>HALFSHIFT;
    m->val[1][2] = m->val[1][2]>>HALFSHIFT; 
	m->val[1][3] = m->val[1][3]>>HALFSHIFT;
    m->val[2][0] = m->val[2][0]>>HALFSHIFT; 
	m->val[2][1] = m->val[2][1]>>HALFSHIFT;
    m->val[2][2] = m->val[2][2]>>HALFSHIFT; 
	m->val[2][3] = m->val[2][3]>>HALFSHIFT;
    m->val[3][0] = m->val[3][0]>>HALFSHIFT; 
	m->val[3][1] = m->val[3][1]>>HALFSHIFT;
    m->val[3][2] = m->val[3][2]>>HALFSHIFT; 
	m->val[3][3] = m->val[3][3]>>HALFSHIFT;
	
}




