/*
    Copyright © 2010-2017, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 1

#include "amdgpu_intern.h"

#include <proto/oop.h>
#include <proto/exec.h>
#include <aros/symbolsets.h>
#include <aros/debug.h>

/* GLOBALS */
APTR AMDGPUMemPool;
struct SignalSemaphore globalLock;

/* This pointer is necessary to limit the number of changes function signatures
   of xf86-video-nouveau codes. Without, carddata needs to be passed to each
   function, since in original codes the data it represents is taken from global
   array */
struct CardData *globalcarddataptr;
/* GLOBALS END */


static ULONG AMDGPU_Init(LIBBASETYPEPTR LIBBASE)
{
    struct OOP_ABDescr attrbases[] =
        {
            {IID_Hidd, &LIBBASE->sd.hiddAttrBase},
            {IID_Hidd_BitMap, &LIBBASE->sd.bitMapAttrBase},
            {IID_Hidd_PixFmt, &LIBBASE->sd.pixFmtAttrBase},
            {IID_Hidd_Sync, &LIBBASE->sd.syncAttrBase},
            {IID_Hidd_Gfx, &LIBBASE->sd.gfxAttrBase},
            {IID_Hidd_Gfx_AMDGPU, &LIBBASE->sd.gfxAMDGPUAttrBase},
            {IID_Hidd_PlanarBM, &LIBBASE->sd.planarAttrBase},
            //{IID_Hidd_I2C_AMDGPU, &LIBBASE->sd.i2cNouveauAttrBase},
            //{IID_Hidd_Gallium, &LIBBASE->sd.galliumAttrBase},
            {IID_Hidd_GC, &LIBBASE->sd.gcAttrBase},
            //{IID_Hidd_Compositor, &LIBBASE->sd.compositorAttrBase},
            //{IID_Hidd_BitMap_Nouveau, &LIBBASE->sd.bitMapNouveauAttrBase},
            {NULL, NULL}
        };

    D(bug("[AMDGPU] AMDGPU_Init(%p)\n", LIBBASE));

    InitSemaphore(&globalLock);

    D(bug("[AMDGPU] Semaphore initialised\n"));

    if (!OOP_ObtainAttrBases(attrbases))
        return FALSE;

    D(bug("[AMDGPU] AttrBases obtained\n"));

    LIBBASE->sd.basegc = OOP_FindClass(CLID_Hidd_GC);
    LIBBASE->sd.basebm = OOP_FindClass(CLID_Hidd_BitMap);

    LIBBASE->sd.mid_CopyMemBox16 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_CopyMemBox16);
    LIBBASE->sd.mid_CopyMemBox32 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_CopyMemBox32);
    LIBBASE->sd.mid_PutMem32Image16 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_PutMem32Image16);
    LIBBASE->sd.mid_GetMem32Image16 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_GetMem32Image16);
    LIBBASE->sd.mid_PutMemTemplate16 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_PutMemTemplate16);
    LIBBASE->sd.mid_PutMemTemplate32 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_PutMemTemplate32);
    LIBBASE->sd.mid_PutMemPattern16 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_PutMemPattern16);
    LIBBASE->sd.mid_PutMemPattern32 = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_PutMemPattern32);
    LIBBASE->sd.mid_ConvertPixels = OOP_GetMethodID((STRPTR)IID_Hidd_BitMap, moHidd_BitMap_ConvertPixels);
    LIBBASE->sd.mid_GetPixFmt = OOP_GetMethodID((STRPTR)IID_Hidd_Gfx, moHidd_Gfx_GetPixFmt);

    InitSemaphore(&LIBBASE->sd.multibitmapsemaphore);

    AMDGPUMemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR | MEMF_SEM_PROTECTED, 32 * 1024, 16 * 1024);
    D(bug("[AMDGPU] MemPool=%p\n", AMDGPUMemPool));

    globalcarddataptr = &LIBBASE->sd.carddata;

    D(bug("[AMDGPU] Init complete\n"));

    return TRUE;
}

static VOID AMDGPU_Exit(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[AMDGPU] AMDGPU_Exit(%p)\n", LIBBASE));

    if (AMDGPUMemPool)
    {
        DeletePool(AMDGPUMemPool);
        AMDGPUMemPool = NULL;
    }
}

APTR HIDDAMDGPUAlloc(ULONG size)
{
    return AllocVecPooled(AMDGPUMemPool, size);
}

VOID HIDDAMDGPUFree(APTR memory)
{
    FreeVecPooled(AMDGPUMemPool, memory);
}

ADD2INITLIB(AMDGPU_Init, 0);
ADD2EXPUNGELIB(AMDGPU_Exit, 0);

//ADD2LIBS((STRPTR) "gallium.hidd", 7, static struct Library *, GalliumHiddBase);
ADD2LIBS((STRPTR) "pci.hidd", 0, static struct Library *, PciHiddBase);
