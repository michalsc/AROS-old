/*
    (C) 1999 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: English
*/
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <utility/tagitem.h>
#include <intuition/intuition.h>
#include "datatypes_intern.h"	/* Must be after <intuition/intuition.h> */

/*****************************************************************************

    NAME */
#include <proto/datatypes.h>

        AROS_LH2(Object *, NewDTObjectA,

/*  SYNOPSIS */
	AROS_LHA(APTR            , name , D0),
	AROS_LHA(struct TagItem *, attrs, A0),

/*  LOCATION */
	struct Library *, DataTypesBase, 8, DataTypes)

/*  FUNCTION

    Create a data type object from a BOOPSI class.

    INPUTS

    name   --  name of the data source; generally an existing file name
    attrs  --  pointer to a TagList specifying additional arguments

    TAGS

    DTA_SourceType  --  The type of source data (defaults to DTST_FILE).
                        If the source is the clipboard the name field
			contains the numeric clipboard unit.

    DTA_Handle      --  Can be used instead of the 'name' field. If the
                        source is DTST_FILE, ths must be a valid FileHandle;
			must be a valid IFFHandle if source is DTST_CLIPBOARD.

    DTA_DataType    --  The class of the data. Data is a pointer to a valid
                        DataType; only used when creating a new object that
			doens't have any source data.

    DTA_GroupID     --  If the object isn't of this type, fail with an IoErr()
                        of ERROR_OBJECT_WRONG_TYPE.

    GA_Left
    GA_RelRight
    GA_Top
    GA_RelBottom
    GA_Width
    GA_RelWidth
    GA_Height
    GA_RelHeight    --  Specify the position of the object relative to the
                        window.

    GA_ID           --  ID of the object.

    GA_UserData     --  Application specific data for the object.

    GA_Previous     --  Previous object / gadget in the list.

    RESULT

    A BOOPSI object. This may be used in different contexts such as a gadget
    or image. NULL indicates failure -- in that case IoErr() gives more
    information:

    ERROR_REQUIRED_ARG_MISSING  --  A required attribute wasn't specified.
    
    ERROR_BAD_NUMBER            --  The group ID specified was invalid.

    ERROR_OBJECT_WRONG_TYPE     --  Object data type doesn't match DTA_GroupID.

    NOTES

    This function invokes the method OM_NEW for the specified class.

    The object should (eventually) be freed by DisposeDTObject() when no
    longer needed.

    EXAMPLE

    BUGS

    SEE ALSO

    AddDTObject(), DisposeDTObject(), RemoveDTObject(),
    intuition.library/NewObjectA()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    ULONG             SourceType;
    struct DataType  *DataType;
    APTR              Handle;
    ULONG             GroupID;
    BPTR              lock = NULL;
    struct IFFHandle *iff = NULL;
    Object           *dtobj = NULL;
    UBYTE            *BaseName = NULL;

kprintf("NewDTObjectA: 1 attrs = %x UtilityBase = %x\n",attrs,UtilityBase);    
    if(!(SourceType = GetTagData(DTA_SourceType, DTST_FILE, attrs)))
{
kprintf("NewDTObejctA: 11\n");
	SetIoErr(ERROR_REQUIRED_ARG_MISSING);
kprintf("NewDTObejctA: 12\n");
}
    else
    {
kprintf("NewDTObjectA: 2\n");    
	DataType = (struct DataType *)GetTagData(DTA_DataType, NULL, attrs);
	Handle   =              (APTR)GetTagData(DTA_Handle,   NULL, attrs);
	GroupID  =                    GetTagData(DTA_GroupID,  0   , attrs);
kprintf("NewDTObjectA: 3\n");    
	
	if((SourceType == DTST_RAM) && GroupID)
	{
	    switch (GroupID)
	    {
	    case GID_ANIMATION:  BaseName="animation";         break;
	    case GID_DOCUMENT:   BaseName="document";          break;
	    case GID_INSTRUMENT: BaseName="instrument";        break;
	    case GID_MOVIE:      BaseName="movie";             break;
	    case GID_MUSIC:      BaseName="music";             break;
	    case GID_PICTURE:    BaseName="picture";           break;
	    case GID_SOUND:      BaseName="sound";             break;
	    case GID_TEXT:       BaseName="ascii";             break;
		
	    default:             SetIoErr(ERROR_BAD_NUMBER);   break;
	    }
	}
	else
	{
kprintf("NewDTObjectA: 4\n");    
	    if(Handle != NULL)
{
kprintf("NewDTObjectA: 5\n");    

		DataType = ObtainDataTypeA(SourceType, Handle, attrs);
kprintf("NewDTObjectA: 6\n");    
}
	    else
	    {
kprintf("NewDTObjectA: 7\n");    
		if(DataType == NULL)
		{
kprintf("NewDTObjectA: 8\n");    
		    switch(SourceType)
		    {
		    case DTST_FILE:
kprintf("NewDTObjectA: 9\n");    
			if((lock = Lock(name, ACCESS_READ)))
			{
kprintf("NewDTObjectA: 10\n");    
			    if((DataType = ObtainDataTypeA(SourceType,
							   (APTR)lock, attrs)))
			    {
kprintf("NewDTObjectA: 11\n");    
				if (GroupID && (DataType->dtn_Header->dth_GroupID != GroupID))
				{
kprintf("NewDTObjectA: 12\n");    
				    ReleaseDataType(DataType);
				    DataType = NULL;
				    SetIoErr(ERROR_OBJECT_WRONG_TYPE);
				}
				else
				    Handle = (APTR)lock;
			    }

			    if(Handle == NULL)
			    {
				UnLock(lock);
				lock = NULL;
			    }
			} /* if lock aquired */
