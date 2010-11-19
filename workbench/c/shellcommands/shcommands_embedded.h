#ifndef SHCOMMANDS_EMBEDDED_H
#define SHCOMMANDS_EMBEDDED_H

/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <dos/dos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <aros/symbolsets.h>

#define _stringify(x) #x
#define stringify(x) _stringify(x)

#define SHArg(name) (*(SHA_##name##_type *)&__shargs[SHA_##name])
#define SHArgLine() __argstr

#define __SHA_ENUM(type, abbr, name, modf, def, help) SHA_##name
#define __SHA_DEF(type, abbr, name, modf, def, help) (IPTR)(def)
#define __SHA_OPT(type, abbr, name, modf, def, help) \
     stringify(abbr) stringify(name) stringify(modf)
#define __SHA_TYPEDEF(type, abbr, name, modf, def, help) \
     typedef type SHA_##name##_type

DECLARESET(SHCOMMANDS)


struct shcommand
{
    STRPTR      sh_Name;
    IPTR        sh_Command;
    ULONG       sh_NumArgs;
    STRPTR      sh_Template;
};

#define __AROS_SH_ARGS(name, numargs, defl, templ, help)       \
static ULONG name##_main(CONST_STRPTR , IPTR *,                \
                         struct ExecBase *SysBase,             \
                         struct DosLibrary *);                 \
AROS_UFP3S(LONG, name##_entry,                                 \
    AROS_UFPA(char *,argstr,A0),                               \
    AROS_UFPA(ULONG,argsize,D0),                               \
    AROS_UFPA(struct ExecBase *,SysBase,A6)                    \
);                                                             \
static const struct shcommand __##name##_##shcommand =         \
{                                                              \
    .sh_Name = stringify(name),                                \
    .sh_Command = (IPTR)name##_entry,                          \
    .sh_NumArgs = numargs,                                     \
    .sh_Template = templ                                       \
};                                                             \
                                                               \
ADD2SET(__##name##_##shcommand, SHCOMMANDS, 0);                \
							       \
AROS_UFH3S(LONG, name##_entry,                                 \
    AROS_UFHA(char *,argstr,A0),                               \
    AROS_UFHA(ULONG,argsize,D0),                               \
    AROS_UFHA(struct ExecBase *,SysBase,A6)                    \
)                                                              \
{                                                              \
    AROS_USERFUNC_INIT                                         \
							       \
    APTR DOSBase;                                              \
                                                               \
    LONG __retcode = RETURN_FAIL;                              \
    IPTR __shargs[numargs] = defl;                             \
    struct RDArgs *__rda  = NULL;                              \
    struct RDArgs *__rda2 = NULL;                              \
							       \
    DOSBase = OpenLibrary(DOSNAME, 0);                         \
                                                               \
    if (!DOSBase)                                              \
    {                                                          \
        SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);              \
	goto __exit;                                           \
    }                                                          \
							       \
    if (help[0])                                               \
    {                                                          \
        __rda2 = (struct RDArgs *)AllocDosObject(DOS_RDARGS, NULL);             \
	if (!__rda2)                                           \
	{                                                      \
            PrintFault(IoErr(), stringify(name));              \
	    goto __exit;                                       \
	}                                                      \
	__rda2->RDA_ExtHelp = help;                            \
    }                                                          \
							       \
    __rda = ReadArgs(templ, __shargs, __rda2);                 \
					                       \
    if (!__rda)                                                \
    {                                                          \
        PrintFault(IoErr(), stringify(name));                  \
	goto __exit;                                           \
    }                                                          \
							       \
    __retcode = name##_main(argstr, __shargs, SysBase, DOSBase); \
    							       \
__exit:                                                        \
    if (__rda) FreeArgs(__rda);                                \
    if (help[0] && __rda2) FreeDosObject(DOS_RDARGS, __rda2);  \
    if (DOSBase) CloseLibrary(DOSBase);                        \
                                                               \
    return __retcode;                                          \
                                                               \
    AROS_USERFUNC_EXIT                                         \
}                                                              \
static ULONG name##_main(CONST_STRPTR __argstr,                \
                         IPTR *__shargs,                       \
                         struct ExecBase *SysBase,             \
                         struct DosLibrary *DOSBase)           \
{

#define AROS_SHCOMMAND_INIT

#define AROS_SHCOMMAND_EXIT \
    }                       \
    return RETURN_OK;       \
}

#define __DEF(x...) {x}

