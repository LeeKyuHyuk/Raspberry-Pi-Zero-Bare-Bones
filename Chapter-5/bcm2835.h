#define MMIO_BASE       0x20000000

/* GPIO registers */
#define GPFSEL1         (*(volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPPUD           (*(volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       (*(volatile unsigned int*)(MMIO_BASE+0x00200098))

/* Auxilary mini UART registers */
#define AUX_ENABLES     (*(volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       (*(volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      (*(volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      (*(volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      (*(volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      (*(volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      (*(volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_CNTL     (*(volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_BAUD     (*(volatile unsigned int*)(MMIO_BASE+0x00215068))

/* Mailbox registers */
#define MAILBOX_BASE	     (MMIO_BASE+0x0000B880)
#define MAILBOX_READ       (*(volatile unsigned int*)(MAILBOX_BASE+0x0))
#define MAILBOX_POLL       (*(volatile unsigned int*)(MAILBOX_BASE+0x10))
#define MAILBOX_SENDER     (*(volatile unsigned int*)(MAILBOX_BASE+0x14))
#define MAILBOX_STATUS     (*(volatile unsigned int*)(MAILBOX_BASE+0x18))
#define MAILBOX_CONFIG     (*(volatile unsigned int*)(MAILBOX_BASE+0x1C))
#define MAILBOX_WRITE      (*(volatile unsigned int*)(MAILBOX_BASE+0x20))
