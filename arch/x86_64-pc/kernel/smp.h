/*
 * This structure is placed in the start of SMP bootstrap code.
 * We pass parameters through it.
 */
struct SMPBootstrap
{
    ULONG Start;
    ULONG Arg1;
    ULONG Arg2;
    ULONG Arg3;
    ULONG Arg4;
    ULONG PML4;
    APTR  SP;
    APTR  IP;
};

int smp_Setup(IPTR num);
int smp_Wake(void);
