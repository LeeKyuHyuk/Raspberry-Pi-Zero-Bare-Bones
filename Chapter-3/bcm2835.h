#define MMIO_BASE       0x20000000

/* System Timer Counter registers */
#define SYSTMR_LO       (*(volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI       (*(volatile unsigned int*)(MMIO_BASE+0x00003008))

/* GPIO registers */
#define GPFSEL4         (*(volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPSET1          (*(volatile unsigned int*)(MMIO_BASE+0x00200020))
#define GPCLR1          (*(volatile unsigned int*)(MMIO_BASE+0x0020002C))
