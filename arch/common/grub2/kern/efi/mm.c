/* mm.c - generic EFI memory management */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2006,2007  Free Software Foundation, Inc.
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

#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>

#define NEXT_MEMORY_DESCRIPTOR(desc, size)	\
  ((grub_efi_memory_descriptor_t *) ((char *) (desc) + (size)))

#define BYTES_TO_PAGES(bytes)	((bytes) >> 12)
#define PAGES_TO_BYTES(pages)	((pages) << 12)

/* The size of a memory map obtained from the firmware. This must be
   a multiplier of 4KB.  */
#define MEMORY_MAP_SIZE	0x1000

/* Maintain the list of allocated pages.  */
struct allocated_page
{
  grub_efi_physical_address_t addr;
  grub_efi_uint64_t num_pages;
};

#define ALLOCATED_PAGES_SIZE	0x1000
#define MAX_ALLOCATED_PAGES	\
  (ALLOCATED_PAGES_SIZE / sizeof (struct allocated_page))

static struct allocated_page *allocated_pages = 0;

/* The minimum and maximum heap size for GRUB itself.  */
#define MIN_HEAP_SIZE	0x100000
#define MAX_HEAP_SIZE	(16 * 0x100000)


/* Allocate pages. Return the pointer to the first of allocated pages.  */
void *
grub_efi_allocate_pages (grub_efi_physical_address_t address,
			 grub_efi_uintn_t pages)
{
  grub_efi_allocate_type_t type;
  grub_efi_status_t status;
  grub_efi_boot_services_t *b;

#if GRUB_CPU_SIZEOF_VOID_P < 8
  /* Limit the memory access to less than 4GB for 32-bit platforms.  */
  if (address > 0xffffffff)
    return 0;
  
  if (address == 0)
    {
      type = GRUB_EFI_ALLOCATE_MAX_ADDRESS;
      address = 0xffffffff;
    }
  else
    type = GRUB_EFI_ALLOCATE_ADDRESS;
#else
  if (address == 0)
    type = GRUB_EFI_ALLOCATE_ANY_PAGES;
  else
    type = GRUB_EFI_ALLOCATE_ADDRESS;
#endif

  b = grub_efi_system_table->boot_services;
  status = b->allocate_pages (type, GRUB_EFI_LOADER_DATA, pages, &address);
  if (status != GRUB_EFI_SUCCESS)
    return 0;

  if (address == 0)
    {
      /* Uggh, the address 0 was allocated... This is too annoying,
	 so reallocate another one.  */
      address = 0xffffffff;
      status = b->allocate_pages (type, GRUB_EFI_LOADER_DATA, pages, &address);
      grub_efi_free_pages (0, pages);
      if (status != GRUB_EFI_SUCCESS)
	return 0;
    }
      
  if (allocated_pages)
    {
      unsigned i;

      for (i = 0; i < MAX_ALLOCATED_PAGES; i++)
	if (allocated_pages[i].addr == 0)
	  {
	    allocated_pages[i].addr = address;
	    allocated_pages[i].num_pages = pages;
	    break;
	  }

      if (i == MAX_ALLOCATED_PAGES)
	grub_fatal ("too many page allocations");
    }
  
  return (void *) ((grub_addr_t) address);
}

/* Free pages starting from ADDRESS.  */
void
grub_efi_free_pages (grub_efi_physical_address_t address,
		     grub_efi_uintn_t pages)
{
  grub_efi_boot_services_t *b;

  if (allocated_pages
      && ((grub_efi_physical_address_t) ((grub_addr_t) allocated_pages)
	  != address))
    {
      unsigned i;
      
      for (i = 0; i < MAX_ALLOCATED_PAGES; i++)
	if (allocated_pages[i].addr == address)
	  {
	    allocated_pages[i].addr = 0;
	    break;
	  }
    }
  
  b = grub_efi_system_table->boot_services;
  b->free_pages (address, pages);
}

/* Get the memory map as defined in the EFI spec. Return 1 if successful,
   return 0 if partial, or return -1 if an error occurs.  */
int
grub_efi_get_memory_map (grub_efi_uintn_t *memory_map_size,
			 grub_efi_memory_descriptor_t *memory_map,
			 grub_efi_uintn_t *map_key,
			 grub_efi_uintn_t *descriptor_size,
			 grub_efi_uint32_t *descriptor_version)
{
  grub_efi_status_t status;
  grub_efi_boot_services_t *b;
  grub_efi_uintn_t key;
  grub_efi_uint32_t version;

  /* Allow some parameters to be missing.  */
  if (! map_key)
    map_key = &key;
  if (! descriptor_version)
    descriptor_version = &version;
  
  b = grub_efi_system_table->boot_services;
  status = b->get_memory_map (memory_map_size, memory_map, map_key,
			      descriptor_size, descriptor_version);
  if (status == GRUB_EFI_SUCCESS)
    return 1;
  else if (status == GRUB_EFI_BUFFER_TOO_SMALL)
    return 0;
  else
    return -1;
}

