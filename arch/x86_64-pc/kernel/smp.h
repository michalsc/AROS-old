/*
 * This structure is placed in the start of SMP bootstrap code.
 * We pass parameters through it.
 */
struct SMPBootstrap
{
    IPTR  Start;	/* Jump code to bypass this struct */
    IPTR  Arg1;		/* Arguments			   */
    IPTR  Arg2;
    IPTR  Arg3;
    IPTR  Arg4;
    APTR  GDT;		/* 64-bit mode GDT address	   */
    APTR  PML4;		/* 64-bit mode PML4 address	   */
    APTR  SP;		/* Stack pointer for 64 bit mode   */
    APTR  IP;		/* Address to jump to		   */
    ULONG reloc1;	/* Two relocation points	   */
    ULONG reloc2;
};

int smp_Setup(IPTR num);
int smp_Wake(void);
