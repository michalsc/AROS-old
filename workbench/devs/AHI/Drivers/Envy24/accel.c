/*
    Copyright � 2004-2014, Davy Wentzler. All rights reserved.
    Copyright � 2010-2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <config.h>

#include <devices/ahi.h>
#include <libraries/ahi_sub.h>

#include "library.h"

/******************************************************************************
** AHIsub_SetVol **************************************************************
******************************************************************************/

ULONG
_AHIsub_SetVol( UWORD                   channel,
		Fixed                   volume,
		sposition               pan,
		struct AHIAudioCtrlDrv* AudioCtrl,
		ULONG                   flags,
		struct DriverBase*      AHIsubBase )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_SetFreq *************************************************************
******************************************************************************/

ULONG
_AHIsub_SetFreq( UWORD                   channel,
		 ULONG                   freq,
		 struct AHIAudioCtrlDrv* AudioCtrl,
		 ULONG                   flags,
		 struct DriverBase*      AHIsubBase )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_SetSound ************************************************************
******************************************************************************/

ULONG
_AHIsub_SetSound( UWORD                   channel,
		  UWORD                   sound,
		  ULONG                   offset,
		  LONG                    length,
		  struct AHIAudioCtrlDrv* AudioCtrl,
		  ULONG                   flags,
		  struct DriverBase*      AHIsubBase )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_SetEffect ***********************************************************
******************************************************************************/

ULONG
_AHIsub_SetEffect( APTR                    effect,
		   struct AHIAudioCtrlDrv* AudioCtrl,
		   struct DriverBase*      AHIsubBase )
{
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_LoadSound ***********************************************************
******************************************************************************/

ULONG
_AHIsub_LoadSound( UWORD                   sound,
		   ULONG                   type,
		   APTR                    info,
		   struct AHIAudioCtrlDrv* AudioCtrl,
		   struct DriverBase*      AHIsubBase )
{ 
  return AHIS_UNKNOWN;
}


/******************************************************************************
** AHIsub_UnloadSound *********************************************************
******************************************************************************/

ULONG
_AHIsub_UnloadSound( UWORD                   sound,
		     struct AHIAudioCtrlDrv* AudioCtrl,
		     struct DriverBase*      AHIsubBase )
{
  return AHIS_UNKNOWN;
}
