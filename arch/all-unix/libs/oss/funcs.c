/*
    Copyright � 1995-2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <sys/soundcard.h>
#include <sys/mman.h>

#include <aros/debug.h>

#define OSSBase GlobalOSSBase

#include "oss_intern.h"

/* Temporary hack. We do not get our library base as a parameter
   so we have to store it globally. */
struct OSS_Base *GlobalOSSBase;

/******************************************************************************/

BOOL OSS_Open(char *filename, BOOL read, BOOL write, BOOL blocking)
{
    int openflags;
    
    if (!filename) filename = "/dev/dsp";
    
    if (write && read)
    {
    	openflags = O_RDWR;
    }
    else if (write)
    {
    	openflags = O_WRONLY;
    }
    else
    {
    	openflags = O_RDONLY;
    }
    
    if (!blocking) openflags |= O_NONBLOCK;

    ObtainSemaphore(&OSSBase->sem);    

    audio_fd = OSSBase->OSSIFace->open(filename, openflags, 0);

    ReleaseSemaphore(&OSSBase->sem);

    return (audio_fd >= 0) ? TRUE : FALSE;
}

/******************************************************************************/

void OSS_Close(void)
{
    if (audio_fd != -1)
    {
	ObtainSemaphore(&OSSBase->sem);
	OSSBase->OSSIFace->close(audio_fd);
	ReleaseSemaphore(&OSSBase->sem);

	audio_fd = -1;
    }
}

/******************************************************************************/

void OSS_Reset(void)
{
    if (audio_fd >= 0)
    {
    	int value = 0;

	ObtainSemaphore(&OSSBase->sem);
	OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_RESET, &value);
	ReleaseSemaphore(&OSSBase->sem);
    }
}

/******************************************************************************/

BOOL OSS_SetFragmentSize(int num_fragments, int fragment_size)
{
    if (audio_fd >= 0)
    {
    	int value;
	int retval;
	
	value = (num_fragments << 16) | fragment_size;

	ObtainSemaphore(&OSSBase->sem);
	retval = OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &value);
	ReleaseSemaphore(&OSSBase->sem);

	return (retval < 0) ? FALSE : TRUE;
    }
    else
    {
    	return FALSE;
    }
}

/******************************************************************************/

BOOL OSS_GetOutputInfo(int *num_fragments_available, int *num_fragments_allocated,
    	    	       int *fragment_size, int *num_bytes_available)

{
    if (audio_fd >= 0)
    {
    	audio_buf_info info;

	ObtainSemaphore(&OSSBase->sem);
	OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_GETOSPACE, &info);
	ReleaseSemaphore(&OSSBase->sem);

	if (num_fragments_available) *num_fragments_available = info.fragments;
	if (num_fragments_allocated) *num_fragments_allocated = info.fragstotal;
	if (fragment_size) *fragment_size = info.fragsize;
	if (num_bytes_available) *num_bytes_available = info.bytes;
	
	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}

/******************************************************************************/

BOOL OSS_GetOutputPointer(int *processed_bytes, int *fragment_transitions, int *dmapointer)
{
    if (audio_fd >= 0)
    {
    	count_info info;

	ObtainSemaphore(&OSSBase->sem);
	OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_GETOPTR, &info);
	ReleaseSemaphore(&OSSBase->sem);

	if (processed_bytes) *processed_bytes = info.bytes;
	if (fragment_transitions) *fragment_transitions = info.blocks;
	if (dmapointer) *dmapointer = info.ptr;
	
	return TRUE;
    }
    else
    {
    	return FALSE;
    }
    
}


/******************************************************************************/

int audio_supported_fmts;

static BOOL get_supported_fmts(void)
{
    if (audio_supported_fmts) return TRUE;
    if (audio_fd < 0) return FALSE;

    ObtainSemaphore(&OSSBase->sem);
    OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_GETFMTS, &audio_supported_fmts);
    ReleaseSemaphore(&OSSBase->sem);

    return TRUE;
}

/******************************************************************************/

