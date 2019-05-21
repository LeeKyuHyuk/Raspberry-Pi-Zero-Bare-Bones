#include "bcm2835.h"

/* Mailbox Empty (Mailbox에서 읽을 내용이 없을 때) */
#define MAILBOX_EMPTY      0x40000000
/* Mailbox Full (Mailbox에 쓸 공간이 없을 때) */
#define MAILBOX_FULL       0x80000000
#define MAILBOX_RESPONSE   0x80000000

/* Mailbox Message Buffer */
volatile unsigned int __attribute__((aligned(16))) mailbox[36];

/**
 * Mailbox를 호출 합니다.
 * 반환값이 0이면 실패이고, 0이 아닌 숫자이면 성공입니다.
 */
int mailbox_call(unsigned char ch)
{
        unsigned int r = (((unsigned int)((unsigned long)&mailbox)&~0xF) | (ch&0xF));
        // Mailbox에 쓸 수 있을때 까지 기다립니다.
        // Full flag가 설정되지 않을 때 까지 Mailbox Status Register를 읽습니다.
        do {
                asm volatile ("nop");
        } while(MAILBOX_STATUS & MAILBOX_FULL);
        // mailbox 변수의 채널 부분을 주소를 Mailbox의 채널 식별자에 씁니다.
        MAILBOX_WRITE = r;
        // Response를 기다립니다.
        while(1) {
                // Response가 올때까지 nop합니다.
                do {
                        asm volatile ("nop");
                } while(MAILBOX_STATUS & MAILBOX_EMPTY);
                // 요청한 Response인지 확인합니다.
                if(r == MAILBOX_READ)
                        // 유효한 Response인지 확인합니다.
                        return mailbox[1]==MAILBOX_RESPONSE;
        }
        return 0;
}
