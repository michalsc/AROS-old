/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <inttypes.h>
#include <aros/kernel.h>
#include <aros/libcall.h>
#include <asm/arm/mmu.h>
#include <stddef.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/kernel.h>

#include "kernel_intern.h"
#include "mmu.h"

pde_t pagetable[4096] __attribute__((aligned(16384)));


void arm_flush_cache(uint32_t addr, uint32_t length);

void *core_MMUVirtToPhys(void *addr)
{
    pde_t *pde;
    int idx = ((uintptr_t)addr) >> 20;
    asm volatile("mrc   p15, 0, %[addr], c2, c0, 0" : [addr] "=r" (pde));

    pde = (pde_t *)(((uintptr_t)pde) &~(16384-1));

    if ((pde[idx].raw & 0x3) == 2)
    {
        uintptr_t phys = pde[idx].section.base_address << 20;
        phys += (uintptr_t)addr & (1024*1024-1);

        return (void *)phys;
    }
}

void core_MMUUpdatePageTables(void)
{
    void * pt_addr = core_MMUVirtToPhys(&pagetable);

    D(bug("[Kernel] Updating PDE address to %p, phys %p\n", pagetable, pt_addr));

    /* Invalidate caches */
    asm volatile("mcr   p15, 0, %[r], c8, c7, 0" : : [r] "r" (0x0));   //Invalidate entire unified TLB
    asm volatile("mcr   p15, 0, %[r], c8, c6, 0" : : [r] "r" (0x0));   //Invalidate entire data TLB
    asm volatile("mcr   p15, 0, %[r], c8, c5, 0" : : [r] "r" (0x0));   //Invalidate entire instruction TLB
    asm volatile("mcr   p15, 0, %[r], c7, c5, 6" : : [r] "r" (0x0));   //Invalidate entire branch prediction array
    asm volatile("mcr   p15, 0, %[r], c7, c5, 0" : : [r] "r" (0x0));   //Invalidate icache

    /* setup_ttbr0/1 */
    asm volatile("mcr   p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
    /* setup_ttbrc */
    asm volatile("mcr   p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
}

void core_SetupMMU(struct TagItem *msg)
{
    pde_t *initial_pde;
    uint32_t control;
    int i;

    D(bug("[Kernel] core_SetupMMU(%p)\n", msg));

    asm volatile("mrc   p15, 0, %[addr], c2, c0, 0" : [addr] "=r" (initial_pde));

    D(bug("[Kernel] Initial PDE @ %p, Kernel PDE @ %p\n", initial_pde, pagetable));

    initial_pde = (pde_t *)(((intptr_t)initial_pde) &~(16384-1));

    for (i=0; i < 4096; i++)
        pagetable[i].raw = initial_pde[i].raw;

    arm_flush_cache((uint32_t)pagetable, 16384);

    core_MMUUpdatePageTables();

    /* Set the domain 0 access control to client */
    asm volatile("mcr   p15, 0, %[r], c3, c0, 0" : : [r] "r" (1));

    /* Enable L1 caches (I-cache and D-cache) and MMU.*/
    asm volatile("mrc   p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
    control |= ( ENABLE_I_CACHE | ENABLE_D_CACHE | ENABLE_MMU );
    asm volatile ("mcr  p15, 0, %[r], c7, c10, 4" : : [r] "r" (0)); /* dsb */
    asm volatile ("mcr  p15, 0, %0, c1, c0, 0" : : "r" (control) : "cc" );
    asm volatile ("mcr  p15, 0, %[r], c7, c5, 4" : : [r] "r" (0)); /* isb */

    D(bug("[Kernel] core_SetupMMU: Done\n"));
}

void core_ProtPage(intptr_t addr, char p, char rw, char us)
{
    D(bug("[Kernel] Marking page 0x%p as read-only\n", addr));

    core_MMUUpdatePageTables();
}

void core_ProtKernelArea(intptr_t addr, intptr_t length, char p, char rw, char us)
{
    D(bug("[Kernel] Protecting area 0x%p - 0x%p\n", addr, addr + length - 1));
    while (length > 0)
    {
        core_ProtPage(addr, p, rw, us);
        addr += 4096;
        length -= 4096;
    }
}
