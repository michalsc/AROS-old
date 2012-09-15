/* lvm.h - LVM support for GRUB utils.  */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2006,2007,2011  Free Software Foundation, Inc.
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

#ifndef GRUB_LVM_UTIL_HEADER
#define GRUB_LVM_UTIL_HEADER	1

#if defined(__linux__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)

#ifdef __linux__
#define LVM_DEV_MAPPER_STRING "/dev/mapper/"
#else
#define LVM_DEV_MAPPER_STRING "/dev/linux_lvm/"
#endif

int grub_util_lvm_isvolume (char *name);
#endif

#endif /* ! GRUB_RAID_UTIL_HEADER */
