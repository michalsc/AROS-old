/* Copyright 2007-2012 Fredrik Wikstrom. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
*/

#include "support.h"
#include <devices/diskimage.h>
#include <proto/exec.h>
//#include <proto/diskimage.h>

struct MsgPort *DiskImagePort;
struct IORequest *DiskImageIO;
struct Library *DiskImageBase;

BOOL OpenDiskImageDevice (ULONG unit_num) {
	DiskImagePort = CreateMsgPort();
	DiskImageIO = CreateIORequest(DiskImagePort, sizeof(struct IOExtTD));
	if (DiskImageIO) {
		if (OpenDevice("diskimage.device", unit_num, DiskImageIO, 0) == IOERR_SUCCESS) {
			DiskImageBase = (struct Library *)DiskImageIO->io_Device;
			return TRUE;
		}
	}
	CloseDiskImageDevice();
	return FALSE;
}

void CloseDiskImageDevice (void) {
	if (DiskImageBase) {
		CloseDevice(DiskImageIO);
		DiskImageBase = NULL;
	}
	DeleteIORequest(DiskImageIO);
	DeleteMsgPort(DiskImagePort);
	DiskImageIO = NULL;
	DiskImagePort = NULL;
}
