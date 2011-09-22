#include <aros/cpu.h>

#if AROS_BIG_ENDIAN
#define ENDIANNESS_DEF ENDIANNESS_BE
#else
#define ENDIANNESS_DEF ENDIANNESS_LE
#endif

#ifdef __i386__
#define PROCESSORARCH_DEF PROCESSORARCH_X86
#endif
#ifdef __x86_64__
#define PROCESSORARCH_DEF PROCESSORARCH_X86
#endif
#ifdef __mc68000__
#define PROCESSORARCH_DEF PROCESSORARCH_M68K
#endif
#ifdef __ppc__
#define PROCESSORARCH_DEF PROCESSORARCH_PPC
#endif
#ifdef __arm__
#define PROCESSORARCH_DEF PROCESSORARCH_ARM
#endif
#ifndef PROCESSORARCH_DEF
#define PROCESSORARCH_DEF PROCESSORARCH_UNKNOWN
#endif
