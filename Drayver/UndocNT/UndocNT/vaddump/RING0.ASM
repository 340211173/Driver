.386
.model small

.code

public _DumpVad
extrn _CFuncDumpVad@4:near
extrn _PebOffset:near
extrn _VadRootOffset:near

include ..\include\undocnt.inc

_DumpVad proc
        Ring0Prolog        
        
        ;Gets the current thread 
        MOV     EAX,FS:[00000124h]

        ;Gets the current process
        ADD     EAX, DWORD PTR [_PebOffset]
        MOV     EAX,[EAX]

        ;Push Vad Tree root 
        ADD     EAX, DWORD PTR [_VadRootOffset]
        MOV     EAX, [EAX]
        PUSH    EAX
        call    _CFuncDumpVad@4

        Ring0Epilog
        retf
_DumpVad endp

END
