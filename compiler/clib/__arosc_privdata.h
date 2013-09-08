#ifndef ___AROSC_PRIVDATA_H
#define ___AROSC_PRIVDATA_H

#include <exec/semaphores.h>
#include <exec/libraries.h>
#include <devices/timer.h>
#include <proto/timer.h>
#include <dos/dos.h>
#include <aros/cpu.h>

#include <sys/types.h>
#include <sys/arosc.h>

#include <aros/types/clock_t.h>
/* Some structs that are defined privately */
struct _fdesc;
struct __env_item;
struct vfork_data;
struct random_state;

struct StdCBase;
struct StdCIOBase;
struct PosixCBase;

struct aroscbase
{
    struct Library acb_library;
    
    struct arosc_userdata acb_acud;
    
    /* arosc_userdata can grow as much as it wishes,
       as long as all new fields are added at the end of it.

       arosc_privdata can also grow at please, but it has no restrictions
       on the way things are modified in it, as long as arosc_userdata
       is always kept at its beginning.  */

    /* common */
    int acb_flags;
    APTR acb_internalpool;

    /* malloc.c */
    APTR acb_mempool;

    /* clock.c */
    clock_t acb_starttime;

    /* __arosc_usedata  */
    APTR  acb_process_returnaddr;
    ULONG acb_usercount;

    /* __upath */
    char *acb_apathbuf;  /* Buffer that holds the AROS path converted from the
                             equivalent *nix path.  */
    int   acb_doupath;   /* BOOL - does the conversion need to be done?  */

    /* spawn* */
    char *acb_joined_args;

    /* flock.c */
    struct List *acb_file_locks;
    
    /* rand */
    unsigned int acb_srand_seed;

    /* Used for random()/srandom() */
    struct random_state *acb_random;

    /* temp bases until we are gone */
    struct StdCBase *StdCBase;
    struct StdCIOBase *StdCIOBase;
    struct PosixCBase *PosixCBase;
};

/* acb_flags */

/* By default arosc.library creates new arosc_privdata when opened if 
   pr_ReturnAddr has changed (for example during RunCommand()). Setting 
   this flag prevents creation of new arosc_privdata. */
#define KEEP_OLD_ACPD 0x00000008

/* By default a new process will get new ACPD when it(or any other library
   it uses) opens arosc.library. This flag prohibits that and forces the
   child process to share ACPD with parent process */
/* FIXME: SHARE_ACPD_WITH_CHILD not implemented, is it still needed ?
   aroscbase can now be used from different tasks without the need
   for SHARE_ACPD_WITH_CHILD
*/
#define SHARE_ACPD_WITH_CHILD 0x00000010

/* !acb_flags */

/* This function is added by genmodule and returns the current arosc
   libbase.
*/
struct aroscbase *__aros_getbase_aroscbase(void);

#endif /* !___AROSC_PRIVDATA_H */
