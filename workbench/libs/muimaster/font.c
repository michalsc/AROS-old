#include <string.h>
#include <exec/types.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/diskfont.h>
#include <proto/dos.h>

#include "muimaster_intern.h"
#include "font.h"
#include "prefs.h"

extern struct Library *MUIMasterBase;

/* Returns a given text font, if necessary it opens the font.
 * Must be called after Area's MUIM_Setup.
 */

struct TextFont *zune_font_get(Object *obj, LONG preset)
{
    struct MUI_GlobalInfo *mgi = muiGlobalInfo(obj);
    struct MUI_RenderInfo *mri = muiRenderInfo(obj);

    if ((preset <= MUIV_Font_Inherit) && (preset >= MUIV_Font_NegCount))
    {
    	CONST_STRPTR name;
	if (preset > 0) return NULL;

	/* font already loaded, just return it */
	if (mri->mri_Fonts[-preset])
	{
/*  	    D(bug("zune_font_get : return mri_Fonts[-preset]=%lx\n", ri->mri_Fonts[-preset])); */
	    return mri->mri_Fonts[-preset];
	}

	/* font name given, load it */
        name = mgi->mgi_Prefs->fonts[-preset];
/*  	D(bug("zune_font_get : preset=%d, name=%s\n", preset, name)); */
	if (name)
	{
	    struct TextAttr ta;
	    if ((ta.ta_Name = (char*)AllocVec(strlen(name)+10,0)))
	    {
	    	char *p;
	    	LONG size;

	    	strcpy(ta.ta_Name,name);
	    	StrToLong(FilePart(ta.ta_Name),&size);
	    	ta.ta_YSize = size;
		ta.ta_Style = 0;
		ta.ta_Flags = 0;

		if ((p = PathPart(ta.ta_Name)))
		    strcpy(p,".font");
/*  		D(bug("zune_font_get : OpenDiskFont(%s)\n", ta.ta_Name)); */
		mri->mri_Fonts[-preset] = OpenDiskFont(&ta);

		FreeVec(ta.ta_Name);
	    }
	    
	}
	else /* fallback to window normal font */
	{
	    if (preset != MUIV_Font_Normal) /* avoid infinite recursion */
	    {
		return zune_font_get(obj, MUIV_Font_Normal);
	    }
	}

	/* no font loaded, fallback to screen font */
	if (!mri->mri_Fonts[-preset])
	{
	    struct TextAttr scr_attr;
	    scr_attr = *(_screen(obj)->Font);
	    scr_attr.ta_Flags = 0;
/*  	    D(bug("zune_font_get : OpenDiskFont(%s) (screen font)\n", scr_attr.ta_Name)); */
	    mri->mri_Fonts[-preset] = OpenDiskFont(&scr_attr);
	}
	return mri->mri_Fonts[-preset];
    }
    return (struct TextFont *)preset;
}
