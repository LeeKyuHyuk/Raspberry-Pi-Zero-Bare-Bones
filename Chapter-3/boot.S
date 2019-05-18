// 바이너리의 첫 부분에 ".text.boot"를 배치합니다.
.section ".text.boot"

// Linker에서 해당 Symbol을 찾을 수 있게 _start를 Global로 만듭니다.
.globl _start

.org 0x8000
// Kernel의 진입점(Entry point)
// r15(sp) -> 0x8000에서 실행해야 합니다.
// r0      -> 0x00000000
// r1      -> 0x00000C42
// r2      -> 0x00000100
// kernel_main에 대한 인수(Argument)로 이 레지스터들을 보존합니다.
_start:
	// Stack Pointer을 0x8000으로 설정
	mov sp, #0x8000

	// BSS를 0으로 초기화.
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b       2f

1:
	// r5~r8 레지스터에 저장된 값을 r4에 저장.
	stmia r4!, {r5-r8}

	// bss_end에 도달할 때까지 반복합니다
2:
	cmp r4, r9
	blo 1b

	// 'kernel_main' 함수 호출
	ldr r3, =kernel_main
	blx r3

	// halt
halt:
	wfe
	b halt
