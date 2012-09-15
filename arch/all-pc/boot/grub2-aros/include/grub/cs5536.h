/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2010  Free Software Foundation, Inc.
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

#ifndef GRUB_CS5536_HEADER
#define GRUB_CS5536_HEADER 1

#ifndef ASM_FILE
#include <grub/pci.h>
#include <grub/err.h>
#include <grub/smbus.h>
#endif

#define GRUB_CS5536_PCIID 0x208f1022
#define GRUB_CS5536_MSR_MAILBOX_CONFIG_ENABLED        0x1
#define GRUB_CS5536_MSR_MAILBOX_CONFIG 0xf0
#define GRUB_CS5536_MSR_MAILBOX_ADDR   0xf4
#define GRUB_CS5536_MSR_MAILBOX_DATA0  0xf8
#define GRUB_CS5536_MSR_MAILBOX_DATA1  0xfc
#define GRUB_CS5536_MSR_IRQ_MAP_BAR 0x80000008
#define GRUB_CS5536_MSR_SMB_BAR 0x8000000b

#define GRUB_CS5536_SMBUS_REGS_SIZE 8
#define GRUB_CS5536_GPIO_REGS_SIZE 256
#define GRUB_CS5536_MFGPT_REGS_SIZE 64
#define GRUB_CS5536_IRQ_MAP_REGS_SIZE 32
#define GRUB_CS5536_PM_REGS_SIZE 128
#define GRUB_CS5536_ACPI_REGS_SIZE 32

#define GRUB_CS5536_USB_OPTION_REGS_SIZE 0x1c
#define GRUB_CS5536_USB_OPTION_REG_UOCMUX  1
#define GRUB_CS5536_USB_OPTION_REG_UOCMUX_PMUX_MASK 0x03
#define GRUB_CS5536_USB_OPTION_REG_UOCMUX_PMUX_HC   0x02

#define GRUB_CS5536_DESTINATION_GLIU     0
#define GRUB_CS5536_DESTINATION_GLPCI_SB 1
#define GRUB_CS5536_DESTINATION_USB      2
#define GRUB_CS5536_DESTINATION_IDE      3
#define GRUB_CS5536_DESTINATION_DD       4
#define GRUB_CS5536_DESTINATION_ACC      5
#define GRUB_CS5536_DESTINATION_GLCP     7

#define GRUB_CS5536_P2D_DEST_SHIFT    61
#define GRUB_CS5536_P2D_LOG_ALIGN  12
#define GRUB_CS5536_P2D_ALIGN  (1 << GRUB_CS5536_P2D_LOG_ALIGN)
#define GRUB_CS5536_P2D_BASE_SHIFT 20
#define GRUB_CS5536_P2D_MASK_SHIFT 0

#define GRUB_CS5536_MSR_GL_IOD_START 0x000100e0
#define GRUB_CS5536_IOD_DEST_SHIFT    61
#define GRUB_CS5536_IOD_BASE_SHIFT    20
#define GRUB_CS5536_IOD_MASK_SHIFT 0
#define GRUB_CS5536_IOD_ADDR_MASK     0xfffff

#define GRUB_CS5536_MSR_GPIO_BAR 0x8000000c
#define GRUB_CS5536_MSR_MFGPT_BAR 0x8000000d
#define GRUB_CS5536_MSR_ACPI_BAR 0x8000000e
#define GRUB_CS5536_MSR_PM_BAR 0x8000000f
#define GRUB_CS5536_MSR_DIVIL_LEG_IO 0x80000014
#define GRUB_CS5536_MSR_DIVIL_LEG_IO_RTC_ENABLE0 0x00000001
#define GRUB_CS5536_MSR_DIVIL_LEG_IO_RTC_ENABLE1 0x00000002
#define GRUB_CS5536_MSR_DIVIL_LEG_IO_MODE_X86    0x10000000
#define GRUB_CS5536_MSR_DIVIL_LEG_IO_F_REMAP     0x04000000
#define GRUB_CS5536_MSR_DIVIL_LEG_IO_UART1_COM1  0x00070000
#define GRUB_CS5536_MSR_DIVIL_LEG_IO_UART2_COM3  0x00500000
#define GRUB_CS5536_MSR_DIVIL_RESET 0x80000017
#define GRUB_CS5536_MSR_DIVIL_IRQ_MAPPER_PRIMARY_MASK 0x80000024
#define GRUB_CS5536_MSR_DIVIL_IRQ_MAPPER_LPC_MASK 0x80000025
#define GRUB_CS5536_DIVIL_LPC_INTERRUPTS 0x1002
#define GRUB_CS5536_MSR_DIVIL_LPC_SERIAL_IRQ_CONTROL 0x8000004e
#define GRUB_CS5536_MSR_DIVIL_LPC_SERIAL_IRQ_CONTROL_ENABLE 0x80
#define GRUB_CS5536_MSR_DIVIL_UART1_CONF 0x8000003a
#define GRUB_CS5536_MSR_DIVIL_UART2_CONF 0x8000003e

