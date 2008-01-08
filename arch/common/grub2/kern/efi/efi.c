/* efi.c - generic EFI support */
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
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/console_control.h>
#include <grub/efi/pe32.h>
#include <grub/machine/time.h>
#include <grub/term.h>
#include <grub/kernel.h>
#include <grub/mm.h>

/* The handle of GRUB itself. Filled in by the startup code.  */
grub_efi_handle_t grub_efi_image_handle;

/* The pointer to a system table. Filled in by the startup code.  */
grub_efi_system_table_t *grub_efi_system_table;

static grub_efi_guid_t console_control_guid = GRUB_EFI_CONSOLE_CONTROL_GUID;
static grub_efi_guid_t loaded_image_guid = GRUB_EFI_LOADED_IMAGE_GUID;
static grub_efi_guid_t device_path_guid = GRUB_EFI_DEVICE_PATH_GUID;

void *
grub_efi_locate_protocol (grub_efi_guid_t *protocol, void *registration)
{
  void *interface;
  grub_efi_status_t status;
  
  status = grub_efi_system_table->boot_services->locate_protocol (protocol,
								  registration,
								  &interface);
  if (status != GRUB_EFI_SUCCESS)
    return 0;
  
  return interface;
}

/* Return the array of handles which meet the requirement. If successful,
   the number of handles is stored in NUM_HANDLES. The array is allocated
   from the heap.  */
grub_efi_handle_t *
grub_efi_locate_handle (grub_efi_locate_search_type_t search_type,
			grub_efi_guid_t *protocol,
			void *search_key,
			grub_efi_uintn_t *num_handles)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;
  grub_efi_handle_t *buffer;
  grub_efi_uintn_t buffer_size = 8 * sizeof (grub_efi_handle_t);
  
  buffer = grub_malloc (buffer_size);
  if (! buffer)
    return 0;
  
  b = grub_efi_system_table->boot_services;
  status = b->locate_handle (search_type, protocol, search_key,
			     &buffer_size, buffer);
  if (status == GRUB_EFI_BUFFER_TOO_SMALL)
    {
      grub_free (buffer);
      buffer = grub_malloc (buffer_size);
      if (! buffer)
	return 0;
      
      status = b->locate_handle (search_type, protocol, search_key,
				 &buffer_size, buffer);
    }

  if (status != GRUB_EFI_SUCCESS)
    {
      grub_free (buffer);
      return 0;
    }

  *num_handles = buffer_size / sizeof (grub_efi_handle_t);
  return buffer;
}

void *
grub_efi_open_protocol (grub_efi_handle_t handle,
			grub_efi_guid_t *protocol,
			grub_efi_uint32_t attributes)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;
  void *interface;
  
  b = grub_efi_system_table->boot_services;
  status = b->open_protocol (handle,
			     protocol,
			     &interface,
			     grub_efi_image_handle,
			     0,
			     attributes);
  if (status != GRUB_EFI_SUCCESS)
    return 0;

  return interface;
}

int
grub_efi_set_text_mode (int on)
{
  grub_efi_console_control_protocol_t *c;
  grub_efi_screen_mode_t mode, new_mode;

  c = grub_efi_locate_protocol (&console_control_guid, 0);
  if (! c)
    /* No console control protocol instance available, assume it is
       already in text mode. */
    return 1;
  
  if (c->get_mode (c, &mode, 0, 0) != GRUB_EFI_SUCCESS)
    return 0;

  new_mode = on ? GRUB_EFI_SCREEN_TEXT : GRUB_EFI_SCREEN_GRAPHICS;
  if (mode != new_mode)
    if (c->set_mode (c, new_mode) != GRUB_EFI_SUCCESS)
      return 0;

  return 1;
}

void
grub_efi_stall (grub_efi_uintn_t microseconds)
{
  grub_efi_system_table->boot_services->stall (microseconds);
}

