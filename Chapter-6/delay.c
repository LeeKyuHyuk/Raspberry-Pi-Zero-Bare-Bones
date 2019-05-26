#include "delay.h"
#include "bcm2835.h"

/**
 * Wait N CPU cycles
 */
void wait_cycles(unsigned int n)
{
								if(n) {
																while(n--) {
																								asm volatile ("nop");
																}
								}
}

/**
 * Get System Timer's counter
 */
unsigned long get_system_timer() {
								unsigned int h = -1, l;
								h = SYSTMR_HI;
								l = SYSTMR_LO;
								// System Timer Counter Higher 32 bits가 변경되면, 위의 작업을 다시 합니다.
								if (h != SYSTMR_HI) {
																h = SYSTMR_HI;
																l = SYSTMR_LO;
								}
								return ((unsigned long)h << 32) | l;
}

/**
 * Wait N microseconds
 */
void wait_msec(unsigned int n) {
								unsigned long t = get_system_timer();
								// 무한 루프를 피하기 위하여 t의 값이 0이 아닌지 확인합니다.
								if (t)
																while (get_system_timer() < t + n)
																								;
}
