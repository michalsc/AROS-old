/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
    *** Automatically generated by genfunctable.awk. Do not edit ***
    Desc: Function table for Layers
    Lang: english
*/
#ifndef LIBCORE_COMPILER_H
#   include <libcore/compiler.h>
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif

#include "libdefs.h"
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(OpenLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(CloseLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(ExpungeLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(LC_BUILDNAME(ExtFuncLib),LibHeader) (void);
extern void AROS_SLIB_ENTRY(InitLayers,BASENAME) (void);
extern void AROS_SLIB_ENTRY(CreateUpfrontLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(CreateBehindLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(UpfrontLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(BehindLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(MoveLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(SizeLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(ScrollLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(BeginUpdate,BASENAME) (void);
extern void AROS_SLIB_ENTRY(EndUpdate,BASENAME) (void);
extern void AROS_SLIB_ENTRY(DeleteLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(LockLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(UnlockLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(LockLayers,BASENAME) (void);
extern void AROS_SLIB_ENTRY(UnlockLayers,BASENAME) (void);
extern void AROS_SLIB_ENTRY(LockLayerInfo,BASENAME) (void);
extern void AROS_SLIB_ENTRY(SwapBitsRastPortClipRect,BASENAME) (void);
extern void AROS_SLIB_ENTRY(WhichLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(UnlockLayerInfo,BASENAME) (void);
extern void AROS_SLIB_ENTRY(NewLayerInfo,BASENAME) (void);
extern void AROS_SLIB_ENTRY(DisposeLayerInfo,BASENAME) (void);
extern void AROS_SLIB_ENTRY(FattenLayerInfo,BASENAME) (void);
extern void AROS_SLIB_ENTRY(ThinLayerInfo,BASENAME) (void);
extern void AROS_SLIB_ENTRY(MoveLayerInFrontOf,BASENAME) (void);
extern void AROS_SLIB_ENTRY(InstallClipRegion,BASENAME) (void);
extern void AROS_SLIB_ENTRY(MoveSizeLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(CreateUpfrontHookLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(CreateBehindHookLayer,BASENAME) (void);
extern void AROS_SLIB_ENTRY(InstallLayerHook,BASENAME) (void);
extern void AROS_SLIB_ENTRY(InstallLayerInfoHook,BASENAME) (void);
extern void AROS_SLIB_ENTRY(DoHookClipRects,BASENAME) (void);
extern void AROS_SLIB_ENTRY(CreateLayerTagList,BASENAME) (void);

void *const LIBFUNCTABLE[]=
{
    AROS_SLIB_ENTRY(LC_BUILDNAME(OpenLib),LibHeader),
    AROS_SLIB_ENTRY(LC_BUILDNAME(CloseLib),LibHeader),
    AROS_SLIB_ENTRY(LC_BUILDNAME(ExpungeLib),LibHeader),
    AROS_SLIB_ENTRY(LC_BUILDNAME(ExtFuncLib),LibHeader),
    AROS_SLIB_ENTRY(InitLayers,BASENAME), /* 5 */
    AROS_SLIB_ENTRY(CreateUpfrontLayer,BASENAME), /* 6 */
    AROS_SLIB_ENTRY(CreateBehindLayer,BASENAME), /* 7 */
    AROS_SLIB_ENTRY(UpfrontLayer,BASENAME), /* 8 */
    AROS_SLIB_ENTRY(BehindLayer,BASENAME), /* 9 */
    AROS_SLIB_ENTRY(MoveLayer,BASENAME), /* 10 */
    AROS_SLIB_ENTRY(SizeLayer,BASENAME), /* 11 */
    AROS_SLIB_ENTRY(ScrollLayer,BASENAME), /* 12 */
    AROS_SLIB_ENTRY(BeginUpdate,BASENAME), /* 13 */
    AROS_SLIB_ENTRY(EndUpdate,BASENAME), /* 14 */
    AROS_SLIB_ENTRY(DeleteLayer,BASENAME), /* 15 */
    AROS_SLIB_ENTRY(LockLayer,BASENAME), /* 16 */
    AROS_SLIB_ENTRY(UnlockLayer,BASENAME), /* 17 */
    AROS_SLIB_ENTRY(LockLayers,BASENAME), /* 18 */
    AROS_SLIB_ENTRY(UnlockLayers,BASENAME), /* 19 */
    AROS_SLIB_ENTRY(LockLayerInfo,BASENAME), /* 20 */
    AROS_SLIB_ENTRY(SwapBitsRastPortClipRect,BASENAME), /* 21 */
    AROS_SLIB_ENTRY(WhichLayer,BASENAME), /* 22 */
    AROS_SLIB_ENTRY(UnlockLayerInfo,BASENAME), /* 23 */
    AROS_SLIB_ENTRY(NewLayerInfo,BASENAME), /* 24 */
    AROS_SLIB_ENTRY(DisposeLayerInfo,BASENAME), /* 25 */
    AROS_SLIB_ENTRY(FattenLayerInfo,BASENAME), /* 26 */
    AROS_SLIB_ENTRY(ThinLayerInfo,BASENAME), /* 27 */
    AROS_SLIB_ENTRY(MoveLayerInFrontOf,BASENAME), /* 28 */
    AROS_SLIB_ENTRY(InstallClipRegion,BASENAME), /* 29 */
    AROS_SLIB_ENTRY(MoveSizeLayer,BASENAME), /* 30 */
    AROS_SLIB_ENTRY(CreateUpfrontHookLayer,BASENAME), /* 31 */
    AROS_SLIB_ENTRY(CreateBehindHookLayer,BASENAME), /* 32 */
    AROS_SLIB_ENTRY(InstallLayerHook,BASENAME), /* 33 */
    AROS_SLIB_ENTRY(InstallLayerInfoHook,BASENAME), /* 34 */
    NULL, /* 35 */
    AROS_SLIB_ENTRY(DoHookClipRects,BASENAME), /* 36 */
    AROS_SLIB_ENTRY(CreateLayerTagList,BASENAME), /* 37 */
    (void *)-1L
};
