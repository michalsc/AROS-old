/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: Onscreen bitmap class for linux fb device
    Lang: English.
*/

#define AROS_ALMOST_COMPATIBLE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/oop.h>
#include <proto/utility.h>
#include <oop/oop.h>

#include <hidd/graphics.h>

#include "linux_intern.h"

struct bitmap_data {
    int dummy;
};

static AttrBase HiddBitMapAttrBase = 0;

static struct ABDescr attrbases[] = {
    { IID_Hidd_BitMap,		&HiddBitMapAttrBase },
    { NULL, NULL }
};

#define DEBUG 0
#include <aros/debug.h>
 
/*********** BitMap::New() *************************************/

static Object *bitmap_new(Class *cl, Object *o, struct pRoot_New *msg)
{
    BOOL ok = TRUE;
// kill(getpid(), 19);
    o = (Object *)DoSuperMethod(cl, o, (Msg) msg);
kprintf("LINUXFB: GOT OBJ %p\n", o);
    if (NULL != o) {
    	struct bitmap_data *data;
	
	data = INST_DATA(cl, o);
	
	if (!ok) {
	    ULONG dispose_mid;
	    dispose_mid = GetMethodID(IID_Root, moRoot_Dispose);
	
	    CoerceMethod(cl, o, (Msg)&dispose_mid);
	    o = NULL;
    	}
    }
    
    return o;
}

/*********  BitMap::ObtainDirectAccess()  *************************************/
static BOOL bitmap_obtaindirectaccess(Class *cl, Object *o, struct pHidd_BitMap_ObtainDirectAccess *msg)
{
    ULONG width, height;
    
    /* Get width & height from bitmap object */
  
    GetAttr(o, aHidd_BitMap_Width,  &width);
    GetAttr(o, aHidd_BitMap_Height, &height);
    
    *msg->addressReturn	= LSD(cl)->baseaddr;
    *msg->widthReturn	= LSD(cl)->vsi.xres_virtual;
    *msg->heightReturn	= LSD(cl)->vsi.yres_virtual;
    *msg->bankSizeReturn = *msg->memSizeReturn = LSD(cl)->fsi.smem_len;
    
    return TRUE;
}

static VOID bitmap_releasedirectaccess(Class *cl, Object *o, struct pHidd_BitMap_ReleaseDirectAccess *msg)
{
     /* Do nothing */
#warning Here we can use mprotect() to detect accesses while no access is granted
     return;
}

/*********** BitMap::PutPixel() ***********************************************/
static VOID bitmap_putpixel(Class *cl, Object *o, struct pHidd_BitMap_PutPixel *msg)
{
    UBYTE *addr;
    LONG xoffset, yoffset;
    HIDDT_Pixel pix;
    
    addr = LSD(cl)->baseaddr;

// kprintf("addr: %p\n", addr);
    /* Find the addr of the pixel */
    xoffset = msg->x * LSD(cl)->pf.bytes_per_pixel;
    yoffset = msg->y * LSD(cl)->fsi.line_length;
// kprintf("xoffset: %d, yoffset: %d\n", xoffset, yoffset);
    
    addr +=  xoffset + yoffset;
    
// kprintf("addr: %p\n", addr);
    
    
    /* Set pixel according to pixelformat */
    pix = msg->pixel;
    
#if 0
    *addr ++ = (pix >> 16) & 0x000000FF;
    *addr ++ = (pix >> 8 ) & 0x000000FF;
    *addr ++ = pix & 0x000000FF;
#else
    *addr ++ = pix & 0x000000FF;
    *addr ++ = (pix >> 8 ) & 0x000000FF;
    *addr ++ = (pix >> 16) & 0x000000FF;
#endif
    return;
}

/*********** BitMap::GetPixel() ***********************************************/
static HIDDT_Pixel  bitmap_getpixel(Class *cl, Object *o, struct pHidd_BitMap_GetPixel *msg)
{
    UBYTE *addr;
    HIDDT_Pixel pix = 0;
    
    addr = LSD(cl)->baseaddr;
    
    /* Find the addr of the pixel */
    addr += msg->y * LSD(cl)->fsi.line_length + msg->x * LSD(cl)->pf.bytes_per_pixel;
    
    /* Set pixel according to pixelformat */

#if 0    
    pix = (addr[0] << 16) | (addr[1] << 8) | (addr[2]);
#else
    pix = (addr[2] | (addr[1] << 8) | (addr[0] << 16));
#endif
/*    *addr ++ = (pix >> 16) & 0x000000FF;
    *addr ++ = (pix >> 8 ) & 0x000000FF;
    *addr ++ = pix & 0x000000FF;
*/    
    return pix;
}

/*** init_onbmclass *********************************************************/

#undef LSD
#define LSD(cl) fsd

#define NUM_ROOT_METHODS	1
#define NUM_BITMAP_METHODS	4

Class *init_bmclass(struct linux_staticdata *fsd)
{
    struct MethodDescr root_descr[NUM_ROOT_METHODS + 1] = {
        {(IPTR (*)())bitmap_new    , moRoot_New    },
        {NULL, 0UL}
    };

    struct MethodDescr bitMap_descr[NUM_BITMAP_METHODS + 1] = {
        {(IPTR (*)())bitmap_obtaindirectaccess,		moHidd_BitMap_ObtainDirectAccess	},
        {(IPTR (*)())bitmap_releasedirectaccess,	moHidd_BitMap_ReleaseDirectAccess	},
        {(IPTR (*)())bitmap_putpixel,			moHidd_BitMap_PutPixel			},
        {(IPTR (*)())bitmap_getpixel,			moHidd_BitMap_GetPixel			},
	
        {NULL, 0UL}
    };
    
    struct InterfaceDescr ifdescr[] =  {
        {root_descr,    IID_Root       , NUM_ROOT_METHODS},
        {bitMap_descr,  IID_Hidd_BitMap, NUM_BITMAP_METHODS},
        {NULL, NULL, 0}
    };

    AttrBase MetaAttrBase = ObtainAttrBase(IID_Meta);

    struct TagItem tags[] = {
        {aMeta_SuperID,        (IPTR)CLID_Hidd_BitMap		},
        {aMeta_InterfaceDescr, (IPTR)ifdescr			},
        {aMeta_InstSize,       (IPTR)sizeof(struct bitmap_data)	},
        {TAG_DONE, 0UL}
    };
    
    Class *cl = NULL;

    if(0 != MetaAttrBase) {
        cl = NewObject(NULL, CLID_HiddMeta, tags);
	if(NULL != cl) {
            cl->UserData     = (APTR) fsd;
           
            /* Get attrbase for the BitMap interface */
	    if (ObtainAttrBases(attrbases)) {
	    	fsd->bmclass = cl;
                AddClass(cl);
            } else {
#warning The failure handlilg code is buggy. How do we know if the class was successfully added before removing it in free_onbcmlass ?
                free_bmclass( fsd );
                cl = NULL;
            }
        }
	
	/* We don't need this anymore */
	ReleaseAttrBase(IID_Meta);
    } /* if(MetaAttrBase) */
    
    return cl;
}


/*** free_bitmapclass *********************************************************/

void free_bmclass(struct linux_staticdata *fsd)
{

    if(NULL != fsd)  {
    
        if(NULL != fsd->bmclass) {
	    RemoveClass(fsd->bmclass);
	    DisposeObject((Object *) fsd->bmclass);
            fsd->bmclass = NULL;
	}
	
	ReleaseAttrBases(attrbases);
	
    }
    
    return;

}
