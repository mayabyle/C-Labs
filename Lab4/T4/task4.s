segment .data
str1: db "%x",10,0
        segment .text
        global  main
        global end_loop
        global loop
        global rand_num            ; let the linker know about main
        extern  printf          ; resolve printf from libc
        extern puts
        global rc
        state dd 0x1e4b
        mask dd 0x1e4b
main:
    push ebp
    mov ebp, esp
rand_num:
    mov edi, [state]
    mov eax , 20    ;loop counter
    mov ecx,0
loop:
    cmp eax, 0
    jz end_loop
    dec eax
    mov ecx,edi
    mov esi ,[mask]  ;esi is the mask  
    xor ecx , esi
    jpe rc
    stc 
rc:
    rcr edi ,1
    pushad         ;print 
    push dword ecx
    push dword str1
    call printf
    add esp, 8
    popad

    jmp loop
end_loop:       ;taken form stackoverflow
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret