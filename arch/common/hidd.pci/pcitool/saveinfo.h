/*
    Copyright (C) 2003-2009, The AROS Development Team.
    $Id$
*/

#ifndef SAVEINFO_H_
#define SAVEINFO_H_


#include <stdio.h>
#include <stdlib.h>
#include <proto/dos.h>


struct PCIInfo
{
char Driver_name[200];
char Direct_bus[200];
char Hardware_info[200];
char Description[200];
char Vendor_name[200];
char Product_name[200];
char Subsystem[200];
char VendorID[10];
char ProductID[10];
char RevisionID[10];
char Class[200];
char Subclass[200];
char Interface[200];
char ROM_Base[10];
char ROM_Size[10];
char IRQ[10];
char Status[200];
char Rangelist_0[200];
char Rangelist_1[200];
char Rangelist_2[200];
char Rangelist_3[200];
char Rangelist_4[200];
char Rangelist_5[200];

};
 
void SaveToDisk(struct PCIInfo *DeviceInfo);

BPTR DeviceInfoFile;

#endif /*SAVEINFO_H_*/