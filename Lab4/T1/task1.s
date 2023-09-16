extern  printf          ;resolve printf from libc
extern puts

segment .data
	format1  db     "%s",10,0
	format2  db     "args: %d",10,0

segment .text
	global  main            ;let the linker know about main

main:
	push    ebp             ; prepare stack frame for main
	mov     ebp, esp
	mov     edi, dword[ebp+8]    ;get argc into edi
	push    dword edi
	push    format2
	call    printf
	mov     esi, dword[ebp+12]   ; get first argv string into esi

start_loop:
	push    dword [esi]     ; must dereference esi; points to argv
	push    format1
	call    printf
	add     esi, 4          ; advance to the next pointer in argv
	dec     edi             ; decrement edi from argc to 0
	cmp     edi, 0          ; when it hits 0, we're done
	jnz     start_loop      ; end with NULL pointer

end_loop:
	leave
	ret