BOOL OSS_FormatSupported_S8(void)
{
    if (!get_supported_fmts()) return FALSE;
    
    return (audio_supported_fmts & AFMT_S8) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_FormatSupported_U8(void)
{
    if (!get_supported_fmts()) return FALSE;
    
    return (audio_supported_fmts & AFMT_U8) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_FormatSupported_S16LE(void)
{
    if (!get_supported_fmts()) return FALSE;
    
    return (audio_supported_fmts & AFMT_S16_LE) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_FormatSupported_S16BE(void)
{
    if (!get_supported_fmts()) return FALSE;
    
    return (audio_supported_fmts & AFMT_S16_BE) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_FormatSupported_U16LE(void)
{
    if (!get_supported_fmts()) return FALSE;
    
    return (audio_supported_fmts & AFMT_U16_LE) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_FormatSupported_U16BE(void)
{
    if (!get_supported_fmts()) return FALSE;
    
    return (audio_supported_fmts & AFMT_U16_BE) ? TRUE : FALSE;
}

/******************************************************************************/

int audio_capabilities;

static BOOL get_capabilities(void)
{    
    if (audio_capabilities) return TRUE;
    if (audio_fd < 0) return FALSE;

    ObtainSemaphore(&OSSBase->sem);
    OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &audio_capabilities);
    ReleaseSemaphore(&OSSBase->sem);

    return TRUE;
}

/******************************************************************************/

int OSS_Revision(void)
{
    if (!get_capabilities()) return 0;
    
    return audio_capabilities & DSP_CAP_REVISION;
}

/******************************************************************************/

BOOL OSS_Capability_Duplex(void)
{
    if (!get_capabilities()) return 0;
    
    return (audio_capabilities & DSP_CAP_DUPLEX) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_Capability_Realtime(void)
{
    if (!get_capabilities()) return 0;
    
    return (audio_capabilities & DSP_CAP_REALTIME) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_Capability_Trigger(void)
{
    if (!get_capabilities()) return 0;
    
    return (audio_capabilities & DSP_CAP_TRIGGER) ? TRUE : FALSE;
}

/******************************************************************************/

BOOL OSS_Capability_MMap(void)
{
    if (!get_capabilities()) return 0;
    
    return (audio_capabilities & DSP_CAP_MMAP) ? TRUE : FALSE;
}

/******************************************************************************/

static BOOL set_format(int fmt)
{
    if (audio_fd >= 0)
    {
    	int val = fmt;	
    	int retval;

	ObtainSemaphore(&OSSBase->sem);
	retval = OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_SETFMT, &val);
	ReleaseSemaphore(&OSSBase->sem);

	if ((val != fmt) || (retval < 0))
	{
	    return FALSE;
	}
	
	return TRUE;
    	
    }
    else
    {
    	return FALSE;
    }
}

/******************************************************************************/

BOOL OSS_SetFormat_S8(void)
{
    return set_format(AFMT_S8); 
}

/******************************************************************************/

BOOL OSS_SetFormat_U8(void)
{
    return set_format(AFMT_U8); 
}

/******************************************************************************/

BOOL OSS_SetFormat_S16LE(void)
{
    return set_format(AFMT_S16_LE); 
}

/******************************************************************************/

BOOL OSS_SetFormat_S16BE(void)
{
    return set_format(AFMT_S16_BE); 
}

/******************************************************************************/

BOOL OSS_SetFormat_U16LE(void)
{
    return set_format(AFMT_S16_LE); 
}

/******************************************************************************/

BOOL OSS_SetFormat_U16BE(void)
{
    return set_format(AFMT_S16_BE); 
}

/******************************************************************************/

BOOL OSS_SetMono(void)
{
    int val = 0;
    int retval;
    
    if (audio_fd < 0) return FALSE;

    ObtainSemaphore(&OSSBase->sem);
    retval = OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_STEREO, &val);
    ReleaseSemaphore(&OSSBase->sem);

    if ((retval < 0) || (val != 0)) return FALSE;
    
    return TRUE;
}

/******************************************************************************/

BOOL OSS_SetStereo(void)
{
    int val = 1;
    int retval;
    
    if (audio_fd < 0) return FALSE;
    
    ObtainSemaphore(&OSSBase->sem);
    retval = OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_STEREO, &val);
    ReleaseSemaphore(&OSSBase->sem);
    
    if ((retval < 0) || (val != 1)) return FALSE;
    
    return TRUE;
}

/******************************************************************************/

BOOL OSS_SetNumChannels(int numchannels)
{
    int val = numchannels;
    int retval;
    
    if (audio_fd < 0) return FALSE;

    ObtainSemaphore(&OSSBase->sem);
    retval = OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &val);
    ReleaseSemaphore(&OSSBase->sem);

    if (retval < 0)
    {
    	return (numchannels > 1) ? OSS_SetStereo() : OSS_SetMono();
    }
    
    return TRUE;  
}

/******************************************************************************/

BOOL OSS_SetWriteRate(int rate, int *used_rate)
{
    int val = rate;
    int retval;
    
    if (audio_fd < 0) return FALSE;

    ObtainSemaphore(&OSSBase->sem);
    retval = OSSBase->OSSIFace->ioctl(audio_fd, SOUND_PCM_WRITE_RATE, &val);
    ReleaseSemaphore(&OSSBase->sem);

    if (retval < 0)
    {
    	return FALSE;
    }
    
    if (used_rate) *used_rate = val;
    
    return TRUE;  
}

/******************************************************************************/

BOOL OSS_MMap(APTR *mapped_address, int len, BOOL read, BOOL write)
{
#if 1
#warning "Can't use mmap yet!"
    kprintf("\n=== Dont' call OSS_MMap! Not implemented yet! ===\n\n");
    return FALSE;
#else      
    APTR buf;
    int protection;
    
    if (audio_fd < 0) return FALSE;
    
    if (read && write)
    {
    	protection = PROT_READ | PROT_WRITE;
    }
    else if (read)
    {
    	protection = PROT_READ;
    }
    else
    {
    	protection = PROT_WRITE;
    }

    ObtainSemaphore(&OSSBase->sem);
    buf = OSSBase->OSSIFace->mmap(NULL, len, protection, MAP_SHARED, audio_fd, 0);
    ReleaseSemaphore(&OSSBase->sem);

    if (buf == MAP_FAILED)
    {
    	return FALSE;
    }
    
    *mapped_address = buf;
#endif
    
    return TRUE;
    
}

/******************************************************************************/

void OSS_MUnmap(APTR mapped_address, int len)
{
#if 1
#warning "Can't use munmap yet!"
    kprintf("\n=== Dont' call OSS_MUnmap! Not implemented yet! ===\n\n");
#else
    if ((audio_fd >= 0) && (mapped_address != MAP_FAILED))
    {
	ObtainSemaphore(&OSSBase->sem);
    	OSSBase->OSSIFace->munmap(mapped_address, len);
	ReleaseSemaphore(&OSSBase->sem);
    }
#endif
}

/******************************************************************************/

BOOL OSS_SetTrigger(BOOL input, BOOL output)
{
    int val = 0;
    int retval;
    
    if (audio_fd < 0) return FALSE;
    
    if (input) val |= PCM_ENABLE_INPUT;
    if (output) val |= PCM_ENABLE_OUTPUT;

    ObtainSemaphore(&OSSBase->sem);
    retval = OSSBase->OSSIFace->ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &val);
    ReleaseSemaphore(&OSSBase->sem);
    
    return (retval < 0) ? FALSE : TRUE;
}

/******************************************************************************/

int OSS_Write(APTR buf, int size)
{
    int written;
    int err;
    
    if (audio_fd < 0)
    {
    	return -1;
    }
    
    ObtainSemaphore(&OSSBase->sem);

    written = OSSBase->OSSIFace->write(audio_fd, buf, size);
    err = *OSSBase->errnoPtr;

    ReleaseSemaphore(&OSSBase->sem);

    if (written == -1)
    {
    	switch(err)
	{
	    case EAGAIN:
	    	written = -2; /* Retval -2. Caller should treat it like EAGAIN. */
		break;
		
	    case EINTR:
	    	written = -3; /* Retval -3. Caller should treat it like EINTR. */
		break;
		
	    case 0:
	    	written = -4; /* Retval -4. Caller should treat it like a 0-errno.
		                 (but since retval of write() was -1, like EAGAIN
				 maybe?) */
		break;
	}
		
    }
    
    return written;
}

/******************************************************************************/
