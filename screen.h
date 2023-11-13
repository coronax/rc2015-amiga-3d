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


