/*
 * fat.handler - FAT12/16/32 filesystem handler
 *
 * Copyright � 2006 Marek Szyprowski
 * Copyright � 2007 The AROS Development Team
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the same terms as AROS itself.
 *
 * $Id$
 */

#define USE_INLINE_STDARG

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <exec/errors.h>

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/filehandler.h>
#include <devices/trackdisk.h>
#include <devices/inputevent.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <string.h>     /* for memset() */

#include "fat_fs.h"
#include "fat_protos.h"

#ifndef ID_BUSY
#define ID_BUSY 0x42555359
#endif


void FillDiskInfo (struct InfoData *id)
{
	struct DosEnvec *de = BADDR(glob->fssm->fssm_Environ);

	id->id_NumSoftErrors = 0;
	id->id_UnitNumber = glob->fssm->fssm_Unit;
	id->id_DiskState = ID_WRITE_PROTECTED;

    if (glob->sb)
	{
		id->id_NumBlocks = glob->sb->total_sectors;
		id->id_NumBlocksUsed = glob->sb->total_sectors;
		id->id_BytesPerBlock = glob->sb->sectorsize;
		id->id_DiskType = ID_FAT_DISK;
		id->id_VolumeNode = MKBADDR(glob->sb->doslist);
		id->id_InUse = glob->sb->doslist->dol_misc.dol_volume.dol_LockList ? DOSTRUE : DOSFALSE;
	}
	else
	{
		id->id_NumBlocks = de->de_Surfaces * de->de_BlocksPerTrack * (de->de_HighCyl+1 - de->de_LowCyl) / de->de_SectorPerBlock;
		id->id_NumBlocksUsed = id->id_NumBlocks;
		id->id_BytesPerBlock = de->de_SizeBlock << 2;
		id->id_DiskState = ID_WRITE_PROTECTED;
		if (glob->disk_inhibited)
			id->id_DiskType = ID_BUSY;
		else if (glob->disk_inserted)
			id->id_DiskType = ID_NOT_REALLY_DOS; //ID_UNREADABLE_DISK;
		else
			id->id_DiskType = ID_NO_DISK_PRESENT;
		id->id_VolumeNode = NULL;
		id->id_InUse = DOSFALSE;
	}
}

LONG InitDevice(struct FileSysStartupMsg *fssm, LONG blocksize)
{
#ifndef API_LIBDEVIO
    glob->blocksize = blocksize;
    return 0;
#else
	LONG err=0;
	struct DosEnvec *de = BADDR(fssm->fssm_Environ);

	glob->dio = InitDeviceIO(NULL, IDIO_FSSM, (LONG) fssm, IDIO_ForceBlockSize, blocksize, IDIO_IOC_Lines, de->de_NumBuffers, IDIO_IOC_ReadAheadSize, DEF_READ_AHEAD, IDIO_PoolSize, DEF_POOL_SIZE, IDIO_PoolTreshold, DEF_POOL_TRESHOLD, TAG_DONE);
	if ((err = IoErr()) == 0)
	{
		kprintf("\tlibDeviceIO successully initialised.\n");
	}
	else
	{
		ErrorReq("Init DeviceIO failed for specified device!\nError code: %ld\n", &err);
		kprintf("Device open failed!\n");
		CleanupDeviceIO(glob->dio);
	}
	return err;
#endif
}

void SendVolumePacket(struct DosList *vol, ULONG action)
{
	struct DosPacket *dospacket;

	dospacket = AllocDosObject(DOS_STDPKT, TAG_DONE);
	dospacket->dp_Type = ACTION_DISK_CHANGE;
	dospacket->dp_Arg1 = ID_FAT_DISK;
	dospacket->dp_Arg2 = (ULONG) vol;
	dospacket->dp_Arg3 = action;
	dospacket->dp_Port = NULL;

	PutMsg(glob->ourport, dospacket->dp_Link);
}
 

void DoDiskInsert(void)
{
	struct FSSuper *sb;

	if (glob->sb == NULL && (sb = FS_AllocMem(sizeof(struct FSSuper))))
	{
		memset(sb, 0, sizeof(struct FSSuper));
		if (ReadFATSuper(sb) == 0)
		{
			LONG found = FALSE;
			if (glob->sblist)
			{
				struct FSSuper *ptr = glob->sblist, *prev = NULL;
				while (ptr != NULL)
				{
					if (CompareFATSuper(sb, ptr) == 0)
						break;
					prev = ptr;
					ptr = ptr->next;
				}
				if (ptr)
				{
					kprintf("\tFound FAT FS Super Block in spare list, freeing obsolete old one\n");

					sb->doslist = ptr->doslist;
					ptr->doslist = NULL;
					sb->doslist->dol_Name = (BSTR)MKBADDR(&sb->name);

					if (prev)
						prev->next = ptr->next;
					else
						glob->sblist = ptr->next;

					FreeFATSuper(ptr);
					FS_FreeMem(ptr);

					found = TRUE;
				}
			}
			if (!found)
			{
				struct DosList *newvol;
				kprintf("\tCreating new volume.\n");
				if ((newvol = FS_AllocMem(sizeof(struct DosList))))
				{
					newvol->dol_Next = NULL;
					newvol->dol_Type = DLT_VOLUME;
					newvol->dol_Task = glob->ourport;
					newvol->dol_Lock = NULL;

					DateStamp(& newvol->dol_misc.dol_volume.dol_VolumeDate);
					newvol->dol_misc.dol_volume.dol_LockList = NULL;
					newvol->dol_misc.dol_volume.dol_DiskType = ID_FAT_DISK;

#ifdef AROS_FAST_BPTR
                                        /* ReadFATSuper() sets a null byte
                                         * after the string, so this should be
                                         * fine */
                                        newvol->dol_Name = (BSTR)MKBADDR(&(sb->name[1]));
#else
					newvol->dol_Name = (BSTR)MKBADDR(&sb->name);
#endif
					sb->doslist = newvol;

					SendVolumePacket(newvol, ACTION_VOLUME_ADD);
				}
			}
			else
				SendEvent(IECLASS_DISKINSERTED);
			glob->sb = sb;
			kprintf("\tDisk successfully initialised\n");
			return;
		}
#ifdef API_LIBDEVIO
		else if (glob->dio)
		{
			CleanupDeviceIO(glob->dio);
			glob->dio = NULL;
		}
#endif
		FS_FreeMem(sb);
	}
	SendEvent(IECLASS_DISKINSERTED);   	
	return;
}

