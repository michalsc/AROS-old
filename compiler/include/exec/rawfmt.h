#ifndef EXEC_RAWFMT_H
#define EXEC_RAWFMT_H

/* Magic constants for RawDoFmt() anv VNewRawDoFmt() to be given as
   PutChProc */

#define RAWFMTFUNC_STRING 0 /* Output to string given in PutChData	     */
#define RAWFMTFUNC_SERIAL 1 /* Output to debug log (usually serial port)     */
#define RAWFMTFUNC_COUNT  2 /* Just count characters, PutChData is a pointer
			       to the counter (ULONG *)			     */

#endif