#define GRUB_CS5536_MSR_USB_OHCI_BASE 0x40000008
#define GRUB_CS5536_MSR_USB_EHCI_BASE 0x40000009
#define GRUB_CS5536_MSR_USB_CONTROLLER_BASE 0x4000000a
#define GRUB_CS5536_MSR_USB_OPTION_CONTROLLER_BASE 0x4000000b
#define GRUB_CS5536_MSR_USB_BASE_ADDR_MASK     0x00ffffff00ULL
#define GRUB_CS5536_MSR_USB_BASE_SMI_ENABLE    0x3f000000000000ULL
#define GRUB_CS5536_MSR_USB_BASE_BUS_MASTER    0x0400000000ULL
#define GRUB_CS5536_MSR_USB_BASE_MEMORY_ENABLE 0x0200000000ULL
#define GRUB_CS5536_MSR_USB_BASE_PME_ENABLED       0x0800000000ULL
#define GRUB_CS5536_MSR_USB_BASE_PME_STATUS        0x1000000000ULL
#define GRUB_CS5536_MSR_USB_EHCI_BASE_FLDJ_SHIFT 40

#define GRUB_CS5536_MSR_IDE_IO_BAR 0x60000008
#define GRUB_CS5536_MSR_IDE_IO_BAR_UNITS 1
#define GRUB_CS5536_MSR_IDE_IO_BAR_ADDR_MASK 0xfffffff0
#define GRUB_CS5536_MSR_IDE_CFG 0x60000010
#define GRUB_CS5536_MSR_IDE_CFG_CHANNEL_ENABLE 2
#define GRUB_CS5536_MSR_IDE_TIMING 0x60000012
#define GRUB_CS5536_MSR_IDE_TIMING_PIO0 0x98
#define GRUB_CS5536_MSR_IDE_TIMING_DRIVE0_SHIFT 24
#define GRUB_CS5536_MSR_IDE_TIMING_DRIVE1_SHIFT 16
#define GRUB_CS5536_MSR_IDE_CAS_TIMING 0x60000013
#define GRUB_CS5536_MSR_IDE_CAS_TIMING_CMD_PIO0 0x99
#define GRUB_CS5536_MSR_IDE_CAS_TIMING_CMD_SHIFT 24
#define GRUB_CS5536_MSR_IDE_CAS_TIMING_DRIVE0_SHIFT 6
#define GRUB_CS5536_MSR_IDE_CAS_TIMING_DRIVE1_SHIFT 4
#define GRUB_CS5536_MSR_IDE_CAS_TIMING_PIO0 2

#define GRUB_CS5536_MSR_GL_PCI_CTRL 0x00000010
#define GRUB_CS5536_MSR_GL_PCI_CTRL_MEMORY_ENABLE 1
#define GRUB_CS5536_MSR_GL_PCI_CTRL_IO_ENABLE 2
#define GRUB_CS5536_MSR_GL_PCI_CTRL_LATENCY_SHIFT 35
#define GRUB_CS5536_MSR_GL_PCI_CTRL_OUT_THR_SHIFT 60
#define GRUB_CS5536_MSR_GL_PCI_CTRL_IN_THR_SHIFT 56

#define GRUB_CS5536_MSR_GL_REGIONS_START 0x00000020
#define GRUB_CS5536_MSR_GL_REGIONS_NUM   16
#define GRUB_CS5536_MSR_GL_REGION_ENABLE 1
#define GRUB_CS5536_MSR_GL_REGION_IO        0x100000000ULL
#define GRUB_CS5536_MSR_GL_REGION_BASE_MASK 0xfffff000ULL
#define GRUB_CS5536_MSR_GL_REGION_IO_BASE_SHIFT 12
#define GRUB_CS5536_MSR_GL_REGION_TOP_MASK 0xfffff00000000000ULL
#define GRUB_CS5536_MSR_GL_REGION_IO_TOP_SHIFT 44

