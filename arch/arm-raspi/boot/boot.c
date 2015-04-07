/*
    Copyright � 2013-2015, The AROS Development Team. All rights reserved.
    $Id$

    Desc: boot.c
    Lang: english
 */

#include <inttypes.h>
#include <asm/cpu.h>
#include <utility/tagitem.h>
#include <asm/arm/mmu.h>
#include <aros/macros.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <hardware/bcm283x.h>
#include <hardware/videocore.h>

#include "boot.h"
#include "serialdebug.h"
#include "bootconsole.h"
#include "atags.h"
#include "elf.h"

#include "vc_mb.h"
#include "vc_fb.h"

#define DBOOT(x) x

#undef ARM_PERIIOBASE
#define ARM_PERIIOBASE (__arm_periiobase)

uint32_t __arm_periiobase;

extern void mem_init(void);
extern unsigned int uartclock;
extern unsigned int uartdivint;
extern unsigned int uartdivfrac;
extern unsigned int uartbaud;

asm("   .section .aros.startup      \n"
"       .globl bootstrap            \n"
"       .type bootstrap,%function   \n"
"bootstrap:                         \n"
"       cps     #0x13               \n" /* Should be in SVC (supervisor) mode already, but just incase.. */
"       ldr     sp, tmp_stack_ptr   \n"
"       b       boot                \n"

".string \"$VER: arosraspi.img v40.45 (" __DATE__ ")\"" "\n\t\n\t"
);

// The bootstrap tmp stack is re-used by the reset handler so we store it at this fixed location
static __used void * tmp_stack_ptr __attribute__((used, section(".aros.startup"))) = (void *)(0x1000 - 16);
static struct TagItem *boottag;
static unsigned long *mem_upper;
static void *pkg_image;
static uint32_t pkg_size;
static pde_t pde[4096] __attribute__((used, aligned(16384)));

/* Setup MMU PDE table. Just zero it */
static void mmu_init()
{
    int i;

    for (i = 0; i < 4096; i++)
        pde[i].raw = 0;
}

static void mmu_load()
{
    uint32_t tmp;

    arm_flush_cache((uint32_t)pde, 16384);

    /* Write page_dir address to ttbr0 */
    asm volatile ("mcr p15, 0, %0, c2, c0, 0"::"r"(pde));
    /* Write ttbr control N = 0 (use only ttbr0) */
    asm volatile ("mcr p15, 0, %0, c2, c0, 2"::"r"(0));

    /* Set domains - Dom0 is usable, rest is disabled */
    asm volatile ("mrc p15, 0, %0, c3, c0, 0":"=r"(tmp));
    kprintf("[BOOT] Domain access control register: %08x\n", tmp);
    asm volatile ("mcr p15, 0, %0, c3, c0, 0"::"r"(0x00000001));

    asm volatile ("mrc p15, 0, %0, c1, c0, 0":"=r"(tmp));
    kprintf("[BOOT] control register %08x\n", tmp);
    tmp |= 1;           /* Enable MMU */
    tmp |= 1 << 23;     /* v6 page tables, subpages disabled */
    asm volatile ("mcr  p15, 0, %[r], c7, c10, 4" : : [r] "r" (0)); /* dsb */
    asm volatile ("mcr p15, 0, %0, c1, c0, 0"::"r"(tmp));
    asm volatile ("mcr  p15, 0, %[r], c7, c5, 4" : : [r] "r" (0)); /* isb */
}

/* Remove mapping for sections from address virt and with given length */
static void mmu_unmap_section(uint32_t virt, uint32_t length)
{
    uint32_t start = virt & ~(1024*1024-1);
    uint32_t end = (start + length) & ~(1024*1024-1);

    start >>= 20;
    end >>= 20;

    while (start < end)
    {
        pde[start].raw = 0;
        start++;
    }
}

