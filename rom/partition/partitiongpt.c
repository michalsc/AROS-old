/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$
    
    Desc: GPT partition table handler
*/

/*
#undef DEBUG
#define DEBUG 1
#define DEBUG_UUID
#define NO_WRITE
*/
#define DREAD(x)
#define DWRITE(x)

/*
 * Note that we use KPrintF() for debugging in some places.
 * KPrintF() uses RawDoFmt() for formatting, which (if patched by locale.library)
 * correctly supports %llu, unlike kprintf().
 * This will change when i386 port gets kernel.resource. After this kprintf()
 * will be moved to libdebug.a and rewritten to simply call KrnBug().
 */

#include <exec/memory.h>
#include <libraries/partition.h>
#include <proto/debug.h>
#include <proto/exec.h>
#include <proto/partition.h>
#include <proto/utility.h>

#include <zlib.h>

#include "partition_support.h"
#include "partition_types.h"
#include "partitiongpt.h"
#include "partitionmbr.h"
#include "platform.h"
#include "debug.h"

struct GPTPartitionHandle
{
    struct PartitionHandle ph;	/* Public part		      */
    ULONG entrySize;		/* Size of table entry	      */
    char name[36];		/* Name in ASCII	      */
    				/* Actual table entry follows */
};

#define GPTH(ph) ((struct GPTPartitionHandle *)ph)

static const uuid_t GPT_Type_Unused = MAKE_UUID(0x00000000, 0x0000, 0x0000, 0x0000, 0x000000000000);
/*
 * This is a bit special.
 * The first four bytes (time_low) hold DOS Type ID (for simple mapping),
 * so we set them to zero here. We ignore it during comparison.
 * I hope this won't create any significant problems. Even if some ID ever collides, it will
 * unlikely collide with existing DOSTypes being used, so it can be blacklisted then.
 */
static const uuid_t GPT_Type_AROS   = MAKE_UUID(0x00000000, 0xBB67, 0x46C5, 0xAA4A, 0xF502CA018E5E);


/*
 * UTF16-LE conversion.
 * Currently these are very basic routines which handle only Latin-1 character set.
 * If needed, conversion can be performed using codesets.library (but don't forget
 * that you can run early during system bootup and codesets.library won't be
 * available by that time).
 */

static void FromUTF16(char *to, char *from, ULONG len)
{
    ULONG i;
    
    for (i = 0; i < len; i++)
    {
    	/* Currently we know only 7-bit ASCII characters */
    	*to++ = from[0];

    	if (!from[0])
    	    return;

	from += 2;
    }
}

static void ToUTF16(char *to, char *from, ULONG len)
{
    ULONG i;
    
    for (i = 0; i < len; i++)
    {
    	/* Currently we know only 7-bit ASCII characters */
    	*to++ = *from;
    	*to++ = 0;

    	if (!*from++)
    	    return;
    }
}

/*
 * Little-endian UUID conversion and comparison.
 * We can't use uuid.library here because it's not available during
 * system bootup. However, we are going to use it for generation.
 */
static inline void uuid_from_le(uuid_t *to, uuid_t *id)
{
    to->time_low            = AROS_LE2LONG(id->time_low);
    to->time_mid            = AROS_LE2WORD(id->time_mid);
    to->time_hi_and_version = AROS_LE2WORD(id->time_hi_and_version);

    /* Do not replace it with CopyMem(), gcc optimizes this nicely */
    memcpy(&to->clock_seq_hi_and_reserved, &id->clock_seq_hi_and_reserved, 8);
}

static inline void uuid_to_le(uuid_t *to, uuid_t *id)
{
    to->time_low            = AROS_LONG2LE(id->time_low);
    to->time_mid            = AROS_WORD2LE(id->time_mid);
    to->time_hi_and_version = AROS_WORD2LE(id->time_hi_and_version);

    /* Do not replace it with CopyMem(), gcc optimizes this nicely */
    memcpy(&to->clock_seq_hi_and_reserved, &id->clock_seq_hi_and_reserved, 8);
}

static inline BOOL uuid_cmp_le(uuid_t *leid, const uuid_t *id)
{
    if (AROS_LE2LONG(leid->time_low) != id->time_low)
    	return FALSE;
    if (AROS_LE2WORD(leid->time_mid) != id->time_mid)
    	return FALSE;
    if (AROS_LE2WORD(leid->time_hi_and_version) != id->time_hi_and_version)
    	return FALSE;

    return !memcmp(&leid->clock_seq_hi_and_reserved, &id->clock_seq_hi_and_reserved, 8);
}

