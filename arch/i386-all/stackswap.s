#    (C) 1995-96 AROS - The Amiga Replacement OS
#    $Id$
#    $Log$
#    Revision 1.7  1996/10/24 15:51:12  aros
#    Use the official AROS macros over the __AROS versions.
#
#    Revision 1.6  1996/10/23 08:04:26  aros
#    Use generated offsets which makes porting much easier
#
#    Revision 1.5  1996/09/11 16:54:29	digulla
#    Always use __AROS_SLIB_ENTRY() to access shared external symbols, because
#	some systems name an external symbol "x" as "_x" and others as "x".
#	(The problem arises with assembler symbols which might differ)
#
#    Revision 1.4  1996/08/23 16:49:22	digulla
#    With some systems, .align 16 aligns to 64K instead of 16bytes. Therefore
#	I replaced it with .balign which does what we want.
#
#    Revision 1.3  1996/08/13 14:03:20	digulla
#    Added standard headers
#
#    Revision 1.2  1996/08/01 17:41:20	digulla
#    Added standard header for all files
#
#    Desc:
#    Lang:

#*****************************************************************************
#
#   NAME
#	AROS_LH0(void, StackSwap,
#
#   LOCATION
#	struct ExecBase *, SysBase, 122, Exec)
#
#   FUNCTION
#
#   INPUTS
#
#   RESULT
#
#   NOTES
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
	.globl	_Exec_StackSwap
	.type	_Exec_StackSwap,@function
_Exec_StackSwap:
	movl 4(%esp),%edx
	movl 8(%edx),%ecx
	popl %eax
	movl %eax,-12(%ecx)
	popl %eax
	movl %eax,-8(%ecx)
	movl (%esp),%eax
	movl %eax,-4(%ecx)
	addl $-12,%ecx
	leal Disable(%eax),%eax
	call *%eax
	popl %eax
	movl %esp,8(%edx)
	movl %ecx,%esp
	movl ThisTask(%eax),%ecx
	leal tc_SPLower(%ecx),%ecx
	push %ebx
	movl (%edx),%eax
	movl (%ecx),%ebx
	movl %eax,(%ecx)
	movl %ebx,(%edx)
	movl 4(%edx),%eax
	movl 4(%ecx),%ebx
	movl %eax,4(%ecx)
	movl %ebx,4(%edx)
	popl %ebx
	movl 8(%esp),%eax
	pushl %eax
	leal Enable(%eax),%eax
	call *%eax
	addl $4,%esp
	ret
