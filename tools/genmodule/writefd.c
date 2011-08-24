/*
    Copyright � 1995-2011, The AROS Development Team. All rights reserved.
    $Id$
    
    Write the functionlist to a FD file for identify.library.
*/

#include "genmodule.h"

static void write_fd_func(FILE *out, struct functionhead *funclistit, unsigned int lvo)
{
    struct functionarg *arglistit;
    char *variable;

    if (funclistit->lvo > lvo + 1)
    {
        if (funclistit->lvo == lvo + 2)
            fprintf(out, "private()()\n");
        else
            fprintf(out, "##bias %u\n", (funclistit->lvo - 1) * 6);
    }

    fprintf(out, "%s(", funclistit->name);

    for (arglistit = funclistit->arguments;
         arglistit!=NULL;
         arglistit = arglistit->next)
    {
        /* Print a , separator when not the first function argument */
        if (arglistit != funclistit->arguments)
            fprintf(out, ",");

        /* Print only variable name */
        variable = arglistit->arg + strlen(arglistit->arg) - 1;
        while ((variable >= arglistit->arg) &&
               (isalnum(*variable) || (*variable == '_')))
        {
            variable--;
        }
        fprintf(out, "%s", variable + 1);
    }
    fprintf(out, ")(");
}

void writefd(struct config *cfg)
{
    FILE *out;
    char line[256];
    struct functionhead *funclistit;
    struct functionarg *arglistit;
    unsigned int lvo;
    char *lower;

    if (!cfg->funclist)
    	return;

    snprintf(line, 255, "%s/%s_lib.fd", cfg->gendir, cfg->modulename);

    out = fopen(line, "w");

    if (out == NULL)
    {
        perror(line);
        exit(20);
    }
    else
    {
        fprintf(out, "##base _%s\n", cfg->libbase);
        
        if (cfg->modtype == DEVICE)
        {
            /* because of BeginIO/EndIO */
            /* FIXME: do we need special handling for other module types, too? */
            fprintf(out, "##bias %u\n", (cfg->firstlvo - 2) * 6);
        }
        else
        {
            fprintf(out, "##bias %u\n", cfg->firstlvo * 6);
        }

        fprintf(out, "*\n"
                     "* Automatically generated from '%s'.\n"
                     "* Edits will be lost. This file is supposed to be a support file for identify.library.\n"
                     "*\n",
                     cfg->conffile
        );

        fprintf(out, "##public\n");
        
        for (funclistit = cfg->funclist, lvo = cfg->firstlvo - 1;
             funclistit != NULL;
             funclistit = funclistit->next
        )
        {
            switch (funclistit->libcall)
            {
            case  REGISTERMACRO:
            	write_fd_func(out, funclistit, lvo);

                for (arglistit = funclistit->arguments;
                     arglistit != NULL;
                     arglistit = arglistit->next
                )
                {
                    /* Print a , separator when not the first function argument */
                    if (arglistit != funclistit->arguments)
                        fprintf(out, ",");

                    /* Print register name in lower case */
                    for (lower = arglistit->reg; *lower != '\0'; lower++)
                    {
                        fputc(tolower(*lower), out);
                    }
                }
                fprintf(out, ")\n");

                lvo = funclistit->lvo;
            	break;
            
            case STACK:
            	write_fd_func(out, funclistit, lvo);
            	/* TODO: Currently all SYSV ABI vectors are base-less */
            	fprintf(out, "sysv)\n");
            	lvo = funclistit->lvo;
            	break;
            }
        }

        fprintf(out, "##end\n");

        if (ferror(out))
        {
            perror(line);
            exit(20);
        }

        fclose(out);
    }
}
