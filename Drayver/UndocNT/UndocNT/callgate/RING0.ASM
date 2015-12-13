.386
.model small

.code

public _func
extrn _cfunc:near

include ..\include\undocnt.inc

_func proc
	Ring0Prolog
	call _cfunc
	Ring0Epilog
	retf
_func endp
END                 
