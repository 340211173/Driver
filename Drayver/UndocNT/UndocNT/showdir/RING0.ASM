.386
.model small

.code

public _GetPageDirectory
extrn _CFuncGetPageDirectory@0:near

include ..\include\undocnt.inc

;Assembly stub called from callgate
_GetPageDirectory proc
        Ring0Prolog        
        call    _CFuncGetPageDirectory@0
        Ring0Epilog        
        retf
_GetPageDirectory endp

END
