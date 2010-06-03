/*
    Copyright � 1995-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Boot your operating system.
    Lang: english
*/

#define DEBUG 0
#include <aros/debug.h>

#include <aros/bootloader.h>
#include <exec/types.h>
#include <exec/alerts.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <exec/execbase.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <dos/dosextens.h>
#include <dos/filesystem.h>
#include <utility/tagitem.h>

#include <proto/bootloader.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "dosboot_intern.h"

void __dosboot_Boot(BOOL hidds_ok, APTR BootLoaderBase, ULONG Flags)
{
    LONG rc = RETURN_FAIL;
    BPTR cis = NULL;

    /*  We have been created as a process by DOS, we should now
    	try and boot the system. */

    struct DosLibrary *DOSBase;
    D(bug("[DOSBoot] __dosboot_Boot()\n")); 

    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
    if( DOSBase == NULL )
    {
    	/* BootStrap couldn't open dos.library */
    	Alert(AT_DeadEnd | AN_BootStrap | AG_OpenLib | AO_DOSLib );
    }

    if (hidds_ok)
	cis = Open("CON:20/20///Boot Shell/AUTO", FMF_READ);
    else
	kprintf("Failed to load system HIDDs\n");
    if (cis)
    {
        BPTR sseq = NULL;

        struct TagItem tags[] =
            {
                { SYS_Asynch,      TRUE       }, /* 0 */
                { SYS_Background,  FALSE      }, /* 1 */
                { SYS_Input,       (IPTR)cis  }, /* 2 */
                { SYS_Output,      (IPTR)NULL }, /* 3 */
                { SYS_Error,       (IPTR)NULL }, /* 4 */
                { SYS_ScriptInput, (IPTR)NULL }, /* 5 */
                { TAG_DONE,       0           }
            };

        D(bug("[DOSBoot] __dosboot_Boot: Open Startup Sequence = %d\n", opensseq));

        if (!(Flags & BF_NO_STARTUP_SEQUENCE))
        {
            sseq = Open("S:Startup-Sequence", FMF_READ);
            tags[5].ti_Data = (IPTR)sseq;
        }
        else
        {
            /* If poseidon is enabled, ensure that ENV: exists to avoid missing volume requester.
             * You could think we should check for this in bootmenu.resource because there we
             * select to boot without startup sequence but there might be other places to do this
             * selection in the future.
             */

            if (BootLoaderBase)
            {
                /* TODO: create something like ExistsBootArg("enableusb") in bootloader.resource */
                struct List *list = GetBootInfo(BL_Args);
                BOOL enable = FALSE;
                if (list)
                {
                    struct Node *node;
                    ForeachNode(list, node)
                    {
                        if (stricmp(node->ln_Name, "enableusb") == 0)
                        {
                            enable = TRUE;
                        }
                    }
                }
                
                if (enable)
                {
                    BPTR lock = CreateDir("RAM:ENV");
                    if (lock)
                        AssignLock("ENV", lock);
                }
            }

            tags[5].ti_Tag = TAG_IGNORE;
        }

        rc = SystemTagList("", tags);
        if (rc != -1)
        {
            cis  = NULL;
            sseq = NULL;
        }
        else
            rc = RETURN_FAIL;
        if (sseq != NULL)
            Close(sseq);
    } else
	kprintf("Cannot open boot console\n");

    /* We get here when the Boot Shell Window is left with EndShell/EndCli.
       There's no RemTask() here, otherwise the process cleanup routines
       are not called. And that would for example mean, that the
       Boot Process (having a CLI) is not removed from the rootnode.
       --> Dead stuff in there -> Crash
    */
}