grub_efi_loaded_image_t *
grub_efi_get_loaded_image (grub_efi_handle_t image_handle)
{
  return grub_efi_open_protocol (image_handle,
				 &loaded_image_guid,
				 GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
}

void
grub_exit (void)
{
  grub_efi_fini ();
  grub_efi_system_table->boot_services->exit (grub_efi_image_handle,
					      GRUB_EFI_SUCCESS,
					      0, 0);
}

int
grub_efi_exit_boot_services (grub_efi_uintn_t map_key)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;
  
  b = grub_efi_system_table->boot_services;
  status = b->exit_boot_services (grub_efi_image_handle, map_key);
  return status == GRUB_EFI_SUCCESS;
}

grub_uint32_t
grub_get_rtc (void)
{
  grub_efi_time_t time;
  grub_efi_runtime_services_t *r;

  r = grub_efi_system_table->runtime_services;
  if (r->get_time (&time, 0) != GRUB_EFI_SUCCESS)
    /* What is possible in this case?  */
    return 0;

  return (((time.minute * 60 + time.second) * 1000
	   + time.nanosecond / 1000000)
	  * GRUB_TICKS_PER_SECOND / 1000);
}

/* Search the mods section from the PE32/PE32+ image. This code uses
   a PE32 header, but should work with PE32+ as well.  */
grub_addr_t
grub_arch_modules_addr (void)
{
  grub_efi_loaded_image_t *image;
  struct grub_pe32_header *header;
  struct grub_pe32_coff_header *coff_header;
  struct grub_pe32_section_table *sections;
  struct grub_pe32_section_table *section;
  struct grub_module_info *info;
  grub_uint16_t i;
  
  image = grub_efi_get_loaded_image (grub_efi_image_handle);
  if (! image)
    return 0;

  header = image->image_base;
  coff_header = &(header->coff_header);
  sections
    = (struct grub_pe32_section_table *) ((char *) coff_header
					  + sizeof (*coff_header)
					  + coff_header->optional_header_size);

  for (i = 0, section = sections;
       i < coff_header->num_sections;
       i++, section++)
    {
      if (grub_strcmp (section->name, "mods") == 0)
	break;
    }

  if (i == coff_header->num_sections)
    return 0;

  info = (struct grub_module_info *) ((char *) image->image_base
				      + section->virtual_address);
  if (info->magic != GRUB_MODULE_MAGIC)
    return 0;

  return (grub_addr_t) info;
}

char *
grub_efi_get_filename (grub_efi_device_path_t *dp)
{
  char *name = 0;
  
  while (1)
    {
      grub_efi_uint8_t type = GRUB_EFI_DEVICE_PATH_TYPE (dp);
      grub_efi_uint8_t subtype = GRUB_EFI_DEVICE_PATH_SUBTYPE (dp);

      if (type == GRUB_EFI_END_DEVICE_PATH_TYPE)
	break;
      else if (type == GRUB_EFI_MEDIA_DEVICE_PATH_TYPE
	       && subtype == GRUB_EFI_FILE_PATH_DEVICE_PATH_SUBTYPE)
	{
	  grub_efi_file_path_device_path_t *fp;
	  grub_efi_uint16_t len;
	  char *p;
	  grub_size_t size;

	  if (name)
	    {
	      size = grub_strlen (name);
	      name[size] = '/';
	      size++;
	    }
	  else
	    size = 0;
	  
	  len = ((GRUB_EFI_DEVICE_PATH_LENGTH (dp) - 4)
		 / sizeof (grub_efi_char16_t));
	  p = grub_realloc (name, size + len * 4 + 1);
	  if (! p)
	    {
	      grub_free (name);
	      return 0;
	    }

	  name = p;
	  fp = (grub_efi_file_path_device_path_t *) dp;
	  *grub_utf16_to_utf8 ((grub_uint8_t *) name + size,
			       fp->path_name, len) = '\0';
	}

      dp = GRUB_EFI_NEXT_DEVICE_PATH (dp);
    }

  if (name)
    {
      /* EFI breaks paths with backslashes.  */
      char *p;

      for (p = name; *p; p++)
	if (*p == '\\')
	  *p = '/';
    }

  return name;
}