kprintf("NewDTObjectA: 13\n");    
			break;
			
		    case DTST_CLIPBOARD:
			if(!(iff = AllocIFF()))
			    SetIoErr(ERROR_NO_FREE_STORE);
			else
			{
			    if((iff->iff_Stream = (ULONG)OpenClipboard((ULONG)name)))
			    {
				InitIFFasClip(iff);
				
				if(!OpenIFF(iff, IFFF_READ))
				{
				    if((DataType = ObtainDataTypeA(SourceType,
								  iff, attrs)))
				    {
					if (GroupID && (DataType->dtn_Header->dth_GroupID != GroupID))
					{
					    ReleaseDataType(DataType);
					    DataType = NULL;
					    SetIoErr(ERROR_OBJECT_WRONG_TYPE);
					}
					else
					    Handle = iff;
				    }
				    if(Handle == NULL)
					CloseIFF(iff);
				}
				if(Handle == NULL)
				    CloseClipboard((struct ClipboardHandle*)iff->iff_Stream);
			    }
			    
			    if(Handle == NULL)
			    {
				FreeIFF(iff);
				iff = NULL;
			    }
			}
			
			break;
		    }
		}
	    }
	}
kprintf("NewDTObjectA: 14\n");    
	
	if(DataType != NULL)
	    BaseName = DataType->dtn_Header->dth_BaseName;
kprintf("NewDTObjectA: 15\n");    
	
	if(BaseName != NULL)
	{
	    UBYTE libname[120];
	    struct Library *DTClassBase;

kprintf("NewDTObjectA: 16\n");    
	    
	    dt_sprintf(DataTypesBase, libname,"datatypes/%s.datatype", BaseName);
kprintf("NewDTObjectA: 17: libname = %s\n", libname);    
	    
	    if(!(DTClassBase = OpenLibrary(libname, 0)))
		SetIoErr(DTERROR_UNKNOWN_DATATYPE);
	    else
	    {
		struct IClass *DTClass;
kprintf("NewDTObjectA: 18\n");    
		
		/* Call ObtainEngine() */
		if((DTClass = AROS_LVO_CALL0(Class *, struct Library *,
					     DTClassBase, 5,)))
		    
		{
		    struct TagItem Tags[4];
		    
		    Tags[0].ti_Tag  = DTA_Name;
		    Tags[0].ti_Data = (ULONG)name;
		    Tags[1].ti_Tag  = DTA_DataType;
		    Tags[1].ti_Data = (ULONG)DataType;
		    Tags[2].ti_Tag  = DTA_Handle;
		    Tags[2].ti_Data = (ULONG)Handle;
		    Tags[3].ti_Tag  = TAG_MORE;
		    Tags[3].ti_Data = (ULONG)attrs;
		    
		    dtobj = NewObjectA(DTClass, NULL, Tags);
		    
		    lock = NULL;
		    iff = NULL;
		}
		
		if(dtobj == NULL)
		    CloseLibrary(DTClassBase);
	    }
kprintf("NewDTObjectA: 30\n");    
	}
kprintf("NewDTObjectA: 31\n");    
	
	if(dtobj == NULL)
	{
kprintf("NewDTObjectA: 32\n");    
	    if(lock != NULL)
		UnLock(lock);
kprintf("NewDTObjectA: 33\n");    
	    
	    if(iff != NULL)
	    {
kprintf("NewDTObjectA: 34\n");    
		CloseIFF(iff);
kprintf("NewDTObjectA: 35\n");    
		CloseClipboard((struct ClipboardHandle *)iff->iff_Stream);
kprintf("NewDTObjectA: 36\n");    
		FreeIFF(iff);
kprintf("NewDTObjectA: 37\n");    
	    }
	}
    }
kprintf("NewDTObjectA: 38\n");    
    
    if(IoErr() == ERROR_OBJECT_NOT_FOUND)
	SetIoErr(DTERROR_COULDNT_OPEN);
kprintf("NewDTObjectA: 39 dto = %x\n", dtobj);    
    
    return dtobj;
    
    AROS_LIBFUNC_EXIT
} /* NewDTObjectA */

