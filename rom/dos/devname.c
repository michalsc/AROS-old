/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: Get a device pointer from a filename.
    Lang: english
*/
#include <exec/memory.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filesystem.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <string.h>
#include "dos_intern.h"

LONG DevName(CONST_STRPTR name, struct Device **devptr, struct DosLibrary *DOSBase)
{
    LONG error = 0L;
    STRPTR volname;
    CONST_STRPTR s1 = NULL;
    struct DosList *dl;
    struct FileHandle *fh;
    struct Process *me = (struct Process *)FindTask(NULL);

    /* If file is "PROGDIR:" or relative to current directory, just return
       device from pr_HomeDir or pr_CurrentDir. */
    if (!Strnicmp(name, "PROGDIR:", 8))
    {
	*devptr = ((struct FileHandle *)BADDR(me->pr_HomeDir))->fh_Device;
	return 0L;
    } else if (*name == ':')
    {
	*devptr = ((struct FileHandle *)BADDR(me->pr_CurrentDir))->fh_Device;
	return 0L;
    }

    /* Copy volume name */
    s1 = name;
    volname = NULL;
    while (*s1)
    {
        if (*s1++ == ':')
        {
            volname = (STRPTR)AllocVec(s1-name, MEMF_ANY);
            if (!volname)
            {
                SetIoErr(ERROR_NO_FREE_STORE);
                return ERROR_NO_FREE_STORE;
            }
            CopyMem(name, volname, s1-name-1);
            volname[s1-name-1] = '\0';
            break;
        }
    }

    /* If path is relative to current directory, get device from pr_CurrentDir.
    */
    if (!volname)
    {
	*devptr = ((struct FileHandle *)BADDR(me->pr_CurrentDir))->fh_Device;
	return 0;
    }

    /* Get the device pointer from dos-list. */
    dl = LockDosList(LDF_ALL|LDF_READ);
    dl = FindDosEntry(dl, volname, LDF_ALL);
    if (dl)
    {
        if (dl->dol_Type == DLT_LATE)
        {
            /* Late binding assign: mount first */
            BPTR lock = Lock(dl->dol_misc.dol_assign.dol_AssignName, SHARED_LOCK);
            UnLockDosList(LDF_ALL|LDF_READ);
            dl = NULL;
            if (lock)
            {
                AssignLock(volname, lock);
                dl = LockDosList(LDF_ALL|LDF_READ);
                dl = FindDosEntry(dl, volname, LDF_ALL);
                if (dl)
                    *devptr = dl->dol_Device;
                else
                    error = ERROR_DEVICE_NOT_MOUNTED;
                UnLockDosList(LDF_ALL|LDF_READ);
            } else
                error = IoErr();

        } else if (dl->dol_Type == DLT_NONBINDING)
        {
            BPTR lock = Lock(dl->dol_misc.dol_assign.dol_AssignName, SHARED_LOCK);
            UnLockDosList(LDF_ALL|LDF_READ);
            fh = (struct FileHandle *)BADDR(lock);
            if (fh)
            {
                *devptr = fh->fh_Device;
                UnLock(lock);
            } else
                error = IoErr();
        } else
        {
            *devptr = dl->dol_Device;
            UnLockDosList(LDF_ALL|LDF_READ);
        }
    } else
    {
        UnLockDosList(LDF_ALL|LDF_READ);
        error = ERROR_DEVICE_NOT_MOUNTED;
    }

    FreeVec(volname);
    SetIoErr(error);
    return error;
} /* DevName */
