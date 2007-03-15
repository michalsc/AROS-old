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

struct BootSector {
    UBYTE bs_jmp_boot[3];
    UBYTE bs_oem_name[8];
    UWORD bpb_bytes_per_sect;
    UBYTE bpb_sect_per_clust;
    UWORD bpb_rsvd_sect_count;
    UBYTE bpb_num_fats;
    UWORD bpb_root_entries_count;
    UWORD bpb_total_sectors_16;
    UBYTE bpb_media;
    UWORD bpb_fat_size_16;
    UWORD bpb_sect_per_track;
    UWORD bpb_num_heads;
    ULONG bpb_hidden_sect;
    ULONG bpb_total_sectors_32;

    union {
        struct {
            UBYTE bs_drvnum;
            UBYTE bs_reserved1;
            UBYTE bs_bootsig;
            ULONG bs_volid;
            UBYTE bs_vollab[11];
            UBYTE bs_filsystype[8];
        } __attribute__ ((__packed__)) fat16;

        struct {
            ULONG bpb_fat_size_32;
            UWORD bpb_extflags;
            UWORD bpb_fs_verion;
            ULONG bpb_root_cluster;
            UWORD bpb_fs_info;
            UWORD bpb_back_bootsec;
            UBYTE bpb_reserved[12];
            UBYTE bs_drvnum;
            UBYTE bs_reserved1;
            UBYTE bs_bootsig;
            ULONG bs_volid;
            UBYTE bs_vollab[11];
            UBYTE bs_filsystype[8];
        } __attribute__ ((__packed__)) fat32;
    } type;

} __attribute__ ((__packed__));

struct FSInfo {
    ULONG lead_sig;
    UBYTE reserved1[480];
    ULONG struct_sig;
    ULONG free_count;
    ULONG next_free;
    UBYTE reserved2[12];
    ULONG trial_sig;
};

struct DirEntry {
    UBYTE name[11];
    UBYTE attr;
    UBYTE nt_res;
    UBYTE create_time_tenth;
    UWORD create_time;
    UWORD create_date;
    UWORD last_access_date;
    UWORD first_cluster_hi;
    UWORD write_time;
    UWORD write_date;
    UWORD first_cluster_lo;
    ULONG file_size;
} __attribute__ ((__packed__));

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#define ATTR_REALENTRY  0x100
#define ATTR_ROOTDIR    0x200

#define ATTR_LONG_NAME (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
#define ATTR_LONG_NAME_MASK (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE)

struct LongDirEntry {
    UBYTE order;
    UBYTE name1[10];
    UBYTE attr;
    UBYTE type;
    UBYTE checksum;
    UBYTE name2[12];
    UWORD first_cluster_lo;
    UBYTE name3[4];
} __attribute__ ((__packed__));

#define FAT_MAX_LONG_FILENAME 0xff
