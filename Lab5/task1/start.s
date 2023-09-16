global _start
global system_call

section .rodata
	lineDown: db "", 10,0

section .data
	ePoint1: dd 0
	ePoint2: dd 0
	input: dd 0					; defult -> read from user
	output: dd 1				; defult -> STDOUT

section .bss
	info: resb 26				; a buffer to save the data

section .text
	extern main


_start:
	pop		dword ecx    		; ecx <- argc
	mov		esi, esp      		; esi <- argv
	mov		edx, 1
	cmp		ecx, edx
	je		readWordLoop				; if there is no argumet (e/o/i) is supplied
	
parseArguments:
	mov		edi, [esi + 4*edx]		; iterate over the arguments
	cmp		word [edi], word "+e"
	je		eFlagHandle
	cmp		word [edi], word "-o"
	je		oFlagHandle
	cmp		word [edi], word "-i"
	je		iFlagHandle


; flags handling:

eFlagHandle:
	add		edi, 2				;move to the first letter of the key
	cmp		byte[edi], 0
	je		endProgram			; jump if equal
	mov		[ePoint1], edi		; save pointer1 to argv
	mov		[ePoint2], edi		; save pointer2 to argv - will iterate over the key
	jmp		iterateArgs

iFlagHandle:
	pushad
	mov		eax, 5				; system call -> Sys_open
	mov		ebx, edi
	add		ebx, 2				; ebx is pointing to the start of the file name
	mov		ecx, 66			; read and write mode
	mov		edx, 0777			; read, write and execute by all
	int 	0x80
	mov		[input], eax		; input <- input file
	popad
	jmp		iterateArgs

oFlagHandle:
	pushad
	mov		eax, 5				; system call -> Sys_open
	mov 	ebx, edi
	add 	ebx, 2				; ebx is pointing to the start of the file name
	mov 	ecx, 2				; read access mode
	mov 	edx, 0777			; read, write and execute by all
	int 	0x80
	mov 	[output], eax		; output <- output file
	popad
	jmp 	iterateArgs


iterateArgs:
	inc		edx
	cmp		edx, ecx			; compare counter (edx) to argc
	je		readWordLoop		; jump if equal
	jmp		parseArguments

	
; parseWord:
	
readWordLoop:
	mov 	eax, 3						; system call -> sys_read
   	mov 	ebx, [input]
   	mov		ecx, info
   	mov		edx, 1						; 1 byte of the information
   	int		0x80

	cmp		eax, 1						; returns the number of bytes read to EAX, in case of error, the error code is in the EAX register.
	jl		endProgram					; Jump to end if there wasnt byte to read 
	cmp		dword [ePoint1], 0 			
	je		writeChar					; jump is there is no {+e} key
	mov		edx, dword [ePoint2]
	cmp		byte[edx], 0 				; if we reached the end of the key
	jne		encrypt
	
	mov		edx, [ePoint1]				; reset key pointer
	mov		[ePoint2], edx	

encrypt:
	mov		dl, byte[edx] 				; get current key
	sub		dl, byte '0'
	cmp		byte[info], byte 10 		; check if the char is '\n'
	je		printNewLine
	add		dword [ePoint2], 1 			; move pointer2 to next char
	add		byte[info], dl
	
writeChar:
	pushad
   	mov		eax, 4				; system call -> sys_write
	mov		edx, 1				; number of bytes
   	mov		ecx, info			; message to write
   	mov		ebx, [output]
   	int		0x80
	popad
	jmp		readWordLoop

endProgram:
	mov		eax, 6				; system call -> close file
  	mov		ebx, [input]		; close input file
	int		0x80

   	mov		eax, 6				; system call -> close file
  	mov		ebx, [output]		; close output file
	int		0x80

	mov     eax, 1				; system call -> exit
	int     0x80
	nop

printNewLine:
	pushad
   	mov		eax, 4				; system call -> sys_write
	mov		edx, 1				; number of bytes
   	mov		ecx,  lineDown		; message to write
   	mov		ebx, [output]
   	int		0x80
	popad
	jmp		readWordLoop


system_call:
	push    ebp             ; Save caller state
	mov     ebp, esp
	sub     esp, 4          ; Leave space for local var on stack
	pushad                  ; Save some more caller state

	mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
	mov     ebx, [ebp+12]   ; Next argument...
	mov     ecx, [ebp+16]   ; Next argument...
	mov     edx, [ebp+20]   ; Next argument...
	int     0x80            ; Transfer control to operating system
	mov     [ebp-4], eax    ; Save returned value...
	popad                   ; Restore caller state (registers)
	mov     eax, [ebp-4]    ; place returned value where caller can see it
	add     esp, 4          ; Restore caller state
	pop     ebp             ; Restore caller state
	ret                     ; Back to caller
