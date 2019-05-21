/* Mailbox Buffer */
extern volatile unsigned int mailbox[36];

#define MAILBOX_REQUEST    0

/* Channels */
#define MAILBOX_CH_POWER   0 // Power Management Interface
#define MAILBOX_CH_FB      1 // Frame Buffer
#define MAILBOX_CH_VUART   2 // Virtual UART
#define MAILBOX_CH_VCHIQ   3 // VCHIQ Interface
#define MAILBOX_CH_LEDS    4 // LEDs Interface
#define MAILBOX_CH_BTNS    5 // Buttons Interface
#define MAILBOX_CH_TOUCH   6 // Touchscreen Interface
#define MAILBOX_CH_COUNT   7 // Counter
#define MAILBOX_CH_PROP    8 // Tags (ARM to VC)

/* Tags */
#define MAILBOX_TAG_GETSERIAL      0x00010004
#define MAILBOX_TAG_LAST           0

int mailbox_call(unsigned char ch);
