/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: Create a new process
    Lang: English
*/
#include <exec/memory.h>
#include <exec/lists.h>
#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos/filesystem.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <utility/tagitem.h>
#include <proto/utility.h>
#include "dos_intern.h"

static void KillCurrentProcess(void);
struct Process *AddProcess(struct Process *process, STRPTR argPtr,
ULONG argSize, APTR initialPC, APTR finalPC, struct DosLibrary *DOSBase);

static void freeLocalVars(struct Process *process);

#include <aros/debug.h>

/* Temporary macro */
#define P(x)

/*****************************************************************************

    NAME */
#include <proto/dos.h>

	AROS_LH1(struct Process *, CreateNewProc,

/*  SYNOPSIS */
	AROS_LHA(struct TagItem *, tags, D1),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 83, Dos)

/*  FUNCTION
	Create a new process using the tagitem array.

    INPUTS
	tags - information on the new process.

    RESULT
	Pointer to the new process or NULL on error.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	29-10-95    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

    /* Allocated resources */
    struct Process  *process = NULL;
    BPTR             input = 0, output = 0, curdir = 0;
    STRPTR           stack = NULL, name = NULL, argptr = NULL;
    ULONG            namesize = 0, argsize = 0;
    struct MemList  *memlist = NULL;
    struct CommandLineInterface *cli = NULL;
    struct Process  *me = (struct Process *)FindTask(NULL);
    STRPTR           s;
    BPTR            *oldpath, *newpath, *nextpath;

    /* NOTE: NP_Synchronous and NP_NotifyOnDeath is not implemented -- they
             aren't in AmigaOS either... */

    /* TODO: NP_CommandName, NP_HomeDir, NP_ConsoleTask */

    struct TagItem defaults[]=
    {
    /* 0 */    { NP_Seglist,	   0 },
    /* 1 */    { NP_Entry,	   (IPTR)NULL },
    /* 2 */    { NP_Input,	   ~0ul },
    /* 3 */    { NP_CloseInput,    1 },
    /* 4 */    { NP_Output,	   ~0ul },
    /* 5 */    { NP_CloseOutput,   1 },
    /* 6 */    { NP_Error,	   0 },
    /* 7 */    { NP_CloseError,    1 },
    /* 8 */    { NP_CurrentDir,    ~0ul },
    /* 9 */    { NP_StackSize,	   AROS_STACKSIZE },
    /*10 */    { NP_Name,	   (IPTR)"New Process" },
    /*11 */    { NP_Priority,	   me->pr_Task.tc_Node.ln_Pri },
    /*12 */    { NP_Arguments,	   (IPTR)NULL },
    /*13 */    { NP_Cli,	   0          },
    /*14 */    { NP_UserData,	   (IPTR)NULL },
    /*15 */    { NP_ExitCode,      (IPTR)NULL },
    /*16 */    { NP_ExitData,      (IPTR)NULL },
    /*17 */    { NP_WindowPtr,     (IPTR)NULL }, /* Default: default public
                                                    screen */
    /*18 */    { NP_CopyVars,      (IPTR)TRUE },
	       { TAG_END,          0           }
    };

    /* C has no exceptions. This is a simple replacement. */
