#include <aros/kernel.h>
#include <proto/exec.h>

#include "../kernel/hostinterface.h"
#include "exec_intern.h"
#include "exec_util.h"

/*
 * Note that we are called very early, so no exec calls here! We don't have
 * ExecBase's functions table yet, only empty data structure!
 */
BOOL Exec_PreparePlatform(struct Exec_PlatformData *pd, struct TagItem *msg)
{
    struct TagItem *tag;
    struct HostInterface *HostIFace;
    void *KernelLib, *UserLib;
    APTR  __stdcall (*GetCurrentProcess)(void);

    tag = Exec_FindTagItem(KRN_HostInterface, msg);
    if (!tag)
    	return FALSE;

    HostIFace = (struct HostInterface *)tag->ti_Data;

    KernelLib = HostIFace->hostlib_Open("kernel32.dll", NULL);
    if (!KernelLib)
	return FALSE;

    UserLib = HostIFace->hostlib_Open("user32.dll", NULL);
    if (!UserLib)
	return FALSE;

    pd->ExitProcess = HostIFace->hostlib_GetPointer(KernelLib, "ExitProcess", NULL);
    if (!pd->ExitProcess)
	return FALSE;

    pd->FlushInstructionCache = HostIFace->hostlib_GetPointer(KernelLib, "FlushInstructionCache", NULL);
    if (!pd->FlushInstructionCache)
	return FALSE;

    pd->MessageBox = HostIFace->hostlib_GetPointer(UserLib, "MessageBoxA", NULL);
    if (!pd->MessageBox)
	return FALSE;

    GetCurrentProcess = HostIFace->hostlib_GetPointer(KernelLib, "GetCurrentProcess", NULL);
    if (!GetCurrentProcess)
	return FALSE;

    pd->Reboot = HostIFace->Reboot;
    pd->MyProcess = GetCurrentProcess();

    return TRUE;
}
