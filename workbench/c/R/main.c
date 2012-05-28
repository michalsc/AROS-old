/*
    Copyright � 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define DEBUG 1
#include <aros/debug.h>

#include "r.h"

#define ARG_TEMPLATE "FILENAME,PROFILE/K,NOGUI/S,ARGUMENTS/F"

enum
{
    ARG_FILENAME,
    ARG_PROFILE,
    ARG_NOGUI,
    ARG_ARGUMENTS,
    ARG_COUNT
};

// functions
static void clean_exit(struct Req *req, CONST_STRPTR s)
{
    LONG retval = RETURN_OK;

    if (s)
    {
        retval = RETURN_FAIL;
        PutStr(s);
    }
    if (req)
    {
        if (req->rda) FreeArgs(req->rda);
        FreeVec(req);
    }
    cleanup_gui();
    exit(retval);
}

static struct Req *alloc_req(void)
{
    return AllocVec(sizeof (struct Req), MEMF_ANY | MEMF_CLEAR);
}

static BOOL handle_args(struct Req *req, int argc, char **argv)
{
    if (argc)
    {
        IPTR args[ARG_COUNT] = {0};
        
        req->rda = ReadArgs(ARG_TEMPLATE, args, NULL);
        if (!req->rda)
        {
            PrintFault(IoErr(), argv[0]);
            return FALSE;
        }
        
        req->filename = (STRPTR)args[ARG_FILENAME];
        req->profile = (STRPTR)args[ARG_PROFILE];
        req->nogui = args[ARG_NOGUI] ? TRUE : FALSE;
        req->arguments = (STRPTR)args[ARG_ARGUMENTS];
    }
    else
    {
        return FALSE;
        // FIXME: it should be possible to use R as default tool
        // of another command
    }
    return TRUE;
}


// search for the command in the Shell's search path. Return TRUE if it exists and
// if it's not a directory
static BOOL check_exist(struct Req *req)
{
    // FIXME: implement me
    return TRUE;
}


// execute the command with "?" option and read the command template
static BOOL get_template(struct Req *req)
{
    BOOL retval = FALSE;

    BPTR input_fh = NULL;
    BPTR output_fh = NULL;

    TEXT out_file_name[30];
    TEXT in_file_name[30];

    TEXT cmd[255];

    LONG i;
    LONG cmd_res = 0;

    if (req->filename == NULL)
    {
        goto cleanup;
    }

    for (i = 0; i < 20 && output_fh == NULL; i++)
    {
        sprintf(out_file_name, "t:%08u.request.outfile", i);
        output_fh = Open(out_file_name, MODE_NEWFILE); 
    }
    if (output_fh == NULL)
    {
        goto cleanup;
    }

    for (i = 0; i < 20 && input_fh == NULL; i++)
    {
        sprintf(in_file_name, "t:%08u.request.infile", i);
        input_fh = Open(in_file_name, MODE_NEWFILE); 
    }
    if (input_fh == NULL)
    {
        goto cleanup;
    }
    Close(input_fh);
    input_fh = Open(in_file_name, MODE_OLDFILE);
    if (input_fh == NULL)
    {
        goto cleanup;
    }

    // append "*>NIL: ?" to the command
    strlcpy(cmd, req->filename, sizeof cmd);
    strlcat(cmd, " *>NIL: ?", sizeof cmd);

    // shut up DOS error message
    struct Process *me = (struct Process*)FindTask(NULL);
    APTR oldwin = me->pr_WindowPtr;
    me->pr_WindowPtr = (APTR)-1;

    // Execute the command
    cmd_res = Execute(cmd, input_fh, output_fh);
    D(bug("[R] Execute() returned: %d\n", cmd_res));

    // restore window ptr
    me->pr_WindowPtr = oldwin;

    // go to the beginning of the output file and read the template
    Seek(output_fh, 0, OFFSET_BEGINNING);
    if (FGets(output_fh, req->cmd_template, sizeof req->cmd_template))
    {
        D(bug("[R] template read: %s\n", req->cmd_template));
        retval = TRUE;
    }

cleanup:
    if (input_fh)
    {
        Close(input_fh);
        DeleteFile(in_file_name);
    }
    if (output_fh)
    {
        Close(output_fh);
        DeleteFile(out_file_name);
    }

    return retval;
}


static BOOL parse_template(struct Req *req)
{
    TEXT *chr = req->cmd_template;
    LONG len;

    if (req->cmd_template[0] == '\0')
        return FALSE;

    while (req->arg_cnt < MAX_ARG_CNT)
    {
        // read name
        TEXT *name_start = chr;
        while (1)
        {
            if (isalnum(*chr))
            {
                chr++;
                continue;
            }
            else if (*chr == '=')
            {
                // we are only interested in the part after the "=".
                chr++;
                name_start = chr;
                continue;
            }
            break;
        }

        len = chr - name_start;
        if (len == 0)
            return FALSE;

        if (len >= MAX_NAME_CNT - 1)
            len = MAX_NAME_CNT - 1;

        memcpy(req->cargs[req->arg_cnt].argname, name_start, len);

        // read modifiers
        while (*chr == '/')
        {
            switch (*(chr + 1))
            {
                case 'A':
                    req->cargs[req->arg_cnt].a_flag = TRUE;
                    chr++;
                    break;
                case 'F':
                    req->cargs[req->arg_cnt].f_flag = TRUE;
                    chr++;
                    break;
                case 'K':
                    req->cargs[req->arg_cnt].k_flag = TRUE;
                    chr++;
                    break;
                case 'M':
                    req->cargs[req->arg_cnt].m_flag = TRUE;
                    chr++;
                    break;
                case 'N':
                    req->cargs[req->arg_cnt].n_flag = TRUE;
                    chr++;
                    break;
                case 'S':
                    req->cargs[req->arg_cnt].s_flag = TRUE;
                    chr++;
                    break;
                default:
                    return FALSE;
                    break;
            }
            chr++;
        }
        req->arg_cnt++;
        if (*chr != ',')
            break;
        chr++;
    }
    return TRUE;
}


// create the command line from the selected options
static void execute_command(struct Req *req)
{
    ULONG i;
    CONST_STRPTR str;

    strlcpy(req->cmd_template, req->filename, sizeof req->cmd_template);

    for (i = 0; i < req->arg_cnt; i++)
    {
        if (req->cargs[i].s_flag)
        {
            if (get_gui_bool(&req->cargs[i]))
            {
                strlcat(req->cmd_template, " ", sizeof req->cmd_template);
                strlcat(req->cmd_template, req->cargs[i].argname, sizeof req->cmd_template);
            }
        }
        else if (req->cargs[i].n_flag)
        {
            str = get_gui_string(&req->cargs[i]);
            if (str[0] != '\0')
            {
                strlcat(req->cmd_template, " ", sizeof req->cmd_template);
                strlcat(req->cmd_template, req->cargs[i].argname, sizeof req->cmd_template);
                strlcat(req->cmd_template, " ", sizeof req->cmd_template);
                strlcat(req->cmd_template, str, sizeof req->cmd_template);
            }
        }
        else
        {
            BOOL quote = FALSE;
            str = get_gui_string(&req->cargs[i]);
            if (str[0] != '\0')
            {
                // do we have a space character in the string?
                // if yes: quote it
                if (strchr(str, ' ') && str[0] != '\"')
                {
                    quote = TRUE;
                }                    
                strlcat(req->cmd_template, " ", sizeof req->cmd_template);
                strlcat(req->cmd_template, req->cargs[i].argname, sizeof req->cmd_template);
                strlcat(req->cmd_template, " ", sizeof req->cmd_template);
                if (quote)
                {
                    strlcat(req->cmd_template, "\"", sizeof req->cmd_template);
                }
                strlcat(req->cmd_template, str, sizeof req->cmd_template);
                if (quote)
                {
                    strlcat(req->cmd_template, "\"", sizeof req->cmd_template);
                }
            }
        }
    }

    D(bug("[R] executing command %s\n", req->cmd_template));
    LONG result = System
    (
        req->cmd_template,
        NULL
    );
    if (result)
    {
        Printf("\"%s\" failed, return code %ld\n", req->filename, result);
    }
}


int main(int argc, char **argv)
{
    struct Req *req = alloc_req();
    if (req == NULL)
        clean_exit(req, "Can't allocate struct Req\n");

    if (! handle_args(req, argc, argv))
        clean_exit(req, "Failed to parse arguments\n");

    if (! check_exist(req))
        clean_exit(req, "Command not found\n");

    if (! get_template(req))
        clean_exit(req, "Failed to get template\n");

    if (! parse_template(req))
        clean_exit(req, "Failed to parse the template\n");

    if (! create_gui(req))
        clean_exit(req, "Failed to create application object\n");

    if (handle_gui())
    {
        execute_command(req);
    }

    clean_exit(req, NULL);

    return RETURN_OK;
}
