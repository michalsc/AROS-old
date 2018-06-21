#ifndef _AMDGPU_INTERN_H
#define _AMDGPU_INTERN_H
/*
    Copyright © 2010-2017, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/semaphores.h>
#include <hidd/gfx.h>
#include <hidd/i2c.h>

#include LC_LIBDEFS_FILE

#define CLID_Hidd_Gfx_AMDGPU "hidd.gfx.amdgpu"
#define IID_Hidd_Gfx_AMDGPU "hidd.gfx.amdgpu"

#define HiddGfxAMDGPUAttrBase __IHidd_Gfx_AMDGPU

#ifndef __OOP_NOATTRBASES__
extern OOP_AttrBase HiddGfxAMDGPUAttrBase;
#endif

extern struct SignalSemaphore globalLock;

enum
{
    aoHidd_Gfx_AMDGPU_VRAMSize, /* [G..] The amount of total VRAM in bytes */
    aoHidd_Gfx_AMDGPU_GARTSize, /* [G..] The amount of total GART in bytes */
    aoHidd_Gfx_AMDGPU_VRAMFree, /* [G..] The amount of free VRAM in bytes */
    aoHidd_Gfx_AMDGPU_GARTFree, /* [G..] The amount of free GART in bytes */

    num_Hidd_Gfx_AMDGPU_Attrs
};

#define aHidd_Gfx_AMDGPU_VRAMSize (HiddGfxAMDGPUAttrBase + aoHidd_Gfx_AMDGPU_VRAMSize)
#define aHidd_Gfx_AMDGPU_GARTSize (HiddGfxAMDGPUAttrBase + aoHidd_Gfx_AMDGPU_GARTSize)
#define aHidd_Gfx_AMDGPU_VRAMFree (HiddGfxAMDGPUAttrBase + aoHidd_Gfx_AMDGPU_VRAMFree)
#define aHidd_Gfx_AMDGPU_GARTFree (HiddGfxAMDGPUAttrBase + aoHidd_Gfx_AMDGPU_GARTFree)

#define IS_GFXAMDGPU_ATTR(attr, idx) \
    (((idx) = (attr)-HiddGfxAMDGPUAttrBase) < num_Hidd_Gfx_AMDGPU_Attrs)

struct HIDDAMDGPUData
{
    OOP_Object *compositor;
};

#define CLID_Hidd_BitMap_AMDGPU "hidd.bitmap.amdgpu"
#define IID_Hidd_BitMap_AMDGPU "hidd.bitmap.amdgpu"

#define HiddBitMapAMDGPUAttrBase __IHidd_BitMap_AMDGPU

#ifndef __OOP_NOATTRBASES__
extern OOP_AttrBase HiddBitMapAMDGPUAttrBase;
#endif

enum
{
    aoHidd_BitMap_AMDGPU_CompositorHidd, /* [I..] The compositor object that will be used by bitmap */

    num_Hidd_BitMap_AMDGPU_Attrs
};

#define aHidd_BitMap_AMDGPU_CompositorHidd (HiddBitMapAMDGPUAttrBase + aoHidd_BitMap_AMDGPU_CompositorHidd)

#define IS_BITMAPAMDGPU_ATTR(attr, idx) \
    (((idx) = (attr)-HiddBitMapAMDGPUAttrBase) < num_Hidd_BitMap_AMDGPU_Attrs)

struct HIDDAMDGPUBitMapData
{
    struct SignalSemaphore semaphore;

    ULONG height;        /* Height of bitmap in pixels */
    ULONG width;         /* Width of bitmap in pixels */
    ULONG pitch;         /* Width of single data row in bytes */
    UBYTE bytesperpixel; /* In bytes, how many bytes to store a pixel */
    UBYTE depth;         /* In bits, how many bits used to represt the color */
    BOOL displayable;    /* Can bitmap be displayed on screen */

    /* Information connected with display */
    OOP_Object *compositor; /* Compositor object used by bitmap */
    LONG xoffset;           /* Offset to bitmap point that is displayed as (0,0) on screen */
    LONG yoffset;           /* Offset to bitmap point that is displayed as (0,0) on screen */
    ULONG fbid;             /* Contains ID under which bitmap 
                                              is registered as framebuffer or 
                                              0 otherwise */
};

struct CardData
{
    /* Card controlling objects */
    BOOL IsPCIE;
};

struct staticdata
{