/* For AROS we put DOS Type ID into first four bytes of UUID (time_low), so we ignore them. */
static inline BOOL is_aros_uuid_le(uuid_t *leid)
{
    if (AROS_LE2WORD(leid->time_mid) != GPT_Type_AROS.time_mid)
    	return 0;
    if (AROS_LE2WORD(leid->time_hi_and_version) != GPT_Type_AROS.time_hi_and_version)
    	return 0;

    return !memcmp(&leid->clock_seq_hi_and_reserved, &GPT_Type_AROS.clock_seq_hi_and_reserved, 8);
}

#ifdef DEBUG_UUID

static void PRINT_LE_UUID(char *s, uuid_t *id)
{
    unsigned int i;

    bug("[GPT] %s UUID: 0x%08X-%04X-%04X-%02X%02X-", s,
        AROS_LE2LONG(id->time_low), AROS_LE2WORD(id->time_mid), AROS_LE2WORD(id->time_hi_and_version),
    	id->clock_seq_hi_and_reserved, id->clock_seq_low);

    for (i = 0; i < sizeof(id->node); i++)
    	bug("%02X", id->node[i]);

    RawPutChar('\n');
}

#else

#define PRINT_LE_UUID(s, id)

#endif

#ifdef NO_WRITE
#define writeDataFromBlock(root, blk, tablesize, table) 1
#endif

static ULONG GPT_GetDosType(struct GPTPartition *p)
{
    if (is_aros_uuid_le(&p->TypeID))
    	return AROS_LE2LONG(p->TypeID.time_low);
    else
    {
        const struct TypeMapping *m;

        for (m = PartTypes; m->DOSType; m++)
        {
            if (m->uuid && uuid_cmp_le(&p->TypeID, m->uuid))
            	return m->DOSType;
        }

    	return 0;
    }
}

static LONG GPTCheckHeader(struct Library *PartitionBase, struct PartitionHandle *root, struct GPTHeader *hdr, UQUAD block)
{
    /* Load the GPT header */
    if (!readBlock(PartitionBase, root, block, hdr))
    {
    	ULONG hdrSize = AROS_LE2LONG(hdr->HeaderSize);

	D(bug("[GPT] Header size: specified %u, expected %u\n", hdrSize, GPT_MIN_HEADER_SIZE));

	/* Check signature, header size, and current block number */
    	if ((!memcmp(hdr->Signature, GPT_SIGNATURE, sizeof(hdr->Signature))) &&
    	    (hdrSize >= GPT_MIN_HEADER_SIZE) && (AROS_LE2LONG(hdr->CurrentBlock) == block))
    	{
    	    /*
    	     * Use zlib routine for CRC32.
    	     * CHECKME: is it correct on bigendian machines? It should, however who knows...
    	     */
    	    ULONG orig_crc = AROS_LE2LONG(hdr->HeaderCRC32);
    	    ULONG crc = crc32(0L, Z_NULL, 0);

	    hdr->HeaderCRC32 = 0;
    	    crc = crc32(crc, (const Bytef *)hdr, hdrSize);

	    D(bug("[GPT] Header CRC: calculated 0x%08X, expected 0x%08X\n", crc, orig_crc));

	    return (crc == orig_crc) ? 1 : 2;
	}
    }
    return 0;
}

static LONG PartitionGPTCheckPartitionTable(struct Library *PartitionBase, struct PartitionHandle *root)
{
    APTR blk;
    LONG res = 0;

    /* GPT can be placed only in the root of the disk */
    if (root->root)
    	return 0;

    blk = AllocMem(root->de.de_SizeBlock << 2, MEMF_ANY);

    if (!blk)
    	return 0;

    /* First of all, we must have valid MBR stub */
    if (MBRCheckPartitionTable(PartitionBase, root, blk))
    {
        struct PCPartitionTable *pcpt = ((struct MBR *)blk)->pcpt;

	D(bug("[GPT] MBR check passed, first partition type 0x%02X, start block %u\n", pcpt[0].type, pcpt[0].first_sector));

	/* We must have partition 0 of type GPT starting at block 1 */
    	if ((pcpt[0].type == MBRT_GPT) && (AROS_LE2LONG(pcpt[0].first_sector) == 1))
    	{
    	    res = GPTCheckHeader(PartitionBase, root, blk, 1);

	    /* 2 is a special return code for "bad CRC" */
    	    if (res == 2)
    	    {
		/* Try to read backup header */
    	    	res = GPTCheckHeader(PartitionBase, root, blk, ((struct GPTHeader *)blk)->BackupBlock);

		/* There's no third backup :( */
    	    	if (res == 2)
    	    	    res = 0;
    	    }
    	}
    }

    FreeMem(blk, root->de.de_SizeBlock << 2);
    return res;
}

