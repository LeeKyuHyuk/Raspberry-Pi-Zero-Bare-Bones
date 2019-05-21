#include <stdint.h>
#include "bcm2835.h"
#include "uart.h"
#include "mailbox.h"

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
        // r0, r1, atags를 사용하지 않는 변수로 선언
        (void) r0;
        (void) r1;
        (void) atags;

        // UART 초기화
        uart_init();

        // 보드의 고유한 Serial Number를 Mailbox 호출로 얻습니다.
        mailbox[0] = 8*4;                   // Message의 길이
        mailbox[1] = MAILBOX_REQUEST;       // Request message임을 표기합니다.
        mailbox[2] = MAILBOX_TAG_GETSERIAL; // Get Serial Number Command
        mailbox[3] = 8;                     // Buffer Size
        mailbox[4] = 8;
        mailbox[5] = 0;                     // Output buffer를 초기화 합니다.
        mailbox[6] = 0;
        mailbox[7] = MAILBOX_TAG_LAST;

        // Message를 GPU로 보내고, 응답을 받습니다.
        if (mailbox_call(MAILBOX_CH_PROP)) {
                uart_puts("My serial number is: ");
                uart_hex(mailbox[6]);
                uart_hex(mailbox[5]);
                uart_puts("\n");
        } else {
                uart_puts("Unable to query serial!\n");
        }

        // UART 입력되는 것을 UART로 출력 (Echo 기능)
        while (1) {
                uart_send(uart_getc());
        }
}