#define ERROR_IF(a)  if(a) goto error  /* Throw a generic error. */
#define ENOMEM_IF(a) if(a) goto enomem /* Throw out of memory. */
    
    ApplyTagChanges(defaults, tags);

    process = (struct Process *)AllocMem(sizeof(struct Process),
					 MEMF_PUBLIC | MEMF_CLEAR);
    ENOMEM_IF(process == NULL);

    /* Do this early to ease implementation of failure code */
    NEWLIST((struct List *)&process->pr_LocalVars);

    stack = AllocMem(defaults[9].ti_Data, MEMF_PUBLIC);
    ENOMEM_IF(stack == NULL);

    s = (STRPTR)defaults[10].ti_Data;
    while(*s++);
    namesize = s - (STRPTR)defaults[10].ti_Data;

    name = AllocMem(namesize, MEMF_PUBLIC);
    ENOMEM_IF(name == NULL);

    s = (STRPTR)defaults[12].ti_Data;
    if(s != NULL)
    {
	while(*s++);

	argsize = s - (STRPTR)defaults[12].ti_Data;
	argptr  = (STRPTR)AllocVec(argsize, MEMF_PUBLIC);
	ENOMEM_IF(argptr == NULL);
    }

    memlist = AllocMem(sizeof(struct MemList) + 2*sizeof(struct MemEntry),
		       MEMF_ANY);
    ENOMEM_IF(memlist == NULL);

    if(defaults[13].ti_Data != NULL)
    {
	/* Don't forget to pass tags to AllocDosObject() */
	cli = (struct CommandLineInterface *)AllocDosObject(DOS_CLI, tags);
	ENOMEM_IF(cli == NULL);

	Forbid();
	process->pr_TaskNum = ++(DOSBase->dl_ProcCnt);
	Permit();

	oldpath = NULL;
	cli->cli_DefaultStack = defaults[9].ti_Data >> 2;

	if(me->pr_Task.tc_Node.ln_Type == NT_PROCESS)
	{
	    struct CommandLineInterface *oldcli = Cli();

	    if(oldcli != NULL)
		oldpath = BADDR(oldcli->cli_CommandDir);
	}

	newpath = &cli->cli_CommandDir;

	/* Add substitute for lock chain */
	while(oldpath != NULL)
	{
	    nextpath = AllocVec(2*sizeof(BPTR), MEMF_CLEAR);
	    ENOMEM_IF(nextpath == NULL);

	    newpath[0]  = MKBADDR(nextpath);
	    nextpath[1] = DupLock(oldpath[1]);
	    ERROR_IF(!nextpath[1]);

	    newpath = nextpath;
	    oldpath = BADDR(oldpath[0]);
	}
    }

    if(defaults[2].ti_Data == ~0ul)
    {
	input = Open("NIL:", MODE_OLDFILE);
	ERROR_IF(!input);

	defaults[2].ti_Data = (IPTR)input;
    }

    if(defaults[4].ti_Data == ~0ul)
    {
	output = Open("NIL:", MODE_NEWFILE);
	ERROR_IF(!output);

	defaults[4].ti_Data = (IPTR)output;
    }

    if(defaults[8].ti_Data == ~0ul)
    {
	if(me->pr_Task.tc_Node.ln_Type == NT_PROCESS)
	{
	    curdir = Lock("", SHARED_LOCK);
	    ERROR_IF(!curdir);

	    defaults[8].ti_Data = (IPTR)curdir;
	}
	else
	    defaults[8].ti_Data = 0;
    }

    CopyMem((APTR)defaults[10].ti_Data, name, namesize);
    CopyMem((APTR)defaults[12].ti_Data, argptr, argsize);
    process->pr_Task.tc_Node.ln_Type = NT_PROCESS;
    process->pr_Task.tc_Node.ln_Name = name;
    process->pr_Task.tc_Node.ln_Pri = defaults[11].ti_Data;
    process->pr_Task.tc_SPLower = stack;
    process->pr_Task.tc_SPUpper = stack + defaults[9].ti_Data;

/*  process->pr_ReturnAddr; */
    NEWLIST(&process->pr_Task.tc_MemEntry);

    memlist->ml_NumEntries = 3;
    memlist->ml_ME[0].me_Addr = process;
    memlist->ml_ME[0].me_Length = sizeof(struct Process);
    memlist->ml_ME[1].me_Addr = stack;
    memlist->ml_ME[1].me_Length = defaults[9].ti_Data;
    memlist->ml_ME[2].me_Addr = name;
    memlist->ml_ME[2].me_Length = namesize;

    AddHead(&process->pr_Task.tc_MemEntry, &memlist->ml_Node);

    process->pr_MsgPort.mp_Node.ln_Type = NT_MSGPORT;
    process->pr_MsgPort.mp_Flags = PA_SIGNAL;
    process->pr_MsgPort.mp_SigBit = SIGB_DOS;
    process->pr_MsgPort.mp_SigTask = process;

    NEWLIST(&process->pr_MsgPort.mp_MsgList);

    process->pr_SegList = (BPTR)defaults[0].ti_Data;
    process->pr_StackSize = defaults[9].ti_Data;
    process->pr_GlobVec = NULL;	                   /* Unused BCPL crap */
    process->pr_StackBase = MKBADDR(process->pr_Task.tc_SPUpper);
    process->pr_Result2 = 0;
    process->pr_CurrentDir = (BPTR)defaults[8].ti_Data;
    process->pr_CIS = (BPTR)defaults[2].ti_Data;
    process->pr_COS = (BPTR)defaults[4].ti_Data;
    process->pr_CES = (BPTR)defaults[6].ti_Data;
    process->pr_Task.tc_UserData = (APTR)defaults[14].ti_Data;

/*  process->pr_ConsoleTask=; */
/*  process->pr_FileSystemTask=; */
    process->pr_CLI = MKBADDR(cli);
/*  process->pr_PktWait=; */
    process->pr_WindowPtr = (struct Window *)defaults[17].ti_Data; 
