#include "bcm2835.h"
#include "uart.h"

/**
 * initialize UART
 */
void uart_init() {
        volatile unsigned char delay;

        AUX_ENABLES |= 1; // Mini UART 활성화
        AUX_MU_CNTL = 0; // Tx, Rx 비활성화
        AUX_MU_LCR = 3; // Data size : 8-bit mode
        AUX_MU_MCR = 0;
        AUX_MU_IER = 0;
        AUX_MU_IIR = 0xC1; // Interrupts 비활성화
        AUX_MU_BAUD = 270; // Baudrate를 115200로 설정

        // UART1 GPIO pins setting
        GPFSEL1 = (2 << 12) | (2 << 15); // GPIO 14, 15핀을 alternate function 5로 설정
        GPPUD = 0;           // GPIO 14, 15핀 활성화
        for (delay = 0; delay < 150; delay++)
                asm volatile ("nop");
        GPPUDCLK0 = (1 << 14) | (1 << 15);
        for (delay = 0; delay < 150; delay++)
                asm volatile ("nop");
        GPPUDCLK0 = 0; // Flush GPIO setup
        AUX_MU_CNTL = 3; // Tx, Rx 활성화
}

/**
 * Send a character
 */
void uart_send(unsigned int data) {
        // AUX_MU_LSR의 Transmitter Empty 비트가 '1'이 될 때까지 대기합니다.
        do {
                asm volatile ("nop");
        } while (!(AUX_MU_LSR & (1 << 5)));
        // Buffer에 문자를 기록합니다.
        AUX_MU_IO = data;
}

/**
 * Receive a character
 */
char uart_getc() {
        char character;
        // Buffer에 무엇인가 있을때까지 대기합니다.
        do {
                asm volatile ("nop");
        } while (!(AUX_MU_LSR & 1));
        // AUX_MU_IO에서 값을 읽어오고 return 합니다.
        character = (char)(AUX_MU_IO);
        // '\r'를 '\n'으로 변환합니다.
        return character == '\r' ? '\n' : character;
        ;
}

/**
 * Display a string
 */
void uart_puts(char *string) {
        while (*string) {
                // '\n'를 '\r'으로 변환합니다.
                if (*string == '\n')
                        uart_send('\r');
                uart_send(*string++);
        }
}
