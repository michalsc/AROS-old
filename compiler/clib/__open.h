#ifndef ___OPEN_H
#define ___OPEN_H

/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: file descriptors handling internals - header file
    Lang: english
*/

typedef struct
{
    void *fh;
    int  flags;
    unsigned int opencount;
} fdesc;

#if !defined(_CLIB_KERNEL_) && !defined(_CLIB_LIBRARY_)
extern void *__stdfiles[3];
#else
#include <libraries/arosc.h>
#endif

fdesc *__getfdesc(register int fd);
void __setfdesc(register int fd, fdesc *fdesc);
int __getfdslot(int wanted_fd);
int __getfirstfd(register int startfd);
int __open(int wanted_fd, const char *pathname, int flags, int mode);
LONG __oflags2amode(int flags);
#endif
