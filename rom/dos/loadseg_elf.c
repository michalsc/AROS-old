/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: Code to dynamically load ELF executables
    Lang: english
*/
#include <exec/memory.h>
#include <proto/exec.h>
#include <dos/dosasl.h>
#include <proto/dos.h>
#include <proto/aros.h>
#include "dos_intern.h"
#include <aros/debug.h>

extern struct DosLibrary * DOSBase;

#define SHT_PROGBITS	1
#define SHT_SYMTAB	2
#define SHT_STRTAB	3
#define SHT_RELA	4
#define SHT_NOBITS	8
#define SHT_REL 	9

#define ET_REL		1

#define EM_386		3
#define EM_68K		4

#define R_386_32	1
#define R_386_PC32	2
#define R_68K_32	1
#define R_68K_PC32	4

#define STT_OBJECT	1
#define STT_FUNC	2

#define ELF32_ST_TYPE(i)    ((i) & 0x0F)

struct elfheader
{
    UBYTE ident[16];
    UWORD type;
    UWORD machine;
    ULONG version;
    APTR  entry;
    ULONG phoff;
    ULONG shoff;
    ULONG flags;
    UWORD ehsize;
    UWORD phentsize;
    UWORD phnum;
    UWORD shentsize;
    UWORD shnum;
    UWORD shstrndx;
};

struct sheader
{
    ULONG name;
    ULONG type;
    ULONG flags;
    APTR  addr;
    ULONG offset;
    ULONG size;
    ULONG link;
    ULONG info;
    ULONG addralign;
    ULONG entsize;
};

struct symbol
{
    ULONG name;     /* Offset of the name string in the string table */
    ULONG value;    /* Varies; eg. the offset of the symbol in its hunk */
    ULONG size;     /* How much memory does the symbol occupy */
    UBYTE info;     /* What kind of symbol is this ? (global, variable, etc) */
    UBYTE other;    /* undefined */
    WORD  shindex;  /* In which section is the symbol defined ? */
};

struct relo
{
    ULONG addr;     /* Address of the relocation (relative to the last loaded hunk) */
    ULONG info;     /* Type of the relocation */
#ifdef __mc68000__
    LONG  addend;   /* Constant addend used to compute value */
#endif
};

struct hunk
{
    ULONG   size;   /* Size of the hunk */
    UBYTE * memory; /* First byte */
};

int read_block (BPTR file, ULONG offset, APTR buffer, ULONG size)
{
    LONG    subsize;
    UBYTE * buf     = (UBYTE *)buffer;

    if (Seek (file, offset, OFFSET_BEGINNING) < 0)
	return 1;

    while (size)
    {
	subsize = Read (file, buf, size);

	if (subsize == 0)
	{
	    ((struct Process *)FindTask (NULL))->pr_Result2 = ERROR_BAD_HUNK;
	    return 1;
	}

	if (subsize < 0)
	    return 1;

	buf  += subsize;
	size -= subsize;
    }

    return 0;
} /* read_block */