#define __AROS_SH0(name, version, help)           \
    __AROS_SH_ARGS(name, 0, {} , "", "")          \
    {                                             \

#define __AROS_SH1(name, version, help, a1)       \
    __AROS_SH_ARGS(name, 1, __DEF(__SHA_DEF(a1)), \
                            __SHA_OPT(a1),        \
                            "")                   \
    {                                             \
	__SHA_TYPEDEF(a1);                        \
        enum {__SHA_ENUM(a1)};

#define __AROS_SH2(name, version, help, a1, a2)                          \
    __AROS_SH_ARGS(name, 2, __DEF(__SHA_DEF(a1), __SHA_DEF(a2)), \
                            __SHA_OPT(a1) "," __SHA_OPT(a2),     \
                            "")                                  \
    {                                                            \
	__SHA_TYPEDEF(a1);                                       \
	__SHA_TYPEDEF(a2);                                       \
        enum {__SHA_ENUM(a1), __SHA_ENUM(a2)};

#define __AROS_SH3(name, version, help, a1, a2, a3)                     \
    __AROS_SH_ARGS(name, 3, __DEF(__SHA_DEF(a1), __SHA_DEF(a2), \
                            __SHA_DEF(a3)),                     \
                            __SHA_OPT(a1) "," __SHA_OPT(a2) "," \
	  	            __SHA_OPT(a3),                      \
	                    "")                                 \
    {                                                           \
	__SHA_TYPEDEF(a1);                                      \
	__SHA_TYPEDEF(a2);                                      \
	__SHA_TYPEDEF(a3);                                      \
        enum {__SHA_ENUM(a1), __SHA_ENUM(a2),                   \
	      __SHA_ENUM(a3)};

#define __AROS_SH4(name, version, help, a1, a2, a3, a4)                 \
    __AROS_SH_ARGS(name, 4, __DEF(__SHA_DEF(a1), __SHA_DEF(a2), \
                            __SHA_DEF(a3), __SHA_DEF(a4)),      \
                            __SHA_OPT(a1) "," __SHA_OPT(a2) "," \
		            __SHA_OPT(a3) "," __SHA_OPT(a4),    \
	                    "")                                 \
    {                                                           \
	__SHA_TYPEDEF(a1);                                      \
	__SHA_TYPEDEF(a2);                                      \
	__SHA_TYPEDEF(a3);                                      \
	__SHA_TYPEDEF(a4);                                      \
        enum {__SHA_ENUM(a1), __SHA_ENUM(a2),                   \
	      __SHA_ENUM(a3), __SHA_ENUM(a4)};

#define __AROS_SH5(name, version, help, a1, a2, a3, a4, a5)             \
    __AROS_SH_ARGS(name, 5, __DEF(__SHA_DEF(a1), __SHA_DEF(a2), \
                            __SHA_DEF(a3), __SHA_DEF(a4),       \
		            __SHA_DEF(a5)),                     \
                            __SHA_OPT(a1) "," __SHA_OPT(a2) "," \
                            __SHA_OPT(a3) "," __SHA_OPT(a4) "," \
		            __SHA_OPT(a5),                      \
	                    "")                                 \
    {                                                           \
	__SHA_TYPEDEF(a1);                                      \
	__SHA_TYPEDEF(a2);                                      \
	__SHA_TYPEDEF(a3);                                      \
	__SHA_TYPEDEF(a4);                                      \
	__SHA_TYPEDEF(a5);                                      \
        enum {__SHA_ENUM(a1), __SHA_ENUM(a2), __SHA_ENUM(a3),   \
	      __SHA_ENUM(a4), __SHA_ENUM(a5)};                  \


#define __AROS_SH10(name, version, help, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    __AROS_SH_ARGS(name, 10, __DEF(__SHA_DEF(a1), __SHA_DEF(a2), \
                            __SHA_DEF(a3), __SHA_DEF(a4),       \
                            __SHA_DEF(a5), __SHA_DEF(a6),       \
                            __SHA_DEF(a7), __SHA_DEF(a8),       \
                            __SHA_DEF(a9), __SHA_DEF(a10)       \
		            ),                                  \
                            __SHA_OPT(a1) "," __SHA_OPT(a2) "," \
                            __SHA_OPT(a3) "," __SHA_OPT(a4) "," \
                            __SHA_OPT(a5) "," __SHA_OPT(a6) "," \
                            __SHA_OPT(a7) "," __SHA_OPT(a8) "," \
                            __SHA_OPT(a9) "," __SHA_OPT(a10),   \
		            "")                                 \
    {                                                           \
	__SHA_TYPEDEF(a1);                                      \
	__SHA_TYPEDEF(a2);                                      \
	__SHA_TYPEDEF(a3);                                      \
	__SHA_TYPEDEF(a4);                                      \
	__SHA_TYPEDEF(a5);                                      \
	__SHA_TYPEDEF(a6);                                      \
	__SHA_TYPEDEF(a7);                                      \
	__SHA_TYPEDEF(a8);                                      \
	__SHA_TYPEDEF(a9);                                      \
	__SHA_TYPEDEF(a10);                                     \
        enum {__SHA_ENUM(a1), __SHA_ENUM(a2), __SHA_ENUM(a3),   \
	      __SHA_ENUM(a4), __SHA_ENUM(a5), __SHA_ENUM(a6),   \
	      __SHA_ENUM(a7), __SHA_ENUM(a8), __SHA_ENUM(a9),   \
	      __SHA_ENUM(a10)                                   \
	};                  \

#define AROS_SHA(type, abbr, name, modf, def) type,abbr,name,modf,def,""
#define AROS_SHAH(type, abbr, name, modf, def, help) type,abbr,name,modf,def,"\t" __SHA_OPT(type,abbr,name,modf,def,help) "\t\t" help "\n"

#define __AROS_SHA(type, abbr, name, modf, def, help) type,abbr,name,modf,def,help

#define AROS_SH12(name, version, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
        __AROS_SH12(name, version, "", __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9), __AROS_SHA(a10), __AROS_SHA(a11), __AROS_SHA(a12))
#define AROS_SH11(name, version, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
        __AROS_SH11(name, version, "", __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9), __AROS_SHA(a10), __AROS_SHA(a11))
#define AROS_SH10(name, version, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)\
        __AROS_SH10(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9), __AROS_SHA(a10))
#define AROS_SH9(name, version, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
        __AROS_SH9(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9))
#define AROS_SH8(name, version, a1, a2, a3, a4, a5, a6, a7, a8) \
        __AROS_SH8(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8))
