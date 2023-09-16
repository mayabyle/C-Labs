extern printf

segment .data
	x_struct: dd 5 			;ebp +8
	x_num: db 0xaa, 1,2,0x44,0x4f   ;rbp+12
	str:  db "%x", 10, 0		;db -> double byte

segment .text
	global main

main:	
	pushad				;to save all registers data before a call
 	push	x_struct
 	call	print_multy
 	add	esp,4
 	popad
 	ret

print_multy:
	pushad
	mov	eax,x_struct		;eax is holding address to beggining x_struct
	mov	ecx, [eax]		;ecx is holding x_struct
   	add  	eax, 3    
   	add  	eax, ecx		;get eax to point on the end of the array for LE

loop:
  	mov   	edx, 0
  	mov  	dl, [eax]
  	pushad   
  	push	edx
   	push 	dword str
   	call 	printf
   	add 	esp, 8
   	popad
   	dec  	eax
   	dec  	ecx
   	jnz   	loop
   	popad
   	ret






