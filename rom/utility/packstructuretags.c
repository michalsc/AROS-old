/*
    $Id$
    $Log$
    Revision 1.1  1997/01/08 03:36:13  iaint
    A few more utility.lib functions

    Desc:
    Lang: english
*/
#include "utility_intern.h"

/*****************************************************************************

    NAME */
#include <clib/utility_protos.h>

        AROS_LH3(ULONG, PackStructureTags,

/*  SYNOPSIS */
        AROS_LHA(APTR            , pack, A0),
        AROS_LHA(ULONG          *, packTable, A1),
        AROS_LHA(struct TagItem *, tagList, A2),

/*  LOCATION */
        struct Library *, UtilityBase, 35, Utility)

/*  FUNCTION
        This function will scan through the packTable, and for each TagItem
        described in a packTable entry which can be found in the tagList,
        the data in the TagItem's ti_Data field will be packed into the
        structure as described in the packTable.

    INPUTS
        pack            -   The structure to fill in.
        packTable       -   Table describing how to pack the structure.
                            See the include file utility/pack.h for
                            information on the format of this table.
        tagList         -   List of TagItems containing data.

    RESULT
        The number of TagItems packed.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        UnpackStructureTags

    INTERNALS

    HISTORY
        29-10-95    digulla automatically created from
                            utility_lib.fd and clib/utility_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct UtilityBase *, UtilityBase)

    Tag         tagBase;
    UWORD       memOff;
    UWORD       tagOff;
    UBYTE       bitOff;
    struct TagItem *ti;
    LONG        count = 0;

    tagBase = *packTable++;
    for( ; *packTable != 0; packTable++)
    {
        /* New base tag */
        if(*packTable == -1)
        {
            tagBase = *++packTable;
            continue;
        }

        /* This entry is not defined for packing */
        if((*packTable & PSTF_PACK))    continue;

        tagOff = (*packTable >> 16) & 0x3FF;

        /* Does the tag we are interested in exist in that list. */
        ti = FindTagItem(tagBase + tagOff, tagList);
        if(ti == NULL)
            continue;

        memOff = *packTable & 0x1FFF;
        bitOff = (*packTable & 0xE000) >> 13;

        /*
            If the PSTF_EXISTS bit is 1, then the tagexists says that we
            set the bit to 1.

            XXX: Have to see what happens when the Tag doesn't exist.
        */
        if(*packTable & (PKCTRL_BIT|PSTF_EXISTS))
        {
            *(UBYTE *)&((UBYTE *)pack)[memOff] |= (1 << bitOff);
            count++;
            continue;
        }

        /*
            I need this rather interesting casting because the offset
            is always in bytes, but the cast to (ULONG *) etc, will cause
            the array indexing to be done in different sizes, so I basically
            override that, then cast to the required size. I've got to cast
            like: &((UBYTE *)pack)[memOff],
            not:  (UBYTE *)&pack[memOff]
            because otherwise I'd be dereferencing a void *.

            Ah, so much easier in assembly eh?

            Also the assigning is different for signed and unsigned since
            ti_Data is not necessarily the same size as the structure field,
            so we have to let the compiler do sign extension.

            Bit shifting the packTable entry >> 24, we can do some more
            instruction space efficient stuff.
        */
        switch((*packTable >> 24) & 0x98)
        {
            case (PKCTRL_ULONG >> 24):
                *(ULONG *)&((UBYTE *)pack)[memOff] = (ULONG)ti->ti_Data;
                break;

            case (PKCTRL_UWORD >> 24):
                *(UWORD *)&((UBYTE *)pack)[memOff] = (ULONG)ti->ti_Data;
                break;

            case (PKCTRL_UBYTE >> 24):
                *(UBYTE *)&((UBYTE *)pack)[memOff] = (ULONG)ti->ti_Data;
                break;

            case (PKCTRL_LONG >> 24):
                *(LONG *)&((UBYTE *)pack)[memOff] = (LONG)ti->ti_Data;
                break;

            case (PKCTRL_WORD >> 24):
                *(WORD *)&((UBYTE *)pack)[memOff] = (LONG)ti->ti_Data;
                break;

            case (PKCTRL_BYTE >> 24):
                *(BYTE *)&((UBYTE *)pack)[memOff] = (LONG)ti->ti_Data;
                break;

            case (PKCTRL_BIT >> 24):
                if(ti->ti_Data)
                    *(UBYTE *)&((UBYTE *)pack)[memOff] |= (1 << bitOff);
                else
                    *(UBYTE *)&((UBYTE *)pack)[memOff] &= ~(1 << bitOff);
                break;

            case (PKCTRL_FLIPBIT >> 24):
                if(ti->ti_Data)
                    *(UBYTE *)&((UBYTE *)pack)[memOff] &= ~(1 << bitOff);
                else
                    *(UBYTE *)&((UBYTE *)pack)[memOff] |= (1 << bitOff);
                break;

            /* We didn't actually pack anything */
            default:
                count--;
        } /* switch() */
        count++;
    } /* for() */

    return count;

    AROS_LIBFUNC_EXIT
} /* PackStructureTags */
