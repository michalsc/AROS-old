/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc: Misc helpfuncs needed in AvailFonts()
    Lang: English.
*/

#include <proto/utility.h>
#include "diskfont_intern.h"

#define DEBUG 1
#include <aros/debug.h>


/****************/
/* AllocFIN		*/
/****************/

struct FontInfoNode *AllocFIN(struct MinList *filist, struct DiskfontBase_intern *DiskfontBase)
{
	struct FontInfoNode *finode;

	D(bug("AllocFIN(filist=%p)\n", filist));
	
	finode = AllocMem(sizeof (struct FontInfoNode), MEMF_ANY|MEMF_CLEAR);
	if (finode)
		AddTail( (struct List*)filist, (struct Node*)finode );
		
	ReturnPtr("AllocFIN", struct FontInfoNode *, finode);
}

/************/
/* FreeFIN	*/
/************/

VOID FreeFIN(struct FontInfoNode *finode, struct DiskfontBase_intern *DiskfontBase)
{
	D(bug( "FreeFIN(finode=%p)\n", finode ));
	
	Remove( (struct Node*)finode);	
	FreeMem(finode, sizeof (struct FontInfoNode) );
	
	ReturnVoid("FreeFIN");
}

/**************/
/* NumTags    */
/**************/

ULONG NumTags(struct TagItem *taglist, struct DiskfontBase_intern *DiskfontBase)
/* Counts the number of tags in at taglist including TAG_DONE */

{
  
    ULONG numtags = 0;

    D(bug("NumTags(taglist=%p)\n", taglist));

    for (; NextTagItem((const struct TagItem **)&taglist); )
        numtags ++;

    numtags ++; /* Count TAG_DONE */
  
    ReturnInt ("NumTags", ULONG, numtags);
}
/****************/
/* CopyTagItems */
/****************/

ULONG CopyTagItems
(
	struct TagItem *desttaglist, 
	struct TagItem* sourcetaglist, 
	struct DiskfontBase_intern *DiskfontBase
)
/* Copies tags from a taglist to another memory location, returning
  the number of tags copied */

{
    ULONG numtags=0;
  
    struct TagItem *tag;

    D(bug("CopyTagItems(desttaglist=%p, sourcetaglist=%p)\n", desttaglist, sourcetaglist));

    for (; (tag = NextTagItem((const struct TagItem **)&sourcetaglist)); )
    {
        desttaglist->ti_Tag   = tag->ti_Tag;
        desttaglist->ti_Data  = tag->ti_Data;
    
        desttaglist++;
        numtags++;
    }

    /* Count TAG_DONE */
    desttaglist++;
    numtags++;
    
    /* Insert TAG_DONE */
    desttaglist->ti_Tag   = TAG_DONE;
    desttaglist->ti_Data  = 0L;
  
    ReturnInt ("CopyTagItems", ULONG, numtags);
}


  

/**********************/
/* FreeFontList       */
/**********************/

VOID FreeFontList(struct MinList *filist, struct DiskfontBase_intern *DiskfontBase)
{
 
    struct MinNode *node, *nextnode;
  	struct TTextAttr	*tattr;
  	
    /* Free the fontinfolist */
    D(bug("FreeFontList(filist=%p)\n", filist));

    node = filist->mlh_Head;
  
    while ( (nextnode = FIN(node)->NodeHeader.mln_Succ) )
    {
    	tattr = &(FIN(node)->TAF.taf_Attr);
    	
    	/* Free fontname */
    	if (tattr->tta_Name)
    		FreeVec(tattr->tta_Name);
    	
    	/* Free tags */
    	if (tattr->tta_Tags)
    		FreeVec(tattr->tta_Tags);
    		
        FreeMem
        (
            node,
            sizeof (struct FontInfoNode)
        );
        node = nextnode;
    }
      
  
  ReturnVoid ("FreeFontList");
  }


