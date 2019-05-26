#define MMIO_BASE       0x20000000

/* System Timer Counter registers */
#define SYSTMR_LO           (*(volatile unsigned int*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI           (*(volatile unsigned int*)(MMIO_BASE+0x00003008))

/* GPIO registers */
#define GPFSEL1             (*(volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL4             (*(volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPFSEL5             (*(volatile unsigned int*)(MMIO_BASE+0x00200014))
#define GPPUD               (*(volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0           (*(volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPHEN1              (*(volatile unsigned int*)(MMIO_BASE+0x00200068))
#define GPPUDCLK1           (*(volatile unsigned int*)(MMIO_BASE+0x0020009C))

/* Auxilary mini UART registers */
#define AUX_ENABLES         (*(volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO           (*(volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER          (*(volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR          (*(volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR          (*(volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR          (*(volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR          (*(volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_CNTL         (*(volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_BAUD         (*(volatile unsigned int*)(MMIO_BASE+0x00215068))

/* Mailbox registers */
#define MAILBOX_BASE        (MMIO_BASE+0x0000B880)
#define MAILBOX_READ        (*(volatile unsigned int*)(MAILBOX_BASE+0x0))
#define MAILBOX_POLL        (*(volatile unsigned int*)(MAILBOX_BASE+0x10))
#define MAILBOX_SENDER      (*(volatile unsigned int*)(MAILBOX_BASE+0x14))
#define MAILBOX_STATUS      (*(volatile unsigned int*)(MAILBOX_BASE+0x18))
#define MAILBOX_CONFIG      (*(volatile unsigned int*)(MAILBOX_BASE+0x1C))
#define MAILBOX_WRITE       (*(volatile unsigned int*)(MAILBOX_BASE+0x20))

/* EMMC module registers */
#define EMMC_ARG2           (*(volatile unsigned int*)(MMIO_BASE+0x00300000))
#define EMMC_BLKSIZECNT     (*(volatile unsigned int*)(MMIO_BASE+0x00300004))
#define EMMC_ARG1           (*(volatile unsigned int*)(MMIO_BASE+0x00300008))
#define EMMC_CMDTM          (*(volatile unsigned int*)(MMIO_BASE+0x0030000C))
#define EMMC_RESP0          (*(volatile unsigned int*)(MMIO_BASE+0x00300010))
#define EMMC_RESP1          (*(volatile unsigned int*)(MMIO_BASE+0x00300014))
#define EMMC_RESP2          (*(volatile unsigned int*)(MMIO_BASE+0x00300018))
#define EMMC_RESP3          (*(volatile unsigned int*)(MMIO_BASE+0x0030001C))
#define EMMC_DATA           (*(volatile unsigned int*)(MMIO_BASE+0x00300020))
#define EMMC_STATUS         (*(volatile unsigned int*)(MMIO_BASE+0x00300024))
#define EMMC_CONTROL0       (*(volatile unsigned int*)(MMIO_BASE+0x00300028))
#define EMMC_CONTROL1       (*(volatile unsigned int*)(MMIO_BASE+0x0030002C))
#define EMMC_INTERRUPT      (*(volatile unsigned int*)(MMIO_BASE+0x00300030))
#define EMMC_INT_MASK       (*(volatile unsigned int*)(MMIO_BASE+0x00300034))
#define EMMC_INT_EN         (*(volatile unsigned int*)(MMIO_BASE+0x00300038))
#define EMMC_CONTROL2       (*(volatile unsigned int*)(MMIO_BASE+0x0030003C))
#define EMMC_SLOTISR_VER    (*(volatile unsigned int*)(MMIO_BASE+0x003000FC))
