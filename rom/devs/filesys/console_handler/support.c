#define AROS_ALMOST_COMPATIBLE 1

#include <proto/exec.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <exec/errors.h>
#include <exec/alerts.h>
#include <utility/tagitem.h>
#include <dos/filesystem.h>
#include <dos/exall.h>
#include <dos/dosasl.h>
#include <intuition/intuition.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <devices/conunit.h>

#define SDEBUG 0
#define DEBUG 0
#include <aros/debug.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "con_handler_intern.h"
#include "support.h"
/******************************************************************************************/

static UBYTE up_seq[] 			= {'A'};
static UBYTE down_seq[] 		= {'B'};
static UBYTE right_seq[]		= {'C'};
static UBYTE left_seq[]			= {'D'};
static UBYTE shift_up_seq[]		= {'T'};
static UBYTE shift_down_seq[]		= {'S'}; 
static UBYTE shift_right_seq[]		= {' ', '@'};
static UBYTE shift_left_seq[]		= {' ', 'A'};
static UBYTE shift_tab_seq[]		= {'Z'};
static UBYTE help_seq[]			= {'?', '~'};
static UBYTE f1_seq[]			= {'0', '~'};
static UBYTE f2_seq[]			= {'1', '~'};
static UBYTE f3_seq[]			= {'2', '~'};
static UBYTE f4_seq[]			= {'3', '~'};
static UBYTE f5_seq[]			= {'4', '~'};
static UBYTE f6_seq[]			= {'5', '~'};
static UBYTE f7_seq[]			= {'6', '~'};
static UBYTE f8_seq[]			= {'7', '~'};
static UBYTE f9_seq[]			= {'8', '~'};
static UBYTE f10_seq[]			= {'9', '~'};
static UBYTE f11_seq[]			= {'2', '0', '~'};
static UBYTE f12_seq[]			= {'2', '1', '~'};
static UBYTE shift_f1_seq[]		= {'1', '0', '~'};
static UBYTE shift_f2_seq[]		= {'1', '1', '~'};
static UBYTE shift_f3_seq[]		= {'1', '2', '~'};
static UBYTE shift_f4_seq[]		= {'1', '3', '~'};
static UBYTE shift_f5_seq[]		= {'1', '4', '~'};
static UBYTE shift_f6_seq[]		= {'1', '5', '~'};
static UBYTE shift_f7_seq[]		= {'1', '6', '~'};
static UBYTE shift_f8_seq[]		= {'1', '7', '~'};
static UBYTE shift_f9_seq[]		= {'1', '8', '~'};
static UBYTE shift_f10_seq[]		= {'1', '9', '~'};
static UBYTE shift_f11_seq[]		= {'3', '0', '~'};
static UBYTE shift_f12_seq[]		= {'3', '1', '~'};
static UBYTE insert_seq[] 		= {'4', '0', '~'};
static UBYTE pageup_seq[] 		= {'4', '1', '~'};
static UBYTE pagedown_seq[] 		= {'4', '2', '~'};
static UBYTE pause_seq[] 		= {'4', '3', '~'};
static UBYTE break_seq[] 		= {'5', '3', '~'};
static UBYTE home_seq[] 		= {'4', '4', '~'};
static UBYTE end_seq[]	 		= {'4', '5', '~'};
static UBYTE shift_insert_seq[] 	= {'5', '0', '~'};
static UBYTE shift_pageup_seq[] 	= {'5', '1', '~'};
static UBYTE shift_pagedown_seq[] 	= {'5', '2', '~'};
static UBYTE shift_home_seq[] 		= {'5', '4', '~'};
static UBYTE shift_end_seq[]	 	= {'5', '5', '~'};

/* F11, F12, insert, pageup, pagedown, ... seq taken from
   RKRM: Devices/Console/Reading from the Console Device/Information about the Input Stream */
   