/* Map a section using b, c, ap and tex. Sections are aligned to 1M boundary */
static void mmu_map_section(uint32_t phys, uint32_t virt, uint32_t length, int b, int c, int ap, int tex)
{
    uint32_t start = virt & ~(1024*1024-1);
    uint32_t end = (start + length) & ~(1024*1024-1);

    kprintf("[BOOT] MMU map %p:%p->%p:%p, b=%d, c=%d, ap=%x, tex=%x\n",
            phys, phys+length-1, virt, virt+length-1, b, c, ap, tex);

    int count = (end - start) >> 20;
    int i = start >> 20;
    phys >>= 20;

    while(count--)
    {
        pde_t s;

        s.section.type = PDE_TYPE_SECTION;
        s.section.b = b;
        s.section.c = c;
        s.section.ap = ap & 3;
        s.section.apx = (ap >> 2) & 1;
        s.section.tex = tex;
        s.section.base_address = phys;

        pde[i] = s;

        phys++;
        i++;
    }
}

static void parse_atags(struct tag *tags)
{
    struct tag *t = NULL;

    kprintf("[BOOT] Parsing ATAGS\n");

    for_each_tag(t, tags)
    {
        kprintf("[BOOT]   %08x: ", t->hdr.tag, t->hdr.size);
        switch (t->hdr.tag)
        {
            case 0:
                kprintf("ATAG_NONE - Ignored\n");
                break;

            case ATAG_CORE:
                kprintf("ATAG_CORE - Ignored\n");
                break;

            case ATAG_MEM:
                kprintf("ATAG_MEM (%08x-%08x)\n", t->u.mem.start, t->u.mem.size + t->u.mem.start - 1);
                boottag->ti_Tag = KRN_MEMLower;
                if ((boottag->ti_Data = t->u.mem.start) == 0)
                    boottag->ti_Data = 0x1000; // Skip the *reserved* space for the cpu vectors/boot tmp stack/kernel private data.

                boottag++;
                boottag->ti_Tag = KRN_MEMUpper;
                boottag->ti_Data = t->u.mem.start + t->u.mem.size;
                mem_upper = &boottag->ti_Data;

                boottag++;

                /* prepare map for memory (outer and inner cacheable, write back) */
                mmu_map_section(t->u.mem.start, t->u.mem.start, t->u.mem.size, 1, 1, 3, 1);

                break;

            case ATAG_VIDEOTEXT:
                kprintf("ATAG_VIDEOTEXT - Ignored\n");
                break;

            case ATAG_RAMDISK:
                kprintf("ATAG_RAMDISK - Ignored\n");
                break;

            case ATAG_INITRD2:
                kprintf("ATAG_INITRD2 (%08x-%08x)\n", t->u.initrd.start, t->u.initrd.size + t->u.initrd.start - 1);
                pkg_image = (void *)t->u.initrd.start;
                pkg_size = t->u.initrd.size;
                break;

            case ATAG_SERIAL:
                kprintf("ATAG_SERIAL - Ignored\n");
                break;

            case ATAG_REVISION:
                kprintf("ATAG_REVISION - Ignored\n");
                break;

            case ATAG_VIDEOLFB:
                kprintf("ATAG_VIDEOLFB - Ignored\n");
                break;

            case ATAG_CMDLINE:
            {
                char *cmdline = malloc(strlen(t->u.cmdline.cmdline) + 1);
                strcpy(cmdline, t->u.cmdline.cmdline);
                kprintf("ATAG_CMDLINE \"%s\"\n", cmdline);

                boottag->ti_Tag = KRN_CmdLine;
                boottag->ti_Data = (intptr_t)cmdline;
                boottag++;
            }
            break;

            default:
                kprintf("(UNKNOWN)...\n");
                break;
        }
    }
}

