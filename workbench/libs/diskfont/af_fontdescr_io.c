/*
    Copyright (C) 1995-2001 AROS - The Amiga Research OS
    $Id$

    Desc: Functions for readin .font files
    Lang: English.
*/

/****************************************************************************************/

#include <proto/arossupport.h>
#include <proto/dos.h>
#include <string.h>
#include "diskfont_intern.h"


#ifndef TURN_OFF_DEBUG
#define DEBUG 0
#endif

#include <aros/debug.h>

/****************************************************************************************/

/******************/
/* ReadFontDescr  */
/******************/

/****************************************************************************************/

struct FontDescrHeader *ReadFontDescr(STRPTR filename, struct DiskfontBase_intern *DiskfontBase)
{
    UWORD aword,
          numentries,
          numtags;
          
    struct FontDescrHeader *fdh = 0;
    
    struct TTextAttr *tattr;

    UBYTE strbuf[MAXFONTNAME];

    BPTR fh;
    
    D(bug("ReadFontDescr(filename=%s\n", filename));
    
    /* Allocate the FontDescrHeader */
    if (!(fh = Open(filename, MODE_OLDFILE)))
    	goto failure;

    if (!(fdh = AllocMem(sizeof (struct FontDescrHeader), MEMF_ANY|MEMF_CLEAR)) )
        goto failure;


    /* First read the file id (FCH_ID or TFCH_ID) */
    if (!ReadWord( &DFB(DiskfontBase)->dsh, &aword, fh ))
        goto failure;
    
    /* Check that this is a .font file */
    
    if ( (aword != FCH_ID) && (aword != TFCH_ID) )
        goto failure;
        
    fdh->Tagged = (aword == TFCH_ID);
    
     /* Read number of (T)FontContents structs in the file */
    if (!ReadWord( &DFB(DiskfontBase)->dsh, &numentries ,fh ))
        goto failure;
    
    fdh->NumEntries = numentries;
        
    /* Allocate mem for array of TTextAttrs */
    fdh->TAttrArray = AllocVec(fdh->NumEntries * sizeof(struct TTextAttr), MEMF_ANY|MEMF_CLEAR);
    if (!fdh->TAttrArray)
        goto failure;
    
    tattr = fdh->TAttrArray;
        
    for (; numentries --; )
    {
        
        /* Read the fontname */
        
        STRPTR	strptr;
        UWORD   oldstrlen = 0; /* Length of fontname in file including null-termination */

        strptr = strbuf;
        
        do
        {
       	    if (!ReadByte( &DFB(DiskfontBase)->dsh, strptr, fh))
        	goto failure;
        		
            oldstrlen ++;
        }
        while (*strptr ++);
     
        /* We don't want a "topaz/9" like name here, but "topaz.font" */
       	strptr = strpbrk(strbuf, "/");

       	/* End string at '/' */
       	if (strptr) *strptr = 0;
       	
       	
       	/* Allocate space for fontname */
       	if ( !(tattr->tta_Name = AllocVec(strptr - strbuf + sizeof(".font") + 1, MEMF_ANY)))
            goto failure;
     	
     	strcpy(tattr->tta_Name, strbuf);
     
     	strcat(tattr->tta_Name, ".font");  	
       	    
        /* Seek to the end of the fontnamebuffer ( - 2 if tagged) */
        Flush(fh);
        Seek
        (
            fh,
            MAXFONTPATH - ( oldstrlen + (fdh->Tagged ? 2 : 0) ),
            OFFSET_CURRENT
        );
            
       
        /* Do special stuff for files with tags in them */
        if (fdh->Tagged)
        {

            /* Next thing up is the tagcount */
            if (!ReadWord(&DFB(DiskfontBase)->dsh, &numtags, fh))
                goto failure;
                
            
            if (numtags)
            {
                
	        /* Seek back and read the tags */
                Flush(fh);
                Seek
                (
                    fh, 
                    - ( (numtags << 3) + sizeof (UWORD) ), /*  sizeof (struct TagItem) = 8 */
                    OFFSET_CURRENT
                );

                if (!(tattr->tta_Tags = ReadTags(fh, numtags, DFB(DiskfontBase) )))
                    goto failure;
                
                /* Seek past the tagcount */
                if (!ReadWord( &DFB(DiskfontBase)->dsh, &aword, fh ))
                    goto failure;
                

            } /* if (numtags) */
            
        
        } /* if (fdh->Tagged) */
        
        /* Read the rest of the info */
        if (!ReadWord( &DFB(DiskfontBase)->dsh, &(tattr->tta_YSize), fh ))
            goto failure;

        if (!ReadByte(&DFB(DiskfontBase)->dsh, &(tattr->tta_Style), fh ))
            goto failure;
            
        if (!ReadByte( &DFB(DiskfontBase)->dsh, &(tattr->tta_Flags), fh ))
            goto failure;

        tattr ++;
        
    } /* for (; numentries --; ) */
    
    ReturnPtr ("ReadFontDescr", struct FontDescrHeader*, fdh);

failure:
    /* Free all allocated memory */
    
    if (fdh)
        FreeFontDescr(fdh, DFB(DiskfontBase) );
        
    ReturnPtr ("ReadFontDescr", struct FontDescrHeader*, FALSE);

}

/****************************************************************************************/

/******************/
/* FreeFontDescr  */
/******************/

/****************************************************************************************/

VOID FreeFontDescr(struct FontDescrHeader *fdh, struct DiskfontBase_intern *DiskfontBase)
{
    struct TTextAttr *tattr;

    UWORD numentries;
    
    D(bug("FreeFontDescr(fdh=%p\n", fdh));

    if (fdh)
    {       
        tattr       = fdh->TAttrArray;
        numentries  = fdh->NumEntries;
    
        if (tattr)
        {
            /* Go through the whole array freeing strings and tags */
            for (; numentries --; )
            {
                if (tattr->tta_Name)
                    FreeVec(tattr->tta_Name);
                
                if (tattr->tta_Tags)
                    FreeVec(tattr->tta_Tags);
                
                tattr ++;
            }
            
            FreeVec(fdh->TAttrArray);
        }
    
        FreeMem(fdh, sizeof (struct FontDescrHeader));
    }
    
    ReturnVoid("FreeFontDescr");
}

/****************************************************************************************/
