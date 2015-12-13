.386
.model small

.code

include ..\include\undocnt.inc
public _NewHandler
extrn _OldHandler:near
extrn _NewHandlerCFunc@4:near

_NewHandler proc near
        Ring0Prolog
        STI
        push eax
        call _NewHandlerCFunc@4
        CLI
        Ring0Epilog
        jmp dword ptr cs:[_OldHandler]
_NewHandler endp
END
