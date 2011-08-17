#define DEBUG 1

#include <aros/debug.h>
#include <exec/alerts.h>
#include <exec/ports.h>
#include <hidd/unixio.h>
#include <hidd/unixio_inline.h>

#include <proto/exec.h>

#include "agfx.h"
#include "server.h"

#undef XSD
#define XSD(x) ((struct agfx_staticdata *)x)

/*
 * WARNING!
 * In this code every pipe error causes instant shutdown.
 * This is intentional because error likely means broken pipe. This can happen
 * if the parent process (bootstrap) exited. In this case we should immediately exit too,
 * otherwise we end up in AROS process running out of any control. On non-rooted phone
 * it can be shut down only by rebooting the phone.
 */

static void ReadPipe(int pipe, void *data, int len, struct agfx_staticdata *xsd)
{
    int res;
    int err;
    
    res = Hidd_UnixIO_ReadFile(xsd->unixio, pipe, data, len, &err);
    if (res != len)
    {
	D(bug("[AGFX.server] Error reading pipe, result %d, error %d\n", res, err));
 	ShutdownA(SD_ACTION_POWEROFF);
    }
}

void agfxInt(int pipe, int mode, void *data)
{
    DB2(bug("[AGFX.server] Event 0x%08X on pipe %d\n", mode, pipe));

    if (mode & vHidd_UnixIO_Error)
    {
    	D(bug("[AGFX.server] Error condition on input pipe\n"));
    	ShutdownA(SD_ACTION_POWEROFF);
    }

    if (mode & vHidd_UnixIO_Read)
    {
    	struct Request header;
    	ULONG status = STATUS_ACK;

	/* First read packet header */
	ReadPipe(pipe, &header, sizeof(header), data);
    	DB2(bug("[AGFX.server] Command 0x%08X with %u parameters from server\n", header.cmd, header.len));

	if (header.cmd == cmd_Nak)
	{
	    /*
	     * Special handling for NAK reply.
	     * We know it has one parameter - original command.
	     */
	    ReadPipe(pipe, &header.cmd, sizeof(ULONG), data);

	    D(bug("[AGFX.server] NAK %d received\n", header.cmd));
	    status = STATUS_NAK;
	}

	if (header.cmd & CMD_NEED_REPLY)
	{
	    /* A WaitRequest is being replied */
	    if (!IsListEmpty(&XSD(data)->waitQueue))
	    {
	    	struct WaitRequest *request = (struct WaitRequest *)XSD(data)->waitQueue.mlh_Head;

	    	if (request->cmd == header.cmd)
	    	{
	    	    if (header.len && (status == STATUS_ACK))
	    	    {
	    	    	ULONG *pkt = &request->cmd;

	    	    	/* Read reply parameters */
    	            	ReadPipe(pipe, &pkt[request->len + 2], header.len * sizeof(ULONG), data);
    	            }

    	    	    DB2(bug("[AGFX.server] Replying request 0x%p\n", request));

		    Remove((struct Node *)request);
    	    	    request->status = status;
		    Signal(request->owner, request->signal);

		    /* We have read the data, exit */
		    return;
    	    	}
    	    	    D(else bug("[AGFX.server] Bogus reply 0x%08X for request 0x%08X\n", header.cmd, request->cmd);)
    	    }
    	    	D(else bug("[AGFX.server] Bogus reply 0x%08X without a request\n", header.cmd);)
    	}

	/*
	 * If we are here, we haven't read the data portion.
	 * This is either unknown command or bogus response.
	 * We don't know what to do with arguments, so just swallow them.
	 */
    	for (status = 0; status < header.len; status++)
    	    ReadPipe(pipe, &header.cmd, sizeof(ULONG), data);
    }
}

void SendRequest(struct Request *req, struct agfx_staticdata *xsd)
{
    /* Total packet length is header plus specified number of ULONG parameters */
    int len = sizeof(struct Request) + req->len * sizeof(ULONG);
    int res, err;

    res = Hidd_UnixIO_WriteFile(xsd->unixio, xsd->DisplayPipe, req, len, &err);
    if (res != len)
    {
        D(bug("[AGFX.server] Error writing pipe, result %d, error %d\n", res, err));
    	ShutdownA(SD_ACTION_POWEROFF);
    }
}

void DoRequest(struct WaitRequest *req, struct agfx_staticdata *xsd)
{
    /*
     * Set request owner.
     * Limitation: one task can have only one request pending. But
     * it's faster than CreateMsgPort() every time.
     */
    req->owner  = FindTask(NULL);
    req->signal = SIGF_BLIT;

    /* Add the request to the wait queue */
    Disable();
    AddTail((struct List *)&xsd->waitQueue, (struct Node *)req);
    Enable();

    /* Make sure the signal isn't already set */
    SetSignal(0, SIGF_BLIT);

    /* Actually send the request */
    SendRequest((struct Request *)&req->cmd, xsd);

    /* Wait for reply */
    Wait(SIGF_BLIT);
}
