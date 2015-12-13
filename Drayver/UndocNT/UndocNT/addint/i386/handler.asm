.386
.model small

.code

public _InterruptHandler
extrn _CFunc:near

include ..\include\undocnt.inc

_InterruptHandler proc
	Ring0Prolog
	mov     edi, edx
	test    edi, edi
	jz      NullPointer

	lea esi, message
	mov ecx, messagelen
	repz movsb

NullPointer:


	call _CFunc

	Ring0Epilog
	iretd

	message db "Newly added interrupt called.", 0
	messagelen dd $-message

_InterruptHandler endp

End