static struct csimatch
{
    UBYTE *seq;
    WORD len;
    WORD inp;
} csimatchtable[] =
{
    {up_seq		, 1, INP_CURSORUP		},
    {down_seq		, 1, INP_CURSORDOWN		},
    {right_seq		, 1, INP_CURSORRIGHT		},
    {left_seq		, 1, INP_CURSORLEFT		},
    {shift_up_seq	, 1, INP_SHIFT_CURSORUP		},
    {shift_down_seq	, 1, INP_SHIFT_CURSORDOWN	},
    {shift_right_seq	, 2, INP_SHIFT_CURSORRIGHT	},
    {shift_left_seq	, 2, INP_SHIFT_CURSORLEFT	},
    {shift_tab_seq	, 1, INP_SHIFT_TAB		},
    {help_seq		, 2, INP_HELP			},
    {f1_seq		, 2, INP_F1			},
    {f2_seq		, 2, INP_F2			},
    {f3_seq		, 2, INP_F3			},
    {f4_seq		, 2, INP_F4			},
    {f5_seq		, 2, INP_F5			},
    {f6_seq		, 2, INP_F6			},
    {f7_seq		, 2, INP_F7			},
    {f8_seq		, 2, INP_F8			},
    {f9_seq		, 2, INP_F9			},
    {f10_seq		, 2, INP_F10			},
    {f11_seq		, 3, INP_F11			},
    {f12_seq		, 3, INP_F12			},
    {shift_f1_seq	, 3, INP_SHIFT_F1		},
    {shift_f2_seq	, 3, INP_SHIFT_F2		},
    {shift_f3_seq	, 3, INP_SHIFT_F3		},
    {shift_f4_seq	, 3, INP_SHIFT_F4		},
    {shift_f5_seq	, 3, INP_SHIFT_F5		},
    {shift_f6_seq	, 3, INP_SHIFT_F6		},
    {shift_f7_seq	, 3, INP_SHIFT_F7		},
    {shift_f8_seq	, 3, INP_SHIFT_F8		},
    {shift_f9_seq	, 3, INP_SHIFT_F9		},
    {shift_f10_seq	, 3, INP_SHIFT_F10		},
    {shift_f11_seq	, 3, INP_SHIFT_F11		},
    {shift_f12_seq	, 3, INP_SHIFT_F12		},
    {insert_seq		, 3, INP_INSERT			},
    {pageup_seq		, 3, INP_PAGEUP			},
    {pagedown_seq	, 3, INP_PAGEDOWN		},
    {pause_seq		, 3, INP_PAUSE			},
    {break_seq		, 3, INP_BREAK			},
    {home_seq		, 3, INP_HOME			},
    {end_seq		, 3, INP_END			},
    {shift_insert_seq	, 3, INP_SHIFT_INSERT		},
    {shift_pageup_seq	, 3, INP_SHIFT_PAGEUP		},
    {shift_pagedown_seq , 3, INP_SHIFT_PAGEDOWN		},
    {shift_home_seq	, 3, INP_SHIFT_HOME		},
    {shift_end_seq	, 3, INP_SHIFT_END		},
    {0			, 0, 0				}
};


/******************************************************************************************/

static LONG stricmp(STRPTR string1, STRPTR string2) /* taken from utility/stricmp */
{
    UBYTE c1, c2;

    /* Loop as long as the strings are identical and valid. */
    do
    {
	/* Get characters, convert them to lower case. */
	c1 = tolower(*string1++);
	c2 = tolower(*string2++);
    }while(c1 == c2 && c1);

    /* Get result. */
    return (LONG)c1 - (LONG)c2;
}

/******************************************************************************************/

struct Task *createConTask(APTR taskparams, struct conbase *conbase)
{
    struct Task *task;
    struct MemList *memlist;
    UBYTE *mem;
    APTR stack;
    
    #define TASKMEM_SIZE (sizeof(struct Task) + CONTASK_STACKSIZE)
    
