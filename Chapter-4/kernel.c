#include <stdint.h>
#include "bcm2835.h"
#include "uart.h"

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	// r0, r1, atags를 사용하지 않는 변수로 선언
        (void) r0;
        (void) r1;
        (void) atags;

        // UART 초기화
        uart_init();

        // "Hello World!"를 UART로 출력
        uart_puts("Hello World!\n");
        uart_puts("Raspberry Pi Zero UART\n");

        // UART 입력되는 것을 UART로 출력 (Echo 기능)
        while (1) {
                uart_send(uart_getc());
        }
}
