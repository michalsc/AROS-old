#include <libraries/reqtools.h>
#include <proto/exec.h>
#include <proto/reqtools.h>

#include <stdio.h>
#include <string.h>

#define PROGNAME "rtfont"

struct Library *ReqToolsBase;

static char s[300];

static void cleanup(char *msg)
{
    if (msg) printf(PROGNAME ": %s\n", msg);
    
    if (ReqToolsBase) CloseLibrary((struct Library *)ReqToolsBase);
    
    exit(0);
}

static void openlibs(void)
{
    ReqToolsBase = (struct ReqToolsBase *)OpenLibrary("reqtools.library", 0);
    if (!ReqToolsBase) cleanup("Can't open reqtools.library");
}

static void action(void)
{
    struct rtFontRequester *req;
    
    struct TagItem tags[] =
    {
    	{TAG_DONE	    		}
    };
    
    if ((req = rtAllocRequestA(RT_FONTREQ, tags)))
    {
        rtFontRequestA(req, "Title", tags);
	rtFreeRequest(req);
    }
}

int main(void)
{
    openlibs();
    action();
    cleanup(0);
}
