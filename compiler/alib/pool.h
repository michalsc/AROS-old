#ifndef _POOL_H
#define _POOL_H
/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
    Original version from libnix
    $Id$

    Desc: amiga.lib internal header file for pools
    Lang: english
*/
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <proto/alib.h>
#include <proto/exec.h>

/*     our PRIVATE! memory pool structure
   (_NOT_ compatible with original amiga.lib!) */

typedef struct Pool
{
  struct MinList PuddleList;
  struct MinList ThreshList;

  ULONG MemoryFlags;
  ULONG PuddleSize;
  ULONG ThreshSize;
} POOL;

#endif /* _POOL_H */
