
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <hardware/custom.h> 
#include <hardware/intbits.h>
#include <clib/timer_protos.h>
#include "matrix44.h"
#include "screen.h"

extern struct GfxBase* GfxBase;

extern struct Custom custom;
//struct Custom* custom = (struct Custom*)0x00DFF000;


const int screen_width = 320;
const int screen_height = 200;


BOOL OpenLibraries();
void CloseLibraries();
BOOL OpenTimer();
void CloseTimer();
BOOL OpenMyScreen();
void DestroyMyScreen();

void CramCopperList (struct MyScreen* s);
void UpdateCopperList (struct MyScreen* s, UWORD* ptr1, UWORD scroll1, UWORD* ptr2, UWORD scroll2);

void PrintCopperList (struct cprlist* cpr)
{
	int i;
	if (cpr == NULL)
	{
		printf ("[empty list]\n");
	}
	else
	{
		for (i = 0; i < cpr->MaxCount; ++i)
		{
			printf ("%#6hx, %#6hx,\n", cpr->start[i*2], cpr->start[i*2+1]);
		}
	}
}



#define MAX_POINTS 64

UWORD rasterpos; // bits of scroll
int bytescroll = 0;
UWORD* bytepos; // scroll location
UWORD* bytepos_base;
struct Interrupt copper_int;
UWORD rasterpos2;
UWORD* bytepos2;
UWORD* bytepos2_base;
int bytescroll2 = 0;

Point4 translated_points[MAX_POINTS];
Point4 onscreen_points[MAX_POINTS];

Matrix44 projection_matrix;

void ComputeNormal (Face* face, Point4* normal)
{
	Point4 vec1, vec2;
	SubtractVec (&vec1, &translated_points[face->mPoints[1]],
		&translated_points[face->mPoints[0]]);
	SubtractVec (&vec2, &translated_points[face->mPoints[2]],
		&translated_points[face->mPoints[1]]);
	CrossProduct (normal, &vec1, &vec2);
}

struct EClockVal edge_time_1, edge_time_2;
//ULONG E_Freq;
ULONG edge_elapsed;


void RenderShapeFaces (struct RastPort* rp, Shape* shape, Matrix44* view_xform, Matrix44* screen_xform)
{
    int i, j;
	Point4 camera_vec, camera_vec_xform, origin; // for calculating facing
	Face* face;
	Point4 normal;
	int dot;
	Point4 *p;
	
	SetPoint (&origin, FIX0, FIX0, FIX0);

    for (i = 0; i < shape->mNumPoints; ++i)
    {
        TransformPoint (view_xform, &shape->mPoints[i], &translated_points[i]);
		TransformPoint2d (screen_xform, &translated_points[i], &onscreen_points[i]);
		//onscreen_points[i].val[0] = FIXTOINT(onscreen_points[i].val[0]);
		//onscreen_points[i].val[1] = FIXTOINT(onscreen_points[i].val[1]);
    }
	/*
    for (i = 0; i < shape->mNumEdges; ++i)
    {
		shape->mEdges[i].drawn = 0;
	}
	*/
	for (i = 0; i < shape->mNumFaces; ++i)
	{
		face = &shape->mFaces[i];

		// OK, we need to figure out the normal vector of 
		// this face in order to decide if we want to draw
		// it or not.
		ComputeNormal (face, &normal);
		//printf ("face %d normal is %f, %f, %f\n", i, FIXTOFLOAT(normal.val[0]), FIXTOFLOAT(normal.val[1]), FIXTOFLOAT(normal.val[2]));
		//TransformPoint (&projection_matrix, &origin, &camera_vec);
		// camera_vec_xform should be the camera to vertex of face vector.
		// (at least for perspective mode)
		SubtractVec (&camera_vec_xform, &translated_points[face->mPoints[0]], &origin);
		
		//printf ("camera vec is %f, %f, %f\n", i, FIXTOFLOAT(camera_vec_xform.val[0]), FIXTOFLOAT(camera_vec_xform.val[1]), FIXTOFLOAT(camera_vec_xform.val[2]));
	
		dot = DotProduct (&camera_vec_xform, &normal);
		//printf ("dot is %f\n", FIXTOFLOAT(dot));
		if (dot > 0)
			continue;
	
		SetAPen (rp, face->mPen);

		// solid
		p = &(onscreen_points[face->mPoints[0]]);
		AreaMove (rp, FIXTOINT(p->val[0]), FIXTOINT(p->val[1]));
		//AreaMove (rp, p->val[0], p->val[1]);
		
		for (j = 1; j < face->mNumEdges; ++j)
		{
			p = &(onscreen_points[face->mPoints[j]]);
			AreaDraw (rp, FIXTOINT(p->val[0]), FIXTOINT(p->val[1]));
			//AreaDraw (rp, p->val[0], p->val[1]);
		}
		
		AreaEnd (rp);
		
		// line
#if 0
		for (j = 0; j < face->mNumEdges; ++j)
		{
			Edge* edge = &shape->mEdges[face->mEdges[j]];
			if (edge->drawn == 0)
			{
				Point4* p = &(onscreen_points[edge->p1]);
				Point4* p2 = &(onscreen_points[edge->p2]);
        //ReadEClock(&edge_time_1);
				//DrawLine (rp->BitMap, 0, FIXTOINT(p->val[0]), FIXTOINT(p->val[1]), FIXTOINT(p2->val[0]), FIXTOINT(p2->val[1]));
				Move (rp, FIXTOINT(p->val[0]), FIXTOINT(p->val[1]));
				Draw (rp, FIXTOINT(p2->val[0]), FIXTOINT(p2->val[1]));
        //ReadEClock(&edge_time_2);
        //edge_elapsed += (edge_time_2.ev_lo - edge_time_1.ev_lo);


				edge->drawn = 1;
			}
		}
#endif
	}

}