#define GRUB_CS5536_MSR_GL_P2D_START 0x00010020

#define GRUB_CS5536_SMB_REG_DATA 0x0
#define GRUB_CS5536_SMB_REG_STATUS 0x1
#define GRUB_CS5536_SMB_REG_STATUS_SDAST (1 << 6)
#define GRUB_CS5536_SMB_REG_STATUS_BER (1 << 5)
#define GRUB_CS5536_SMB_REG_STATUS_NACK (1 << 4)
#define GRUB_CS5536_SMB_REG_CTRL1 0x3
#define GRUB_CS5536_SMB_REG_CTRL1_START 0x01
#define GRUB_CS5536_SMB_REG_CTRL1_STOP  0x02
#define GRUB_CS5536_SMB_REG_CTRL1_ACK   0x10
#define GRUB_CS5536_SMB_REG_ADDR 0x4
#define GRUB_CS5536_SMB_REG_ADDR_MASTER 0x0
#define GRUB_CS5536_SMB_REG_CTRL2 0x5
#define GRUB_CS5536_SMB_REG_CTRL2_ENABLE 0x1
#define GRUB_CS5536_SMB_REG_CTRL3 0x6

#ifdef ASM_FILE
#define GRUB_ULL(x) x
#else
#define GRUB_ULL(x) x ## ULL
#endif

#define GRUB_CS5536_LBAR_ADDR_MASK GRUB_ULL (0x000000000000fff8)
#define GRUB_CS5536_LBAR_ENABLE GRUB_ULL (0x0000000100000000)
#define GRUB_CS5536_LBAR_MASK_MASK GRUB_ULL (0x0000f00000000000)
#define GRUB_CS5536_LBAR_TURN_ON (GRUB_CS5536_LBAR_ENABLE | GRUB_CS5536_LBAR_MASK_MASK)

/* PMON-compatible LBARs.  */
#define GRUB_CS5536_LBAR_GPIO      0xb000
#define GRUB_CS5536_LBAR_ACC       0xb200
#define GRUB_CS5536_LBAR_PM        0xb280
#define GRUB_CS5536_LBAR_MFGPT     0xb300
#define GRUB_CS5536_LBAR_ACPI      0xb340
#define GRUB_CS5536_LBAR_IRQ_MAP   0xb360
#define GRUB_CS5536_LBAR_IDE       0xb380
#define GRUB_CS5536_LBAR_SMBUS     0xb390

#define GRUB_GPIO_SMBUS_PINS ((1 << 14) | (1 << 15))
#define GRUB_GPIO_REG_OUT_EN 0x4
#define GRUB_GPIO_REG_OUT_AUX1 0x10
#define GRUB_GPIO_REG_IN_EN 0x20
#define GRUB_GPIO_REG_IN_AUX1 0x34

#ifndef ASM_FILE
int EXPORT_FUNC (grub_cs5536_find) (grub_pci_device_t *devp);

grub_uint64_t EXPORT_FUNC (grub_cs5536_read_msr) (grub_pci_device_t dev,
						  grub_uint32_t addr);
void EXPORT_FUNC (grub_cs5536_write_msr) (grub_pci_device_t dev,
					  grub_uint32_t addr,
					  grub_uint64_t val);
grub_err_t grub_cs5536_read_spd_byte (grub_port_t smbbase, grub_uint8_t dev,
				      grub_uint8_t addr, grub_uint8_t *res);
grub_err_t EXPORT_FUNC (grub_cs5536_read_spd) (grub_port_t smbbase,
					       grub_uint8_t dev,
					       struct grub_smbus_spd *res);
grub_err_t grub_cs5536_smbus_wait (grub_port_t smbbase);
grub_err_t EXPORT_FUNC (grub_cs5536_init_smbus) (grub_pci_device_t dev,
						 grub_uint16_t divisor,
						 grub_port_t *smbbase);

void grub_cs5536_init_geode (grub_pci_device_t dev);
#endif

#endif
