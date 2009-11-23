/* set.s
set the stack pointer to the address that was passed in
*/
/*	This code was derived from a threads package written by:
 *
 *		Geoff Kuenning		geoff@ITcorp.com
 *  
 *  and obtained from ftp site:
 *		ftp.inria.fr 
 *		prog/threads/thread.shar.Z
 *
 *  The following is condensed from the original code.  
 *
 */
	.seg	"text"
	.proc	4
	.global	_set_sp
_set_sp:
	sub	%o0,0x60,%o0		! Make room for frame-clobbering
	mov	%o0,%sp				! Move first argument to stack pointer
	retl					! Return to caller
	nop						! ..delayed-branch slot will do nothing

