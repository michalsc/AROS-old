/***************************************************************************/
/*                                                                         */
/*  tttags.h                                                               */
/*                                                                         */
/*    Tags for TrueType tables (specification only).                       */
/*                                                                         */
/*  Copyright 1996-2000 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used        */
/*  modified and distributed under the terms of the FreeType project       */
/*  license, LICENSE.TXT. By continuing to use, modify, or distribute      */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef TTAGS_H
#define TTAGS_H


#include <freetype.h>   /* for MAKE_TT_TAG() */


#define TTAG_cmap  FT_MAKE_TAG( 'c', 'm', 'a', 'p' )
#define TTAG_cvt   FT_MAKE_TAG( 'c', 'v', 't', ' ' )
#define TTAG_EBDT  FT_MAKE_TAG( 'E', 'B', 'D', 'T' )
#define TTAG_EBLC  FT_MAKE_TAG( 'E', 'B', 'L', 'C' )
#define TTAG_EBSC  FT_MAKE_TAG( 'E', 'B', 'S', 'C' )
#define TTAG_fpgm  FT_MAKE_TAG( 'f', 'p', 'g', 'm' )
#define TTAG_gasp  FT_MAKE_TAG( 'g', 'a', 's', 'p' )
#define TTAG_glyf  FT_MAKE_TAG( 'g', 'l', 'y', 'f' )
#define TTAG_GSUB  FT_MAKE_TAG( 'G', 'S', 'U', 'B' )
#define TTAG_hdmx  FT_MAKE_TAG( 'h', 'd', 'm', 'x' )
#define TTAG_head  FT_MAKE_TAG( 'h', 'e', 'a', 'd' )
#define TTAG_hhea  FT_MAKE_TAG( 'h', 'h', 'e', 'a' )
#define TTAG_hmtx  FT_MAKE_TAG( 'h', 'm', 't', 'x' )
#define TTAG_kern  FT_MAKE_TAG( 'k', 'e', 'r', 'n' )
#define TTAG_loca  FT_MAKE_TAG( 'l', 'o', 'c', 'a' )
#define TTAG_LTSH  FT_MAKE_TAG( 'L', 'T', 'S', 'H' )
#define TTAG_maxp  FT_MAKE_TAG( 'm', 'a', 'x', 'p' )
#define TTAG_name  FT_MAKE_TAG( 'n', 'a', 'm', 'e' )
#define TTAG_OS2   FT_MAKE_TAG( 'O', 'S', '/', '2' )
#define TTAG_OTTO  FT_MAKE_TAG( 'O', 'T', 'T', 'O' )
#define TTAG_PCLT  FT_MAKE_TAG( 'P', 'C', 'L', 'T' )
#define TTAG_post  FT_MAKE_TAG( 'p', 'o', 's', 't' )
#define TTAG_prep  FT_MAKE_TAG( 'p', 'r', 'e', 'p' )
#define TTAG_true  FT_MAKE_TAG( 't', 'r', 'u', 'e' )
#define TTAG_ttc   FT_MAKE_TAG( 't', 't', 'c', ' ' )
#define TTAG_ttcf  FT_MAKE_TAG( 't', 't', 'c', 'f' )
#define TTAG_VDMX  FT_MAKE_TAG( 'V', 'D', 'M', 'X' )
#define TTAG_vhea  FT_MAKE_TAG( 'v', 'h', 'e', 'a' )
#define TTAG_vmtx  FT_MAKE_TAG( 'v', 'm', 't', 'x' )


#endif /* TTAGS_H */


/* END */
