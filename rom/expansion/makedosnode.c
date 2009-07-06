/*
    Copyright � 1995-2007, The AROS Development Team. All rights reserved.
    $Id$

    Desc: MakeDosNode() - Create a DOS DeviceNode structure.
    Lang: English
*/

#include "expansion_intern.h"
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <string.h>

# define  DEBUG 0
# include <aros/debug.h>

/*****************************************************************************

    NAME */
#include <dos/filehandler.h>
#include <proto/expansion.h>

	AROS_LH1(struct DeviceNode *, MakeDosNode,

/*  SYNOPSIS */
	AROS_LHA(APTR, parmPacket, A0),

/*  LOCATION */
	struct ExpansionBase *, ExpansionBase, 24, Expansion)

/*  FUNCTION
	MakeDosNode() will create a DeviceNode structure suitable for
	passing to dos.library which contains all the information about
	a device stored in the parmPacket array. This will allow you to
	enter a DOS device into the system from the information contained
	in a DosEnvec structure (such as in a RigidDiskBlock PartitionBlock
	structure).

	MakeDosNode() will allocate the memory that it needs to construct
	the DeviceNode, the strings and a FileSysStartupMsg that is passed
	to the filesystem handler on startup.

	You can use AddBootNode() to add a node to the system.

    INPUTS
	parmPacket  -   an IPTR array containing the device parameters
			required to initialize the structures. This is a
			variable length structure. See also the DosEnvec
			structure in dos/filehandler.h

	    Index       Description
	    --------    -----------
	    0           Exec string with dos device name (eg. DH0)
	    1           Exec string with exec device name (eg. fdsk.device)
	    2           unit number of device to open
	    3           flags (for OpenDevice())
	    4           length of the remaining data
	    5-n         environment data - consists of:

	    5           Size of standard device block in 32 bit longwords
	    6           not used; 0
	    7           # of heads - drive specific
	    8           # of sectors per block - not used; 0
	    9           # of blocks per track - drive specific
	    10          # of reserved blocks at the start of the partition
	    11          # of reserved blocks at the end of the partition
	    12          device interleave
	    13          starting cylinder of partition
	    14          end cylinder of partition
	    15          initial number of buffers
	    16          type of memory for buffers (CHIP, FAST,...)
	    17          max number of bytes to transfer at one time
	    18          address mask allowable for DMA transfers
	    19          boot priority for autobootable devices
	    20          standard DOS filesystem ID (eg 'DOS\1')
	    21          baud rate for serial handler
	    22          control word for handler/filesystem
	    23          number of boot blocks on this partition

    RESULT
	deviceNode  -   An initialized DeviceNode structure, or NULL if
			the required memory could not be allocated. The
			caller will have to modify this structure before
			passing it to AddBootNode().

    NOTES
	There are a number of fields of the DeviceNode structure that this
	function cannot initialize due to a lack of information. You
	should fill these in yourself.

    EXAMPLE

    BUGS

    SEE ALSO
	AddBootNode(), AddDosNode(), dos.library/MakeDosEntry()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct DeviceNode *dn;
    struct FileSysStartupMsg *fssm;
    struct DosEnvec *de;
    struct DosEnvec *newde;
    ULONG  desize;
    STRPTR s1, s2 = 0;
    BSTR   bs1, bs2;
    int    strLen1, strLen2, sz1, sz2;
    int    i;			/* Loop variable */

    if (parmPacket == NULL)
    {
	return NULL;
    }

    /* This is the environment structure */
    de = (struct DosEnvec *)((IPTR *)parmPacket + 4);
    desize = sizeof(IPTR) * (de->de_TableSize + 1);
    newde = AllocMem(desize, MEMF_CLEAR | MEMF_PUBLIC);
    if(de == NULL)
    {
        return NULL;
    }
    CopyMemQuick(de, newde, desize);
        
    dn = AllocMem(sizeof(struct DeviceNode), MEMF_CLEAR | MEMF_PUBLIC);
    
    if (dn == NULL)
    {
	FreeMem(newde, desize);
	return NULL;
    }
    
    fssm = AllocMem(sizeof(struct FileSysStartupMsg),
		    MEMF_CLEAR | MEMF_PUBLIC);
    
    if (fssm == NULL)
    {
	FreeMem(newde, desize);
	FreeMem(dn, sizeof(struct DeviceNode));
	
	return NULL;
    }
    
    /* To help prevent fragmentation I will allocate both strings in the
       same block of memory.
    */

    strLen1 = strlen((STRPTR)((IPTR *)parmPacket)[0]);
    /* Round size to alloc to nearest mutiple of 4 */
    sz1 = (AROS_BSTR_MEMSIZE4LEN(strLen1) + 3) & ~3;

    /* There doesn't have to exist an underlying block device */
    if ((STRPTR)((IPTR *)parmPacket)[1] != NULL)
    {
	strLen2 = strlen((STRPTR)((IPTR *)parmPacket)[1]);
    }
    else
    {
	strLen2 = 0;
    }
    sz2 = AROS_BSTR_MEMSIZE4LEN(strLen2);

    s1 = AllocVec(sz1 + sz2, MEMF_CLEAR | MEMF_PUBLIC);
    
    if (s1 == NULL)
    {
	FreeMem(newde, desize);
	FreeMem(dn, sizeof(struct DeviceNode));
	FreeMem(fssm, sizeof(struct FileSysStartupMsg));
	
	return NULL;
    }
    
    /* We have no more allocations */
    s2 = s1 + sz1;

    bs1 = MKBADDR(s1);
    bs2 = MKBADDR(s2);
    
    for (i = 0; i < strLen1; i++)
    {
	AROS_BSTR_putchar(bs1, i, ((STRPTR)((IPTR *)parmPacket)[0])[i]);
    }

    for (i = 0; i < strLen2; i++)
    {
	AROS_BSTR_putchar(bs2, i, ((STRPTR)((IPTR *)parmPacket)[1])[i]);
    }
    
    AROS_BSTR_setstrlen(bs1, strLen1 > 255 ? 255 : strLen1);
    AROS_BSTR_setstrlen(bs2, strLen2 > 255 ? 255 : strLen2);

    /* Strings are done, now the FileSysStartupMsg */
    fssm->fssm_Unit = ((IPTR *)parmPacket)[2];
    fssm->fssm_Device = bs2;
    fssm->fssm_Environ = MKBADDR(newde);
    fssm->fssm_Flags = ((IPTR *)parmPacket)[3];
    
    /* FSSM is done, now the DeviceNode */
    dn->dn_Startup = MKBADDR(fssm);
    dn->dn_Type = DLT_DEVICE;
    dn->dn_Name = bs1;
    dn->dn_Ext.dn_AROS.dn_DevName = AROS_BSTR_ADDR(bs1);

    return dn;

    AROS_LIBFUNC_EXIT
} /* MakeDosNode */
