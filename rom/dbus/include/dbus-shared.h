/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-shared.h  Stuff used by both dbus/dbus.h low-level and C/C++ binding APIs
 *
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef DBUS_SHARED_H
#define DBUS_SHARED_H

/* Don't include anything in here from anywhere else. It's
 * intended for use by any random library.
 */

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum
{
  DBUS_BUS_SESSION,    /**< The login session bus */
  DBUS_BUS_SYSTEM,     /**< The systemwide bus */
  DBUS_BUS_ACTIVATION  /**< The bus that activated us, if any */
} DBusBusType;

typedef enum
{
  DBUS_HANDLER_RESULT_HANDLED,         /**< Message has had its effect */ 
  DBUS_HANDLER_RESULT_NOT_YET_HANDLED, /**< Message has not had any effect */
  DBUS_HANDLER_RESULT_NEED_MEMORY      /**< Need more memory to return another result */
} DBusHandlerResult;

#ifdef __cplusplus
}
#endif

#endif /* DBUS_PROTOCOL_H */
