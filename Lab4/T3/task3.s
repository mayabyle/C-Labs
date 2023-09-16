extern malloc
extern free
extern printf

segment .data
	x_struct: dd 4
		x_num: db 0xff, 0xff, 0xff, 0xfe
	y_struct: dd 3
		y_num: db 1,1,1
	big_size: 	dd  0 
	small_size:	dd  0 
	carry:		db  0 
	temp:		dd  0
	len_form:	db      "length is:  %d",10,0    
	sum_form:	db      "%x",10,0   

segment .text
	global main

main:
	push    ebp
	mov     ebp, esp

max_length:	
	mov	eax, dword [x_struct]
	mov	ebx, dword [y_struct]
	cmp	eax, ebx
	cmovl	eax, ebx
	mov	[big_size], eax
	mov	[small_size], ebx
	pushad
	push	eax
	push	dword len_form
	call	printf
	add	esp,8
	pop	ebp
	popad

add_multiy:
	pushad
	mov	edi, dword[big_size]			; get edi register to hold big_size
	add	edi,4					; multiply by 4 to get a byte count
	push 	edi
	call	malloc
	add		esp, 4
	popad
	mov	esi, dword[small_size]			; get esi register to hold small_size
	mov	ebp,0
	mov	edx, x_struct
	mov	ecx, y_struct
	add	edx, 4
	add	ecx, 4
	jmp	end_loop

loop:
	mov	al, [edx]				; byte i of x array
	mov	ah, [ecx]				; byte i of y array
	mov	bl, [carry]
	add	bl, al
	add	bl, ah
	mov	bh, 0					; restart carry - get carry to hold 0
	mov	[carry], bh

	cmp	bl, 0xff				; if the values sum is 16 -> get carry to hold 1
	setc	[carry]

	add	edx, 1
	add	ecx, 1
	mov	[temp], bl 
	pushad
	push	dword[temp]
	push	sum_form
	call	printf
   	add 	esp, 8
	popad
	add	ebp,1					; add 1 to index counter

end_loop:						; loop on big array only
	cmp	ebp,esi
	jl	loop
	add	edi, 1
	mov	al, [edx]
	add	al,[carry]
	mov	bh, 0
	mov	[carry], bh
	mov	[temp],al
	pushad 
	push	dword[temp]
	push	sum_form
	call	printf
   	add 	esp, 8
	popad
	ret

