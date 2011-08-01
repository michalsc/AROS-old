#include <aros/bootloader.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <proto/aros.h>
#include <proto/bootloader.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/processor.h>
#include <resources/processor.h>

#include <stdio.h>

APTR ProcessorBase = NULL;

ULONG ExtUDivMod32(ULONG a, ULONG b, ULONG *mod)
{
    *mod = a % b;

    return a/b;
}

void PrintNum(ULONG num)
{
    /* MBytes ? */
    if(num > 1023) 
    {
	ULONG  x, xx;
	char* fmt = "meg";
	
	/* GBytes ? */
	if(num > 0xfffff)
	{ 
	    num >>= 10; 
	    fmt = "gig";
	}
	
	num = ExtUDivMod32(UMult32(num, 100) >> 10, 100, &x);
	
	/* round */
	x = ExtUDivMod32(x, 10, &xx);
	
	if(xx > 4)
	{
	    if(++x > 9)
	    {
		x = 0;
		num++;
	    }
	}

        printf("%d.%d %s", (int)num, (int)x, fmt);
    }
    else 
    {
        printf("%d K", (int)num);
    }
}

ULONG ComputeKBytes(APTR a, APTR b)
{
    IPTR result = b - a;

    return (ULONG)(result >> 10);
}

static ULONG GetProcessorsCount()
{
    ULONG count = 0;
    struct TagItem tags [] = 
    {
        {GCIT_NumberOfProcessors, (IPTR)&count},
        {TAG_DONE, TAG_DONE}
    };

    GetCPUInfo(tags);

    return count;
}

struct
{
    ULONG Architecture;
    STRPTR Description;
} ProcessorArchitecture [] =
{
    { PROCESSORARCH_UNKNOWN, "Unknown" },
    { PROCESSORARCH_M68K, "Motorola 68K" },
    { PROCESSORARCH_PPC, "PowerPC" },
    { PROCESSORARCH_X86, "X86" },
    { PROCESSORARCH_ARM, "ARM" },
    { 0, NULL }   
};

struct
{
    ULONG Endianness;
    STRPTR Description;
} CurrentEndianness [] =
{
    { ENDIANNESS_UNKNOWN, "Unknown" },
    { ENDIANNESS_LE, "LE" },
    { ENDIANNESS_BE, "BE" },
    { 0, NULL}
};

static VOID PrintProcessorInformation()
{
    ULONG count = GetProcessorsCount();
    ULONG i, j;
    CONST_STRPTR modelstring;
    ULONG architecture, endianness;
    CONST_STRPTR architecturestring = "", endiannessstring = "";
    UQUAD cpuspeed;
    
    for (i = 0; i < count; i++)
    {
        struct TagItem tags [] =
        {
            {GCIT_SelectedProcessor, i},
            {GCIT_ModelString, (IPTR)&modelstring},
            {GCIT_Architecture, (IPTR)&architecture},
            {GCIT_Endianness, (IPTR)&endianness},
            {GCIT_ProcessorSpeed, (IPTR)&cpuspeed},
            {TAG_DONE, TAG_DONE}
        };
        
        GetCPUInfo(tags);

        j = 0;
        while(ProcessorArchitecture[j].Description != NULL)
        {
            if (ProcessorArchitecture[j].Architecture == architecture)
            {
                architecturestring = ProcessorArchitecture[j].Description;
                break;
            }
            j++;
        }

        j = 0;
        while(CurrentEndianness[j].Description != NULL)
        {
            if (CurrentEndianness[j].Endianness == endianness)
            {
                endiannessstring = CurrentEndianness[j].Description;
                break;
            }
            j++;
        }       


        printf("PROCESSOR %d:\t[%s/%s] %s", (int)(i + 1), architecturestring, endiannessstring, modelstring);
        if (cpuspeed)
            printf(" (%llu Mhz)", (unsigned long long)(cpuspeed / 1000000));
        printf("\n");
    }
}

int __nocommandline;
char __stdiowin[]="CON://800/400/ShowConfig/AUTO/CLOSE/WAIT";

int main()
{
    struct MemHeader *mh;
    APTR BootLoaderBase;
    STRPTR bootldr;
    struct List *args;
    struct Node *n;
    
    ProcessorBase = OpenResource(PROCESSORNAME);
    if (ProcessorBase)
        PrintProcessorInformation();        
    
    printf("VERS:\t\tAROS version %d.%d, Exec version %d.%d\n", ArosBase->lib_Version, ArosBase->lib_Revision,
	   SysBase->LibNode.lib_Version, SysBase->LibNode.lib_Revision);
    
    printf("RAM:");
    for (mh = (struct MemHeader *)SysBase->MemList.lh_Head; mh->mh_Node.ln_Succ; mh = (struct MemHeader *)mh->mh_Node.ln_Succ) {
        char *memtype = "ROM";

        if (mh->mh_Attributes & MEMF_CHIP)
            memtype = "CHIP";
        if (mh->mh_Attributes & MEMF_FAST)
            memtype = "FAST";
        printf("\t\tNode Type 0x%X, Attributes 0x%X (%s), at $%p-$%p (", mh->mh_Node.ln_Type, mh->mh_Attributes, memtype, mh->mh_Lower, mh->mh_Upper - 1);
        PrintNum(ComputeKBytes(mh->mh_Lower, mh->mh_Upper));
        printf(")\n");
    }

    BootLoaderBase = OpenResource("bootloader.resource");
    if (BootLoaderBase) {
	bootldr = GetBootInfo(BL_LoaderName);

	if (bootldr)
    	    printf("BOOTLDR:\t%s\n", bootldr);

	args = GetBootInfo(BL_Args);
	if (args) {
            printf("ARGS:\t\t");
            for (n = args->lh_Head; n->ln_Succ; n = n->ln_Succ) {
        	printf("%s ", n->ln_Name);
            }
        	printf("\n");
	}
    }
    return 0;
}
