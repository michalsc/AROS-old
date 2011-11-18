#ifndef DEBUG_H
#define DEBUG_H

#ifdef __AROS__

#ifdef KS13WRAPPER
#include "ks13wrapper.h"

#if KS13WRAPPER_DEBUG

#define DB(x)
#define Trace(x)
#define ENTER(x) do { DebugPutStr("ENTER: "); DebugPutStr(x); DebugPutStr("\n"); } while(0);
#define EXIT(x) do { DebugPutStr("EXIT: "); DebugPutStr(x); DebugPutStr("\n"); } while(0);

#else

#define DebugPutStr(x)
#define DebugPutHex(x,y)
#define DB(x)
#define Trace(x)
#define ENTER(x)
#define EXIT(x)

#endif

#endif

#else

#if defined(__SASC) && defined(USE_GLOBALDEBUG)
#include <debug/debug.h>
#else
#define DB(x)
#define Trace(x)
#define ENTER(x)
#define EXIT(x)
#endif

#endif

#endif /* DEBUG_H */
