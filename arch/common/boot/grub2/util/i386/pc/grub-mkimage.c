/* grub-mkimage.c - make a bootable image */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009  Free Software Foundation, Inc.
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

#include <config.h>
#include <grub/types.h>
#include <grub/machine/boot.h>
#include <grub/machine/kernel.h>
#include <grub/machine/memory.h>
#include <grub/kernel.h>
#include <grub/disk.h>
#include <grub/util/misc.h>
#include <grub/util/resolve.h>
#include <grub/misc.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define _GNU_SOURCE	1
#include <getopt.h>

#ifdef ENABLE_LZMA
#include <grub/lib/LzmaEnc.h>

static void *SzAlloc(void *p, size_t size) { p = p; return xmalloc(size); }
static void SzFree(void *p, void *address) { p = p; free(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

static void
compress_kernel (char *kernel_img, size_t kernel_size,
		 char **core_img, size_t *core_size)
{
  CLzmaEncProps props;
  unsigned char out_props[5];
  size_t out_props_size = 5;

  LzmaEncProps_Init(&props);
  props.dictSize = 1 << 16;
  props.lc = 3;
  props.lp = 0;
  props.pb = 2;
  props.numThreads = 1;

  if (kernel_size < GRUB_KERNEL_MACHINE_RAW_SIZE)
    grub_util_error ("the core image is too small");

  *core_img = xmalloc (kernel_size);
  memcpy (*core_img, kernel_img, GRUB_KERNEL_MACHINE_RAW_SIZE);

  *core_size = kernel_size - GRUB_KERNEL_MACHINE_RAW_SIZE;
  if (LzmaEncode((unsigned char *) *core_img + GRUB_KERNEL_MACHINE_RAW_SIZE,
                 core_size,
                 (unsigned char *) kernel_img + GRUB_KERNEL_MACHINE_RAW_SIZE,
                 kernel_size - GRUB_KERNEL_MACHINE_RAW_SIZE,
                 &props, out_props, &out_props_size,
                 0, NULL, &g_Alloc, &g_Alloc) != SZ_OK)
    grub_util_error ("cannot compress the kernel image");

  *core_size += GRUB_KERNEL_MACHINE_RAW_SIZE;
}

#else	/* No lzma compression */

static void
compress_kernel (char *kernel_img, size_t kernel_size,
               char **core_img, size_t *core_size)
{
  *core_img = xmalloc (kernel_size);
  memcpy (*core_img, kernel_img, kernel_size);
  *core_size = kernel_size;
}

#endif	/* No lzma compression */

static void
generate_image (const char *dir, char *prefix, FILE *out, char *mods[],
		char *memdisk_path, char *config_path)
{
  char *kernel_img, *boot_img, *core_img;
  size_t kernel_size, boot_size, total_module_size, core_size;
  size_t memdisk_size = 0, config_size = 0;
  char *kernel_path, *boot_path;
  unsigned num;
  size_t offset;
  struct grub_util_path_list *path_list, *p, *next;
  struct grub_module_info *modinfo;

  path_list = grub_util_resolve_dependencies (dir, "moddep.lst", mods);

  kernel_path = grub_util_get_path (dir, "kernel.img");
  kernel_size = grub_util_get_image_size (kernel_path);

  total_module_size = sizeof (struct grub_module_info);

  if (memdisk_path)
    {
      memdisk_size = ALIGN_UP(grub_util_get_image_size (memdisk_path), 512);
      grub_util_info ("the size of memory disk is 0x%x", memdisk_size);
      total_module_size += memdisk_size + sizeof (struct grub_module_header);
    }

  if (config_path)
    {
      config_size = grub_util_get_image_size (config_path) + 1;
      grub_util_info ("the size of config file is 0x%x", config_size);
      total_module_size += config_size + sizeof (struct grub_module_header);
    }

  for (p = path_list; p; p = p->next)
    total_module_size += (grub_util_get_image_size (p->name)
			  + sizeof (struct grub_module_header));

  grub_util_info ("the total module size is 0x%x", total_module_size);

  kernel_img = xmalloc (kernel_size + total_module_size);
  grub_util_load_image (kernel_path, kernel_img);

  if (GRUB_KERNEL_MACHINE_PREFIX + strlen (prefix) + 1 > GRUB_KERNEL_MACHINE_DATA_END)
    grub_util_error ("prefix too long");
  strcpy (kernel_img + GRUB_KERNEL_MACHINE_PREFIX, prefix);

  /* Fill in the grub_module_info structure.  */
  modinfo = (struct grub_module_info *) (kernel_img + kernel_size);
  memset (modinfo, 0, sizeof (struct grub_module_info));
  modinfo->magic = GRUB_MODULE_MAGIC;
  modinfo->offset = sizeof (struct grub_module_info);
  modinfo->size = total_module_size;

  offset = kernel_size + sizeof (struct grub_module_info);
  for (p = path_list; p; p = p->next)
    {
      struct grub_module_header *header;
      size_t mod_size;

      mod_size = grub_util_get_image_size (p->name);

      header = (struct grub_module_header *) (kernel_img + offset);
      memset (header, 0, sizeof (struct grub_module_header));
      header->type = grub_cpu_to_le32 (OBJ_TYPE_ELF);
      header->size = grub_cpu_to_le32 (mod_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (p->name, kernel_img + offset);
      offset += mod_size;
    }

  if (memdisk_path)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      memset (header, 0, sizeof (struct grub_module_header));
      header->type = grub_cpu_to_le32 (OBJ_TYPE_MEMDISK);
      header->size = grub_cpu_to_le32 (memdisk_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (memdisk_path, kernel_img + offset);
      offset += memdisk_size;
    }

  if (config_path)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      memset (header, 0, sizeof (struct grub_module_header));
      header->type = grub_cpu_to_le32 (OBJ_TYPE_CONFIG);
      header->size = grub_cpu_to_le32 (config_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (config_path, kernel_img + offset);
      offset += config_size;
      *(kernel_img + offset - 1) = 0;
    }

  grub_util_info ("kernel_img=%p, kernel_size=0x%x", kernel_img, kernel_size);
  compress_kernel (kernel_img, kernel_size + total_module_size,
		   &core_img, &core_size);

  grub_util_info ("the core size is 0x%x", core_size);

  num = ((core_size + GRUB_DISK_SECTOR_SIZE - 1) >> GRUB_DISK_SECTOR_BITS);
  if (num > 0xffff)
    grub_util_error ("the core image is too big");

#if defined(GRUB_MACHINE_PCBIOS)

  boot_path = grub_util_get_path (dir, "diskboot.img");
  boot_size = grub_util_get_image_size (boot_path);
  if (boot_size != GRUB_DISK_SECTOR_SIZE)
    grub_util_error ("diskboot.img is not one sector size");

  boot_img = grub_util_read_image (boot_path);

  /* i386 is a little endian architecture.  */
  *((grub_uint16_t *) (boot_img + GRUB_DISK_SECTOR_SIZE
		       - GRUB_BOOT_MACHINE_LIST_SIZE + 8))
    = grub_cpu_to_le16 (num);

  grub_util_write_image (boot_img, boot_size, out);
  free (boot_img);
  free (boot_path);

#elif defined(GRUB_MACHINE_QEMU)

  {
    char *rom_img;
    size_t rom_size;

    boot_path = grub_util_get_path (dir, "boot.img");
    boot_size = grub_util_get_image_size (boot_path);
    boot_img = grub_util_read_image (boot_path);

    /* Rom sizes must be 64k-aligned.  */
    rom_size = ALIGN_UP (core_size + boot_size, 64 * 1024);

    rom_img = xmalloc (rom_size);
    memset (rom_img, 0, rom_size);

    *((grub_int32_t *) (core_img + GRUB_KERNEL_MACHINE_CORE_ENTRY_ADDR))
      = grub_cpu_to_le32 ((grub_uint32_t) -rom_size);

    memcpy (rom_img, core_img, core_size);

    *((grub_int32_t *) (boot_img + GRUB_BOOT_MACHINE_CORE_ENTRY_ADDR))
      = grub_cpu_to_le32 ((grub_uint32_t) -rom_size);

    memcpy (rom_img + rom_size - boot_size, boot_img, boot_size);

    free (core_img);
    core_img = rom_img;
    core_size = rom_size;

    free (boot_img);
    free (boot_path);
  }

#endif

#ifdef GRUB_KERNEL_MACHINE_TOTAL_MODULE_SIZE
  *((grub_uint32_t *) (core_img + GRUB_KERNEL_MACHINE_TOTAL_MODULE_SIZE))
    = grub_cpu_to_le32 (total_module_size);
#endif
  *((grub_uint32_t *) (core_img + GRUB_KERNEL_MACHINE_KERNEL_IMAGE_SIZE))
    = grub_cpu_to_le32 (kernel_size);
#ifdef GRUB_KERNEL_MACHINE_COMPRESSED_SIZE
  *((grub_uint32_t *) (core_img + GRUB_KERNEL_MACHINE_COMPRESSED_SIZE))
    = grub_cpu_to_le32 (core_size - GRUB_KERNEL_MACHINE_RAW_SIZE);
#endif

#if defined(GRUB_KERNEL_MACHINE_INSTALL_DOS_PART) && defined(GRUB_KERNEL_MACHINE_INSTALL_BSD_PART)
  /* If we included a drive in our prefix, let GRUB know it doesn't have to
     prepend the drive told by BIOS.  */
  if (prefix[0] == '(')
    {
      *((grub_int32_t *) (core_img + GRUB_KERNEL_MACHINE_INSTALL_DOS_PART))
	= grub_cpu_to_le32 (-2);
      *((grub_int32_t *) (core_img + GRUB_KERNEL_MACHINE_INSTALL_BSD_PART))
	= grub_cpu_to_le32 (-2);
    }
#endif

#ifdef GRUB_MACHINE_PCBIOS
  if (GRUB_KERNEL_MACHINE_LINK_ADDR + core_size > GRUB_MEMORY_MACHINE_UPPER)
    grub_util_error ("Core image is too big (%p > %p)\n",
 		     GRUB_KERNEL_MACHINE_LINK_ADDR + core_size, GRUB_MEMORY_MACHINE_UPPER);
#endif

  grub_util_write_image (core_img, core_size, out);
  free (kernel_img);
  free (core_img);
  free (kernel_path);

  while (path_list)
    {
      next = path_list->next;
      free ((void *) path_list->name);
      free (path_list);
      path_list = next;
    }
}



static struct option options[] =
  {
    {"directory", required_argument, 0, 'd'},
    {"prefix", required_argument, 0, 'p'},
    {"memdisk", required_argument, 0, 'm'},
    {"config", required_argument, 0, 'c'},
    {"output", required_argument, 0, 'o'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {"verbose", no_argument, 0, 'v'},
    {0, 0, 0, 0}
  };

static void
usage (int status)
{
  if (status)
    fprintf (stderr, "Try ``grub-mkimage --help'' for more information.\n");
  else
    printf ("\
Usage: grub-mkimage [OPTION]... [MODULES]\n\
\n\
Make a bootable image of GRUB.\n\
\n\
  -d, --directory=DIR     use images and modules under DIR [default=%s]\n\
  -p, --prefix=DIR        set grub_prefix directory [default=%s]\n\
  -m, --memdisk=FILE      embed FILE as a memdisk image\n\
  -c, --config=FILE       embed FILE as boot config\n\
  -o, --output=FILE       output a generated image to FILE [default=stdout]\n\
  -h, --help              display this message and exit\n\
  -V, --version           print version information and exit\n\
  -v, --verbose           print verbose messages\n\
\n\
Report bugs to <%s>.\n\
", GRUB_LIBDIR, DEFAULT_DIRECTORY, PACKAGE_BUGREPORT);

  exit (status);
}

int
main (int argc, char *argv[])
{
  char *output = NULL;
  char *dir = NULL;
  char *prefix = NULL;
  char *memdisk = NULL;
  char *config = NULL;
  FILE *fp = stdout;

  progname = "grub-mkimage";

  while (1)
    {
      int c = getopt_long (argc, argv, "d:p:m:c:o:hVv", options, 0);

      if (c == -1)
	break;
      else
	switch (c)
	  {
	  case 'o':
	    if (output)
	      free (output);

	    output = xstrdup (optarg);
	    break;

	  case 'd':
	    if (dir)
	      free (dir);

	    dir = xstrdup (optarg);
	    break;

	  case 'm':
	    if (memdisk)
	      free (memdisk);

	    memdisk = xstrdup (optarg);

	    if (prefix)
	      free (prefix);

	    prefix = xstrdup ("(memdisk)/boot/grub");
	    break;

	  case 'c':
	    if (config)
	      free (config);

	    config = xstrdup (optarg);
	    break;

	  case 'h':
	    usage (0);
	    break;

	  case 'p':
	    if (prefix)
	      free (prefix);

	    prefix = xstrdup (optarg);
	    break;

	  case 'V':
	    printf ("grub-mkimage (%s) %s\n", PACKAGE_NAME, PACKAGE_VERSION);
	    return 0;

	  case 'v':
	    verbosity++;
	    break;

	  default:
	    usage (1);
	    break;
	  }
    }

  if (output)
    {
      fp = fopen (output, "wb");
      if (! fp)
	grub_util_error ("cannot open %s", output);
      free (output);
    }

  generate_image (dir ? : GRUB_LIBDIR, prefix ? : DEFAULT_DIRECTORY, fp,
		  argv + optind, memdisk, config);

  fclose (fp);

  if (dir)
    free (dir);

  return 0;
}
