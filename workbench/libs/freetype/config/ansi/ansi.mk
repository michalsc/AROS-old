#
# FreeType 2 configuration rules for a `normal' ANSI compiler
#


# Copyright 1996-2000 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used modified
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


ifndef TOP
  TOP := .
endif

DELETE   := rm -f
SEP      := /
HOSTSEP  := $(SEP)
BUILD    := $(TOP)/config/ansi
PLATFORM := ansi

# The directory where all object files are placed.
#
# Note that this is not $(TOP)/obj!
# This lets you build the library in your own directory with something like
#
#   set TOP=.../path/to/freetype2/top/dir...
#   mkdir obj
#   make -f $TOP/Makefile setup [options]
#   make -f $TOP/Makefile
#
OBJ_DIR := obj


# The directory where all library files are placed
#
# By default, this is the same as $(OBJ_DIR), however, this can be changed
# to suit particular needs.
#
LIB_DIR := $(OBJ_DIR)


# The object file extension.  This can be .o, .tco, .obj, etc., depending on
# the platform.
#
O := o

# The library file extension.  This can be .a, .lib, etc., depending on the
# platform.
#
A := a


# The name of the final library file.  Note that the DOS-specific Makefile
# uses a shorter (8.3) name.
#
LIBRARY := libfreetype


# Path inclusion flag.  Some compilers use a different flag than `-I' to
# specify an additional include path.  Examples are `/i=' or `-J'.
#
I := -I


# C flag used to define a macro before the compilation of a given source
# object.  Usually is `-D' like in `-DDEBUG'.
#
D := -D


# The link flag used to specify a given library file on link.  Note that
# this is only used to compile the demo programs, not the library itself.
#
L := -l


# Target flag.
#
T := -o # Don't remove this comment line!  We need the space after `-o'.


# C flags
#
#   These should concern: debug output, optimization & warnings.
#
#   Use the ANSIFLAGS variable to define the compiler flags used to enfore
#   ANSI compliance.
#
ifndef CFLAGS
  CFLAGS := -c
endif

# ANSIFLAGS: Put there the flags used to make your compiler ANSI-compliant.
#
ANSIFLAGS :=


ifdef BUILD_FREETYPE

  # Now include the main sub-makefile.  It contains all the rules used to
  # build the library with the previous variables defined.
  #
  include $(TOP)/config/freetype.mk

  # The cleanup targets.
  #
  clean_freetype: clean_freetype_std
  distclean_freetype: distclean_freetype_std

  # Librarian to use to build the static library
  #
  FT_LIBRARIAN := $(AR) -r


  # This final rule is used to link all object files into a single library.
  # It is part of the system-specific sub-Makefile because not all
  # librarians accept a simple syntax like:
  #
  #    librarian library_file {list of object files} 
  #
  $(FT_LIBRARY): $(OBJECTS_LIST)
	  -$(DELETE) $@
	  $(FT_LIBRARIAN) $@ $(OBJECTS_LIST)

endif

# EOF
