
	SECTION .data
	schlnum  db "20337021", 0Ah
	SECTION .text
	global _start	
	_start:	
	org 0x7c00
	[bits 16]
	xor ax, ax
	;;  initialized data segment
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov sp, 0x7c00
	mov ax, 0xb800
	mov gs, ax

	
	;; set position
	mov ah, 0x2
	mov bh, 0x0
	mov dh, 0x7
	mov dl, 0x8
	int 10h

	mov al, '2'
	call print_char
	call increment_cursor
	mov al, '0'
	call print_char
	call increment_cursor
	mov al, '3'
	call print_char
	call increment_cursor	
	mov al, '3'
	call print_char
	call increment_cursor	
	mov al, '7'
	call print_char
	call increment_cursor	
	mov al, '0'
	call print_char
	call increment_cursor	
	mov al, '2'
	call print_char
	call increment_cursor	
	mov al, '1'
	call print_char		
print_char:
	push ebp
	mov ebp, esp
	;; .outerLoop
	mov ah, 0x9
	mov bh, 0x0
	mov bl, 0x10
	mov cx, 0x1
	int 10h


	mov  esp, ebp
	pop ebp
	ret 	
increment_cursor:
	push ebp
	mov ebp, esp	
	;; retrieve the x and y of cursor into dh and dl
	mov ah, 0x3
	mov bx, 0x0
	int 10h
	;; increment cursor
	mov ah, 0x2
	mov bh, 0x0
	mov dh, 0x7
	add dl, 0x1
	int 10h	

	mov  esp, ebp
	pop ebp
	ret 		

	jmp $ 	 		;
	times 510  - ($ - $$) db 0
	db 0x55, 0xaa
