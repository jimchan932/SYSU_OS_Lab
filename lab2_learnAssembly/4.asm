org 0x7c00
[bits 16]
xor ax, ax ; eax = 0

mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
mov ax, 0xb800
mov gs, ax



mov esi, 0
mov edi, 600 ；打印的次数

mov dh, 2
mov dl, 0
mov ah, 02h
int 10h
mov al, '0'
mov bl, 0x63
mov cx, 1
mov ah, 09h
int 10h

mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dh
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code
mov ah, 02h
int 10h
mov al, '1'
mov bl, 0x36 
mov cx, 1
mov ah, 09h
int 10h
mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code
jmp right_up


right_down:
mov ah, 86h
int 15h

inc dh
inc dl
mov ah, 02h
int 10h
inc esi

inc al
cmp al, '9'
jg goto_rd
jle continue_rd
goto_rd:
mov al, '0'

continue_rd:
mov bl, 0x63
mov cx, 1
mov ah, 09h
int 10h

mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code
mov ah, 02h
int 10h
inc al
mov bl, 0x36 
mov cx, 1
mov ah, 09h
int 10h
mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code


cmp dh, 24
je right_up
cmp dl, 79
je left_down
cmp esi, edi
jge next

jmp right_down



right_up:
mov ah, 86h
int 15h
dec dh
inc dl
mov ah, 02h
int 10h
inc esi

inc al
cmp al, '9'
jg goto_ru
jle continue_ru
goto_ru:
mov al, '0'

continue_ru:
mov bl, 0x63
mov cx, 1
mov ah, 09h
int 10h

mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code
mov ah, 02h
int 10h
inc al
mov bl, 0x36 
mov cx, 1
mov ah, 09h
int 10h
mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code

cmp dh, 0
je right_down
cmp dl, 79
je left_up
cmp esi, edi
jge next

jmp right_up



left_down:
mov ah, 86h
int 15h
inc dh
dec dl
mov ah, 02h
int 10h
inc esi

inc al
cmp al, '9'
jg goto_ld
jle continue_ld
goto_ld:
mov al, '0'

continue_ld:
mov bl, 0x63
mov cx, 1
mov ah, 09h
int 10h

mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code
mov ah, 02h
int 10h
inc al
mov bl, 0x36 
mov cx, 1
mov ah, 09h
int 10h
mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code

cmp dh, 24
je left_up
cmp dl, 0
je right_down
cmp esi, edi
jge next

jmp left_down



left_up:
mov ah, 86h
int 15h
dec dh
dec dl
mov ah, 02h
int 10h
inc esi

inc al
cmp al, '9'
jg goto_lu
jle continue_lu
goto_lu:
mov al, '0'

continue_lu:
mov bl, 0x63
mov cx, 1
mov ah, 09h
int 10h

mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code
mov ah, 02h
int 10h
inc al
mov bl, 0x36 
mov cx, 1
mov ah, 09h
int 10h
mov bh, 24	;reverse dh
sub bh, dh
mov dh, bh
mov bh, 79	;reverse dl
sub bh, dl
mov dl, bh
mov bh, 0	;reset bh as page code

cmp dh, 0
je left_down
cmp dl, 0
je right_up
cmp esi, edi
jge next

jmp left_up





next:



jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa
