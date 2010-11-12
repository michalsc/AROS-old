#include <aros/config.h>
#include <exec/lists.h>

#include <stdarg.h>
#include <stdio.h>

#include "hostinterface.h"

#include "hostlib.h"
#include "shutdown.h"

#if AROS_MODULES_DEBUG
/* gdb hooks from which it obtains modules list */

/* This is needed in order to bring in definition of struct segment */
#include "../../../rom/kernel/debug_intern.h"

struct segment *seg = NULL;
struct MinList *Debug_ModList = NULL;
#endif

/*
 * Redirect debug output to stderr. This is especially
 * needed on iOS where reading stdout is only possible with
 * remote gdb, which is tied to XCode's own build system.
 * On other unixes this won't hurt either.
 */
static int VKPrintf(const char *format, va_list ap)
{
    return vfprintf(stderr, format, ap);
}

/*
 * Some helpful functions that link us to the underlying host OS.
 * Without them we would not be able to estabilish any interaction with it.
 */
static struct HostInterface _HostIFace = {
    AROS_ARCHITECTURE,
    HOSTINTERFACE_VERSION,
    Host_HostLib_Open,
    Host_HostLib_Close,
    Host_HostLib_GetPointer,
    VKPrintf,
    Host_Shutdown,
#if AROS_MODULES_DEBUG
    &Debug_ModList
#else
    NULL
#endif
};

void *HostIFace = &_HostIFace;