/*  process->pr_HomeDir=; */
    process->pr_Flags = (defaults[3].ti_Data  ? PRF_CLOSEINPUT  : 0) |
		        (defaults[5].ti_Data  ? PRF_CLOSEOUTPUT : 0) |
		        (defaults[7].ti_Data  ? PRF_CLOSEERROR  : 0) |
		        (defaults[13].ti_Data ? PRF_FREECLI     : 0) |
		        PRF_FREEARGS | PRF_FREESEGLIST | PRF_FREECURRDIR;
    process->pr_ExitCode = (APTR)defaults[15].ti_Data; 
    process->pr_ExitData = defaults[16].ti_Data; 
    process->pr_Arguments = argptr;

    if((BOOL)defaults[18].ti_Data)      /* NP_CopyVars */
    {
	/* We must have variables to copy... */
	if(me->pr_Task.tc_Node.ln_Type == NT_PROCESS)
	{
	    struct LocalVar *varNode;
	    struct LocalVar *newVar;

	    /* We use the same strategy as in the ***Var() functions */
	    ForeachNode((struct List *)&me->pr_LocalVars, 
			(struct Node *)varNode)
	    {
		LONG  copyLength = strlen(varNode->lv_Node.ln_Name) + 1 +
                                   sizeof(struct LocalVar);

		newVar = (struct LocalVar *)AllocVec(copyLength,
						     MEMF_PUBLIC | MEMF_CLEAR);
		ENOMEM_IF(newVar == NULL);
		
		CopyMem(varNode, newVar, copyLength);
		newVar->lv_Node.ln_Name = (char *)newVar +
		                                  sizeof(struct LocalVar);
		P(kprintf("Variable with name %s copied.\n", 
			  newVar->lv_Node.ln_Name));

		newVar->lv_Value = AllocMem(varNode->lv_Len, MEMF_PUBLIC);
		
		if(newVar->lv_Value == NULL)
		{
		    /* Free variable node before shutting down */
		    FreeVec(newVar);
		    ENOMEM_IF(newVar->lv_Value == NULL);
		}
		
		CopyMem(varNode->lv_Value, newVar->lv_Value, varNode->lv_Len);

		AddTail((struct List *)&process->pr_LocalVars,
			(struct Node *)newVar);
	    }
	}
    }

    process->pr_ShellPrivate = 0;

    if(AddProcess(process, argptr, argsize, 
		  defaults[0].ti_Data ?
		  (BPTR *)BADDR(defaults[0].ti_Data) + 1 :
		  (BPTR *)defaults[1].ti_Data,
		  KillCurrentProcess, DOSBase) != NULL)
	return process;

    /* Fall through */
enomem:
    if(me->pr_Task.tc_Node.ln_Type == NT_PROCESS)
	SetIoErr(ERROR_NO_FREE_STORE);

    freeLocalVars(process);

error:
    if(cli)
	FreeDosObject(DOS_CLI,cli);

    if(curdir)
	UnLock(curdir);

    if(output)
	Close(output);

    if(input)
	Close(input);

    if(argptr)
	FreeVec(argptr);

    if(memlist != NULL)
	FreeMem(memlist, sizeof(struct MemList) + 2*sizeof(struct MemEntry));

    if(name != NULL)
	FreeMem(name, namesize);

    if(stack != NULL)
	FreeMem(stack, defaults[9].ti_Data);

    if(process != NULL)
	FreeMem(process, sizeof(struct Process));

    return NULL;
    AROS_LIBFUNC_EXIT
} /* CreateNewProc */


static void KillCurrentProcess(void)
{
    /* I need the global here because there is no local way to get it */
    extern struct DosLibrary *DOSBase;
    struct Process *me = (struct Process *)FindTask(NULL);

    /* Call user defined exit function before shutting down. */
    if(me->pr_ExitCode != NULL)
	me->pr_ExitCode(me->pr_ExitData);

    P(kprintf("Deleting local variables\n"));

    /* Clean up */
    freeLocalVars(me);

    P(kprintf("Closing input stream\n"));

    if(me->pr_Flags & PRF_CLOSEINPUT)
	Close(me->pr_CIS);

    P(kprintf("Closing output stream\n"));

    if(me->pr_Flags & PRF_CLOSEOUTPUT)
	Close(me->pr_COS);

    P(kprintf("Closing error stream\n"));

    if(me->pr_Flags & PRF_CLOSEERROR)
	Close(me->pr_CES);

    P(kprintf("Freeing arguments\n"));

    if(me->pr_Flags & PRF_FREEARGS)
	FreeVec(me->pr_Arguments);

    P(kprintf("Unloading segment\n"));

    if(me->pr_Flags & PRF_FREESEGLIST)
	UnLoadSeg(me->pr_SegList);

    P(kprintf("Unlocking current dir\n"));

    if(me->pr_Flags & PRF_FREECURRDIR)
	UnLock(me->pr_CurrentDir);

    P(kprintf("Freeing cli structure\n"));

    if(me->pr_Flags & PRF_FREECLI)
	FreeDosObject(DOS_CLI,BADDR(me->pr_CLI));

    RemTask(NULL);
}


static void freeLocalVars(struct Process *process)
{
    struct LocalVar *varNode;
    struct Node     *tempNode;
    
    ForeachNodeSafe((struct List *)&process->pr_LocalVars,
		    (struct Node *)varNode, tempNode)
    {
	P(kprintf("Freeing variable %s with value %s at %p\n",
		  varNode->lv_Node.ln_Name, varNode->lv_Value, varNode));
	FreeMem(varNode->lv_Value, varNode->lv_Len);
	Remove((struct Node *)varNode);
	FreeVec(varNode);
    }
}
