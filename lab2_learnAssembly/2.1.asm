
	SECTION .data
	msg  db "(x,y) = ", 0Ah
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
	
	mov ah, 0x2
	mov bh, 0x0
	mov dh, 0x7
	mov dl, 0x8
	int 10h

	
	mov ah, 0x3
	mov bx, 0x0
	int 10h
	
	mov ah, 0x18
	mov al, 'x'	
	mov [gs:0], al
	mov al, '='
	mov [gs:2], al
	mov al, dh
	add al, 0x30
	mov [gs:4], al
	
	mov al, ','
	mov [gs:6], al		
	mov al, 'y'	
	mov [gs:8], al
	mov al, '='
	mov [gs:10], al
	mov al, dl
	add al, 0x30
	mov [gs:12], al
	
	jmp $ 	 		;
	times 510  - ($ - $$) db 0
	db 0x55, 0xaa
