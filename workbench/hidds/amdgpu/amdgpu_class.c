/*
    Copyright © 2010-2017, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "amdgpu_intern.h"

#include <graphics/displayinfo.h>
#include <proto/utility.h>

#define DEBUG 1
#include <aros/debug.h>
#include <proto/oop.h>

#undef HiddAttrBase
#undef HiddPixFmtAttrBase
#undef HiddGfxAttrBase
#undef HiddGfxNouveauAttrBase
#undef HiddSyncAttrBase
#undef HiddBitMapAttrBase
#undef HiddCompositorAttrBase
#undef HiddBitMapNouveauAttrBase

#define HiddAttrBase (SD(cl)->hiddAttrBase)
#define HiddPixFmtAttrBase (SD(cl)->pixFmtAttrBase)
#define HiddGfxAttrBase (SD(cl)->gfxAttrBase)
#define HiddGfxNouveauAttrBase (SD(cl)->gfxNouveauAttrBase)
#define HiddSyncAttrBase (SD(cl)->syncAttrBase)
#define HiddBitMapAttrBase (SD(cl)->bitMapAttrBase)
#define HiddCompositorAttrBase (SD(cl)->compositorAttrBase)
#define HiddBitMapNouveauAttrBase (SD(cl)->bitMapNouveauAttrBase)

#define MAX_BITMAP_WIDTH 4096
#define MAX_BITMAP_HEIGHT 4096

/* PUBLIC METHODS */


OOP_Object *METHOD(AMDGPU, Root, New)
{
//    struct amdgpu_device *dev = NULL;
//    struct amdgpu_device_priv *nvdev = NULL;
//    struct TagItem *syncs = NULL;
//    struct CardData *carddata = &(SD(cl)->carddata);
//    LONG ret;
//    ULONG selectedcrtcid;

    D(bug("[AMDGPU] Root::New()\n"));

    amdgpu_init();
    while(1);

//    if (amdgpu_init() < 0)
//        return NULL;

    LOCK_ENGINE


    UNLOCK_ENGINE

    return NULL;
}
