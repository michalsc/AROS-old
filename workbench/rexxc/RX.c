/*
    Copyright � 2007, The AROS Development Team. All rights reserved.
    $Id$

    Run rexx scripts
*/

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <rexx/storage.h>
#include <rexx/errors.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/rexxsyslib.h>
#include <proto/alib.h>

#include <stdio.h>
#include <ctype.h>

static struct RexxMsg *msg = NULL;
static struct MsgPort *rexxport = NULL, *replyport = NULL;
static BPTR out;

static BOOL init(void)
{
#ifdef __AROS__
    out = Error();
#else 
    out = Output();
#endif
    
    rexxport = FindPort("REXX");
    if (rexxport == NULL)
    {
	FPuts(out, "starting rexxmast not implemented\n");
	return FALSE;
    }
    
    replyport = CreatePort(NULL, 0);
    if (replyport == NULL)
    {
	FPuts(out, "Could not create a port\n");
	return FALSE;
    }
    
    msg = CreateRexxMsg(replyport, NULL, NULL);
    if (msg == NULL)
    {
	FPuts(out, "Could not create RexxMsg\n");
	return FALSE;
    }
    msg->rm_Action = RXCOMM | RXFF_RESULT;
    msg->rm_Stdin = Input();
    msg->rm_Stdout = Output();
    
    return TRUE;
}

void cleanup(void)
{
    if (msg)
	DeleteRexxMsg(msg);
    if (replyport)
	DeletePort(replyport);
}

#include <aros/debug.h>
int main(int argc, char **argv)
{
    struct RexxMsg *reply;
    int ret;
    
    if (!init())
    {
	cleanup();
	return RC_ERROR;
    }
    
    if (argc == 1)
    {
	FPuts(out, "Required argument missing\n");
	cleanup();
	return RC_ERROR;
    }

    if (argc == 0)
    {
	/* TODO: start from workbench */
	FPuts(out, "RX started from workbench not implemented\n");
	cleanup();
	return RC_ERROR;
    }
    else
    {
	UBYTE *s;
	struct Process *me = (struct Process *)FindTask(NULL);
	ULONG length = 0;
	
	s = me->pr_Arguments;
	while(isspace(*s)) s++;
	
	if (*s == '"')
	{
	    s++;
	    while((s[length] != '"') && (s[length] != '\0')) length++;
	    if (length == 0)
	    {
		FPuts(out, "Empty command\n");
		cleanup();
		return RC_ERROR;
	    }
	    if (s[length] == '\0')
	    {
		FPuts(out, "Unterminated string\n");
		cleanup();
		return RC_ERROR;
	    }
	    
	    msg->rm_Args[0] = (IPTR)CreateArgstring(s, length);
	    /* It is a literal command with 1 argument */
	    msg->rm_Action |= (RXFF_STRING | 1);
	}
	else if (*s == '\'')
	{
	    s++;
	    while((s[length] != '\'')
                  && (s[length] != '\0')
                  && (s[length] != '\n')
            )
                length++;
	    
	    msg->rm_Args[0] = (IPTR)CreateArgstring(s, length);
	    /* It is a literal command with 1 argument */
	    msg->rm_Action |= (RXFF_STRING | 1);
	}
	else
	{
            if (s[strlen(s)-1] == '\n')
                s[strlen(s)-1] = '\0';
            
            msg->rm_Args[0] = (IPTR)CreateArgstring(s, strlen(s));
	    msg->rm_Action |= 1;
	}

	PutMsg(rexxport, (struct Message *)msg);
	do {
	    reply = (struct RexxMsg *)WaitPort(replyport);
	} while (reply != msg);

	ret = msg->rm_Result1;
	if (msg->rm_Result1 == RC_OK)
	    FPrintf(out, "Script executed and returned: %ld\n", msg->rm_Result2);
	else
	    FPrintf(out, "Error executing script %ld/%ld\n",
		    msg->rm_Result1, msg->rm_Result2
	    );
    }

    ClearRexxMsg(msg, msg->rm_Action & RXARGMASK);
    cleanup();
    
    return ret;
}
