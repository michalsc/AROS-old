/* fixvideo.c - fix video problem in efi */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2009  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/dl.h>
#include <grub/misc.h>
#include <grub/file.h>
#include <grub/pci.h>
#include <grub/command.h>

static struct grub_video_patch
{
  const char *name;
  grub_uint32_t pci_id;
  grub_uint32_t mmio_bar;
  grub_uint32_t mmio_reg;
  grub_uint32_t mmio_old;
} video_patches[] =
  {
    {"Intel 945GM", 0x27a28086, 0, 0x71184, 0x1000000}, /* DSPBBASE  */
    {"Intel 965GM", 0x2a028086, 0, 0x7119C, 0x1000000}, /* DSPBSURF  */
    {0, 0, 0, 0, 0}
  };

static int NESTED_FUNC_ATTR
scan_card (int bus, int dev, int func, grub_pci_id_t pciid)
{
  grub_pci_address_t addr;

  addr = grub_pci_make_address (bus, dev, func, 2);
  if (grub_pci_read_byte (addr + 3) == 0x3)
    {
      struct grub_video_patch *p = video_patches;

      while (p->name)
	{
	  if (p->pci_id == pciid)
	    {
	      grub_target_addr_t base;

	      grub_printf ("Found graphic card: %s\n", p->name);
	      addr += 8 + p->mmio_bar * 4;
	      base = grub_pci_read (addr);
	      if ((! base) || (base & GRUB_PCI_ADDR_SPACE_IO) ||
		  (base & GRUB_PCI_ADDR_MEM_PREFETCH))
		grub_printf ("Invalid MMIO bar %d\n", p->mmio_bar);
	      else
		{
		  base &= GRUB_PCI_ADDR_MEM_MASK;
		  base += p->mmio_reg;

		  if (*((volatile grub_uint32_t *) base) != p->mmio_old)
		    grub_printf ("Old value don't match\n");
		  else
		    {
		      *((volatile grub_uint32_t *) base) = 0;
		      if (*((volatile grub_uint32_t *) base))
			grub_printf ("Set MMIO fails\n");
		    }
		}

	      return 1;
	    }
	  p++;
	}

      grub_printf ("Unknown graphic card: %x\n", pciid);
    }

  return 0;
}

static grub_err_t
grub_cmd_fixvideo (grub_command_t cmd __attribute__ ((unused)),
		   int argc __attribute__ ((unused)),
		   char *argv[] __attribute__ ((unused)))
{
  grub_pci_iterate (scan_card);
  return 0;
}

static grub_command_t cmd_fixvideo;

GRUB_MOD_INIT(fixvideo)
{
  cmd_fixvideo = grub_register_command ("fix_video", grub_cmd_fixvideo,
					0, "Fix video problem.");

}

GRUB_MOD_FINI(fixvideo)
{
  grub_unregister_command (cmd_fixvideo);
}
