/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Add a DOS device to the system.
    Lang: English
*/
#include "expansion_intern.h"
#include <exec/io.h>
#include <dos/filesystem.h>
#include <proto/exec.h>
#include <proto/dos.h>

/*****************************************************************************

    NAME */
#include <dos/filehandler.h>
#include <dos/dosextens.h>
#include <libraries/expansion.h>
#include <proto/expansion.h>

	AROS_LH3(BOOL, AddDosNode,

/*  SYNOPSIS */
	AROS_LHA(LONG               , bootPri, D0),
	AROS_LHA(ULONG              , flags, D1),
	AROS_LHA(struct DeviceNode *, deviceNode, A0),

/*  LOCATION */
	struct ExpansionBase *, ExpansionBase, 25, Expansion)

/*  FUNCTION
	This is the old function for adding devices to the system. It
	is recommended that you use the AddBootNode() function.

	Unlike AddBootNode() you will have to add a BootNode to the
	system yourself.

    INPUTS
	bootPri     -   The priority of the device (-128 --> 127).
	flags       -   Flags (ADNF_STARTPROC etc)
	deviceNode  -   The device to add to the system.

    RESULT
	non-zero if everything succeeded, zero on failure.

    NOTES
	It is much better to use AddBootNode() as it will also
	construct the BootNode structure, and add it to the system.

    EXAMPLE
	//  Add a bootable disk to the system. This will start a
	//  file handler process immediately.

	if( AddDosNode( 0, ADNF_STARTPROC, MakeDosNode( paramPacket )))
	{
	    // AddDosNode() ok
	}

    BUGS

    SEE ALSO
	AddBootNode(), MakeDosNode()

    INTERNALS

    HISTORY
	19-05-07    sonic   Rewritten to use dos.library for starting up
			    a handler.
	27-11-96    digulla automatically created from
			    expansion_lib.fd and clib/expansion_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct DosLibrary *DOSBase;
    BOOL ok = FALSE;
    struct BootNode *bn;

    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);

    /* Aha, DOS is up and running... */
    if (DOSBase != NULL)
    {
	/* Due to race conditions while booting (dos becomes available), 
	   we will check the mount list if the entry is already there.
	   As long as we are in the boot process, leave the mounting to 
	   that process. Otherwise, mount it immediately. */
	ForeachNode(&ExpansionBase->MountList, bn)
	{
	    if(stricmp(AROS_BSTR_ADDR(((struct DeviceNode *) bn->bn_DeviceNode)->dn_Name), AROS_BSTR_ADDR(deviceNode->dn_Name)) == 0)
	    {
		// so there was already an entry with that DOS name.
		if(ExpansionBase->Flags & EBF_BOOTFINISHED)
		{
		    // well, just add it to the DOS List, the mount list won't be touched anymore anyway.
		    // we won't remove the duplicate entry though.
		} else {
		    // if that node is already the mount list, don't bother
		    CloseLibrary((struct Library *)DOSBase);
		    return FALSE;
		}
		break;
	    }
	}

	if(deviceNode->dn_SegList == BNULL && deviceNode->dn_Handler == BNULL && deviceNode->dn_Task == NULL)
	    deviceNode->dn_SegList = DOSBase->dl_Root->rn_FileHandlerSegment;

	/* We should add the filesystem to the DOS device list. It will
	   be usable from this point onwards.

	   The DeviceNode structure that was passed to us can be added
	   to the DOS list as it is, and we will let DOS start the
	   filesystem task if it is necessary to do so.
	*/

	ok = AddDosEntry((struct DosList *)deviceNode);
	/* Have we been asked to start a filesystem, and there is none already */
	if (flags & ADNF_STARTPROC)
	{
		STRPTR dosname = AROS_BSTR_ADDR(deviceNode->dn_Name);
		char namebuffer[32];
		char *tarptr = namebuffer;
		ULONG len = 30;

    	/* append a colon to the name, DeviceProc() needs a full path */
    	while((*tarptr++ = *dosname++) && (--len));
    	if(tarptr[-1] == 0) --tarptr;
    	*tarptr++ = ':';
    	*tarptr = 0;

	    /* Yes, better do so.

	       DeviceProc() will see that dn_Device for this node is NULL
	       and start up the handler. */
	    DeviceProc(namebuffer);
	}

	CloseLibrary((struct Library *)DOSBase);
    }
    else
    {
    	/*
    	 * CHECKME: Is this correct? V36 autodocs say this
    	 * (http://cataclysm.cx/random/amiga/reference/Includes_and_Autodocs_2._guide/node03B0.html):
    	 *
    	 * Before V36 Kickstart, no function existed to add BOOTNODES.
	 * If an older expansion.library is in use, driver code will need
         * to manually construct a BootNode and Enqueue() it to eb_Mountlist.
         * If you have a V36 or better expansion.library, your code should
	 * use AddBootNode().
	 *
	 * Perhaps we should just fail here, and the driver has already constructed a BootNode
	 * before calling AddDosEntry() ?
	 */
    	return AddBootNode(bootPri, flags, deviceNode, NULL);
    }
    return ok;

    AROS_LIBFUNC_EXIT
} /* AddDosNode */