static LONG GPTReadPartitionTable(struct Library *PartitionBase, struct PartitionHandle *root, struct GPTHeader *hdr, UQUAD block)
{
    LONG res;
    LONG err = ERROR_NOT_A_DOS_DISK;

    res = readBlock(PartitionBase, root, block, hdr);
    if (!res)
    {
	struct GPTPartition *table;
        ULONG cnt       = AROS_LE2LONG(hdr->NumEntries);
    	ULONG entrysize = AROS_LE2LONG(hdr->EntrySize);
	ULONG tablesize = AROS_ROUNDUP2(entrysize * cnt, root->de.de_SizeBlock << 2);

	DREAD(bug("[GPT] Read: %u entries per %u bytes, %u bytes total\n", cnt, entrysize, tablesize));

	table = AllocMem(tablesize, MEMF_ANY);
    	if (!table)
    	    return ERROR_NO_FREE_STORE;

	DREAD(KPrintF("[GPT] Read: start block %llu\n", hdr->StartBlock));

	res = readDataFromBlock(root, AROS_LE2QUAD(hdr->StartBlock), tablesize, table);
	if (!res)
	{
	    struct GPTPartition *p = table;
	    ULONG i;

	    /* TODO: Check CRC of partition table, use backup if wrong */
	    DREAD(bug("[GPT] Adding partitions...\n"));
	    err = 0;

	    for (i = 0; i < cnt; i++)
    	    {
	    	struct GPTPartitionHandle *gph;

		DREAD(PRINT_LE_UUID("Type     ", &p->TypeID));
		DREAD(PRINT_LE_UUID("Partition", &p->PartitionID));
		DREAD(KPrintF("[GPT] Blocks    %llu - %llu\n", AROS_LE2QUAD(p->StartBlock), AROS_LE2QUAD(p->EndBlock)));
		DREAD(KPrintF("[GPT] Flags     0x%08lX 0x%08lX\n", AROS_LE2LONG(p->Flags0), AROS_LE2LONG(p->Flags1)));
		DREAD(KPrintF("[GPT] Offset    0x%p\n", (APTR)p - (APTR)table));

		/*
		 * Skip unused entries. NumEntries in the header holds total number of preallocated entries,
		 * not the number of used ones.
		 * Normally GPT table has 128 preallocated entries, but only first of them are used.
		 * Just in case, we allow gaps between used entries. However (tested with MacOS X Disk Utility)
		 * partition editors seem to squeeze the table and do not leave empty entries when deleting
		 * partitions in the middle of the disk.
		 */
		if (!memcmp(&p->TypeID, &GPT_Type_Unused, sizeof(uuid_t)))
		    continue;

	    	gph = AllocVec(sizeof(struct GPTPartitionHandle) + entrysize, MEMF_CLEAR);
		if (gph)
		{
		    UQUAD startblk = AROS_LE2QUAD(p->StartBlock);
		    UQUAD endblk   = AROS_LE2QUAD(p->EndBlock);

		    initPartitionHandle(root, &gph->ph, startblk, endblk - startblk + 1);

		    /* Map UUID to a DOSType */
		    gph->ph.de.de_DosType   = GPT_GetDosType(p);
		    gph->ph.de.de_TableSize = 16;

		    /* Store the whole entry and convert name into ASCII form */
		    CopyMem(p, &gph[1], entrysize);
		    FromUTF16(gph->name, p->Name, 36);

		    gph->ph.ln.ln_Name = gph->name;
		    gph->entrySize     = entrysize;

		    ADDTAIL(&root->table->list, gph);
		    DREAD(bug("[GPT] Added partition %u (%s), handle 0x%p\n", i, gph->name, gph));
		}
		else
		{
	    	    err = ERROR_NO_FREE_STORE;
	    	    break;
	    	}

		/* Jump to next entry, skip 'entrysize' bytes */
    	    	p = (APTR)p + entrysize;
    	    }
    	}

    	FreeMem(table, tablesize);
    }

    return err;
}

static void PartitionGPTClosePartitionTable(struct Library *PartitionBase, struct PartitionHandle *root)
{
    struct PartitionHandle *ph, *ph2;

    /* Free all partition entries */
    ForeachNodeSafe(&root->table->list, ph, ph2)
        FreeVec(ph);

    FreeMem(root->table->data, root->de.de_SizeBlock<<2);
}

