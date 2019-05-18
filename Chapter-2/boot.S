.equ PERI_BASE ,0x20000000             @ Peripheral Base Address
.equ GPIO_BASE ,PERI_BASE + 0x00200000 @ GPIO Base Address

.section ".text.boot"
.globl _start

.org 0x8000                            @ 프로그램과 데이터의 시작 주소를 0x8000으로 설정

_start:
    @--------------------------------------------------
    @ ACT LED Blink
    @--------------------------------------------------
    LDR R0 ,=GPIO_BASE                 @ R0에 GPIO_BASE 주소를 넣습니다.
    MOV R1 ,#1                         @ R1에 1을 넣습니다.
    LSL R1 ,#21                        @ R1을 21번 Left Shift하여
                                       @ R1의 21번 비트를 1로 설정합니다
                                       @ 이렇게 하면, GPFSEL0의 [23:21]
                                       @ 비트를 001로 설정하게 됩니다.
    STR R1 ,[R0, #0x0010]              @ R0(0x20200000) + GPFSEL4(0x0010)에
                                       @ R1의 값을 넣습니다.

    MOV R1 ,#1                         @ R1에 1을 넣습니다.
    LSL R1 ,#15                        @ 15번 Left Shift하여 R1의 15번 비트를
                                       @ 1로 설정합니다.
    STR R1 ,[R0, #0x0020]              @ R0(0x20200000) + GPSET1(0x0020)에
                                       @ R1의 값을 넣습니다.
                                       @ GPIO 47번을 LOW로 설정하면 LED가 켜집니다.
