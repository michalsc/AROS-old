/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef GADGETS_H
#define GADGETS_H

#include <exec/types.h>
#include <libraries/gadtools.h>
#include <graphics/text.h>

struct creategadget {
	ULONG kind;
	struct NewGadget newgadget;
	struct TagItem *tags;
	struct Gadget *gadget;
};

enum
{
	ID_MAIN_FIRST_GADGET,
	ID_MAIN_TEXT=ID_MAIN_FIRST_GADGET,
	ID_MAIN_HARDDISK,
	ID_MAIN_CHANGE_DRIVE_TYPE,
	ID_MAIN_MODIFY_BBL,
	ID_MAIN_LL_FORMAT,
	ID_MAIN_PARTITION_DRIVE,
	ID_MAIN_VERIFY_DD,
	ID_MAIN_SAVE_CHANGES,
	ID_MAIN_HELP,
	ID_MAIN_EXIT,
	ID_MAIN_LAST_GADGET,
	ID_PCP_FIRST_GADGET=ID_MAIN_LAST_GADGET,
	ID_PCP_PARTITION=ID_PCP_FIRST_GADGET,
	ID_PCP_ADD_PARTITION,
	ID_PCP_DELETE_PARTITION,
	ID_PCP_STARTCYL,
	ID_PCP_ENDCYL,
	ID_PCP_TOTALCYL,
	ID_PCP_SIZE,
	ID_PCP_NAME,
	ID_PCP_BOOTABLE,
	ID_PCP_BOOTPRI,
	ID_PCP_FILESYSTEM,
	ID_PCP_UPDATE_FS,
	ID_PCP_EDIT_PARTITION,
	ID_PCP_OK,
	ID_PCP_CANCEL,
	ID_PCP_LAST_GADGET,
	ID_DET_FIRST_GADGET=ID_PCP_LAST_GADGET,
	ID_DET_TYPELV = ID_DET_FIRST_GADGET,
	ID_DET_TYPESTRING,
	ID_DET_PARTITION_TABLE,
	ID_DET_BLOCKSIZE,
	ID_DET_BUFFERS,
	ID_DET_MASK,
	ID_DET_MAX_TRANSFER,
	ID_DET_AUTOMOUNT,
	ID_DET_CUSTBOOT,
	ID_DET_CUSTBB,
	ID_DET_RESERVED,
	ID_DET_BEGINING,
	ID_DET_END,
	ID_DET_OK,
	ID_DET_CANCEL,
	ID_DET_LAST_GADGET,
	ID_PCP_PARTITION_GUI
};

struct Gadget *createGadgets(struct creategadget *, ULONG, ULONG, APTR);
void freeGadgets(struct Gadget *);
#if 0
void clearGadgets(struct ExtGadget *, struct Window *, ULONG);
#else
void clearWindow(struct Window *);
#define clearGadgets(a,b,c) clearWindow(b)
#endif
void allocPTGadget(struct Screen *, struct Gadget *);
void freePTGadget(struct Screen *);

#endif