    memlist = AllocMem(sizeof (struct MemList), MEMF_PUBLIC|MEMF_CLEAR);
    if (memlist)
    {
	mem = AllocMem(TASKMEM_SIZE, MEMF_PUBLIC | MEMF_CLEAR);
	if (mem)
	{
            task = (struct Task *)mem;
	    stack = mem + sizeof(struct Task);

	    memlist->ml_NumEntries = 1;
	    memlist->ml_ME[0].me_Addr = mem;
	    memlist->ml_ME[0].me_Length = TASKMEM_SIZE;
	    
    	    NEWLIST(&task->tc_MemEntry);
	    AddHead(&task->tc_MemEntry, &memlist->ml_Node);

    	    task->tc_Node.ln_Type = NT_TASK;
    	    task->tc_Node.ln_Name = "CON: Window";
    	    task->tc_Node.ln_Pri  = CONTASK_PRIORITY;

	    task->tc_SPLower=stack;
	    task->tc_SPUpper=(BYTE *)stack + CONTASK_STACKSIZE;

#if AROS_STACK_GROWS_DOWNWARDS
	    task->tc_SPReg = (BYTE *)task->tc_SPUpper-SP_OFFSET - sizeof(APTR);
	    ((APTR *)task->tc_SPUpper)[-1] = taskparams;
#else
	    task->tc_SPReg=(BYTE *)task->tc_SPLower-SP_OFFSET + sizeof(APTR);
	    *(APTR *)task->tc_SPLower = taskparams;
#endif

	    if(AddTask(task, conTaskEntry, NULL) != NULL)
	    {
		return (task);
	    }	
            FreeMem(mem,TASKMEM_SIZE);
	}
	FreeMem(memlist, sizeof(struct MemList));
    }
    return (NULL);

}


/******************************************************************************************/

void parse_filename(struct conbase *conbase, struct filehandle *fh, struct IOFileSys *iofs, struct NewWindow *nw)
{
    UBYTE *filename = iofs->io_Union.io_OPEN.io_Filename;
    UBYTE *param, c;
    WORD paramid = 1;
    LONG paramval = 0;
    BOOL done = FALSE, paramok = FALSE;
    
    param = filename;

    while (!done)
    {
	
	c = *filename++;
	
        switch(c)
	{
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	    	if (paramid <= 4)
		{
		    paramval *= 10;
		    paramval += c - '0';
		}
		paramok = TRUE;
		break;
	
	    case '\0':
	    	done = TRUE;
		/* fall through */
		
	    case '/':
	    	if (paramok)
		{
		    UWORD paramlen = filename - param - 1;
		    
		    if (paramid == 1)
		    {
		    	nw->LeftEdge = paramval;
		    } else if (paramid == 2)
		    {
		    	nw->TopEdge = paramval;
		    } else if (paramid == 3)
		    {
		    	nw->Width = paramval;
		    } else if (paramid == 4)
		    {
		    	nw->Height = paramval;
		    } else if (paramid == 5)
		    {
			if ((fh->wintitle = AllocVec(paramlen + 1, MEMF_PUBLIC)))
			{
			    CopyMem(param, fh->wintitle, paramlen);
			    fh->wintitle[paramlen] = '\0';
			    nw->Title = fh->wintitle;
			}
		    } else {
		        filename[-1] = '\0';
			
			if (!stricmp(param, "WAIT"))
			{
			    fh->flags |= FHFLG_WAIT;
			} else if (!stricmp(param, "CLOSE"))
			{
			    nw->Flags |= WFLG_CLOSEGADGET;
			}
			
			filename[-1] = c;			
		    }
		    
		    paramok = FALSE;
		}
		paramval = 0;
		paramid++;
		param = filename;
		break;
	
	    default:
	        paramok = TRUE;
		break;
		
	} /* switch(c) */
	
    } /* while (!done) */
    
}

/******************************************************************************************/


