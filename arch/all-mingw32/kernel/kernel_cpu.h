#ifndef KERNEL_CPU_H_
#define KERNEL_CPU_H_

/* First include CPU-dependent definitions */

#ifdef __i386__
#include "cpu_i386.h"
#endif
#ifdef __x86_64__
#include "cpu_x86_64.h"
#endif

/* This macro serves as an indicator of included file */
#ifndef PRINT_CPUCONTEXT
#error Unsupported CPU
#endif

#ifdef __AROS__

/* Some common definitions */
#define cpumode_t __unused char

#define goSuper() 0
#define goUser()
#define goBack(mode)

#define AROS_EXCEPTION_SYSCALL 0x00080001
#define AROS_EXCEPTION_RESUME  0x00080002

/* Our virtual CPU interface. It's needed here for krnSysCall() definition */
struct KernelInterface
{
    int (*core_init)(unsigned int TimerPeriod);
    void (*core_raise)(ULONG num, const IPTR n);
    unsigned int (*core_protect)(void *addr, unsigned int len, unsigned int prot);
    void (*core_putc)(char c);
    int (*core_getc)(void);
    int (**TrapVector)(unsigned int num, IPTR *args, CONTEXT *regs);
    int (**IRQVector)(unsigned char *irqs, CONTEXT *regs);
    volatile int *IntState;
    volatile int *SuperState;
    volatile unsigned char *SleepState;
    volatile ULONG **LastErrorPtr;
};

extern struct KernelInterface KernelIFace;

#define krnSysCall(n) KernelIFace.core_raise(AROS_EXCEPTION_SYSCALL, n)
#define Sleep_Mode   (*KernelIFace.SleepState)
#define LastErrorPtr (*KernelIFace.LastErrorPtr)

#endif

/* Interrupt enable states */
#define INT_DISABLE 0
#define INT_ENABLE  1
#define INT_HALT    -1

/* Sleep states */
#define SLEEP_MODE_OFF     0
#define SLEEP_MODE_PENDING 1
#define SLEEP_MODE_ON      2

#endif
