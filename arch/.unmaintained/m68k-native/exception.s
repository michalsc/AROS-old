#    (C) 1995-96 AROS - The Amiga Replacement OS
#    $Id$
#    $Log$
#    Revision 1.5  1996/10/24 15:51:30  aros
#    Use the official AROS macros over the __AROS versions.
#
#    Revision 1.4  1996/10/24 01:38:31  aros
#    Include machine.i
#
#    Revision 1.3  1996/10/21 21:08:57  aros
#    Changed AROS_LA to AROS_LHA
#
#    Revision 1.2  1996/08/01 17:41:35  digulla
#    Added standard header for all files
#
#    Desc:
#    Lang:

#*****************************************************************************
#
#   NAME
#	AROS_LH0(void, Exception,
#
#   LOCATION
#	struct ExecBase *, SysBase, 8, Exec)
#
#   FUNCTION
#	Exception handler. This function is called by the dispatcher if a
#	exception has to be delivered. It is called in Disable()d state
#	(atomically) so that all Signals are still unchanged.
#	TF_EXCEPT is still set and must be reset by this routine.
#
#   INPUTS
#
#   RESULT
#
#   NOTES
#	This function has a context on its own and doesn't need to preserve
#	any registers.
#
#	Internal exec function.
#
#   EXAMPLE
#
#   BUGS
#
#   SEE ALSO
#
#   INTERNALS
#
#   HISTORY
#
#******************************************************************************

	.include "machine.i"

	.text
	.balign 16
	.globl	_Exec_Exception
	.type	_Exec_Exception,@function

_Exec_Exception:
	# First clear task exception bit.
	movel	a6@(ThisTask),a2
	bclr	#TB_EXCEPT,a2@(tc_Flags)

	# If the exception is raised out of a Wait()
	# IDNestCnt may be almost everything.
	# Store nesting level and set it to a
	# defined value 1 beyond -1.
excusr: moveb	a6@(IDNestCnt),d2
	clrb	a6@(IDNestCnt)

exloop: # get signals causing the exception
	# (do nothing if there are none)
	movel	a2@(tc_SigExcept),d0
	andl	a2@(tc_SigRecvd),d0
	jeq	excend

	# disable the signals
	eorl	d0,a2@(tc_SigExcept)
	eorl	d0,a2@(tc_SigRecvd)

	# call the exception vector with interrupts enabled
	movel	a2@(tc_ExceptData),a1
	movel	a2@(tc_ExceptCode),a0
	jsr	a6@(Enable)
	jsr	a0@
	jsr	a6@(Disable)

	# reenable signals and look again
	orl	d0,a2@(tc_SigExcept)
	jra	exloop

	# restore state of Disable() and return
excend: moveb	d2,a6@(IDNestCnt)
	rts