#define AROS_SH7(name, version, a1, a2, a3, a4, a5, a6, a7) \
        __AROS_SH7(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7))
#define AROS_SH6(name, version, a1, a2, a3, a4, a5, a6) \
        __AROS_SH6(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6))
#define AROS_SH5(name, version, a1, a2, a3, a4, a5) \
        __AROS_SH5(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5))
#define AROS_SH4(name, version, a1, a2, a3, a4) \
        __AROS_SH4(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4))
#define AROS_SH3(name, version, a1, a2, a3) \
        __AROS_SH3(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3))
#define AROS_SH2(name, version, a1, a2) \
        __AROS_SH2(name, version, "",  __AROS_SHA(a1), __AROS_SHA(a2))
#define AROS_SH1(name, version, a1) \
        __AROS_SH1(name, version, "",  __AROS_SHA(a1))
#define AROS_SH0(name, version) \
        __AROS_SH0(name, version, "")

#define __SH_HELP(name, help) stringify(name) ": " help "\n"
#define __SHA_HELP(type, abbr, name, modf, def, help) help

#define AROS_SH12H(name, version, help, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
        __AROS_SH12(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6) __SHA_HELP(a7) __SHA_HELP(a8) __SHA_HELP(a9) __SHA_HELP(a10) __SHA_HELP(a11) __SHA_HELP(a12), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9), __AROS_SHA(a10), __AROS_SHA(a11), __AROS_SHA(a12))
#define AROS_SH11H(name, version, help, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
        __AROS_SH11(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6) __SHA_HELP(a7) __SHA_HELP(a8) __SHA_HELP(a9) __SHA_HELP(a10) __SHA_HELP(a11), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9), __AROS_SHA(a10), __AROS_SHA(a11))
#define AROS_SH10H(name, version, help, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
        __AROS_SH10(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6) __SHA_HELP(a7) __SHA_HELP(a8) __SHA_HELP(a9) __SHA_HELP(a10), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9), __AROS_SHA(a10))
#define AROS_SH9H(name, version, help, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
        __AROS_SH9(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6) __SHA_HELP(a7) __SHA_HELP(a8) __SHA_HELP(a9), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8), __AROS_SHA(a9))
#define AROS_SH8H(name, version, help, a1, a2, a3, a4, a5, a6, a7, a8) \
        __AROS_SH8(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6) __SHA_HELP(a7) __SHA_HELP(a8), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7), __AROS_SHA(a8))
#define AROS_SH7H(name, version, help, a1, a2, a3, a4, a5, a6, a7) \
        __AROS_SH7(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6) __SHA_HELP(a7), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6), __AROS_SHA(a7))
#define AROS_SH6H(name, version, help, a1, a2, a3, a4, a5, a6) \
        __AROS_SH6(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5) __SHA_HELP(a6), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5), __AROS_SHA(a6))
#define AROS_SH5H(name, version, help, a1, a2, a3, a4, a5) \
        __AROS_SH5(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4) __SHA_HELP(a5), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4), __AROS_SHA(a5))
#define AROS_SH4H(name, version, help, a1, a2, a3, a4) \
        __AROS_SH4(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3) __SHA_HELP(a4), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3), __AROS_SHA(a4))
#define AROS_SH3H(name, version, help, a1, a2, a3) \
        __AROS_SH3(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2) __SHA_HELP(a3), __AROS_SHA(a1), __AROS_SHA(a2), __AROS_SHA(a3))
#define AROS_SH2H(name, version, help, a1, a2) \
        __AROS_SH2(name, version, __SH_HELP(name, help) __SHA_HELP(a1) __SHA_HELP(a2), __AROS_SHA(a1), __AROS_SHA(a2))
#define AROS_SH1H(name, version, help, a1) \
        __AROS_SH1(name, version, __SH_HELP(name, help) __SHA_HELP(a1),  __AROS_SHA(a1))
#define AROS_SH0H(name, version, help) \
        __AROS_SH0(name, version, __SH_HELP(name, help))



#endif
