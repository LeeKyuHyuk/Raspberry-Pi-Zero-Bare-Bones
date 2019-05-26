#include <stdint.h>
#include "uart.h"
#include "sd.h"

// get the end of bss segment from linker
extern unsigned char __bss_end;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
        // r0, r1, atags를 사용하지 않는 변수로 선언
        (void) r0;
        (void) r1;
        (void) atags;

        // UART 초기화
        uart_init();

        // initialize EMMC and detect SD card type
        if(sd_init()==SD_OK) {
                // read the master boot record after our bss segment
                if(sd_readblock(0,&__bss_end,1)) {
                        // dump it to serial console
                        uart_dump(&__bss_end);
                }
        }

        // UART 입력되는 것을 UART로 출력 (Echo 기능)
        while (1) {
                uart_send(uart_getc());
        }
}
