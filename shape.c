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

#include <stdio.h>
#include <stdlib.h>
#include "matrix44.h"

Shape** LoadShapes (char* filename)
{
	int i, j;
	int num_shapes;
	Shape** shapes;
	float a2;
	FILE* f = fopen (filename, "r");
	if (!f)
		return NULL;

	//printf ("scan test 1\n");
	fscanf (f, "%f\n", &a2);
	//printf ("scan test 2\n");
	//printf ("val: %f\n", a2);
		
	fscanf (f, "%d\n", &num_shapes);
	printf ("reading %d shapes\n", num_shapes);
	shapes = (Shape**)malloc((num_shapes+1)*sizeof(Shape*));
	shapes[num_shapes] = NULL;
	for (i = 0; i < num_shapes; ++i)
	{
		float a,b,c;
		Shape* shape = (Shape*)malloc(sizeof(Shape));
		fscanf (f, "%s\n", shape->mName);
		printf ("reading %s\n", shape->mName);
		fscanf (f, "%hd\n", &shape->mNumPoints);
		shape->mPoints = (Point4*)malloc(shape->mNumPoints*sizeof(Point4));
		printf ("numpoints %d\n", shape->mNumPoints);
		for (j = 0; j < shape->mNumPoints; ++j)
		{
		  //printf("a\n");
	     
			fscanf (f, "%f %f %f\n", &a, &b, &c);
			//printf("b\n");
			printf ("read point %f %f %f\n", a, b, c);
			//printf("c\n");
			shape->mPoints[j].val[0] = FLOATTOFIX(a);
			shape->mPoints[j].val[1] = FLOATTOFIX(b);
			shape->mPoints[j].val[2] = FLOATTOFIX(c);
			shape->mPoints[j].val[3] = FLOATTOFIX(1.0);
			//printf("d\n");
		}
		fscanf (f, "%hd\n", &shape->mNumEdges);
		shape->mEdges = (Edge*)malloc(shape->mNumEdges*sizeof(Edge));
		for (j = 0; j < shape->mNumEdges; ++j)
		{
			fscanf (f, "%hd %hd\n", &shape->mEdges[j].p1,
			&shape->mEdges[j].p2);
			//printf ("read edge %d %d\n", &shape->mEdges[j].p1,
			//	&shape->mEdges[j].p2);
			shape->mEdges[j].drawn = 0;
		}
		fscanf (f, "%hd\n", &shape->mNumFaces);
		shape->mFaces = (Face*)malloc(shape->mNumFaces*sizeof(Face));
		printf ("num faces is %d\n",shape->mNumFaces);
		for (j = 0; j < shape->mNumFaces; ++j)
		{
			Face* face = &shape->mFaces[j];
			fscanf (f, "%d", &face->mPen);
			fscanf (f, "%hd", &face->mNumEdges);
			printf ("num edges is %d\n",face->mNumEdges);
			switch (face->mNumEdges)
			{
				case 3:
					fscanf (f, "%hd %hd %hd\n", &face->mPoints[0],
					&face->mPoints[1], &face->mPoints[2]);
					face->mPoints[3] = face->mPoints[4] = -1;

					fscanf (f, "%hd %hd %hd\n", &face->mEdges[0],
					&face->mEdges[1], &face->mEdges[2]);
					face->mEdges[3] = face->mEdges[4] = -1;
					break;
				case 4:
					fscanf (f, "%hd %hd %hd %hd\n", &face->mPoints[0],
					&face->mPoints[1], &face->mPoints[2],
					&face->mPoints[3]);
					face->mPoints[4] = -1;

					fscanf (f, "%hd %hd %hd %hd\n", &face->mEdges[0],
					&face->mEdges[1], &face->mEdges[2],
					&face->mEdges[3]);
					face->mEdges[4] = -1;
					
					//printf ("points %d %d %d %d %d\n", face->mPoints[0],
					//face->mPoints[1], face->mPoints[2], 				
					//face->mPoints[3], face->mPoints[4]);
					//printf ("edges  %d %d %d %d %d\n", face->mEdges[0],
					//face->mEdges[1], face->mEdges[2],
					//face->mEdges[3], face->mEdges[4]);
					break;
				case 5:
					fscanf (f, "%hd %hd %hd %hd %hd\n", &face->mPoints[0],
					&face->mPoints[1], &face->mPoints[2],
					&face->mPoints[3], &face->mPoints[4]);

					fscanf (f, "%hd %hd %hd %hd %hd\n", &face->mEdges[0],
					&face->mEdges[1], &face->mEdges[2],
					&face->mEdges[3], &face->mEdges[4]);
					break;
				default:
					break;
			}
		}
	
		shapes[i] = shape;
	}
	
	fclose(f);
	
	return shapes;
}



void DeleteShapes (Shape** shapes)
{
	int i;
	for (i = 0; shapes[i] != NULL; ++i)
	{
		free (shapes[i]->mPoints);
		free (shapes[i]->mEdges);
		free (shapes[i]);
	}
	free (shapes);
}
	


void PrintShape (Shape* shape)
{
	int i;
	printf ("%s\n%d\n", shape->mName, shape->mNumPoints);
	for (i = 0; i < shape->mNumPoints; ++i)
	{
		float a,b,c;
		a = FIXTOFLOAT (shape->mPoints[i].val[0]);
		b = FIXTOFLOAT (shape->mPoints[i].val[1]);
		c = FIXTOFLOAT (shape->mPoints[i].val[2]);
		printf ("%f %f %f\n", a, b, c);
	}
	printf ("%d\n", shape->mNumEdges);
	for (i = 0; i < shape->mNumEdges; ++i)
	{
		printf ("%d %d\n", shape->mEdges[i].p1, shape->mEdges[i].p2);
	}
}

	

