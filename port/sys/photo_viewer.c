/* Photo-viewer build (make -f port/Makefile.dc VIEWER=1).
 *
 * Boots straight to the photo the game never uses (asset 0x13B0, 320x240),
 * decoded at run time from the player's own ROM by the game's own decoder and
 * shown with the game's own PhotoView, exactly like the title screen sets up
 * its background (Title_Main in ovl1_attract.c), but with nothing on top.
 * Soft reset (A+B+X+Y+Start) restarts the viewer. Only compiled into the
 * viewer build: Main_Start starts this task instead of Main_GameLoop. */
#ifdef VD_PHOTO_VIEWER
#include "heap.h"
#include "object.h"
#include "photoview.h"
#include "jpeg64.h"

#define UNUSED_PHOTO_ASSET 0x13B0
#define ASSET_ROM(off) (((u32)(off) & 0xFFFFFF) + 0x611F0)

void Task_Yield(void);
extern Heap gGameHeap;
extern ObjSceneNode *gSceneGroup0;

void vd_photo_viewer_task(void)
{
    u16 *photo, *image;
    s32 i, w = 0, h = 0;
    PhotoView *view;
    Obj *bg;

    Task_Yield();
    photo = Heap_Alloc(&gGameHeap, 0x96000);
    image = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++) {
        photo[i] = 0;
        image[i] = 0;
    }
    Jpeg64_Decode(ASSET_ROM(UNUSED_PHOTO_ASSET), photo, 0, 0, &w, &h);
    Image_CopyRect(photo, w, h, 0, 0, w, h, image, w, h, 0, 0);
    view = PhotoView_New(NULL, image, w, h, 0, 0);
    bg = Obj_NewDL(gSceneGroup0, view->setup_dl, view->dl, view->finish_dl);
    Obj_Show(bg);
    for (;;) {
        Task_Yield();
    }
}
#endif
