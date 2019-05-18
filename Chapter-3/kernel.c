#include <stdint.h>
#include "bcm2835.h"
#include "delay.h"

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	// r0, r1, atags를 사용하지 않는 변수로 선언
	(void) r0;
	(void) r1;
	(void) atags;
	GPFSEL4 = 1 << 21; // GPFSEL4의 21번 비트를 1로 설정합니다.
	while (1) {
		GPSET1 = 1 << 15; // GPSET1의 15번 비트를 1로 설정합니다.
		// GPIO 47번을 LOW (ACT LED ON)
		wait_msec(1000000 * 1); // 1초 Delay
		GPCLR1 = 1 << 15; // GPCLR1의 15번 비트를 1로 설정합니다.
		// GPIO 47번을 HIGH (ACT LED OFF)
		wait_msec(1000000 * 1); // 1초 Delay
	}
}
