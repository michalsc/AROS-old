/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: Installer V43.3
    Lang: english
*/

#include "Installer.h"
#include "main.h"

static const char version[] = "$VER: Installer 43.3 (09.07.2000)\n";


/* External variables */
extern int line;

/* External function prototypes */
extern void parse_file( ScriptArg * );
extern void execute_script( ScriptArg * , int );
extern void cleanup();
extern void set_preset_variables( int );
extern void *get_variable( char *name );
extern long int get_var_int( char *name );
extern void set_variable( char *name, char *text, long int intval );
extern void end_malloc();
#ifdef DEBUG
extern void dump_varlist();
#endif /* DEBUG */
extern void show_parseerror( char *, int );
extern void final_report();
extern void init_gui();

/* Internal function prototypes */
int main( int, char ** );


struct IconBase *IconBase = NULL;

char *filename = NULL;
BPTR inputfile;
char buffer[MAXARGSIZE];
int error = 0, grace_exit = 0;

InstallerPrefs preferences;
ScriptArg script;

IPTR * args[TOTAL_ARGS] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
UBYTE **tooltypes;

/*
 * MAIN
 */
int main( int argc, char *argv[] )
{
struct RDArgs *rda;
char *ttemp, *tstring;

ScriptArg *currentarg, *dummy;
int nextarg, endoffile, count;

  if(argc!=0)
  { /* Invoked form Shell */
    preferences.fromcli = TRUE;
    /* evaluate args with RDArgs(); */
    rda = ReadArgs( ARG_TEMPLATE, (LONG *)args, NULL );
    if( rda == NULL )
    {
      PrintFault( IoErr(), INSTALLER_NAME );
      exit(-1);
    }
  }
  else
  { /* Invoked from Workbench */
    preferences.fromcli = FALSE;
    IconBase = (struct IconBase *)OpenLibrary( "icon.library", 0 );
    if( !IconBase )
    {
      fprintf( stderr, "Could not open icon.library!\n" );
      exit(-1);
    }
    tooltypes = ArgArrayInit( argc, (UBYTE **)argv );
  }

  /* open script file */
#ifdef DEBUG
  if( args[ARG_SCRIPT] )
  {
    printf( "Using script %s.\n", (STRPTR)args[ARG_SCRIPT] );
    filename = strdup( (STRPTR)args[ARG_SCRIPT] );
  }
  else
  {
    printf( "Using default script.\n" );
    filename = strdup( "SYS:Utilities/test.script" );
  }
#else /* DEBUG */
  if(argc)
  {
    filename = strdup( (STRPTR)args[ARG_SCRIPT] );
  }
  else
  {
    ttemp = ArgString( tooltypes, "SCRIPT", NULL );
    if(ttemp == NULL)
    {
#ifdef DEBUG
      fprintf( stderr, "No SCRIPT ToolType in Icon!\n" );
#endif /* DEBUG */
      ArgArrayDone();
      CloseLibrary( (struct Library *)IconBase);
      exit(-1);
    }
    filename = strdup( ttemp );
  }
#endif /* DEBUG */

  inputfile = Open( filename, MODE_OLDFILE );
  if( inputfile == NULL )
  {
#ifdef DEBUG
    PrintFault( IoErr(), INSTALLER_NAME );
    exit(-1);
#endif /* DEBUG */
  }

  preferences.welcome = FALSE;
  preferences.transcriptstream = NULL;
  preferences.pretend = 0;
  if(argc)
  {
  preferences.debug = TRUE;
    if( args[ARG_NOLOG] )
    {
      preferences.novicelog = FALSE;
    }
    else
    {
      preferences.novicelog = TRUE;
    }
    preferences.transcriptfile = strdup( ( args[ARG_LOGFILE] ) ? (char *)args[ARG_LOGFILE] : "install_log_file" );
    preferences.nopretend = (int)args[ARG_NOPRETEND];
    if( args[ARG_MINUSER] )
    {
      preferences.minusrlevel = _NOVICE;
      if( strcasecmp( "average", (char *)args[ARG_MINUSER] ) == 0 )
      {
	preferences.minusrlevel = _AVERAGE;
      }
      else if( strcasecmp( "expert", (char *)args[ARG_MINUSER] ) == 0 )
      {
	preferences.minusrlevel = _EXPERT;
      }
      else
      {
	preferences.minusrlevel = _NOVICE;
      }
    }
    else
    {
      preferences.minusrlevel = _NOVICE;
    }
    if( args[ARG_DEFUSER] )
    {
      preferences.defusrlevel = preferences.minusrlevel;
      if( strcasecmp( "average", (char *)args[ARG_DEFUSER] ) == 0 )
      {
	preferences.defusrlevel = _AVERAGE;
      }
      else if( strcasecmp( "expert", (char *)args[ARG_DEFUSER] ) == 0 )
      {
	preferences.defusrlevel = _EXPERT;
      }
      else
      {
	preferences.defusrlevel = _NOVICE;
      }
    }
    else
    {
      preferences.defusrlevel = _NOVICE;
    }
    if( preferences.defusrlevel < preferences.minusrlevel )
    {
      preferences.defusrlevel = preferences.minusrlevel;
    }
  }
  else
  {
  preferences.debug = FALSE;

    /* Create a log file in Novice mode? (TRUE) */
    if( strcmp( "TRUE", ArgString(tooltypes, "LOG", "TRUE") ) == 0 )
    {
      preferences.novicelog = TRUE;
    }
    else
    {
      preferences.novicelog = FALSE;
    }

    /* Is PRETEND possible? */
    if( strcmp( "TRUE", ArgString(tooltypes, "PRETEND", "TRUE") ) == 0 )
    {
      preferences.nopretend = FALSE;
    }
    else
    {
      preferences.nopretend = TRUE;
    }
    preferences.transcriptfile = strdup( ArgString( tooltypes, "LOGFILE", "install_log_file" ) );
    ttemp = ArgString( tooltypes, "DEFUSER", "NOVICE" );
    tstring = NULL;
    preferences.minusrlevel = _NOVICE;
    if( strcasecmp( "average", ttemp ) == 0 )
    {
      preferences.minusrlevel = _AVERAGE;
      tstring = strdup( "AVERAGE" );
    }
    else if( strcasecmp( "expert", ttemp ) == 0 )
    {
      preferences.minusrlevel = _EXPERT;
      tstring = strdup( "EXPERT" );
    }
    if( tstring == NULL )
    {
      tstring = strdup( "NOVICE" );
    }

    ttemp = ArgString( tooltypes, "DEFUSER", tstring );
    preferences.defusrlevel = preferences.minusrlevel;
    if( strcasecmp( "average", ttemp ) == 0 )
    {
      preferences.defusrlevel = _AVERAGE;
    }
    else if( strcasecmp( "expert", ttemp ) == 0 )
    {
      preferences.defusrlevel = _EXPERT;
    }
    if( preferences.defusrlevel < preferences.minusrlevel )
    {
      preferences.defusrlevel = preferences.minusrlevel;
    }
    free( tstring );
  }

  preferences.copyfail = COPY_FAIL;
  preferences.copyflags = 0;

  preferences.onerror.cmd = NULL;
  preferences.onerror.next = NULL;
  preferences.onerror.parent = NULL;
  preferences.onerrorparent = NULL;
  for( count = 0 ; count < NUMERRORS ; count++ )
  {
    dummy = &(preferences.trap[count]);
    dummy->cmd = NULL;
    dummy->next = NULL;
    dummy->parent = NULL;
    preferences.trapparent[count] = NULL;
  }

#warning FIXME: distinguish between cli/workbench invocation

  /* Init GUI -- i.e open empty window */
  init_gui();

  line = 1;

  endoffile = FALSE;
  script.arg = NULL;
  script.cmd = NULL;
  script.next = NULL;
  script.parent = NULL;
  script.intval = 0;
  script.ignore = 0;
  currentarg = script.cmd;
  /* parse script file */
  do
  {
    /* Allocate space for script cmd and save first one to scriptroot */
    if( script.cmd == NULL )
    {
      script.cmd = (ScriptArg *)malloc( sizeof(ScriptArg) );
      if( script.cmd == NULL )
      {
	end_malloc();
      }
      currentarg = script.cmd;
      currentarg->parent = &script;
    }
    else
    {
      currentarg->next = (ScriptArg *)malloc( sizeof(ScriptArg) );
      if( currentarg->next == NULL )
      {
	end_malloc();
      }
      currentarg->next->parent = currentarg->parent;
      currentarg = currentarg->next;
    }
    /* Set initial values */
    currentarg->arg = NULL;
    currentarg->cmd = NULL;
    currentarg->next = NULL;
    currentarg->intval = 0;
    currentarg->ignore = 0;

    nextarg = FALSE;
    do
    {
      count = Read( inputfile, &buffer[0], 1 );
      if( count == 0 )
	endoffile = TRUE;

      if( !isspace( buffer[0] ) && !endoffile )
      {
	/* This is text, is it valid ? */
	switch( buffer[0] )
	{
	  case SEMICOLON  : /* A comment, ok - Go on with next line */
			    do
			    {
			      count = Read( inputfile, &buffer[0], 1 );
			    } while( buffer[0] != LINEFEED && count != 0 );
			    line++;
			    if( count == 0 )
			      endoffile = TRUE;
			    break;

	  case LBRACK	  : /* A command (...) , parse the content of braces */
			    currentarg->cmd = (ScriptArg *)malloc( sizeof(ScriptArg) );
			    if( currentarg->cmd == NULL )
			    {
			      end_malloc();
			    }
			    dummy = currentarg->cmd;
			    dummy->parent = currentarg;
			    dummy->arg = NULL;
			    dummy->ignore = 0;
			    dummy->intval = 0;
			    dummy->cmd = NULL;
			    dummy->next = NULL;
			    parse_file( currentarg->cmd );
			    nextarg = TRUE;
			    break;

	  default	  : /* Plain text or closing bracket is not allowed */
			    Close( inputfile );
			    show_parseerror( "Too many closing brackets!", line );
			    cleanup();
			    exit(-1);
			    break;
	}
      }
      else
      {
	if( buffer[0] == LINEFEED )
	{
	  line++;
	}
      }
    } while( nextarg != TRUE && !endoffile );
  } while( !endoffile );

  /* Okay, we (again) have allocated one ScriptArg too much, so get rid of it */
  currentarg = script.cmd;
  if( currentarg->next != NULL )
  {
    while( currentarg->next->next != NULL )
    {
      currentarg = currentarg->next;
    }
    free( currentarg->next );
    currentarg->next = NULL;
  }

  free( filename );
  Close( inputfile );

  if( preferences.transcriptfile != NULL )
  {
    /* open transcript file */
    preferences.transcriptstream = Open( preferences.transcriptfile, MODE_NEWFILE );
    if( preferences.transcriptstream == NULL )
    {
      PrintFault( IoErr(), INSTALLER_NAME );
      cleanup();
      exit(-1);
    }
  }

  /* Set variables which are not constant */
  set_preset_variables( argc );

  /* NOTE: Now everything from commandline(ReadArgs)/ToolTypes(Workbench)
	   will become invalid!
  */
  if( argc!=0 )
  { /* Finally free ReadArgs struct (set_preset_variables() needed them) */
    FreeArgs(rda);
  }
  else
  { /* Or free tooltypes array if started from WB */
    ArgArrayDone();
  }

  if( get_var_int( "@user-level" ) == _NOVICE )
  {
    preferences.copyflags &= ~(COPY_ASKUSER & preferences.copyflags);
  }
  else
  {
    preferences.copyflags |= COPY_ASKUSER;
  }

  /* execute parsed script */
  execute_script( script.cmd, 0 );

#ifdef DEBUG
  dump_varlist();
#endif /* DEBUG */

  final_report();
  cleanup();

return error;
}