void query_vmem()
{
    volatile unsigned int *vc_msg = (unsigned int *) MESSAGE_BUFFER;

    kprintf("[BOOT] Query VC memory\n");
    vc_msg[0] = 8 * 4;
    vc_msg[1] = VCTAG_REQ;
    vc_msg[2] = VCTAG_GETVCRAM;
    vc_msg[3] = 8;
    vc_msg[4] = 0;
    vc_msg[5] = 0;
    vc_msg[6] = 0;
    vc_msg[7] = 0;

    vcmb_write(VCMB_BASE, VCMB_PROPCHAN, vc_msg);
    vc_msg = vcmb_read(VCMB_BASE, VCMB_PROPCHAN);

    kprintf("[BOOT] Base = %08x, Size = %08x\n", vc_msg[5], vc_msg[6]);

    boottag->ti_Tag = KRN_VMEMLower;
    boottag->ti_Data = vc_msg[5];
    boottag++;

    boottag->ti_Tag = KRN_VMEMUpper;
    boottag->ti_Data = vc_msg[5] + vc_msg[6];
    boottag++;

    /* map framebuffer as shared device */
    mmu_map_section(vc_msg[5], vc_msg[5], vc_msg[6], 1, 0, 3, 0);
}

static const char bootstrapName[] = "Bootstrap/RasPI ARM";

