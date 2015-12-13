Ring0Prolog macro
	PUSHAD
	PUSHFD
	PUSH FS

	MOV     EBX,00000030h
	MOV     FS,BX
	SUB     ESP, 50h        
	MOV     EBP,ESP

	;Setup the exception frame to NULL
	MOV     EBX,DWORD PTR CS:[0FFDFF000h]
	MOV     DWORD PTR DS:[0FFDFF000h], 0FFFFFFFFh
	MOV     DWORD PTR [EBP],EBX

	;Save away the existing KSS EBP
	MOV     ESI, DWORD PTR CS:[0FFDFF124h]
	MOV     EBX,DWORD PTR [ESI+00000128h]
	MOV     DWORD PTR [EBP+4h],EBX
	MOV     DWORD PTR [ESI+00000128h],EBP

	;Save away the kernel time and the thread mode (kernel/user)
	MOV     EDI,DWORD PTR [ESI+00000137h]
	MOV     DWORD PTR [EBP+8h],EDI

	;Set the thread mode (kernel/user) based on the code selector
	MOV     EBX,DWORD PTR [EBP+7Ch]
	AND     EBX,01
	MOV     BYTE PTR [ESI+00000137h],BL

	STI
endm

Ring0Epilog macro
	;Restore the KSS EBP
	MOV     ESI,DWORD PTR CS:[0FFDFF124h]
	MOV     EBX,DWORD PTR [EBP+4]
	MOV     DWORD PTR [ESI+00000128h],EBX

	;Restore the exception frame 
	MOV     EBX,DWORD PTR [EBP]
	MOV     DWORD PTR FS:[00000000],EBX

	;Restore the thread mode
	MOV     EBX,DWORD PTR [EBP+8h]
	MOV     ESI,DWORD PTR FS:[00000124h]
	MOV     BYTE PTR [ESI+00000137h],BL
	ADD     ESP, 50h
	POP     FS
	POPFD
	POPAD
endm