static LONG PartitionGPTOpenPartitionTable(struct Library *PartitionBase, struct PartitionHandle *root)
{
    LONG res;

    /*
     * The header is attached to partition table handle.
     * This allows us to write back the complete header and keep
     * data we don't know about in future GPT revisions.
     */
    root->table->data = AllocMem(root->de.de_SizeBlock << 2, MEMF_ANY);
    if (!root->table->data)
    	return ERROR_NO_FREE_STORE;

    /* Read primary GPT table */
    res = GPTReadPartitionTable(PartitionBase, root, root->table->data, 1);

    /* Cleanup if reading failed */
    if (res)
    	PartitionGPTClosePartitionTable(PartitionBase, root);

    return res;
}

static LONG PartitionGPTWritePartitionTable(struct Library *PartitionBase, struct PartitionHandle *root)
{
    struct GPTHeader *hdr = root->table->data;
    ULONG cnt       = AROS_LE2LONG(hdr->NumEntries);
    ULONG entrysize = AROS_LE2LONG(hdr->EntrySize);
    ULONG tablesize = AROS_ROUNDUP2(entrysize * cnt, root->de.de_SizeBlock << 2);
    struct GPTPartition *table;

    /*
     * TODO: Update legacy MBR data here when adding/moving is implemented. IntelMacs have
     * legacy MBR filled in with copies of four first entries in GPT table if at least one
     * FAT partition is defined on the drive (to support Windows XP).
     * CHS data for these entries is always set to (c=1023, h=254, s=63), however start and end
     * block numbers reflect the real position. Apple's disk utility always keeps these entries
     * in sync with their respective GPT entries. We need to do the same. Also remember to keep
     * boot code in sector 0.
     */

    DWRITE(bug("[GPT] Write: %u entries per %u bytes, %u bytes total\n", cnt, entrysize, tablesize));

    /* Allocate buffer for the whole table */
    table = AllocMem(tablesize, MEMF_CLEAR);
    if (table)
    {
	struct GPTPartition *p = table;
	struct GPTPartitionHandle *gph;
	LONG res;

	/*
	 * Collect our entries and build up the whole table.
	 * At this point we are guaranteed to have no more entries than
	 * can fit into reserved space. It's AddPartition()'s job to ensure this.
	 */
	ForeachNode(&root->table->list, gph)
    	{
	    DWRITE(bug("[GPT] Writing partition %s, handle 0x%p\n", gph->name, gph));

    	    /*
    	     * Put our entry into the buffer.
    	     * Use entry's own length, because if this entry is created by AddPartition(),
    	     * it can be shorter than on-disk one (if someone uses extended length we don't know about).
    	     */
    	    CopyMem(&gph[1], p, gph->entrySize);

	    DWRITE(PRINT_LE_UUID("Type     ", &p->TypeID));
	    DWRITE(PRINT_LE_UUID("Partition", &p->PartitionID));
	    DWRITE(KPrintF("[GPT] Blocks    %llu - %llu\n", AROS_LE2QUAD(p->StartBlock), AROS_LE2QUAD(p->EndBlock)));
	    DWRITE(KPrintF("[GPT] Flags     0x%08lX 0x%08lX\n", AROS_LE2LONG(p->Flags0), AROS_LE2LONG(p->Flags1)));
	    DWRITE(KPrintF("[GPT] Offset    0x%p\n", (APTR)p - (APTR)table));

    	    /* Jump to next entry */
    	    p = (APTR)p + entrysize;
    	}

	DWRITE(KPrintF("[GPT] Write: start block %llu\n", hdr->StartBlock));

	res = writeDataFromBlock(root, AROS_LE2QUAD(hdr->StartBlock), tablesize, table);
	DWRITE(bug("[GPT] Write result: %u\n", res));

	FreeMem(table, tablesize);

	return res ? ERROR_WRITE_PROTECTED : 0;
    }

    return ERROR_NO_FREE_STORE;
}

