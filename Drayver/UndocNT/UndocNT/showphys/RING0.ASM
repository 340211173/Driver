.386
.model small

.code

public __GetPhysicalAddressAndPageAttributes
extrn _CFuncGetPhysicalAddressAndPageAttributes@12:near

__GetPhysicalAddressAndPageAttributes proc
	push ebp
	mov ebp, esp

	push [ebp+14h]
	push [ebp+10h]
	push [ebp+0Ch]

	call _CFuncGetPhysicalAddressAndPageAttributes@12
	pop ebp
	retf 0Ch
__GetPhysicalAddressAndPageAttributes endp

END