void do_write(struct conbase *conbase, struct filehandle *fh, APTR data, ULONG length)
{
    fh->conwriteio.io_Command	= CMD_WRITE;
    fh->conwriteio.io_Data	= data;
    fh->conwriteio.io_Length	= length;

    DoIO((struct IORequest *)&fh->conwriteio);
}

/******************************************************************************************/

void do_movecursor(struct conbase *conbase, struct filehandle *fh, UBYTE direction, UBYTE howmuch)
{
    UBYTE seq[6]; /* 9B <N> <N> <N> <dir> <0> */
    ULONG size;
    
    if (howmuch > 0)
    {
	seq[0] = 0x9B;

	if (howmuch == 1)
	{
	    seq[1] = direction;
	    size = 2;
	} else {
            sprintf(&seq[1],"%ld%lc", (LONG)howmuch, (LONG)direction);
	    size = strlen(seq);
	}
    	
	do_write(conbase, fh, seq, size);
    }
}

/******************************************************************************************/

void do_cursorvisible(struct conbase *conbase, struct filehandle *fh, BOOL on)
{
    UBYTE seq[4];
    ULONG size = 0;
    
    seq[size++] = 0x9B;
    if (!on) seq[size++] = '0';
    seq[size++] = ' ';
    seq[size++] = 'p';
    
    do_write(conbase, fh, seq, size);
}

/******************************************************************************************/

void do_deletechar(struct conbase *conbase, struct filehandle *fh)
{
    UBYTE seq[] = {0x9B, 'P'};
    
    do_write(conbase, fh, seq, 2);
}

/******************************************************************************************/

void do_eraseinline(struct conbase *conbase, struct filehandle *fh)
{
    UBYTE seq[] = {0x9B, 'K'};
    
    do_write(conbase, fh, seq, 2);
}

/******************************************************************************************/

void do_eraseindisplay(struct conbase *conbase, struct filehandle *fh)
{
    UBYTE seq[] = {0x9B, 'J'};
    
    do_write(conbase, fh, seq, 2);
}
/******************************************************************************************/

WORD scan_input(struct conbase *conbase, struct filehandle *fh, UBYTE *buffer)
{
    struct csimatch *match;
    UBYTE c;
    
    WORD result = INP_DONE;
    
    if (fh->conbufferpos < fh->conbuffersize)
    {
        c = fh->consolebuffer[fh->conbufferpos++];
	D(bug("scan_input: check char %d\n",c));
	switch(c)
	{
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	        result = INP_CTRL_C - 3 + (WORD)c;
		break;
		
	    case 8:
	        result = INP_BACKSPACE;
		break;
	
	    case 9:
	    	result = INP_TAB;
		break;

	    case 10:
	    	result = INP_LINEFEED;
		break;
		
	    case 13:
	    	result = INP_RETURN;
		break;
		
	    case 127:
	    	result = INP_DELETE;
		break;
	
	    case 24:
	        result = INP_CONTROL_X;
		break;
		
	    case 0x9B: /* CSI */
	    	result = INP_UNKNOWN;
		
		match = csimatchtable;
		for(;match->seq;match++)
		{
		    if (!strncmp(match->seq, &(fh->consolebuffer[fh->conbufferpos]), match->len))
		    {
		        result = match->inp;
			fh->conbufferpos += match->len;
			break;
		    }
		}
	    	break;
	
	    default:
	        /* normal keys */		
		*buffer = c;		
		result = INP_STRING;
	        break;
		
	} /* switch(c) */
	
    } /* if (fh->conbufferpos < fh->conbuffersize) */
    
    D(bug("scan_input: result %d\n",result));
    
    return result;
}

/******************************************************************************************/

