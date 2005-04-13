
/* This file was generated by idltool 51.5. Do not edit */

#ifndef EXEC_TYPES_H
#include <exec/types.h>

#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif
#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif
#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef INTUITION_GADGETCLASS_H
#include <intuition/gadgetclass.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
/*
#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif
*/

extern ULONG                VARARGS68K _ZuneMaster_Obtain(struct ZuneMasterIFace *);
extern ULONG                VARARGS68K _ZuneMaster_Release(struct ZuneMasterIFace *);
extern Object *             VARARGS68K _ZuneMaster_MUI_NewObjectA(struct ZuneMasterIFace *, CONST_STRPTR classname, struct TagItem * tags);
extern Object *             VARARGS68K _ZuneMaster_MUI_NewObject(struct ZuneMasterIFace *, CONST_STRPTR classname, ...) __stackparm;
extern VOID                 VARARGS68K _ZuneMaster_MUI_DisposeObject(struct ZuneMasterIFace *, Object * obj);
extern LONG                 VARARGS68K _ZuneMaster_MUI_RequestA(struct ZuneMasterIFace *, APTR app, APTR win, ULONG flags, CONST_STRPTR title, CONST_STRPTR gadgets, CONST_STRPTR format, APTR params);
extern LONG                 VARARGS68K _ZuneMaster_MUI_Request(struct ZuneMasterIFace *, APTR app, APTR win, ULONG flags, CONST_STRPTR title, CONST_STRPTR gadgets, CONST_STRPTR format, ...) __stackparm;
extern APTR                 VARARGS68K _ZuneMaster_MUI_AllocAslRequest(struct ZuneMasterIFace *, ULONG reqType, struct TagItem * tagList);
extern APTR                 VARARGS68K _ZuneMaster_MUI_AllocAslRequestTags(struct ZuneMasterIFace *, ULONG reqType, ...) __stackparm;
extern BOOL                 VARARGS68K _ZuneMaster_MUI_AslRequest(struct ZuneMasterIFace *, APTR requester, struct TagItem * tagList);
extern BOOL                 VARARGS68K _ZuneMaster_MUI_AslRequestTags(struct ZuneMasterIFace *, APTR requester, ...) __stackparm;
extern VOID                 VARARGS68K _ZuneMaster_MUI_FreeAslRequest(struct ZuneMasterIFace *, APTR requester);
extern LONG                 VARARGS68K _ZuneMaster_MUI_Error(struct ZuneMasterIFace *);
extern LONG                 VARARGS68K _ZuneMaster_MUI_SetError(struct ZuneMasterIFace *, LONG num);
extern struct IClass *      VARARGS68K _ZuneMaster_MUI_GetClass(struct ZuneMasterIFace *, CONST_STRPTR classname);
extern VOID                 VARARGS68K _ZuneMaster_MUI_FreeClass(struct ZuneMasterIFace *, struct IClass * classptr);
extern VOID                 VARARGS68K _ZuneMaster_MUI_RequestIDCMP(struct ZuneMasterIFace *, Object * obj, ULONG flags);
extern VOID                 VARARGS68K _ZuneMaster_MUI_RejectIDCMP(struct ZuneMasterIFace *, Object * obj, ULONG flags);
extern VOID                 VARARGS68K _ZuneMaster_MUI_Redraw(struct ZuneMasterIFace *, Object * obj, ULONG flags);
extern struct MUI_CustomClass * VARARGS68K _ZuneMaster_MUI_CreateCustomClass(struct ZuneMasterIFace *, struct Library * base, CONST_STRPTR supername, struct MUI_CustomClass * supermcc, LONG datasize, APTR dispatcher);
extern BOOL                 VARARGS68K _ZuneMaster_MUI_DeleteCustomClass(struct ZuneMasterIFace *, struct MUI_CustomClass * mcc);
extern Object *             VARARGS68K _ZuneMaster_MUI_MakeObjectA(struct ZuneMasterIFace *, LONG type, ULONG * params);
extern Object *             VARARGS68K _ZuneMaster_MUI_MakeObject(struct ZuneMasterIFace *, LONG type, ...) __stackparm;
extern BOOL                 VARARGS68K _ZuneMaster_MUI_Layout(struct ZuneMasterIFace *, Object * obj, LONG left, LONG top, LONG width, LONG height, ULONG flags);
extern LONG                 VARARGS68K _ZuneMaster_MUI_ObtainPen(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, struct MUI_PenSpec * spec, ULONG flags);
extern VOID                 VARARGS68K _ZuneMaster_MUI_ReleasePen(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, LONG pen);
extern APTR                 VARARGS68K _ZuneMaster_MUI_AddClipping(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, WORD left, WORD top, WORD width, WORD height);
extern VOID                 VARARGS68K _ZuneMaster_MUI_RemoveClipping(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, APTR handle);
extern APTR                 VARARGS68K _ZuneMaster_MUI_AddClipRegion(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, struct Region * r);
extern VOID                 VARARGS68K _ZuneMaster_MUI_RemoveClipRegion(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, APTR handle);
extern BOOL                 VARARGS68K _ZuneMaster_MUI_BeginRefresh(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, ULONG flags);
extern VOID                 VARARGS68K _ZuneMaster_MUI_EndRefresh(struct ZuneMasterIFace *, struct MUI_RenderInfo * mri, ULONG flags);


static void *main_vectors[] = {
	(void *)_ZuneMaster_Obtain,
	(void *)_ZuneMaster_Release,
	(void *)NULL,
	(void *)NULL,
	(void *)_ZuneMaster_MUI_NewObjectA,
	(void *)_ZuneMaster_MUI_NewObject,
	(void *)_ZuneMaster_MUI_DisposeObject,
	(void *)_ZuneMaster_MUI_RequestA,
	(void *)_ZuneMaster_MUI_Request,
	(void *)_ZuneMaster_MUI_AllocAslRequest,
	(void *)_ZuneMaster_MUI_AllocAslRequestTags,
	(void *)_ZuneMaster_MUI_AslRequest,
	(void *)_ZuneMaster_MUI_AslRequestTags,
	(void *)_ZuneMaster_MUI_FreeAslRequest,
	(void *)_ZuneMaster_MUI_Error,
	(void *)_ZuneMaster_MUI_SetError,
	(void *)_ZuneMaster_MUI_GetClass,
	(void *)_ZuneMaster_MUI_FreeClass,
	(void *)_ZuneMaster_MUI_RequestIDCMP,
	(void *)_ZuneMaster_MUI_RejectIDCMP,
	(void *)_ZuneMaster_MUI_Redraw,
	(void *)_ZuneMaster_MUI_CreateCustomClass,
	(void *)_ZuneMaster_MUI_DeleteCustomClass,
	(void *)_ZuneMaster_MUI_MakeObjectA,
	(void *)_ZuneMaster_MUI_MakeObject,
	(void *)_ZuneMaster_MUI_Layout,
	(void *)NULL,
	(void *)NULL,
	(void *)NULL,
	(void *)NULL,
	(void *)_ZuneMaster_MUI_ObtainPen,
	(void *)_ZuneMaster_MUI_ReleasePen,
	(void *)_ZuneMaster_MUI_AddClipping,
	(void *)_ZuneMaster_MUI_RemoveClipping,
	(void *)_ZuneMaster_MUI_AddClipRegion,
	(void *)_ZuneMaster_MUI_RemoveClipRegion,
	(void *)_ZuneMaster_MUI_BeginRefresh,
	(void *)_ZuneMaster_MUI_EndRefresh,
	(void *)-1
};