grub_efi_device_path_t *
grub_efi_get_device_path (grub_efi_handle_t handle)
{
  return grub_efi_open_protocol (handle, &device_path_guid,
				 GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
}

/* Print the chain of Device Path nodes. This is mainly for debugging. */
void
grub_efi_print_device_path (grub_efi_device_path_t *dp)
{
  while (1)
    {
      grub_efi_uint8_t type = GRUB_EFI_DEVICE_PATH_TYPE (dp);
      grub_efi_uint8_t subtype = GRUB_EFI_DEVICE_PATH_SUBTYPE (dp);
      grub_efi_uint16_t len = GRUB_EFI_DEVICE_PATH_LENGTH (dp);

      switch (type)
	{
	case GRUB_EFI_END_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE:
	      grub_printf ("/EndEntire\n");
	      //grub_putchar ('\n');
	      break;
	    case GRUB_EFI_END_THIS_DEVICE_PATH_SUBTYPE:
	      grub_printf ("/EndThis\n");
	      //grub_putchar ('\n');
	      break;
	    default:
	      grub_printf ("/EndUnknown(%x)\n", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_HARDWARE_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_PCI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_pci_device_path_t pci;
		grub_memcpy (&pci, dp, len);
		grub_printf ("/PCI(%x,%x)",
			     (unsigned) pci.function, (unsigned) pci.device);
	      }
	      break;
	    case GRUB_EFI_PCCARD_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_pccard_device_path_t pccard;
		grub_memcpy (&pccard, dp, len);
		grub_printf ("/PCCARD(%x)",
			     (unsigned) pccard.function);
	      }
	      break;
	    case GRUB_EFI_MEMORY_MAPPED_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_memory_mapped_device_path_t mmapped;
		grub_memcpy (&mmapped, dp, len);
		grub_printf ("/MMap(%x,%llx,%llx)",
			     (unsigned) mmapped.memory_type,
			     mmapped.start_address,
			     mmapped.end_address);
	      }
	      break;
	    case GRUB_EFI_VENDOR_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_vendor_device_path_t vendor;
		grub_memcpy (&vendor, dp, sizeof (vendor));
		grub_printf ("/Vendor(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) vendor.vendor_guid.data1,
			     (unsigned) vendor.vendor_guid.data2,
			     (unsigned) vendor.vendor_guid.data3,
			     (unsigned) vendor.vendor_guid.data4[0],
			     (unsigned) vendor.vendor_guid.data4[1],
			     (unsigned) vendor.vendor_guid.data4[2],
			     (unsigned) vendor.vendor_guid.data4[3],
			     (unsigned) vendor.vendor_guid.data4[4],
			     (unsigned) vendor.vendor_guid.data4[5],
			     (unsigned) vendor.vendor_guid.data4[6],
			     (unsigned) vendor.vendor_guid.data4[7]);
	      }
	      break;
	    case GRUB_EFI_CONTROLLER_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_controller_device_path_t controller;
		grub_memcpy (&controller, dp, len);
		grub_printf ("/Ctrl(%x)",
			     (unsigned) controller.controller_number);
	      }
	      break;
	    default:
	      grub_printf ("/UnknownHW(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_ACPI_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_ACPI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_acpi_device_path_t acpi;
		grub_memcpy (&acpi, dp, len);
		grub_printf ("/ACPI(%x,%x)",
			     (unsigned) acpi.hid,
			     (unsigned) acpi.uid);
	      }
	      break;
	    case GRUB_EFI_EXPANDED_ACPI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_expanded_acpi_device_path_t eacpi;
		grub_memcpy (&eacpi, dp, sizeof (eacpi));
		grub_printf ("/ACPI(");
		
		if (GRUB_EFI_EXPANDED_ACPI_HIDSTR (dp)[0] == '\0')
		  grub_printf ("%x,", (unsigned) eacpi.hid);
		else
		  grub_printf ("%s,", GRUB_EFI_EXPANDED_ACPI_HIDSTR (dp));
		
		if (GRUB_EFI_EXPANDED_ACPI_UIDSTR (dp)[0] == '\0')
		  grub_printf ("%x,", (unsigned) eacpi.uid);
		else
		  grub_printf ("%s,", GRUB_EFI_EXPANDED_ACPI_UIDSTR (dp));
		
		if (GRUB_EFI_EXPANDED_ACPI_CIDSTR (dp)[0] == '\0')
		  grub_printf ("%x)", (unsigned) eacpi.cid);
		else
		  grub_printf ("%s)", GRUB_EFI_EXPANDED_ACPI_CIDSTR (dp));
	      }
	      break;
	    default:
	      grub_printf ("/UnknownACPI(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_MESSAGING_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_ATAPI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_atapi_device_path_t atapi;
		grub_memcpy (&atapi, dp, len);
		grub_printf ("/ATAPI(%x,%x,%x)",
			     (unsigned) atapi.primary_secondary,
			     (unsigned) atapi.slave_master,
			     (unsigned) atapi.lun);
	      }
	      break;
	    case GRUB_EFI_SCSI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_scsi_device_path_t scsi;
		grub_memcpy (&scsi, dp, len);
		grub_printf ("/SCSI(%x,%x)",
			     (unsigned) scsi.pun,
			     (unsigned) scsi.lun);
	      }
	      break;
	    case GRUB_EFI_FIBRE_CHANNEL_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_fibre_channel_device_path_t fc;
		grub_memcpy (&fc, dp, len);
		grub_printf ("/FibreChannel(%llx,%llx)",
			     fc.wwn, fc.lun);
	      }
	      break;
	    case GRUB_EFI_1394_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_1394_device_path_t firewire;
		grub_memcpy (&firewire, dp, len);
		grub_printf ("/1394(%llx)", firewire.guid);
	      }
	      break;
	    case GRUB_EFI_USB_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_usb_device_path_t usb;
		grub_memcpy (&usb, dp, len);
		grub_printf ("/USB(%x,%x)",
			     (unsigned) usb.parent_port_number,
			     (unsigned) usb.interface);
	      }
	      break;
	    case GRUB_EFI_USB_CLASS_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_usb_class_device_path_t usb_class;
		grub_memcpy (&usb_class, dp, len);
		grub_printf ("/USBClass(%x,%x,%x,%x,%x)",
			     (unsigned) usb_class.vendor_id,
			     (unsigned) usb_class.product_id,
			     (unsigned) usb_class.device_class,
			     (unsigned) usb_class.device_subclass,
			     (unsigned) usb_class.device_protocol);
	      }
	      break;
	    case GRUB_EFI_I2O_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_i2o_device_path_t i2o;
		grub_memcpy (&i2o, dp, len);
		grub_printf ("/I2O(%x)", (unsigned) i2o.tid);
	      }
	      break;
	    case GRUB_EFI_MAC_ADDRESS_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_mac_address_device_path_t mac;
		grub_memcpy (&mac, dp, len);
		grub_printf ("/MacAddr(%02x:%02x:%02x:%02x:%02x:%02x,%x)",
			     (unsigned) mac.mac_address[0],
			     (unsigned) mac.mac_address[1],
			     (unsigned) mac.mac_address[2],
			     (unsigned) mac.mac_address[3],
			     (unsigned) mac.mac_address[4],
			     (unsigned) mac.mac_address[5],
			     (unsigned) mac.if_type);
	      }
	      break;
	    case GRUB_EFI_IPV4_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_ipv4_device_path_t ipv4;
		grub_memcpy (&ipv4, dp, len);
		grub_printf ("/IPv4(%u.%u.%u.%u,%u.%u.%u.%u,%u,%u,%x,%x)",
			     (unsigned) ipv4.local_ip_address[0],
			     (unsigned) ipv4.local_ip_address[1],
			     (unsigned) ipv4.local_ip_address[2],
			     (unsigned) ipv4.local_ip_address[3],
			     (unsigned) ipv4.remote_ip_address[0],
			     (unsigned) ipv4.remote_ip_address[1],
			     (unsigned) ipv4.remote_ip_address[2],
			     (unsigned) ipv4.remote_ip_address[3],
			     (unsigned) ipv4.local_port,
			     (unsigned) ipv4.remote_port,
			     (unsigned) ipv4.protocol,
			     (unsigned) ipv4.static_ip_address);
	      }
	      break;
	    case GRUB_EFI_IPV6_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_ipv6_device_path_t ipv6;
		grub_memcpy (&ipv6, dp, len);
		grub_printf ("/IPv6(%x:%x:%x:%x:%x:%x:%x:%x,%x:%x:%x:%x:%x:%x:%x:%x,%u,%u,%x,%x)",
			     (unsigned) ipv6.local_ip_address[0],
			     (unsigned) ipv6.local_ip_address[1],
			     (unsigned) ipv6.local_ip_address[2],
			     (unsigned) ipv6.local_ip_address[3],
			     (unsigned) ipv6.local_ip_address[4],
			     (unsigned) ipv6.local_ip_address[5],
			     (unsigned) ipv6.local_ip_address[6],
			     (unsigned) ipv6.local_ip_address[7],
			     (unsigned) ipv6.remote_ip_address[0],
			     (unsigned) ipv6.remote_ip_address[1],
			     (unsigned) ipv6.remote_ip_address[2],
			     (unsigned) ipv6.remote_ip_address[3],
			     (unsigned) ipv6.remote_ip_address[4],
			     (unsigned) ipv6.remote_ip_address[5],
			     (unsigned) ipv6.remote_ip_address[6],
			     (unsigned) ipv6.remote_ip_address[7],
			     (unsigned) ipv6.local_port,
			     (unsigned) ipv6.remote_port,
			     (unsigned) ipv6.protocol,
			     (unsigned) ipv6.static_ip_address);
	      }
	      break;
	    case GRUB_EFI_INFINIBAND_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_infiniband_device_path_t ib;
		grub_memcpy (&ib, dp, len);
		grub_printf ("/InfiniBand(%x,%llx,%llx,%llx)",
			     (unsigned) ib.port_gid[0], /* XXX */
			     ib.remote_id,
			     ib.target_port_id,
			     ib.device_id);
	      }
	      break;
	    case GRUB_EFI_UART_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_uart_device_path_t uart;
		grub_memcpy (&uart, dp, len);
		grub_printf ("/UART(%llu,%u,%x,%x)",
			     uart.baud_rate,
			     uart.data_bits,
			     uart.parity,
			     uart.stop_bits);
	      }
	      break;
	    case GRUB_EFI_VENDOR_MESSAGING_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_vendor_messaging_device_path_t vendor;
		grub_memcpy (&vendor, dp, sizeof (vendor));
		grub_printf ("/Vendor(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) vendor.vendor_guid.data1,
			     (unsigned) vendor.vendor_guid.data2,
			     (unsigned) vendor.vendor_guid.data3,
			     (unsigned) vendor.vendor_guid.data4[0],
			     (unsigned) vendor.vendor_guid.data4[1],
			     (unsigned) vendor.vendor_guid.data4[2],
			     (unsigned) vendor.vendor_guid.data4[3],
			     (unsigned) vendor.vendor_guid.data4[4],
			     (unsigned) vendor.vendor_guid.data4[5],
			     (unsigned) vendor.vendor_guid.data4[6],
			     (unsigned) vendor.vendor_guid.data4[7]);
	      }
	      break;
	    default:
	      grub_printf ("/UnknownMessaging(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_MEDIA_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_hard_drive_device_path_t hd;
		grub_memcpy (&hd, dp, len);
		grub_printf ("/HD(%u,%llx,%llx,%02x%02x%02x%02x%02x%02x%02x%02x,%x,%x)",
			     hd.partition_number,
			     hd.partition_start,
			     hd.partition_size,
			     (unsigned) hd.partition_signature[0],
			     (unsigned) hd.partition_signature[1],
			     (unsigned) hd.partition_signature[2],
			     (unsigned) hd.partition_signature[3],
			     (unsigned) hd.partition_signature[4],
			     (unsigned) hd.partition_signature[5],
			     (unsigned) hd.partition_signature[6],
			     (unsigned) hd.partition_signature[7],
			     (unsigned) hd.mbr_type,
			     (unsigned) hd.signature_type);
	      }
	      break;
	    case GRUB_EFI_CDROM_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_cdrom_device_path_t cd;
		grub_memcpy (&cd, dp, len);
		grub_printf ("/CD(%u,%llx,%llx)",
			     cd.boot_entry,
			     cd.partition_start,
			     cd.partition_size);
	      }
	      break;
	    case GRUB_EFI_VENDOR_MEDIA_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_vendor_media_device_path_t vendor;
		grub_memcpy (&vendor, dp, sizeof (vendor));
		grub_printf ("/Vendor(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) vendor.vendor_guid.data1,
			     (unsigned) vendor.vendor_guid.data2,
			     (unsigned) vendor.vendor_guid.data3,
			     (unsigned) vendor.vendor_guid.data4[0],
			     (unsigned) vendor.vendor_guid.data4[1],
			     (unsigned) vendor.vendor_guid.data4[2],
			     (unsigned) vendor.vendor_guid.data4[3],
			     (unsigned) vendor.vendor_guid.data4[4],
			     (unsigned) vendor.vendor_guid.data4[5],
			     (unsigned) vendor.vendor_guid.data4[6],
			     (unsigned) vendor.vendor_guid.data4[7]);
	      }
	      break;
	    case GRUB_EFI_FILE_PATH_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_file_path_device_path_t *fp;
		grub_uint8_t buf[(len - 4) * 2 + 1];
		fp = (grub_efi_file_path_device_path_t *) dp;
		*grub_utf16_to_utf8 (buf, fp->path_name,
				     (len - 4) / sizeof (grub_efi_char16_t))
		  = '\0';
		grub_printf ("/File(%s)", buf);
	      }
	      break;
	    case GRUB_EFI_PROTOCOL_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_protocol_device_path_t proto;
		grub_memcpy (&proto, dp, sizeof (proto));
		grub_printf ("/Protocol(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) proto.guid.data1,
			     (unsigned) proto.guid.data2,
			     (unsigned) proto.guid.data3,
			     (unsigned) proto.guid.data4[0],
			     (unsigned) proto.guid.data4[1],
			     (unsigned) proto.guid.data4[2],
			     (unsigned) proto.guid.data4[3],
			     (unsigned) proto.guid.data4[4],
			     (unsigned) proto.guid.data4[5],
			     (unsigned) proto.guid.data4[6],
			     (unsigned) proto.guid.data4[7]);
	      }
	      break;
	    default:
	      grub_printf ("/UnknownMedia(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_BIOS_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_BIOS_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_bios_device_path_t bios;
		grub_memcpy (&bios, dp, sizeof (bios));
		grub_printf ("/BIOS(%x,%x,%s)",
			     (unsigned) bios.device_type,
			     (unsigned) bios.status_flags,
			     (char *) (dp + 1));
	      }
	      break;
	    default:
	      grub_printf ("/UnknownBIOS(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	default:
	  grub_printf ("/UnknownType(%x,%x)\n",
		       (unsigned) type,
		       (unsigned) subtype);
	  return;
	  break;
	}
      
      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (dp))
	break;

      dp = (grub_efi_device_path_t *) ((char *) dp + len);
    }
}