void answer_read_request(struct conbase *conbase, struct filehandle *fh, struct IOFileSys *iofs)
{
    ULONG readlen;

    readlen = (fh->inputsize < iofs->io_Union.io_READ.io_Length) ? fh->inputsize :
								   iofs->io_Union.io_READ.io_Length;

    iofs->io_Union.io_READ.io_Length = readlen;
    iofs->IOFS.io_Error = 0;
    iofs->io_DosError = 0;

    CopyMem(fh->inputbuffer, iofs->io_Union.io_READ.io_Buffer, readlen);
    CopyMem(fh->inputbuffer + readlen, fh->inputbuffer, fh->inputsize - readlen);

    fh->inputsize  -= readlen;
    fh->inputpos   -= readlen;
    fh->inputstart -= readlen;

    ReplyMsg(&iofs->IOFS.io_Message);
}


/******************************************************************************************/

void answer_write_request(struct conbase *conbase, struct filehandle *fh, struct IOFileSys *iofs)
{
    UBYTE *buffer = iofs->io_Union.io_WRITE.io_Buffer;
    LONG *length_ptr = &(iofs->io_Union.io_WRITE.io_Length);

    do_write(conbase, fh, buffer, *length_ptr);

    iofs->IOFS.io_Error = 0;
    iofs->io_DosError = fh->conwriteio.io_Error;    
    *length_ptr = fh->conwriteio.io_Actual;

    ReplyMsg(&iofs->IOFS.io_Message);
}


/******************************************************************************************/

void add_to_history(struct conbase *conbase, struct filehandle *fh)
{
    BOOL add_to_history = FALSE;
    
    fh->inputbuffer[fh->inputsize] = '\0';
    		    
    if (fh->historysize == 0)
    {
	add_to_history = TRUE;
    } else {
        WORD old_historypos;
	
	old_historypos = fh->historypos - 1;	
	if (old_historypos < 0) old_historypos = fh->historysize - 1;
	
	if (strcmp(&fh->inputbuffer[fh->inputstart], fh->historybuffer[old_historypos]))
	{
	    /* add to history only if different from last history entry */
	    
	    add_to_history = TRUE;
	}
    }
    
    if (add_to_history)
    {
	if (fh->historysize < CMD_HISTORY_SIZE) fh->historysize++;
	
	strcpy(fh->historybuffer[fh->historypos], &fh->inputbuffer[fh->inputstart]);

        fh->historypos++;
	if (fh->historypos >= CMD_HISTORY_SIZE) fh->historypos = 0;
    }
    
    fh->historyviewpos = fh->historypos;
}

/******************************************************************************************/

void history_walk(struct conbase *conbase, struct filehandle *fh, WORD inp)
{
    if (fh->historysize)
    {
	WORD len;

        switch(inp)
	{
	    case INP_SHIFT_CURSORUP:
		fh->historyviewpos = 0;
		break;
	
	    case INP_SHIFT_CURSORDOWN:
	    	fh->historyviewpos = fh->historysize - 1;;
		break;
		
	    case INP_CURSORUP:
	    	fh->historyviewpos--;
		if (fh->historyviewpos < 0) fh->historyviewpos = fh->historysize - 1;
		break;
		
	    case INP_CURSORDOWN:
	    	fh->historyviewpos++;
		if (fh->historyviewpos >= fh->historysize) fh->historyviewpos = 0;
		break;
	}

	if (fh->inputpos > fh->inputstart)
	{
	    do_movecursor(conbase, fh, CUR_LEFT, fh->inputpos - fh->inputstart);
	}

	do_eraseinline(conbase, fh);

	fh->inputsize = fh->inputstart;
	fh->inputpos = fh->inputstart;

	len = strlen(fh->historybuffer[fh->historyviewpos]);
	if (len > (INPUTBUFFER_SIZE - fh->inputstart))
	{
	    len = INPUTBUFFER_SIZE - fh->inputstart;
	}

	if (len > 0)
	{
	    CopyMem(fh->historybuffer[fh->historyviewpos],
		    &fh->inputbuffer[fh->inputstart],
		    len);

	    fh->inputsize += len;
	    fh->inputpos += len;

	    do_write(conbase, fh, &fh->inputbuffer[fh->inputstart], len);
	}
	
    } /* if (fh->historysize) */
}

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