void boot(uintptr_t dummy, uintptr_t arch, struct tag * atags)
{
    uint32_t tmp, initcr;
    int plus_board = 0;
    void (*entry)(struct TagItem *);

    /*
     * Disable MMU, enable caches and branch prediction. Also enabled unaligned memory
     * access
     */
    asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r"(initcr));
    tmp = initcr;
    tmp &= ~1;                                  /* Disable MMU */
    tmp |= (1 << 2) | (1 << 12) | (1 << 11);    /* I and D caches, branch prediction */
    tmp = (tmp & ~2) | (1 << 22);               /* Unaligned access enable */
    asm volatile ("mcr p15, 0, %0, c1, c0, 0" : : "r"(tmp));

    /*
        Check processor type - armv6 is old raspberry pi with SOC IO base at 0x20000000.
        armv7 will be raspberry pi 2 with SOC IO base at 0x3f000000
     */
    asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r" (tmp));

    tmp = (tmp >> 4) & 0xfff;

    mmu_init();

    /* tmp == 7 means armv6 architecture. */
    if (tmp == 0xc07) /* armv7, also RaspberryPi 2 */
    {
        __arm_periiobase = BCM2836_PERIPHYSBASE;
        plus_board = 1;
    }
    else
    {
        __arm_periiobase = BCM2835_PERIPHYSBASE;
        /* Need to detect the plus board here in order to control LEDs properly */
    }

    mem_init();

    boottag = tmp_stack_ptr - BOOT_STACK_SIZE - BOOT_TAGS_SIZE;

    /* Init LED */
    {
        if (plus_board)
        {
            /*
             * Either B+ or rpi2 board. Uses two leds (power and activity) on GPIOs
             * 47 and 35. Enable both leds as output and turn both of them off.
             *
             * The power led will be brought back up once AROS boots.
             */

            tmp = *(volatile unsigned int *)GPFSEL4;
            tmp &= ~(7 << 21); // GPIO 47 = 001 - output
            tmp |= (1 << 21);
            *(volatile unsigned int *)GPFSEL4 = tmp;

            tmp = *(volatile unsigned int *)GPFSEL3;
            tmp &= ~(7 << 15); // GPIO 35 = 001 - output
            tmp |= (1 << 15);
            *(volatile unsigned int *)GPFSEL3 = tmp;

            /* LEDS off */
            *(volatile unsigned int *)GPCLR1 = (1 << (47-32));
            *(volatile unsigned int *)GPCLR1 = (1 << (35-32));
        }
        else
        {
            /*
             * Classic rpi board has only one controlable LED - activity on GPIO 16. Turn it
             * off now, kernel.resource will bring it back later.
             */

            tmp = *(volatile unsigned int *)GPFSEL1;
            tmp &= ~(7 << 18); // GPIO 16 = 001 - output
            tmp |= (1 << 18);
            *(volatile unsigned int *)GPFSEL1 = tmp;

            *(volatile unsigned int *)GPSET0 = (1 << 16);
        }
    }

    serInit();

    boottag->ti_Tag = KRN_BootLoader;
    boottag->ti_Data = (IPTR)bootstrapName;
    boottag++;

    if (vcfb_init())
    {
        boottag->ti_Tag = KRN_FuncPutC;
        boottag->ti_Data = (IPTR)fb_Putc;
        boottag++;
    }

    kprintf("\n\n[BOOT] AROS %s\n", bootstrapName);

    DBOOT({
        kprintf("[BOOT] UART clock speed: %d\n", uartclock);
        kprintf("[BOOT] using %d.%d divisor for %d baud\n", uartdivint, uartdivfrac, uartbaud);

        asm volatile ("mrc p15, 0, %0, c1, c0, 0" : "=r"(tmp));
        kprintf("[BOOT] control register init:%08x, now:%08x\n", initcr, tmp);

        asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r"(tmp));
        kprintf("[BOOT] main id register: %08x\n", tmp);
    })

    parse_atags(atags);

    query_vmem();

    kprintf("[BOOT] Bootstrap @ %08x-%08x\n", &__bootstrap_start, &__bootstrap_end);

    boottag->ti_Tag = KRN_ProtAreaStart;
    boottag->ti_Data = (IPTR)&__bootstrap_start;
    boottag++;

    boottag->ti_Tag = KRN_ProtAreaEnd;
    boottag->ti_Data = (IPTR)&__bootstrap_end;
    boottag++;

    /* Prepare map for MMIO registers */
    mmu_map_section(__arm_periiobase, __arm_periiobase, ARM_PERIIOSIZE, 1, 0, 3, 0);

    kprintf("[BOOT] Topmost address for kernel: %p\n", mem_upper);

    if (mem_upper)
    {
        *mem_upper = *mem_upper & ~4095;

        unsigned long kernel_phys = *mem_upper;
        unsigned long kernel_virt = kernel_phys;

        unsigned long total_size_ro, total_size_rw;
        uint32_t size_ro, size_rw;

        /* Calculate total size of kernel and modules */
        getElfSize(&_binary_core_bin_start, &size_rw, &size_ro);

        total_size_ro = size_ro = (size_ro + 4095) & ~4095;
        total_size_rw = size_rw = (size_rw + 4095) & ~4095;

        if (pkg_image && pkg_size)
        {
            uint8_t *base = pkg_image;

            if (base[0] == 0x7f && base[1] == 'E' && base[2] == 'L' && base[3] == 'F')
            {
                getElfSize(base, &size_rw, &size_ro);

                total_size_ro += (size_ro + 4095) & ~4095;
                total_size_rw += (size_rw + 4095) & ~4095;
            }
            else if (base[0] == 'P' && base[1] == 'K' && base[2] == 'G' && base[3] == 0x01)
            {
                uint8_t *file = base+4;
                uint32_t total_length = AROS_BE2LONG(*(uint32_t*)file); /* Total length of the module */
                const uint8_t *file_end = base+total_length;
                uint32_t len, cnt = 0;

                file = base + 8;

                while(file < file_end)
                {
                    const char *filename = remove_path(file+4);

                    /* get text length */
                    len = AROS_BE2LONG(*(uint32_t*)file);

                    file += len + 5;

                    len = AROS_BE2LONG(*(uint32_t *)file);
                    file += 4;

                    /* load it */
                    getElfSize(file, &size_rw, &size_ro);

                    total_size_ro += (size_ro + 4095) & ~4095;
                    total_size_rw += (size_rw + 4095) & ~4095;

                    /* go to the next file */
                    file += len;
                    cnt++;
                }
            }
        }

        kprintf("[BOOT] RO size of kernel: %dkB\n", (total_size_ro + 1023) / 1024);
        kprintf("[BOOT] RW size of kernel: %dkB\n", (total_size_rw + 1023) / 1024);

        /* Round up the RW and RO sizes to 1MB boundary, use large pages */
        total_size_ro = (total_size_ro + 1024*1024-1) & ~(1024*1024-1);
        total_size_rw = (total_size_rw + 1024*1024-1) & ~(1024*1024-1);

        kernel_phys = *mem_upper - total_size_ro - total_size_rw;
        kernel_virt = 0xf8000000;

        *mem_upper -= total_size_ro + total_size_rw;

        kprintf("[BOOT] Physical address of kernel: %p\n", kernel_phys);
        kprintf("[BOOT] Virtual address of kernel: %p\n", kernel_virt);

        /* Clean memory for kernel */
        bzero((void*)kernel_phys, total_size_ro + total_size_rw);

        /* map kernel memory for supervisor access (read/write) */
        mmu_map_section(kernel_phys, kernel_phys, total_size_ro + total_size_rw, 1, 1, 1, 1);

        /* map kernel memory for user access */
        mmu_map_section(kernel_phys, kernel_virt, total_size_ro, 1, 1, 2, 1);
        mmu_map_section(kernel_phys + total_size_ro, kernel_virt + total_size_ro, total_size_rw, 1, 1, 3, 1);

        entry = (void (*)(struct TagItem))kernel_virt;

        initAllocator(kernel_phys, kernel_phys + total_size_ro, kernel_virt - kernel_phys);

        boottag->ti_Tag = KRN_KernelLowest;
        boottag->ti_Data = kernel_virt;
        boottag++;

        boottag->ti_Tag = KRN_KernelHighest;
        boottag->ti_Data = kernel_virt + ((total_size_ro + 4095) & ~4095) + ((total_size_rw + 4095) & ~4095);
        boottag++;

        loadElf(&_binary_core_bin_start);

        if (pkg_image && pkg_size)
        {
            uint8_t *base = pkg_image;

            if (base[0] == 0x7f && base[1] == 'E' && base[2] == 'L' && base[3] == 'F')
            {
                kprintf("[BOOT] Kernel image is ELF file\n");

                getElfSize(base, &size_rw, &size_ro);

                total_size_ro += (size_ro + 4095) & ~4095;
                total_size_rw += (size_rw + 4095) & ~4095;
            }
            else if (base[0] == 'P' && base[1] == 'K' && base[2] == 'G' && base[3] == 0x01)
            {
                kprintf("[BOOT] Kernel image is a package:\n");

                uint8_t *file = base+4;
                uint32_t total_length = AROS_BE2LONG(*(uint32_t*)file); /* Total length of the module */
                const uint8_t *file_end = base+total_length;
                uint32_t len, cnt = 0;

                kprintf("[BOOT] Package size: %dKB", total_length >> 10);

                file = base + 8;

                while(file < file_end)
                {
                    const char *filename = remove_path(file+4);

                    /* get text length */
                    len = AROS_BE2LONG(*(uint32_t*)file);
                    /* display the file name */
                    if (cnt % 4 == 0)
                        kprintf("\n[BOOT]    %s", filename);
                    else
                        kprintf(", %s", filename);

                    file += len + 5;

                    len = AROS_BE2LONG(*(uint32_t *)file);
                    file += 4;

                    /* load it */
                    loadElf(file);

                    total_size_ro += (size_ro + 4095) & ~4095;
                    total_size_rw += (size_rw + 4095) & ~4095;

                    /* go to the next file */
                    file += len;
                    cnt++;
                }
                kprintf("\n");
            }
        }

        arm_flush_cache(kernel_phys, total_size_ro + total_size_rw);

        boottag->ti_Tag = KRN_KernelBss;
        boottag->ti_Data = (IPTR)tracker;
        boottag++;
    }

    boottag->ti_Tag = TAG_DONE;
    boottag->ti_Data = 0;

    kprintf("[BOOT] Kernel taglist contains %d entries\n", ((intptr_t)boottag - (intptr_t)(tmp_stack_ptr - BOOT_STACK_SIZE - BOOT_TAGS_SIZE))/sizeof(struct TagItem));
    kprintf("[BOOT] Bootstrap wasted %d bytes of memory for kernels use\n", mem_used()   );

    kprintf("[BOOT] Activating MMU\n");

    mmu_load();

    kprintf("[BOOT] Heading over to AROS kernel @ %08x\n", entry);

    entry((struct TagItem *)(tmp_stack_ptr - BOOT_STACK_SIZE - BOOT_TAGS_SIZE));

    kprintf("[BOOT] Back? Something wrong happened...\n");

    while(1);
}
