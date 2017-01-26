#ifndef KERNEL_INTERN_H_
#define KERNEL_INTERN_H_

#include <inttypes.h>

#include <exec/nodes.h>
#include <exec/lists.h>
#include <aros/kernel.h>
#include <utility/tagitem.h>
#include <asm/cpu.h>

#include "apic.h"
#include "tls.h"

#define STACK_SIZE      65536
#define PAGE_SIZE	0x1000
#define PAGE_MASK	0x0FFF

struct ACPIData;
struct IOAPICData;

/*
 * Boot-time private data.
 * This structure is write-protected in user mode and survives warm restarts.
 */
struct KernBootPrivate
{
    IPTR		_APICBase;		/* Bootstrap APIC base address				*/
    UWORD               kbp_APIC_BSPID;		/* Bootstrap APIC logical ID				*/
    unsigned short	debug_y_resolution;	/* Parameters of screen's lower half ('vesahack' mode)	*/
    void	       *debug_framebuffer;
    IPTR	        SystemStack;		/* System stack base address	       			*/
    APTR                BOOTTLS,
                        BOOTGDT,
                        BOOTIDT;
    void	       *TSS;
    void	       *PML4;
    void	       *PDP;
    void	       *PDE;
    void	       *PTE;
    int		        used_page;
};

extern struct KernBootPrivate *__KernBootPrivate;

/* Platform-specific part of KernelBase */
struct PlatformData
{
    APTR                kb_APIC_TrampolineBase;	/* Starting address of secondary core bootstrap code	*/
    struct ACPIData     *kb_ACPI;
    struct APICData     *kb_APIC;
    struct IOAPICData   *kb_IOAPIC;
};


#define IDT_SIZE                sizeof(struct int_gate_64bit) * 256
#define GDT_SIZE                sizeof(struct gdt_64bit) + 128
#define TLS_SIZE                sizeof(tls_t)
#define TLS_ALIGN               sizeof(APTR)

#define IDT_GET()               TLS_GET(IDT)
#define IDT_SET(val)            TLS_SET(IDT, val);
#define GDT_GET()               TLS_GET(GDT)
#define GDT_SET(val)            TLS_SET(GDT, val);

/* Main boot code */
void core_Kick(struct TagItem *msg, void *target);
void kernel_cstart(const struct TagItem *msg);

/** CPU Functions **/
void core_SetupIDT(struct KernBootPrivate *, apicid_t, APTR);
void core_SetupGDT(struct KernBootPrivate *, apicid_t, APTR, APTR, APTR);
void core_SetupMMU(struct KernBootPrivate *, IPTR memtop);

void core_CPUSetup(apicid_t, APTR, IPTR);

void core_ProtKernelArea(intptr_t addr, intptr_t length, char p, char rw, char us);
void core_DefaultIRETQ();
void ictl_Initialize(struct KernelBase *KernelBase);

struct ExceptionContext;

/* Interrupt processing */
void core_LeaveInterrupt(struct ExceptionContext *);
void core_Supervisor(struct ExceptionContext *);

void PlatformPostInit(void);

#endif /*KERNEL_INTERN_H_*/
