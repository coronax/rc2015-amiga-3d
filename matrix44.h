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


/* Layout:
 *   00  01  02  03
 *   10  11  12  13
 *   20  21  22  23
 *   30  31  32  33
 */

/*
#define FIXSHIFT 16
#define HALFSHIFT 8

#define FLOATTOFIX(x) ((int)(((float)x)*65536.0))
#define FIXTOFLOAT(x) ((float)(x) / (1<<FIXSHIFT))
*/
#define FIXSHIFT 12
#define HALFSHIFT 6

#define FLOATTOFIX(x) ((int)(((float)x)*4096.0))
#define FIXTOFLOAT(x) ((float)(x) / (1<<FIXSHIFT))

#define INTTOFIX(x) ((x)<<FIXSHIFT)
#define FIXTOINT(x) ((x)>>FIXSHIFT)

#define FIX0 0
#define FIX1 (1<<FIXSHIFT)
#define FIX2 (2<<FIXSHIFT)
#define FIX3 (3<<FIXSHIFT)
#define FIX4 (4<<FIXSHIFT)
#define FIX5 (5<<FIXSHIFT)
#define FIX6 (6<<FIXSHIFT)

#define FIXMULT(x,y) (((x)>>HALFSHIFT)*((y)>>HALFSHIFT))
#define FIXDIV(x,y) (((x)/(y>>HALFSHIFT))<<HALFSHIFT)


typedef struct Matrix44
{
    int val[4][4];
} Matrix44;


typedef struct Point4
{
    int val[4];
} Point4;


/* an edge, defined by indices of its endpoints in the points[] array */
typedef struct Edge
{
    short p1;
    short p2;
	short drawn;
} Edge;


typedef struct Face
{
	// vertex indices, in CCW winding order
	short mPoints[5];
	// edges, so we can keep track of which ones are
	// already drawn.
	short mEdges[5];
	short mNumEdges; // also number of points.  Convenient, huh?
	int mPen; // fill color / pen number
} Face;


typedef struct Shape
{
	char mName[80];
	Point4* mPoints;
	short mNumPoints;
	Edge* mEdges;
	short mNumEdges;
	Face* mFaces;
	short mNumFaces;
} Shape;


Shape** LoadShapes (char* filename);
void DeleteShapes (Shape** shapes);
void PrintShape (Shape* shape);

void PrintMatrix (Matrix44* m);

void PrintPoint (Point4* p);

void SetPoint (Point4* p, int x, int y, int z);

/* b = a. */
void CopyMatrix (Matrix44* a, Matrix44* b);

/* Orthogonal projection matrix. */
void MakeOrthoMatrix (Matrix44* m, int min_x, int max_x, int min_y, 
                      int max_y, int min_z, int max_z);

/* Perspective projection matrix. */
void MakePerspectiveMatrix (Matrix44* m, int min_x, int max_x, int min_y, 
                      int max_y, int min_z, int max_z);

void MakeScreenMatrix (Matrix44* m, int min_x, int max_x, int min_y, 
                      int max_y);

void MakeTransMatrix (Matrix44* m, int x, int y, int z);

void MakeXRot (Matrix44* m, int sin_val, int cos_val);
void MakeYRot (Matrix44* m, int sin_val, int cos_val);
void MakeZRot (Matrix44* m, int sin_val, int cos_val);

void MakeIdentityMatrix (struct Matrix44* m);

/* q = m*p */
void TransformPoint (Matrix44* m, Point4* p, Point4* q);
/* q = m*p , but only calculates q->val[0] and q->val[1] */
void TransformPoint2d (Matrix44* m, Point4* p, Point4* q);

/* out <- u - v */
void SubtractVec (Point4* out, Point4* u, Point4* v);

/* out = u cross v */
void CrossProduct (Point4* out, Point4* u, Point4* v);

/* result = u dot v */
int DotProduct (Point4* u, Point4* v);

/* m3 = m1 * m2 */
void MultMatrix (Matrix44* m1, Matrix44* m2, Matrix44* m3);

/* m3 = m1 * m2
 * This version takes some shortcuts, and only works if the bottom row of
 * m1 and m2 are both [0 0 0 1]. */
void MultAffine (Matrix44* m1, Matrix44* m2, Matrix44* m3);

/* Right-shift each element of the matrix by 8 bits.  
 * This is needed before the matrix can be used for 
 * TransformPoint or TransformPoint2d
 */
void PreShiftMatrix (Matrix44* m);