void DoDiskRemove(void)
{
#ifdef API_LIBDEVIO
	if (glob->dio)
	{
		CleanupDeviceIO(glob->dio);
		kprintf("\tlibDeviceIO cleaned its resources.\n");
		glob->dio = NULL;
	}
#endif

	if (glob->sb)
	{
		struct FSSuper *sb = glob->sb;
		glob->sb = NULL;
		if (sb->doslist->dol_misc.dol_volume.dol_LockList == NULL) /* check if the device can be removed */
		{
			kprintf("\tRemoving disk completely\n");
			SendVolumePacket(sb->doslist, ACTION_VOLUME_REMOVE);
			sb->doslist = NULL;
			FreeFATSuper(sb);
			FS_FreeMem(sb);
		}
		else
		{
			sb->next = glob->sblist;
			glob->sblist = sb;
			kprintf("\tMoved in-memory super block to spare list. Waiting for locks to be freed\n");
			SendEvent(IECLASS_DISKREMOVED);
		}
	}
}
 
void ProcessDiskChange (void)
{
	kprintf("\nGot disk change request\n");
	
	if (glob->disk_inhibited)
	{
		kprintf("Disk is inhibited, ignoring disk change\n");
		return;
	}

	glob->diskioreq->iotd_Req.io_Command = TD_CHANGESTATE;
	glob->diskioreq->iotd_Req.io_Data = NULL;
	glob->diskioreq->iotd_Req.io_Length = 0;
	glob->diskioreq->iotd_Req.io_Flags = IOF_QUICK;
	DoIO((struct IORequest*)glob->diskioreq);

	if (glob->diskioreq->iotd_Req.io_Error == 0 && glob->diskioreq->iotd_Req.io_Actual == 0)
	{
		/* Disk has been inserted. */
		kprintf("\tDisk has been inserted\n");
		glob->disk_inserted = TRUE;
		DoDiskInsert();
	}
	else
	{
		/* Disk has been removed. */
		kprintf("\tDisk has been removed\n");
		glob->disk_inserted = FALSE;
		DoDiskRemove();
	}
	kprintf("Done\n");
}

#define TD64_READ    (24)

LONG DoRawRead (ULONG n, void *buff)
{
	struct DosEnvec *de = BADDR(glob->fssm->fssm_Environ);
	UQUAD bytes_start;
	ULONG bytes_low, bytes_high;
	LONG err;

	bytes_start = 1LL * de->de_Surfaces * de->de_BlocksPerTrack * de->de_LowCyl * de->de_SizeBlock * 4 + n * TD_SECTOR;
	bytes_low = bytes_start & 0xFFFFFFFF;
	bytes_high = bytes_start >> 32;

	kprintf("\tReading sector %ld\n", n);
	if (bytes_high)
	{
		glob->diskioreq->iotd_Req.io_Command = TD64_READ;
		glob->diskioreq->iotd_Req.io_Data = 0;
		glob->diskioreq->iotd_Req.io_Length = 0;
		glob->diskioreq->iotd_Req.io_Flags = 0;
		glob->diskioreq->iotd_Req.io_Offset = 0;

		err	= DoIO((struct IORequest*)glob->diskioreq);
		if (err != -1 && err != IOERR_NOCMD)
			glob->diskioreq->iotd_Req.io_Command = TD64_READ;
		else
		{
			kprintf("\t64bit interface required, but device doesn't support TD64 extension.\n");
			return ERROR_OBJECT_WRONG_TYPE;
		}
	}
	else
		glob->diskioreq->iotd_Req.io_Command = CMD_READ;

	glob->diskioreq->iotd_Req.io_Data = buff;
	glob->diskioreq->iotd_Req.io_Length = TD_SECTOR;
	glob->diskioreq->iotd_Req.io_Flags = 0;
	glob->diskioreq->iotd_Req.io_Offset = bytes_low;
	glob->diskioreq->iotd_Req.io_Actual = bytes_high;

	err = DoIO((struct IORequest*)glob->diskioreq);
	kprintf("\tDoIO returned %ld\n", err);
	return err;
}