    OOP_Class *             gfxclass;
    OOP_Class *             basegc;
    OOP_Class *             basebm;

    OOP_AttrBase            hiddAttrBase;
    OOP_AttrBase            bitMapAttrBase;
    OOP_AttrBase            pixFmtAttrBase;
    OOP_AttrBase            syncAttrBase;
    OOP_AttrBase            gfxAttrBase;
    OOP_AttrBase            gfxAMDGPUAttrBase;
    OOP_AttrBase            planarAttrBase;
    OOP_AttrBase            gcAttrBase;

    OOP_MethodID            mid_CopyMemBox16;
    OOP_MethodID            mid_CopyMemBox32;
    OOP_MethodID            mid_PutMem32Image16;
    OOP_MethodID            mid_GetMem32Image16;
    OOP_MethodID            mid_PutMemTemplate16;
    OOP_MethodID            mid_PutMemTemplate32;
    OOP_MethodID            mid_PutMemPattern16;
    OOP_MethodID            mid_PutMemPattern32;
    OOP_MethodID            mid_ConvertPixels;
    OOP_MethodID            mid_GetPixFmt;

    struct CardData         carddata;

    struct SignalSemaphore  multibitmapsemaphore;
};

LIBBASETYPE
{
    struct Library base;
    struct staticdata sd;
};

#define METHOD(base, id, name) \
    base##__##id##__##name(OOP_Class *cl, OOP_Object *o, struct p##id##_##name *msg)

#define BASE(lib) ((LIBBASETYPEPTR)(lib))

#define SD(cl) (&BASE(cl->UserData)->sd)

#define LOCK_ENGINE                   \
    {                                 \
        ObtainSemaphore(&globalLock); \
    }
#define UNLOCK_ENGINE                  \
    {                                  \
        ReleaseSemaphore(&globalLock); \
    }

#define LOCK_BITMAP                          \
    {                                        \
        ObtainSemaphore(&bmdata->semaphore); \
    }
#define UNLOCK_BITMAP                         \
    {                                         \
        ReleaseSemaphore(&bmdata->semaphore); \
    }

#define LOCK_BITMAP_BM(bmdata)                 \
    {                                          \
        ObtainSemaphore(&(bmdata)->semaphore); \
    }
#define UNLOCK_BITMAP_BM(bmdata)                \
    {                                           \
        ReleaseSemaphore(&(bmdata)->semaphore); \
    }

#define LOCK_MULTI_BITMAP                                 \
    {                                                     \
        ObtainSemaphore(&(SD(cl))->multibitmapsemaphore); \
    }
#define UNLOCK_MULTI_BITMAP                                \
    {                                                      \
        ReleaseSemaphore(&(SD(cl))->multibitmapsemaphore); \
    }

    #if 0
#define UNMAP_BUFFER                      \
    {                                     \
        if (bmdata->bo->map)              \
            nouveau_bo_unmap(bmdata->bo); \
    }
#define UNMAP_BUFFER_BM(bmdata)             \
    {                                       \
        if ((bmdata)->bo->map)              \
            nouveau_bo_unmap((bmdata)->bo); \
    }

#define MAP_BUFFER                                       \
    {                                                    \
        if (!bmdata->bo->map)                            \
            nouveau_bo_map(bmdata->bo, NOUVEAU_BO_RDWR); \
    }
#define MAP_BUFFER_BM(bmdata)                              \
    {                                                      \
        if (!(bmdata)->bo->map)                            \
            nouveau_bo_map((bmdata)->bo, NOUVEAU_BO_RDWR); \
    }
#endif

#define IS_AMDGPU_BM_CLASS(x) ((x) == SD(cl)->bmclass)

#define writeq(val, addr) (*(volatile UQUAD *)(addr) = (val))
#define readq(addr) (*(volatile UQUAD *)(addr))
#define writel(val, addr) (*(volatile ULONG *)(addr) = (val))
#define readl(addr) (*(volatile ULONG *)(addr))
#define writew(val, addr) (*(volatile UWORD *)(addr) = (val))
#define readw(addr) (*(volatile UWORD *)(addr))
#define writeb(val, addr) (*(volatile UBYTE *)(addr) = (val))
#define readb(addr) (*(volatile UBYTE *)(addr))

/* Commom memory allocation */
APTR HIDDAMDGPUAlloc(ULONG size);
VOID HIDDAMDGPUFree(APTR memory);

int amdgpu_init();

#endif /* _AMDGPU_INTERN_H */