/* Sort the memory map in place.  */
static void
sort_memory_map (grub_efi_memory_descriptor_t *memory_map,
		 grub_efi_uintn_t desc_size,
		 grub_efi_memory_descriptor_t *memory_map_end)
{
  grub_efi_memory_descriptor_t *d1;
  grub_efi_memory_descriptor_t *d2;
  
  for (d1 = memory_map;
       d1 < memory_map_end;
       d1 = NEXT_MEMORY_DESCRIPTOR (d1, desc_size))
    {
      grub_efi_memory_descriptor_t *max_desc = d1;
      
      for (d2 = NEXT_MEMORY_DESCRIPTOR (d1, desc_size);
	   d2 < memory_map_end;
	   d2 = NEXT_MEMORY_DESCRIPTOR (d2, desc_size))
	{
	  if (max_desc->num_pages < d2->num_pages)
	    max_desc = d2;
	}

      if (max_desc != d1)
	{
	  grub_efi_memory_descriptor_t tmp;

	  tmp = *d1;
	  *d1 = *max_desc;
	  *max_desc = tmp;
	}
    }
}

/* Filter the descriptors. GRUB needs only available memory.  */
static grub_efi_memory_descriptor_t *
filter_memory_map (grub_efi_memory_descriptor_t *memory_map,
		   grub_efi_memory_descriptor_t *filtered_memory_map,
		   grub_efi_uintn_t desc_size,
		   grub_efi_memory_descriptor_t *memory_map_end)
{
  grub_efi_memory_descriptor_t *desc;
  grub_efi_memory_descriptor_t *filtered_desc;

  for (desc = memory_map, filtered_desc = filtered_memory_map;
       desc < memory_map_end;
       desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size))
    {
      if (desc->type == GRUB_EFI_CONVENTIONAL_MEMORY
#if GRUB_CPU_SIZEOF_VOID_P < 8
	  && desc->physical_start <= 0xffffffff
#endif
	  && desc->physical_start + PAGES_TO_BYTES (desc->num_pages) > 0x100000
	  && desc->num_pages != 0)
	{
	  grub_memcpy (filtered_desc, desc, desc_size);
	  
	  /* Avoid less than 1MB, because some loaders seem to be confused.  */
	  if (desc->physical_start < 0x100000)
	    {
	      desc->num_pages -= BYTES_TO_PAGES (0x100000
						 - desc->physical_start);
	      desc->physical_start = 0x100000;
	    }
	  
#if GRUB_CPU_SIZEOF_VOID_P < 8
	  if (BYTES_TO_PAGES (filtered_desc->physical_start)
	      + filtered_desc->num_pages
	      > BYTES_TO_PAGES (0x100000000LL))
	    filtered_desc->num_pages
	      = (BYTES_TO_PAGES (0x100000000LL)
		 - BYTES_TO_PAGES (filtered_desc->physical_start));
#endif
	  
	  if (filtered_desc->num_pages == 0)
	    continue;
	  
	  filtered_desc = NEXT_MEMORY_DESCRIPTOR (filtered_desc, desc_size);
	}
    }

  return filtered_desc;
}

/* Return the total number of pages.  */
static grub_efi_uint64_t
get_total_pages (grub_efi_memory_descriptor_t *memory_map,
		 grub_efi_uintn_t desc_size,
		 grub_efi_memory_descriptor_t *memory_map_end)
{
  grub_efi_memory_descriptor_t *desc;
  grub_efi_uint64_t total = 0;
  
  for (desc = memory_map;
       desc < memory_map_end;
       desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size))
    total += desc->num_pages;

  return total;
}

/* Add memory regions.  */
static void
add_memory_regions (grub_efi_memory_descriptor_t *memory_map,
		    grub_efi_uintn_t desc_size,
		    grub_efi_memory_descriptor_t *memory_map_end,
		    grub_efi_uint64_t required_pages)
{
  grub_efi_memory_descriptor_t *desc;
  
  for (desc = memory_map;
       desc < memory_map_end;
       desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size))
    {
      grub_efi_uint64_t pages;
      grub_efi_physical_address_t start;
      void *addr;

      start = desc->physical_start;
      pages = desc->num_pages;
      if (pages > required_pages)
	{
	  start += PAGES_TO_BYTES (pages - required_pages);
	  pages = required_pages;
	}

      addr = grub_efi_allocate_pages (start, pages);
      if (! addr)
	grub_fatal ("cannot allocate conventional memory %p with %u pages",
		    (void *) ((grub_addr_t) start),
		    (unsigned) pages);

      grub_mm_init_region (addr, PAGES_TO_BYTES (pages));

      required_pages -= pages;
      if (required_pages == 0)
	break;
    }

  if (required_pages > 0)
    grub_fatal ("too little memory");
}

