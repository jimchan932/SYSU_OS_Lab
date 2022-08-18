	org 0x7c00
	[bits 16]
	xor ax, ax		;eax = 0
	;;  initialized data segment
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov sp, 0x7c00
	mov ax, 0xb800
	mov gs, ax

        ;;  my screen is 79 x 22
	;;  therefore the calculator of position is
	;;  0xb800 + 2*(79*x + y)
	;;  we need to display school num
	;;  starting from (12, 12)
	;;  therefore calculator is 0xb800 + 2*(79*12 + 12 + y) = 

	;; no blinking blue background
	;; brown color text
	;; binary = 00011000 = 0x18
	mov ah, 0x18
	mov al, '2'
	mov [gs:1824 + 2*0], ax

	mov al, '0'
	mov [gs:1824 + 2*1], ax

	mov al, '3'
	mov [gs:1824 + 2*2], ax

	mov al, '3'
	mov [gs:1824 + 2*3], ax	

	mov al, '7'
	mov [gs:1824 + 2*4], ax	

	mov al, '0'
	mov [gs:1824 + 2*5], ax
	
	mov al, '2'
	mov [gs:1824 + 2*6], ax

	mov al, '1'
	mov [gs:1824 + 2*7], ax

	jmp $ 			;
	times 510  - ($ - $$) db 0
	db 0x55, 0xaa
