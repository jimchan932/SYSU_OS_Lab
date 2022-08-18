
	; atomic exchange through assembly code 
asm_atomic_exchange:
    push ebp
    mov ebp, esp
    pushad

    mov ebx, [ebp + 4 * 2] ; register
    mov eax, [ebx]         ; 
    mov ebx, [ebp + 4 * 3] ; memory
    xchg [ebx], eax        ;
    mov ebx, [ebp + 4 * 2] ; memory
    mov [ebx], eax         ; 

    popad
    pop ebp
    ret

	; use of bts instruction to implement atomic operation
asm_atomic_set_lock:
	push ebp
	mov ebp, esp
	pushad

	mov ebx, [ebp+4*2]		
	mov eax, [ebx]
	bts eax, 0       ; 使用bts 把锁设定为1
	mov [ebx], eax
	popad
	pop ebp
	ret
