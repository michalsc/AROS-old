/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include "dos_intern.h"
#include <proto/exec.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <dos/dos.h>

/*****************************************************************************

    NAME */
#include <dos/dosasl.h>
#include <proto/dos.h>

	AROS_LH2(LONG, MatchFirst,

/*  SYNOPSIS */
	AROS_LHA(STRPTR             , pat, D1),
	AROS_LHA(struct AnchorPath *, AP , D2),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 137, Dos)

/*  FUNCTION

	Searches for the first file or directory that matches a given pattern.
	MatchFirst() initializes the AnchorPath structure for you but you
	must initilize the following fields: ap_Flags, ap_Strlen, ap_BreakBits
	and ap_FoundBreak. The first call to MatchFirst() also passes you
	the first matching file which you can examine in ap_Info and the directory
	the files is in in ap_Current->an_Lock. After the first call to
        MatchFirst() call MatchNext().
	The search begins whereever the current directory is set to. See
	CurrentDir();
	For more info on patterns see ParsePattern().

    INPUTS
        pat  - pattern to search for
	AP   - pointer to (initilized) AnchorPath structure
	
    RESULT
	0     = success
	other = DOS error code

    NOTES

    EXAMPLE

    BUGS
	Copying of the relative path to ap_Buf is not implemented yet

    SEE ALSO
	MatchNext(), MatchEnd(), ParsePattern(), Examine(), CurrentDir()
	<dos/dosasl.h>

    INTERNALS

    HISTORY
	29-04-97    bergers, initial revision
	27-11-96    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
{
  AROS_LIBFUNC_INIT
  AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

  struct AChain * AC;
  struct AChain * AC_Prev = NULL;
  LONG PatLength;
  STRPTR ParsedPattern;
  BPTR firstlock; 
  
  if (!pat)
    return FALSE;
  
  PatLength = 2*strlen(pat)+2;
  
  ParsedPattern = AllocMem(PatLength, MEMF_ANY);
  
  AP->ap_Base = NULL;
  
  if (NULL != ParsedPattern)
  {
    LONG PatStart = 0;
    LONG PatEnd   = 0;
    BOOL AllDone  = FALSE;
    LONG index;
    BOOL success = FALSE;
    /* put the preparsed string to some memory */
    /* If there are any wildcards then leave info */
    if (1 == ParsePatternNoCase(pat, ParsedPattern, PatLength))
      AP->ap_Flags = (BYTE)APF_ITSWILD;

    /* First I search for the very first ':'. If a '/' comes along
       before that I quit. The string before and including the ':' 
       is assumed to be an assigned director, for example 'libs:'.
       So I will start looking for the pattern in that directory.
     */
    while (TRUE)
    {
      if (ParsedPattern[PatEnd] == ':')
      {
        success = TRUE;
        break;
      }
      else
      {
        if ( ParsedPattern[PatEnd]         == '/'  ||
             ParsedPattern[PatEnd]         == '\0' ||
            (ParsedPattern[PatEnd] & 0x80) != 0 
                           /* a token or nonprintable letter */)
        {
          PatEnd = 0;
          break;
        }
      } 
      PatEnd++;
    }

    /* Only if success == TRUE an assigned dir was found. */
    if (TRUE == success)
    {
      /* try to create a lock to that assigned dir. */
      char Remember = ParsedPattern[PatEnd+1];
      PatEnd++;
      ParsedPattern[PatEnd] = '\0';
      firstlock = Lock(ParsedPattern, ACCESS_READ);
      /* check whether an error occurred */
      if (NULL == firstlock)
      {
kprintf("Coudln't lock dir!\n");
        FreeMem(ParsedPattern, PatLength);
        return ERROR_DIR_NOT_FOUND; /* hope that's the right one... */
      }
      
      /* I have the correct lock. */
      ParsedPattern[PatEnd] = Remember;
      PatStart=PatEnd;
    }
    else
    {
      /* Create a lock to the current dir. */
      firstlock = CurrentDir(NULL);
      firstlock = DupLock(firstlock);
      (void)CurrentDir(firstlock);
    }
    
    /* Build the Anchor Chain. For every subdirector I allocate
       a AChain structure and link them all together */   
    while (FALSE == AllDone)
    {


      /* 
         Search for the next '/' in the pattern and everything behind
         the previous '/' and before this '/' will go to an_String 
       */
      while (TRUE)
      {
        if (ParsedPattern[PatEnd] == '\0')
        {
          AllDone = TRUE;
          PatEnd--;
          break;
        }
        if (ParsedPattern[PatEnd] == '/')
        {
          PatEnd--;
          break;
        }
        PatEnd++;
      }
      
      AC = AllocMem(sizeof(struct AChain)+(PatEnd-PatStart+2), MEMF_CLEAR);
      if (NULL == AC)
      {
        /* not so bad if this was not the very first AC. */
        if (NULL == AP->ap_Base)
        {
          /* oops, it was the very first one. I really cannot do anything for 
             you. - sorry */
          FreeMem(ParsedPattern, PatLength);
          return ERROR_NO_FREE_STORE;
        }
        
        /* let me out of here. I will at least try to do something for you.
           I can check the first few subdirs but that's gonna be it. 
         */
        AP->ap_Flags |= APF_NOMEMERR;
        break;
      }
      
      if (NULL == AP->ap_Base)
      {
        AP->ap_Base = AC;
        AP->ap_Current = AC;
      }
      
      if (NULL != AC_Prev)
      {
        AC_Prev->an_Child = AC;
      }
      AC->an_Parent = AC_Prev;
      AC_Prev       = AC;
      
      /* copy the part of the pattern to the end of the AChain. */
      index = 0;
      while (PatStart <= PatEnd)
      {
        AC->an_String[index] = ParsedPattern[PatStart];
        index++;
        PatStart++;
      }
      /* Put PatStart and PetEnd behind the '/' that was found. */
      PatStart   = PatEnd + 2;
      PatEnd    += 2;
      /* 
         the trailing '\0' is there automatically as I allocated enough store
         with MEMF_CLEAR
      */

    } /* while () */

    /* The AnchorChain to work with is the very first one. */
    AC = AP->ap_Base;
    AC->an_Lock = firstlock;
    
    /* look for the first file that matches the given pattern */
    success = Examine(AC->an_Lock, &AC->an_Info);
    success = ExNext (AC->an_Lock, &AC->an_Info);
    while (DOSTRUE == success &&
           DOSFALSE == MatchPatternNoCase(AC->an_String,
                                          AC->an_Info.fib_FileName))
    {
      /* I still haven't found what I've been looking for ... */
      success = ExNext(AC->an_Lock, &AC->an_Info); 
    }  
    
    if (DOSFALSE == success)
    {
      return ERROR_NO_MORE_ENTRIES;
    }
    
    /* Hooray! A matching file was found. Also show the data in AP */
    CopyMem(&AC->an_Info, &AP->ap_Info, sizeof(struct FileInfoBlock));
    if (0 != AP->ap_Strlen)
    {
       if (FALSE == writeFullPath(AP))
          return ERROR_BUFFER_OVERFLOW;
    }
    return 0;
     
  }
  else
  {
    return ERROR_NO_FREE_STORE;
  }

  return 0;

  AROS_LIBFUNC_EXIT
} /* MatchFirst */
