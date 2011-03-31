/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Emit one character via raw IO
    Lang: english
*/

#include <aros/libcall.h>
#include <exec/types.h>

#include <bootconsole.h>

#define __save_flags(x)		__asm__ __volatile__("pushfq ; popq %0":"=g" (x): /* no input */)
#define __restore_flags(x) 	__asm__ __volatile__("pushq %0 ; popfq": /* no output */ :"g" (x):"memory", "cc")
#define __cli() 		__asm__ __volatile__("cli": : :"memory")
#define __sti()			__asm__ __volatile__("sti": : :"memory")

AROS_LH1(void, RawPutChar,
	 AROS_LHA(UBYTE, chr, D0),
	 struct ExecBase *, SysBase, 86, Exec)
{
    AROS_LIBFUNC_INIT

    unsigned long flags;

    __save_flags(flags);

    /*
     * stegerg: Don't use Disable/Enable, because we want  interrupt enabled flag
     * to stay the same as it was before the Disable() call
     */
    __cli();

    con_Putc(chr);

    /*
     * Interrupt flag is stored in flags - if it was enabled before,
     * it will be renabled when the flags are restored
     */
    __restore_flags(flags);

    AROS_LIBFUNC_EXIT
} /* RawPutChar */
