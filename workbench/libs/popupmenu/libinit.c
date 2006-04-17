#define  _USEOLDEXEC_ 1

#include <exec/types.h>
#include <exec/exec.h>
#include <exec/execbase.h>
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>

#include <libraries/pm.h>

/* Prototypes */
ULONG __asm _LibExpunge( register __a6 struct PopupMenuBase *libbase );
ULONG __asm _LibInit   ( register __a0 APTR seglist,
                         register __d0 struct PopupMenuBase *libbase );

int  __saveds __asm __UserLibInit   (register __a6 struct PopupMenuBase *libbase);
void __saveds __asm __UserLibCleanup(register __a6 struct PopupMenuBase *libbase);

int  __saveds __asm __UserDevInit   (register __d0 long unit,
                                     register __a0 struct IORequest *ior,
                                     register __a6 struct PopupMenuBase *libbase);
void __saveds __asm __UserDevCleanup(register __a0 struct IORequest *ior,
                                     register __a6 struct PopupMenuBase *libbase);

int  __saveds __asm __libfpinit     (register __a6 struct PopupMenuBase *libbase);
void __saveds __asm __libfpterm     (register __a6 struct PopupMenuBase *libbase);


typedef LONG (*myPFL)();   /* pointer to function returning 32-bit int      */

/* library initialization table, used for AUTOINIT libraries                */
struct InitTable {
        ULONG        *it_DataSize;       /* library data space size         */
        myPFL        *it_FuncTable;      /* table of entry points           */
        APTR         it_DataInit;        /* table of data initializers      */
        myPFL        it_InitFunc;        /* initialization function to run  */
};

/* symbols generated by blink */
extern char __far _LibID[];             /* ID string                        */
extern char __far _LibName[];           /* Name string                      */
extern char __far RESLEN;               /* size of init data                */
extern long __far NEWDATAL;             /* size of global data              */
extern long __far NUMJMPS;              /* number of jmp vectors to copy    */
extern myPFL _LibFuncTab[];             /* my function table                */
extern long __far _LibVersion;          /* Version of library               */
extern long __far _LibRevision;         /* Revision of library              */
#define MYVERSION ((long)&_LibVersion)
#define MYREVISION ((long)&_LibRevision)
#define DATAWORDS ((long)&NEWDATAL)     /* magic to get right tpye of reloc */ 
#define SIZEJMPTAB ((long)libbase->pmb_origbase->pmb_numjmps)
                                        /* size in bytes of jmp table       */

/* From libent.o, needed to determine where data is loaded by loadseg       */
extern long far _Libmergeddata; 

#define MYLIBRARYSIZE ((sizeof(struct PopupMenuBase) +3) & ~3)



struct InitTable __far _LibInitTab =  {
        (long *)(&RESLEN+MYLIBRARYSIZE),
        _LibFuncTab,
        NULL,                        /* will initialize my own data */
        _LibInit,
};

__asm ULONG _LibInit( register __a0 APTR seglist,
                      register __d0 struct PopupMenuBase *libbase )
{
    long *reloc;
    long *sdata;
    char *ddata;
    long nrelocs;

    
    libbase->pmb_SegList = (ULONG) seglist;

    /* init. library structure (since I don't do automatic data init.) */
    libbase->pmb_Library.lib_Node.ln_Type = NT_LIBRARY;
    libbase->pmb_Library.lib_Node.ln_Name =  _LibName;
    libbase->pmb_Library.lib_Node.ln_Succ =  NULL;
    libbase->pmb_Library.lib_Node.ln_Pred =  NULL;
    libbase->pmb_Library.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    libbase->pmb_Library.lib_Version = MYVERSION;
    libbase->pmb_Library.lib_Revision = MYREVISION;
    libbase->pmb_Library.lib_IdString = (APTR) _LibID;

     /* Start of copy of global data after structure */
    ddata = (char *)libbase + MYLIBRARYSIZE; 

    sdata = (long *)&_Libmergeddata; /* where loadseg loaded the data */
    memcpy(ddata, (void *)sdata, DATAWORDS*4);

    /* perform relocs if we want one global section for all programs */
    /* that have this lib open. If we want a global section for each */
    /* open, copy the relocs, and do them on each open call.         */
    sdata = sdata + DATAWORDS;
    nrelocs = *sdata;
    sdata++;
    while (nrelocs > 0)
    {
       reloc = (long *)((long)ddata + *sdata++);
       *reloc += (long)ddata;
       nrelocs--;
    }
    
    if (__UserLibInit(libbase) != 0)
       return NULL; /* abort if user init failed */

    return ( (ULONG) libbase );
}

LONG __asm _LibOpen( 
                     register __a6 struct PopupMenuBase *libbase )
{
    /* mark us as having another customer */
    libbase->pmb_Library.lib_OpenCnt++;

    /* clear delayed expunges (standard procedure) */
    libbase->pmb_Library.lib_Flags &= ~LIBF_DELEXP;

    return ( (LONG) libbase );
}

ULONG __asm _LibClose( 
                       register __a6 struct PopupMenuBase *libbase )
{
    ULONG retval = 0;
    
    if (( --libbase->pmb_Library.lib_OpenCnt == 0 ) &&
                        ( libbase->pmb_Library.lib_Flags & LIBF_DELEXP ))
    {
        /* no more people have me open,
         * and I have a delayed expunge pending
         */
         retval = _LibExpunge( libbase ); /* return segment list        */
    }

    return (retval);
}

ULONG __asm _LibExpunge( register __a6 struct PopupMenuBase *libbase )
{
    ULONG seglist = 0;
    LONG  libsize;

    libbase->pmb_Library.lib_Flags |= LIBF_DELEXP;
    if ( libbase->pmb_Library.lib_OpenCnt == 0 )
    {
        /* really expunge: remove libbase and freemem        */
        __UserLibCleanup(libbase);
	//KPutStr("libcleanup done\n");

        seglist = libbase->pmb_SegList;

	//kprintf("Removing: %08lx (%s)\n", libbase, libbase->lib_Node.ln_Name);
	//kprintf("SysBase: %08lx\n", SysBase);
	SysBase=*((struct ExecBase **)4);
	//kprintf("SysBase: %08lx\n", SysBase);
        Remove( (struct Node *) libbase);
	//KPutStr("Removed library\n");

        libsize = libbase->pmb_Library.lib_NegSize + libbase->pmb_Library.lib_PosSize;
        FreeMem( (char *) libbase - libbase->pmb_Library.lib_NegSize,(LONG) libsize );
    }

    /* return NULL or real seglist                                */
    return ( (ULONG) seglist );
}