static LONG PartitionGPTGetPartitionAttr(struct Library *PartitionBase, struct PartitionHandle *ph, struct TagItem *tag)
{
    struct GPTPartition *part = (APTR)ph + sizeof(struct GPTPartitionHandle);

    switch (tag->ti_Tag)
    {
    case PT_TYPE:
    	uuid_from_le((uuid_t *)tag->ti_Data, &part->TypeID);
    	PTYPE(tag->ti_Data)->id_len = sizeof(uuid_t);
        return TRUE;

    case PT_BOOTABLE:
    	/* This extra flag is valid only for AROS partitions */
    	if (is_aros_uuid_le(&part->TypeID))
    	    *((ULONG *)tag->ti_Data) = (AROS_LE2LONG(part->Flags1) & GPT_PF1_AROS_BOOTABLE) ? TRUE : FALSE;
    	else
    	    *((ULONG *)tag->ti_Data) = FALSE;
        return TRUE;

    case PT_AUTOMOUNT:
	*((ULONG *)tag->ti_Data) = (AROS_LE2LONG(part->Flags1) & GPT_PF1_NOMOUNT) ? FALSE : TRUE;
        return TRUE;

    case PT_STARTBLOCK:
	*((ULONG *)tag->ti_Data) = AROS_LE2LONG(part->StartBlock);
    	return TRUE;

    case PT_ENDBLOCK:
	*((ULONG *)tag->ti_Data) = AROS_LE2LONG(part->EndBlock);
    	return TRUE;
    }

    return 0;
}

static LONG PartitionGPTSetPartitionAttrs(struct Library *PartitionBase, struct PartitionHandle *ph, const struct TagItem *taglist)
{
    struct GPTPartition *part = (APTR)ph + sizeof(struct GPTPartitionHandle);
    struct TagItem *tag;
    struct TagItem *bootable = NULL;

    while ((tag = NextTagItem(&taglist)))
    {
    	switch (tag->ti_Tag)
    	{
    	case PT_NAME:
    	    strncpy(GPTH(ph)->name, (char *)tag->ti_Data, 36);
    	    ToUTF16(part->Name, GPTH(ph)->name, 36);
    	    break;

    	case PT_TYPE:
	    /* Foolproof check */
    	    if (PTYPE(tag->ti_Data)->id_len == sizeof(uuid_t))
    	    {
	        uuid_to_le(&part->TypeID, (uuid_t *)tag->ti_Data);
	        /* Update DOSType according to a new type ID */
	        ph->de.de_DosType = GPT_GetDosType(part);
	    }
	    break;

	case PT_BOOTABLE:
	    bootable = tag;
	    break;

	case PT_AUTOMOUNT:
	    D(bug("[GPT] Setting automount flag to %ld\n", tag->ti_Data));
	    D(bug("[GPT] Partition handle 0x%p, flags 0x%08X\n", ph, part->Flags1));

	    if (tag->ti_Data)
	    	part->Flags1 &= ~AROS_LONG2LE(GPT_PF1_NOMOUNT);
	    else
	  	part->Flags1 |= AROS_LONG2LE(GPT_PF1_NOMOUNT);

	    D(bug("[GPT] New flags: 0x%08X\n", part->Flags1));

	    break;

	/* TODO: implement the rest (geometry, dosenvec, start/end block) */
	}
    }

    /*
     * Now check bootable attribute.
     * It is applicable only to AROS partitions, so we check it here,
     * after possible type change.
     */
    if (bootable && is_aros_uuid_le(&part->TypeID))
    {
	if (bootable->ti_Data)
	    part->Flags1 |= AROS_LONG2LE(GPT_PF1_AROS_BOOTABLE);
	else
	    part->Flags1 &= ~AROS_LONG2LE(GPT_PF1_AROS_BOOTABLE);
    }

    return 0;
}

static const struct PartitionAttribute PartitionGPTPartitionTableAttrs[]=
{
    {PTTA_TYPE,           PLAM_READ},
    {PTTA_DONE,           0}
};

static const struct PartitionAttribute PartitionGPTPartitionAttrs[]=
{
    {PTA_GEOMETRY,  PLAM_READ},
    {PTA_TYPE,      PLAM_READ|PLAM_WRITE},
    {PTA_POSITION,  PLAM_READ},
    {PTA_NAME,      PLAM_READ|PLAM_WRITE},
    {PTA_BOOTABLE,  PLAM_READ|PLAM_WRITE},
    {PTA_AUTOMOUNT, PLAM_READ|PLAM_WRITE},
    {PT_STARTBLOCK, PLAM_READ},
    {PT_ENDBLOCK,   PLAM_READ},
    {PTA_DONE, 0}
};

const struct PTFunctionTable PartitionGPT =
{
    PHPTT_GPT,
    "GPT",
    PartitionGPTCheckPartitionTable,
    PartitionGPTOpenPartitionTable,
    PartitionGPTClosePartitionTable,
    PartitionGPTWritePartitionTable,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PartitionGPTGetPartitionAttr,
    PartitionGPTSetPartitionAttrs,
    PartitionGPTPartitionTableAttrs,
    PartitionGPTPartitionAttrs,
    NULL,
    NULL
};
