%include "head.include"
your_if:
	mov eax, [a1]
	cmp eax, 0xc
	jl ifBranch
	jmp elseIfBranch

ifBranch:
	mov eax, [a1]
	mov ebx, 0x2
	div ebx
	add eax, 1
	mov [if_flag], eax
	jmp outOfIf
elseIfBranch:
	mov eax, [a1]
	cmp eax, 24
	jl elseIfBlock
	jmp elseBlock
elseIfBlock:
	mov ecx, 0x18
	sub ecx, eax
	imul ecx, eax
	mov [if_flag], ecx
	jmp outOfIf
elseBlock:
	mov eax, [a1]
	shl eax, 4
	mov [if_flag], eax
	jmp outOfIf
outOfIf:

your_while:
	mov ebx, [a2]
	cmp ebx, 0xc
	jl outOfWhile
	exit
while:
	call my_random
	mov edx, [while_flag]
	sub ebx, 0xc
	mov [edx + ebx], eax
	dec ebx
	mov [a2], ebx
	cmp ebx, 12
	jge while
outOfWhile:

	%include "end.include"

your_function:
	mov eax, 0
	mov edx, [your_string]
	mov ebx, 0
	cmp [edx], ebx
	je endFunction

forLoop:
	pushad
	mov edx, [edx + eax]
	 push edx
	call print_a_char
	 pop edx
	popad
	inc eax
	
	;; mov ecx, [edx]
	
	cmp byte [edx + eax], 0
	jne forLoop
endFunction:
	

	
	
	
	
