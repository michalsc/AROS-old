/*
    Copyright � 1995-2003, The AROS Development Team. All rights reserved.
    $Id$
    
    Main for genlibdefs. A tool to generate files for building modules.
*/
#include "genmodule.h"

int main(int argc, char **argv)
{
    char *s;
    
    if (argc!=5)
    {
	fprintf(stderr, "Usage: %s modname modtype conffile gendir\n", argv[0]);
	exit(20);
    }

    modulename = argv[1];
    modulenameupper = strdup(modulename);
    for (s=modulenameupper; *s!='\0'; *s = toupper(*s), s++)
	;
    
    if (strcmp(argv[2],"library")==0)
    	modtype = LIBRARY;
    else if (strcmp(argv[2],"mcc")==0)
    	modtype = MCC;
    else if (strcmp(argv[2],"mui")==0)
    	modtype = MUI;
    else if (strcmp(argv[2],"mcp")==0)
    	modtype = MCP;
    else
    {
	fprintf(stderr, "Unknown modtype \"%s\" speficied for second argument\n", argv[2]);
	exit(20);
    }

    if (modtype == LIBRARY)
        firstlvo = 5;
    else if (modtype == MCC || modtype == MUI || modtype == MCP)
        firstlvo = 6;
    
    conffile = argv[3];

    if (strlen(argv[4])>200)
    {
	fprintf(stderr, "Ridiculously long path for gendir\n");
	exit(20);
    }
    if (argv[4][strlen(argv[4])-1]=='/') argv[2][strlen(argv[2])-1]='\0';
    gendir = argv[4];

    readconfig();
    writeinclibdefs();
    
    return 0;
}