struct MyScreen view[2] = {
	{0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
	{0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}};

/* Open a double-buffered low res 32-color screen. */
BOOL OpenTheScreens ()
{
    BOOL success = TRUE;
    //ULONG mode = DEFAULT_MONITOR_ID | LORESDPF_KEY;
    ULONG mode = DEFAULT_MONITOR_ID | LORES_KEY;
    InitMyScreen (&view[0], mode, screen_width, screen_height, 3);
    InitMyScreen (&view[1], mode, screen_width, screen_height, 3);
    success = success && OpenMyScreen (&view[0]);
    success = success && OpenMyScreen (&view[1]);

    return success;
}

void CloseTheScreens ()
{
	DestroyMyScreen (&view[0]);
	DestroyMyScreen (&view[1]);
}


void ShowScreen (struct MyScreen* s)
{
    LoadView (s->View);
	// since we start out doing geometry calculations, maybe we 
	// don't have to wait for the next viewport to be ready.
    WaitBOVP (s->View->ViewPort);
}



/*
BOOL DoIDCMP ()
{
    struct IntuiMessage* m;
    ULONG class;
    if (m = (struct IntuiMessage*)GetMsg (window->UserPort))
    {
        class = m->Class;
        ReplyMsg ((struct Message*)m);

        switch (class)
        {
            case IDCMP_CLOSEWINDOW:
                return FALSE;
            default:
                ;
        }
    }
    return TRUE;
}
*/



int main (int argc, char** argv)
{
	Shape** shapes;
    struct View* old_view = NULL;
    BOOL success = TRUE;
//    Matrix44 projection_matrix;
    Matrix44 screen_matrix;
    Matrix44 rotation_matrix;
    Matrix44 temp_matrix;
    Matrix44 transform_matrix;
    Matrix44 rot2_matrix, rot3_matrix;
    Matrix44 temp2_matrix, temp3_matrix;
    Matrix44 camera_matrix, cameratrans_matrix, sp_matrix;
	//Matrix44 trans_matrix;
	int y_pos;
	int y_delta;
	int y_max;
	int y_min;
	int num_shapes = 0;
	Shape* shape;

    // let's check a few things first
    printf ("sizeof int is %d\n", sizeof(int));
    printf ("sizeof long is %d\n", sizeof(long));
	printf ("sizeof short is %d\n", sizeof(short));
	printf ("sizeof float is %d\n", sizeof(float));
	
	y_pos = 0;
	y_max = FLOATTOFIX(3.4);
	y_min = FLOATTOFIX(-2.75);
	y_delta = FLOATTOFIX(0.27);
	
	shapes = LoadShapes("shapes.txt");
	if (!shapes)
	{
		printf ("Error loading shapes file\n");
		exit(0);
	}
	for (num_shapes = 0; shapes[num_shapes] != NULL; num_shapes++)
		;
	printf ("shapes count is %d\n", num_shapes);
	//PrintShape (shapes[0]);
	//exit(0);
	
    //MakeOrthoMatrix (&projection_matrix, -INTTOFIX(5), INTTOFIX(5), -INTTOFIX(5), INTTOFIX(5), -INTTOFIX(5), INTTOFIX(5));
    MakePerspectiveMatrix (&projection_matrix, -INTTOFIX(5), INTTOFIX(5), -INTTOFIX(5), INTTOFIX(5), INTTOFIX(10), INTTOFIX(50));
    MakeScreenMatrix (&screen_matrix, INTTOFIX(80), INTTOFIX(240), INTTOFIX(25), INTTOFIX(173));
    MultMatrix (&screen_matrix, &projection_matrix, &temp_matrix);
    MakeTransMatrix (&cameratrans_matrix, 0, 0, INTTOFIX(-30));
    MultMatrix (&temp_matrix, &cameratrans_matrix, &camera_matrix);
	
	MultMatrix (&projection_matrix, &cameratrans_matrix, &camera_matrix);

	// ok let's try this again. transform matrix will be camera * t
	// so I just need camera_matrix to be cameratrans
	// and projection is screen * projection sp
	MakeTransMatrix (&camera_matrix, 0, 0, INTTOFIX(-30));
	MultMatrix (&screen_matrix, &projection_matrix, &sp_matrix);
	PreShiftMatrix(&sp_matrix);

	printf ("Built initial matrices\n");
	
    success = success && OpenLibraries();
    success = success && OpenTimer();

    old_view = ((struct GfxBase*)GfxBase)->ActiView;
/*
    puts ("Perspective matrix:\n");
    PrintMatrix(&projection_matrix);
    puts ("Screen matrix:\n");
    PrintMatrix(&screen_matrix);
    puts ("camera pos matrix:\n");
    PrintMatrix(&cameratrans_matrix);
    puts ("view matrix:\n");
    PrintMatrix (&camera_matrix);
*/

	//success = 0;
	//goto shutdown;
    success = success && OpenTheScreens();
	
	CramCopperList (&view[0]);
	CramCopperList (&view[1]);
	/*
	printf ("view 0 LOF copper:\n");
	PrintCopperList (view[0].View->LOFCprList);
	printf ("view 0 SHF copper:\n");
	PrintCopperList (view[0].View->SHFCprList);

	printf ("view 1 LOF copper:\n");
	PrintCopperList (view[1].View->LOFCprList);
	printf ("view 1 SHF copper:\n");
	PrintCopperList (view[1].View->SHFCprList);
	*/
	custom.ddfstrt = 0x0030;
	custom.bpl1mod = 36;
	custom.bpl2mod = 36;
	bytepos_base = (UWORD*)view[0].RasInfo1->BitMap->Planes[1];
	bytepos_base += 95*view[0].RasInfo1->BitMap->BytesPerRow/2;
	bytepos2_base = (UWORD*)view[0].RasInfo1->BitMap->Planes[1];
	bytepos2_base += 134*view[0].RasInfo1->BitMap->BytesPerRow/2;
	bytescroll = 0;
	bytepos = bytepos_base + bytescroll;
	bytepos2 = bytepos2_base;
	
	UpdateCopperList (&view[0], bytepos, rasterpos, bytepos2, rasterpos2);
	UpdateCopperList (&view[1], bytepos, rasterpos, bytepos2, rasterpos2);

	
    if (success)
    {
        struct EClockVal *time1, *time2;
        ULONG E_Freq;
        double elapsed;
        int i,j,k;
        int* sin_table;
        int* cos_table;


        //float theta = 0.0;
		int current_buffer = 0;
        int next_buffer = 1;
        struct RastPort* rp;

		time1 = (struct EClockVal*)malloc(sizeof(struct EClockVal)*num_shapes);
		time2 = (struct EClockVal*)malloc(sizeof(struct EClockVal)*num_shapes);
		
        sin_table = (int*)malloc(sizeof(int)*126);
        cos_table = (int*)malloc(sizeof(int)*126);
        for (i = 0; i < 126; ++i)
        {
            sin_table[i] = FLOATTOFIX(sin(i*0.05));
            cos_table[i] = FLOATTOFIX(cos(i*0.05));
        }


		ShowScreen (&view[0]);
		WaitBOVP (view[0].ViewPort);
		
	//	view[0].RasInfo2->RxOffset = 1;

		for (k = 0; shapes[k] != NULL; ++k)
		{
			E_Freq = ReadEClock(&time1[k]);

			shape = shapes[k];
			
			y_pos = 0;
 
			for (i = 0; i < 2*126; ++i)
			{
				rp = view[next_buffer].RastPort;
				j = i%126;
				

				ClearPlane (view[next_buffer].RasInfo1->BitMap, 0);
				ClearPlane (view[next_buffer].RasInfo1->BitMap, 2);

//				int_handler();
/*				
				view[next_buffer].RasInfo2->RxOffset += 2;
				if (view[next_buffer].RasInfo2->RxOffset == 321)
					view[next_buffer].RasInfo2->RxOffset = 1;
				else if (view[next_buffer].RasInfo2->RxOffset == 320)
					view[next_buffer].RasInfo2->RxOffset = 0;
				ScrollVPort (view[next_buffer].ViewPort);
	*/		
				y_pos += y_delta;
				if (y_pos > y_max)
				{
					y_pos = y_max;
					y_delta = -y_delta;
				}
				else if (y_pos < y_min)
				{
					y_pos = y_min;
					y_delta = -y_delta;
				}
				
				//MakeZRot (&rotation_matrix, sin_table[j], cos_table[j]);
            	MakeXRot (&rot2_matrix, sin_table[j], cos_table[j]);
            	MakeYRot (&rot3_matrix, sin_table[j], cos_table[j]);
				//MakeTransMatrix (&trans_matrix, INTTOFIX(15), y_pos, 0);

            	//MultAffine (&rot2_matrix, &rot3_matrix, &temp2_matrix);
 				//MultAffine (&rotation_matrix, &temp2_matrix, &temp3_matrix);
				
				
				
//		MultAffine (&rot2_matrix, &trans_matrix, &temp2_matrix);
//				MultAffine (&temp2_matrix, &rot2_matrix, &temp3_matrix); 
				rot2_matrix.val[0][3] = INTTOFIX(15);
				rot2_matrix.val[1][3] = y_pos;

				// so this is transform_matrix = camera_matrix * rot3_matrix * rot2_matrix
				
				MultAffine (&rot3_matrix, &rot2_matrix, &temp3_matrix);
            	MultMatrix (&camera_matrix, &temp3_matrix, &transform_matrix);

				PreShiftMatrix	(&transform_matrix);
				// these are in here just as a slight delay, otherwise we're 
				// occasionally clearplaneing before we've actually hit vblank
				// and swapped buffers - because i'm not waiting ;)
				//for (l = 0; l < 18; ++l)
				//	SetAPen(rp,l);

				
				RenderShapeFaces (rp, shape, &transform_matrix, &sp_matrix);
				
				//DrawTest (rp->BitMap, 0);

				WaitBlit();
				
				UpdateCopperList (&view[next_buffer], bytepos, rasterpos, bytepos2, rasterpos2);

				ShowScreen (&view[next_buffer]);
								
				rasterpos = (rasterpos-16)&0x00f0;
				//printf ("rasterpos is %d ", (int)rasterpos);
				if (rasterpos == 256-16)
				{
					bytescroll = (bytescroll+1)%20;
					bytepos = bytepos_base + bytescroll;
				}

				rasterpos2 = (rasterpos2-32)&0x00f0;
				//printf ("rasterpos is %d ", (int)rasterpos);
				if (rasterpos2 == 256-32)
				{
					bytescroll2 = (bytescroll2+1)%20;
					bytepos2 = bytepos2_base + bytescroll2;
				}

				current_buffer = next_buffer;
				next_buffer = next_buffer?0:1;
           	 	//if (!DoIDCMP())
            	//    break;
				//for (l = 0; l < 65536; ++l)
				//	;
			}
			E_Freq = ReadEClock(&time2[k]);

		}


		
		LoadView (old_view);
		WaitTOF();

		for (k = 0; k < num_shapes; ++k)
		{
			elapsed = (time2[k].ev_lo - time1[k].ev_lo) / (double)E_Freq;
			printf ("Shape %d: Elapsed time: %f; framerate: %f\n", k, elapsed, (2*126.0)/elapsed);
		}
		
		elapsed = edge_elapsed / (double)E_Freq;
		//printf ("Edge drawing time: %f\n", elapsed);
		
        printf ("ticks per second is %d\n", E_Freq);
		
		free (sin_table);
		free (cos_table);
	}
	
	CloseTheScreens();
shutdown:

	DeleteShapes (shapes);

    CloseTimer();
    CloseLibraries();

    return 0;
}