BPTR LoadSeg_ELF (BPTR file)
{
    struct elfheader eh;
    UBYTE	   * shtab    = NULL;
    UBYTE	   * shstrtab = NULL;
    UBYTE	   * strtab   = NULL;
    struct symbol  * symtab   = NULL;
    struct hunk    * hunks    = NULL;
    struct relo    * reltab   = NULL;
    struct symbol  * symbol;
    struct sheader * sh;

    ULONG   numsym,
	    numrel,
	    i;
    WORD    t,
	    mint   = 0,
	    maxt   = 0;
    UBYTE * loaded;
    BPTR    last   = 0;
    LONG  * error  = &((struct Process *)FindTask (NULL))->pr_Result2;

#define ERROR(a)    { *error = a; goto end; }

    /* Load the header */
    if (read_block (file, 0, &eh, sizeof (eh)))
	goto end;

    /* Check the header of the file */
    if
    (
	eh.ident[0] != 0x7f
	|| eh.ident[1] != 'E'
	|| eh.ident[2] != 'L'
	|| eh.ident[3] != 'F'
    )
	ERROR (ERROR_NOT_EXECUTABLE);

    /* Check file type and the CPU the file is for */
    if (eh.type != ET_REL || (eh.machine != EM_386 && eh.machine != EM_68K))
	ERROR (ERROR_OBJECT_WRONG_TYPE);

    /* Get memory for section headers */
    shtab = AllocVec (eh.shentsize * eh.shnum, MEMF_ANY);

    if (shtab == NULL)
	ERROR (ERROR_NO_FREE_STORE);

    /* Read section table */
    if (read_block(file, eh.shoff, shtab, eh.shentsize * eh.shnum))
	goto end;

    /* Look up the symbol table */
    for (t=1; t<eh.shnum; t++)
    {
	sh = (struct sheader *)(shtab + t*eh.shentsize);

	if (sh->type == SHT_SYMTAB)
	    break;
    }

    if (t == eh.shnum)
	ERROR (ERROR_OBJECT_WRONG_TYPE);

    /* Allocate memory for the symbol table */
    symtab = AllocVec (sh->size, MEMF_ANY);

    if (symtab == NULL)
	ERROR (ERROR_NO_FREE_STORE);

    /* Read the symbol table */
    if (read_block (file, sh->offset, symtab, sh->size))
	goto end;

    numsym = sh->size / sizeof (struct symbol);

    mint = maxt = symtab[0].shindex;

/* kprintf ("Symbol %d: index=%d\n", 0, symtab[0].shindex); */

    /* Find the minimal number of hunks which are neccessary to satisfy
       all symbol references (ie. all hunks in which a symbol resides) */
    for (i=1; i<numsym; i++)
    {
/* kprintf ("Symbol %d: index=%d\n", i, symtab[i].shindex); */

	if (symtab[i].shindex < mint)
	    mint = symtab[i].shindex;

	if (symtab[i].shindex > maxt)
	    maxt = symtab[i].shindex;
    }

    /* Allocate memory for information about every hunk */
    hunks = AllocVec (sizeof (struct hunk) * (maxt - mint + 1), MEMF_CLEAR);

    if (hunks == NULL)
	ERROR (ERROR_NO_FREE_STORE);

    /* Offset the base. Now I can simply access the first hunk as
	hunks[t] instead of hunks[t-mint] */
    hunks -= mint;

    /* Load names of sections */
    if (eh.shstrndx)
    {
	sh = (struct sheader *)(shtab + eh.shstrndx*eh.shentsize);

	shstrtab = AllocVec (sh->size, MEMF_ANY);

	if (shstrtab == NULL)
	    ERROR (ERROR_NO_FREE_STORE);

	if (read_block (file, sh->offset, shstrtab, sh->size))
	    goto end;

	/* {
	    int n, t;

	    for (n=t=0; t<sh->size; n++)
	    {
		kprintf ("String %d@%d: \"%s\"\n", n, t, &shstrtab[t]);
		t += strlen (&shstrtab[t]) + 1;
	    }
	} */
    }

    /* Find the basic size for each hunk */
    for (t=1; t<eh.shnum; t++)
    {
	sh = (struct sheader *)(shtab + t*eh.shentsize);

	if (sh->type == SHT_PROGBITS || sh->type == SHT_NOBITS)
	{
	    /* Don't load debug hunks */
	    /* if (!strncmp (".stab", &shstrtab[sh->name], 5)
		|| !strcmp (".comment", &shstrtab[sh->name])
	    )
		sh->size = 0; */

	    hunks[t].size = sh->size;
	}
    }

    /* Look for names of symbols */
    for (t=eh.shnum; t>0; t--)
    {
	sh = (struct sheader *)(shtab + t*eh.shentsize);

	if (sh->type == SHT_STRTAB)
	    break;
    }

    /* Found the section with the names ? Load the symbols' names */
    if (t)
    {
	strtab = AllocVec (sh->size, MEMF_ANY);

	if (strtab == NULL)
	    ERROR (ERROR_NO_FREE_STORE);

	/* kprintf ("Reading StrTab at %d (offset=%ld, size=%ld)\n", eh.shstrndx, sh->offset, sh->size); */

	if (read_block (file, sh->offset, strtab, sh->size))
	    goto end;

	/*{
	    int n, t;

	    for (n=t=0; t<sh->size; n++)
	    {
		kprintf ("String %d@%d: \"%s\"\n", n, t, &strtab[t]);
		t += strlen (&strtab[t]) + 1;
	    }
	} */
    }

    /* kprintf ("    File has %d sections.\n", eh.shnum); */

    /* Reserve memory for each global symbol in its hunk */
    for (i=0; i<numsym; i++)
    {
	if (symtab[i].shindex < 0)
	{
	    symtab[i].value = hunks[symtab[i].shindex].size;

	    hunks[symtab[i].shindex].size += symtab[i].size;
	}
    }

    /* Allocate memory for each segment */
    for (t=mint; t<=maxt; t++)
    {
	/* Don't allocate memory for hunks which don't need any */
	if (hunks[t].size)
	{
	    hunks[t].memory = AllocVec (hunks[t].size + sizeof (BPTR), MEMF_CLEAR);

	    if (hunks[t].memory == NULL)
		ERROR (ERROR_NO_FREE_STORE);

	    hunks[t].memory += sizeof(BPTR);

D(bug("   Hunk %3d: 0x%p - 0x%p\n", t, hunks[t].memory, hunks[t].memory+hunks[t].size));
	}
    }

    /* Show the final addresses of global symbols */
    if (strtab)
    {
	for (i=0; i<numsym; i++)
	{
	    /* Print the symbol if it has a name and if it's a variable
		or function */
	    if (strtab[symtab[i].name]
		&& (
		    ELF32_ST_TYPE(symtab[i].info) == STT_OBJECT
		    || ELF32_ST_TYPE(symtab[i].info) == STT_FUNC
		)
	    )
	    {
		if (!strcmp ("entry", &strtab[symtab[i].name]))
		{
		    kprintf ("    Symbol at 0x%p: %s\n"
			, hunks[symtab[i].shindex].memory + symtab[i].value
			, &strtab[symtab[i].name]
		    );

		    /* Print only this symbol */
		    break;
		}
	    }
	}
    }

    loaded = NULL;

    /* Now load the pieces into memory */
    for (t=1; t<eh.shnum; t++)
    {
	sh = (struct sheader *)(shtab + t*eh.shentsize);

	if (!sh->size)
	    continue;

	switch(sh->type)
	{
	case SHT_PROGBITS: /* Code */
	    if (read_block (file, sh->offset, hunks[t].memory, sh->size))
		goto end;

	    loaded = hunks[t].memory;

	    if (strtab
	       /* && (
		    !strcmp (".text", &shstrtab[sh->name])
		    || !strcmp (".rodata", &shstrtab[sh->name])
		    || !strcmp (".data", &shstrtab[sh->name])
		) */
	    )
	    {
		kprintf ("    Section at 0x%p ... 0x%p: %s\n"
		    , loaded
		    , loaded + sh->size - 1
		    , &shstrtab[sh->name]
		);
	    }

	    break;

	case SHT_RELA:
	case SHT_REL: /* Relocation table */
	    if (loaded == NULL)
		ERROR (ERROR_OBJECT_WRONG_TYPE);

	    /* Get memory for the relocation table */
	    reltab = AllocVec (sh->size, MEMF_ANY);

	    if (reltab == NULL)
		ERROR (ERROR_NO_FREE_STORE);

	    /* Load it */
	    if (read_block (file, sh->offset, reltab, sh->size))
		goto end;

	    numrel = sh->size / sizeof (struct relo);

	    /* For each relocation ... */
	    for (i=0; i<numrel; i++)
	    {
		symbol = &symtab[reltab[i].info >> 8];

		switch (reltab[i].info & 0xFF)
		{
#ifdef __i386__
		case R_386_32: /* 32bit absolute */
		    /* The address of a symbol is the base address of the
			hunk in which the symbol is plus the offset of the
			symbol to the beginning of this hunk. */
		    *(ULONG *)&loaded[reltab[i].addr] +=
			(ULONG)hunks[symbol->shindex].memory + symbol->value;
		    break;

		case R_386_PC32: /* 32bit PC relative */
		    /* Similar to R_386_32 but relative to the address where
			the relocation is in memory. */
		    *(ULONG *)&loaded[reltab[i].addr] +=
			(ULONG)hunks[symbol->shindex].memory +
			symbol->value - (ULONG)&loaded[reltab[i].addr];
		    break;
#endif
#ifdef __mc68000__
		case R_68K_32:
		    *(ULONG *)&loaded[reltab[i].addr] =
			(ULONG)hunks[symbol->shindex].memory + symbol->value +
			reltab[i].addend;
		    break;
		case R_68K_PC32:
		    *(ULONG *)&loaded[reltab[i].addr] =
			((ULONG)hunks[symbol->shindex].memory+ symbol->value +
			reltab[i].addend - (ULONG)&loaded[reltab[i].addr]);
		    break;
#endif

		default:
		    ERROR (ERROR_BAD_HUNK);
		} /* switch */
	    } /* for */

	    /* Release memory */
	    FreeVec (reltab);
	    reltab = NULL;
	    loaded = NULL;

	    break;
	} /* switch */
    }

    /* Link hunks (and flush caches) */
    for (t=mint; t<0; t++)
    {
	if (hunks[t].size)
	{
	    CacheClearE(hunks[t].memory, hunks[t].size,
		CACRF_ClearI|CACRF_ClearD);
	    ((BPTR *)hunks[t].memory)[-1] = last;
	    last = MKBADDR((BPTR *)hunks[t].memory - 1);
	}
    }

    for (t=maxt; t>=0; t--)
    {
	if (hunks[t].size)
	{
	    CacheClearE(hunks[t].memory, hunks[t].size,
		CACRF_ClearI|CACRF_ClearD);
	    ((BPTR *)hunks[t].memory)[-1] = last;
	    last = MKBADDR((BPTR *)hunks[t].memory-1);
	}
    }

    /* Free hunk information table */
    FreeVec (hunks+mint);

    hunks = NULL;

end:
    FreeVec (reltab);

    /* Free all hunks, too ? */
    if (hunks != NULL)
    {
	for (t=mint; t<=maxt; t++)
	{
	    if (hunks[t].memory != NULL)
		FreeVec (hunks[t].memory - sizeof(BPTR));
	}

	FreeVec (hunks + mint);

	/* Fail */
	last = NULL;
    }

    if (shstrtab)
	FreeVec (shstrtab);

    if (strtab)
	FreeVec (strtab);

    FreeVec (symtab);
    FreeVec (shtab);

    return last;
} /* LoadSeg_ELF */