#if 0
/* Print the memory map.  */
static void
print_memory_map (grub_efi_memory_descriptor_t *memory_map,
		  grub_efi_uintn_t desc_size,
		  grub_efi_memory_descriptor_t *memory_map_end)
{
  grub_efi_memory_descriptor_t *desc;
  int i;
  
  for (desc = memory_map, i = 0;
       desc < memory_map_end;
       desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size), i++)
    {
      grub_printf ("MD: t=%x, p=%llx, v=%llx, n=%llx, a=%llx\n",
		   desc->type, desc->physical_start, desc->virtual_start,
		   desc->num_pages, desc->attribute);
    }
}
#endif

void
grub_efi_mm_init (void)
{
  grub_efi_memory_descriptor_t *memory_map;
  grub_efi_memory_descriptor_t *memory_map_end;
  grub_efi_memory_descriptor_t *filtered_memory_map;
  grub_efi_memory_descriptor_t *filtered_memory_map_end;
  grub_efi_uintn_t map_size;
  grub_efi_uintn_t desc_size;
  grub_efi_uint64_t total_pages;
  grub_efi_uint64_t required_pages;

  /* First of all, allocate pages to maintain allocations.  */
  allocated_pages
    = grub_efi_allocate_pages (0, BYTES_TO_PAGES (ALLOCATED_PAGES_SIZE));
  if (! allocated_pages)
    grub_fatal ("cannot allocate memory");

  grub_memset (allocated_pages, 0, ALLOCATED_PAGES_SIZE);
  
  /* Prepare a memory region to store two memory maps.  */
  memory_map = grub_efi_allocate_pages (0,
					2 * BYTES_TO_PAGES (MEMORY_MAP_SIZE));
  if (! memory_map)
    grub_fatal ("cannot allocate memory");

  filtered_memory_map = NEXT_MEMORY_DESCRIPTOR (memory_map, MEMORY_MAP_SIZE);

  /* Obtain descriptors for available memory.  */
  map_size = MEMORY_MAP_SIZE;

  if (grub_efi_get_memory_map (&map_size, memory_map, 0, &desc_size, 0) < 0)
    grub_fatal ("cannot get memory map");

  memory_map_end = NEXT_MEMORY_DESCRIPTOR (memory_map, map_size);
  
  filtered_memory_map_end = filter_memory_map (memory_map, filtered_memory_map,
					       desc_size, memory_map_end);
  
  /* By default, request a quarter of the available memory.  */
  total_pages = get_total_pages (filtered_memory_map, desc_size,
				 filtered_memory_map_end);
  required_pages = (total_pages >> 2);
  if (required_pages < BYTES_TO_PAGES (MIN_HEAP_SIZE))
    required_pages = BYTES_TO_PAGES (MIN_HEAP_SIZE);
  else if (required_pages > BYTES_TO_PAGES (MAX_HEAP_SIZE))
    required_pages = BYTES_TO_PAGES (MAX_HEAP_SIZE);

  /* Sort the filtered descriptors, so that GRUB can allocate pages
     from smaller regions.  */
  sort_memory_map (filtered_memory_map, desc_size, filtered_memory_map_end);

  /* Allocate memory regions for GRUB's memory management.  */
  add_memory_regions (filtered_memory_map, desc_size,
		      filtered_memory_map_end, required_pages);

#if 0
  /* For debug.  */
  map_size = MEMORY_MAP_SIZE;

  if (grub_efi_get_memory_map (&map_size, memory_map, 0, &desc_size, 0) < 0)
    grub_fatal ("cannot get memory map");

  grub_printf ("printing memory map\n");
  print_memory_map (memory_map, desc_size,
		    NEXT_MEMORY_DESCRIPTOR (memory_map, map_size));
  grub_abort ();
#endif
  
  /* Release the memory maps.  */
  grub_efi_free_pages ((grub_addr_t) memory_map,
		       2 * BYTES_TO_PAGES (MEMORY_MAP_SIZE));
}

void
grub_efi_mm_fini (void)
{
  if (allocated_pages)
    {
      unsigned i;

      for (i = 0; i < MAX_ALLOCATED_PAGES; i++)
	{
	  struct allocated_page *p;

	  p = allocated_pages + i;
	  if (p->addr != 0)
	    grub_efi_free_pages ((grub_addr_t) p->addr, p->num_pages);
	}

      grub_efi_free_pages ((grub_addr_t) allocated_pages,
			   BYTES_TO_PAGES (ALLOCATED_PAGES_SIZE));
    }
}
