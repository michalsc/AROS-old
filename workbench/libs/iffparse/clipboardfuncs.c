/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc: Helpfuncs needed when iffparse is used for clipboard handling.
    Lang: English.
*/
#include "iffparse_intern.h"
#include <exec/io.h>


/***********************/
/* Port initialization */
/***********************/

/* Initializes and Closes PRIVATE ports     */
/* Used in OpenClipboard and CloseClipboard */
/* Look at page 501-502 in RKM Libraries     */

BOOL InitPort (struct MsgPort *mp, struct Task *t,
	struct IFFParseBase_intern * IFFParseBase)
{
    LONG sigbit;

    if ((sigbit = AllocSignal(-1L) == -1)) return (FALSE);

    mp->mp_Node.ln_Type = NT_MSGPORT;
    mp->mp_Flags	=  PA_SIGNAL;
    mp->mp_SigBit	 =  sigbit;
    mp->mp_SigTask	=  t;

    NewList(&(mp->mp_MsgList));

    return (TRUE);
}

VOID ClosePort (struct MsgPort *mp,
    struct IFFParseBase_intern * IFFParseBase)
{
    mp->mp_SigTask	    =  (struct Task*)-1;
    mp->mp_MsgList.lh_Head  = (struct Node*)-1;

    FreeSignal( mp->mp_SigBit );

    return;
}


/**********************/
/* ClipStreamHandler  */
/**********************/

#undef SysBase
#define SysBase     (IPB(hook->h_Data)->sysbase)

ULONG ClipStreamHandler
(
    struct Hook 	* hook,
    struct IFFHandle	* iff,
    struct IFFStreamCmd * cmd
)
{
    #define CLIPSCANBUFSIZE 500
    LONG error = NULL;

    /* Buffer neede for reading rest of clip in IFFCMD_CLEANUP. Eats some stack */
    UBYTE  buf[CLIPSCANBUFSIZE];

    struct IOClipReq *req;

    req = &( ((struct ClipboardHandle*)iff->iff_Stream)->cbh_Req);

    switch (cmd->sc_Command)
    {
	case IFFCMD_READ:
	    req->io_Command = CMD_READ;
	    req->io_Data    = cmd->sc_Buf;
	    req->io_Length  =  cmd->sc_NBytes;

	    error = (DoIO((struct IORequest*)req));

	    break;

	case IFFCMD_WRITE:
	    req->io_Command = CMD_WRITE;
	    req->io_Data    = cmd->sc_Buf;
	    req->io_Length  =  cmd->sc_NBytes;

	    error = (DoIO((struct IORequest*)req));

	    break;

	case IFFCMD_SEEK:

	    req->io_Offset += cmd->sc_NBytes;

	    if (req->io_Offset < 0)
		error = TRUE;

	    break;

	case IFFCMD_INIT:
	    /* Start reading and writing at offset 0 */
	    req->io_ClipID = 0;
	    req->io_Offset = 0;
	    break;

	case IFFCMD_CLEANUP:
	    /* Read past end of clip if we are in read mode */

	    req->io_Command = CMD_READ;
	    req->io_Data    = buf;
	    req->io_Length  =  CLIPSCANBUFSIZE;


	    if ((iff->iff_Flags & IFFF_RWBITS) == IFFF_READ)
	    {
		/* Read until there is not more left */
		while (req->io_Actual)
		    DoIO((struct IORequest*)req);

	    }
	    
	    if ((iff->iff_Flags & IFFF_RWBITS) == IFFF_WRITE)
	    {
	        req->io_Command = CMD_UPDATE;
		DoIO((struct IORequest*)req);
	    }
	    break;

    }

    return (error);
}
