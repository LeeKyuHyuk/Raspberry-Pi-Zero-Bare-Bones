#include "bcm2835.h"
#include "uart.h"
#include "delay.h"
#include "sd.h"

// command flags
#define CMD_NEED_APP        0x80000000
#define CMD_RSPNS_48        0x00020000
#define CMD_ERRORS_MASK     0xfff9c004
#define CMD_RCA_MASK        0xffff0000

// COMMANDs
#define CMD_GO_IDLE         0x00000000
#define CMD_ALL_SEND_CID    0x02010000
#define CMD_SEND_REL_ADDR   0x03020000
#define CMD_CARD_SELECT     0x07030000
#define CMD_SEND_IF_COND    0x08020000
#define CMD_STOP_TRANS      0x0C030000
#define CMD_READ_SINGLE     0x11220010
#define CMD_READ_MULTI      0x12220032
#define CMD_SET_BLOCKCNT    0x17020000
#define CMD_APP_CMD         0x37000000
#define CMD_SET_BUS_WIDTH   (0x06020000|CMD_NEED_APP)
#define CMD_SEND_OP_COND    (0x29020000|CMD_NEED_APP)
#define CMD_SEND_SCR        (0x33220010|CMD_NEED_APP)

// STATUS register settings
#define SR_READ_AVAILABLE   0x00000800
#define SR_DAT_INHIBIT      0x00000002
#define SR_CMD_INHIBIT      0x00000001
#define SR_APP_CMD          0x00000020

// INTERRUPT register settings
#define INT_DATA_TIMEOUT    0x00100000
#define INT_CMD_TIMEOUT     0x00010000
#define INT_READ_RDY        0x00000020
#define INT_CMD_DONE        0x00000001

#define INT_ERROR_MASK      0x017E8000

// CONTROL register settings
#define C0_SPI_MODE_EN      0x00100000
#define C0_HCTL_HS_EN       0x00000004
#define C0_HCTL_DWITDH      0x00000002

#define C1_SRST_DATA        0x04000000
#define C1_SRST_CMD         0x02000000
#define C1_SRST_HC          0x01000000
#define C1_TOUNIT_DIS       0x000f0000
#define C1_TOUNIT_MAX       0x000e0000
#define C1_CLK_GENSEL       0x00000020
#define C1_CLK_EN           0x00000004
#define C1_CLK_STABLE       0x00000002
#define C1_CLK_INTLEN       0x00000001

// SLOTISR_VER values
#define HOST_SPEC_NUM       0x00ff0000
#define HOST_SPEC_NUM_SHIFT 16
#define HOST_SPEC_V3        2
#define HOST_SPEC_V2        1
#define HOST_SPEC_V1        0

// SCR flags
#define SCR_SD_BUS_WIDTH_4  0x00000400
#define SCR_SUPP_SET_BLKCNT 0x02000000
// added by my driver
#define SCR_SUPP_CCS        0x00000001

#define ACMD41_VOLTAGE      0x00ff8000
#define ACMD41_CMD_COMPLETE 0x80000000
#define ACMD41_CMD_CCS      0x40000000
#define ACMD41_ARG_HC       0x51ff8000

unsigned long sd_scr[2], sd_ocr, sd_rca, sd_err, sd_hv;

/**
 * Wait for data or command ready
 */
int sd_status(unsigned int mask)
{
        int cnt = 500000;
        while((EMMC_STATUS & mask) && !(EMMC_INTERRUPT & INT_ERROR_MASK) && cnt--)
                wait_msec(1);
        if(cnt <= 0 || (EMMC_INTERRUPT & INT_ERROR_MASK)) {
                return SD_ERROR;
        }
        return SD_OK;
}

/**
 * Wait for interrupt
 */
int sd_int(unsigned int mask)
{
        unsigned int r, m=mask | INT_ERROR_MASK;
        int cnt = 1000000;
        while(!(EMMC_INTERRUPT & m) && cnt--)
                wait_msec(1);
        r=EMMC_INTERRUPT;
        if(cnt<=0 || (r & INT_CMD_TIMEOUT) || (r & INT_DATA_TIMEOUT) ) {
                EMMC_INTERRUPT=r; return SD_TIMEOUT;
        }
        else {
                if(r & INT_ERROR_MASK) {
                        EMMC_INTERRUPT=r; return SD_ERROR;
                }
        }
        EMMC_INTERRUPT=mask;
        return 0;
}

/**
 * Send a command
 */
int sd_cmd(unsigned int code, unsigned int arg)
{
        int r=0;
        sd_err=SD_OK;
        if(code&CMD_NEED_APP) {
                r=sd_cmd(CMD_APP_CMD|(sd_rca ? CMD_RSPNS_48 : 0),sd_rca);
                if(sd_rca && !r) {
                        uart_puts("ERROR: failed to send SD APP command\n");
                        sd_err=SD_ERROR;
                        return 0;
                }
                code &= ~CMD_NEED_APP;
        }
        if(sd_status(SR_CMD_INHIBIT)) {
                uart_puts("ERROR: EMMC busy\n");
                sd_err= SD_TIMEOUT; return 0;
        }
        uart_puts("EMMC: Sending command "); uart_hex(code); uart_puts(" arg "); uart_hex(arg); uart_puts("\n");
        EMMC_INTERRUPT=EMMC_INTERRUPT;
        EMMC_ARG1=arg;
        EMMC_CMDTM=code;
        if(code==CMD_SEND_OP_COND)
                wait_msec(1000);
        else {
                if(code==CMD_SEND_IF_COND || code==CMD_APP_CMD) wait_msec(100);
        }
        if((r=sd_int(INT_CMD_DONE))) {
                uart_puts("ERROR: failed to send EMMC command\n");
                sd_err=r;
                return 0;
        }
        r=EMMC_RESP0;
        if(code==CMD_GO_IDLE || code==CMD_APP_CMD)
                return 0;
        else {
                if(code==(CMD_APP_CMD|CMD_RSPNS_48))
                        return r&SR_APP_CMD;
                else {
                        if(code==CMD_SEND_OP_COND)
                                return r;
                        else {
                                if(code==CMD_SEND_IF_COND)
                                        return r==arg ? SD_OK : SD_ERROR;
                                else{
                                        if(code==CMD_ALL_SEND_CID) {
                                                r|=EMMC_RESP3;
                                                r|=EMMC_RESP2;
                                                r|=EMMC_RESP1;
                                                return r;
                                        }
                                        else {
                                                if(code==CMD_SEND_REL_ADDR) {
                                                        sd_err=(((r&0x1fff))|((r&0x2000)<<6)|((r&0x4000)<<8)|((r&0x8000)<<8))&CMD_ERRORS_MASK;
                                                        return r&CMD_RCA_MASK;
                                                }
                                        }
                                }
                        }
                }
        }
        return r&CMD_ERRORS_MASK;
        // make gcc happy
        return 0;
}

/**
 * read a block from sd card and return the number of bytes read
 * returns 0 on error.
 */
int sd_readblock(unsigned int lba, unsigned char *buffer, unsigned int num)
{
        int r,c=0,d;
        if(num<1)
                num=1;
        uart_puts("sd_readblock lba "); uart_hex(lba); uart_puts(" num "); uart_hex(num); uart_puts("\n");
        if(sd_status(SR_DAT_INHIBIT)) {
                sd_err=SD_TIMEOUT;
                return 0;
        }
        unsigned int *buf=(unsigned int *)buffer;
        if(sd_scr[0] & SCR_SUPP_CCS) {
                if(num > 1 && (sd_scr[0] & SCR_SUPP_SET_BLKCNT)) {
                        sd_cmd(CMD_SET_BLOCKCNT,num);
                        if(sd_err) return 0;
                }
                EMMC_BLKSIZECNT = (num << 16) | 512;
                sd_cmd(num == 1 ? CMD_READ_SINGLE : CMD_READ_MULTI,lba);
                if(sd_err) return 0;
        } else {
                EMMC_BLKSIZECNT = (1 << 16) | 512;
        }
        while( c < num ) {
                if(!(sd_scr[0] & SCR_SUPP_CCS)) {
                        sd_cmd(CMD_READ_SINGLE,(lba+c)*512);
                        if(sd_err) return 0;
                }
                if((r=sd_int(INT_READ_RDY))) {
                        uart_puts("\rERROR: Timeout waiting for ready to read\n");
                        sd_err=r;
                        return 0;
                }
                for(d=0; d<128; d++)
                        buf[d] = EMMC_DATA;
                c++;
                buf+=128;
        }
        if( num > 1 && !(sd_scr[0] & SCR_SUPP_SET_BLKCNT) && (sd_scr[0] & SCR_SUPP_CCS))
                sd_cmd(CMD_STOP_TRANS,0);
        return sd_err!=SD_OK || c!=num ? 0 : num*512;
}

/**
 * set SD clock to frequency in Hz
 */
int sd_clk(unsigned int freq)
{
        unsigned int d,x,s=32,h=0;
        unsigned int c=41666666/freq; // Pi SD frequency is always 41.666666···Mhz on baremetal
        int cnt = 100000;
        while((EMMC_STATUS & (SR_CMD_INHIBIT|SR_DAT_INHIBIT)) && cnt--)
                wait_msec(1);
        if(cnt<=0) {
                uart_puts("ERROR: timeout waiting for inhibit flag\n");
                return SD_ERROR;
        }

        EMMC_CONTROL1 &= ~C1_CLK_EN;
        wait_msec(10);
        x=c-1;
        if(!x)
                s=0;
        else {
                if(!(x & 0xffff0000u)) {
                        x <<= 16;
                        s -= 16;
                }
                if(!(x & 0xff000000u)) {
                        x <<= 8;
                        s -= 8;
                }
                if(!(x & 0xf0000000u)) {
                        x <<= 4;
                        s -= 4;
                }
                if(!(x & 0xc0000000u)) {
                        x <<= 2;
                        s -= 2;
                }
                if(!(x & 0x80000000u)) {
                        x <<= 1;
                        s -= 1;
                }
                if(s>0)
                        s--;
                if(s>7)
                        s=7;
        }
        if(sd_hv>HOST_SPEC_V2) d=c; else d=(1<<s);
        if(d<=2) {d=2; s=0;}
        uart_puts("sd_clk divisor "); uart_hex(d); uart_puts(", shift "); uart_hex(s); uart_puts("\n");
        if(sd_hv>HOST_SPEC_V2) h=(d&0x300)>>2;
        d=(((d&0x0ff)<<8)|h);
        EMMC_CONTROL1=(EMMC_CONTROL1&0xffff003f)|d; wait_msec(10);
        EMMC_CONTROL1 |= C1_CLK_EN; wait_msec(10);
        cnt=10000; while(!(EMMC_CONTROL1 & C1_CLK_STABLE) && cnt--) wait_msec(10);
        if(cnt<=0) {
                uart_puts("ERROR: failed to get stable clock\n");
                return SD_ERROR;
        }
        return SD_OK;
}

/**
 * initialize EMMC to read SDHC card
 */
int sd_init()
{
        long r,cnt,ccs=0;
        // SD_CARD_DET
        // GPIO47~53은 SDCard Interface에서 사용됩니다.
        // GPIO 47은 SDCard Detect로 사용됩니다. (SD_CARD_DET)
        // https://elinux.org/RPi_Low-level_peripherals#GPIO_hardware_hacking
        r=GPFSEL4; // GPFSEL4는 GPIO 40~49까지 담당함.
        r&=~(7<<(7*3)); // GPIO Pin 47을 Input으로 설정
        GPFSEL4=r; // GPFSEL4에 위의 설정을 자장
        GPPUD=2; // Enable Pull Up control (GPPUD에 기록하여 필요한 control signal를 설정)
        wait_cycles(150); // control signal를 설정하기 위해 150 사이클을 기다립니다.
        GPPUDCLK1=(1<<15); // GPIO Pin 47을 Assert Clock on line
                           // GPPUD에서 수정한 GPIO의 control signal에 clock을 보내기 위해 GPPUDCLK1에서도 해당 GPIO 핀을 수정합니다.
        wait_cycles(150); // 150 사이클을 기다립니다.
        GPPUD=0; // GPPUD에 기록했던 GPIO의 설정을 제거합니다.
        GPPUDCLK1=0; // GPPUDCLK1에 기록했던 GPIO의 설정을 제거합니다.
                     // 이 과정들은 101페이지의 'GPIO Pull-up/down Clock Registers (GPPUDCLKn)'에 나와있음.
        r=GPHEN1; // event detect status register (GPHEN1)
        r|=1<<15; // High on GPIO pin 47 sets corresponding bit in GPEDS
        GPHEN1=r;

        // SD_CLK_R, SD_CMD_R
        r=GPFSEL4;
        r|=(7<<(8*3))|(7<<(9*3)); // GPIO Pin 48, 49을 Input으로 설정
        GPFSEL4=r;
        GPPUD=2; // Enable Pull Up control (GPPUD에 기록하여 필요한 control signal를 설정)
        wait_cycles(150); // control signal를 설정하기 위해 150 사이클을 기다립니다.
        GPPUDCLK1=(1<<16)|(1<<17); // GPIO Pin 48, 49을 Assert Clock on line
                                   // GPPUD에서 수정한 GPIO의 control signal에 clock을 보내기 위해 GPPUDCLK1에서도 해당 GPIO 핀을 수정합니다.
        wait_cycles(150); // 150 사이클을 기다립니다.
        GPPUD=0; // GPPUD에 기록했던 GPIO의 설정을 제거합니다.
        GPPUDCLK1=0; // GPPUDCLK1에 기록했던 GPIO의 설정을 제거합니다.

        // SD_DATA0_R, SD_DATA1_R, SD_DATA2_R, SD_DATA3_R
        r=GPFSEL5; // GPFSEL4는 GPIO 50~59까지 담당함.
        r|=(7<<(0*3)) | (7<<(1*3)) | (7<<(2*3)) | (7<<(3*3)); // GPIO 50, 51, 52, 53을 Input으로 설정
        GPFSEL5=r;
        GPPUD=2; // Enable Pull Up control (GPPUD에 기록하여 필요한 control signal를 설정)
        wait_cycles(150); // control signal를 설정하기 위해 150 사이클을 기다립니다.
        GPPUDCLK1=(1<<18) | (1<<19) | (1<<20) | (1<<21); // GPIO Pin 50, 51, 52, 53을 Assert Clock on line
                                                         // GPPUD에서 수정한 GPIO의 control signal에 clock을 보내기 위해 GPPUDCLK1에서도 해당 GPIO 핀을 수정합니다.
        wait_cycles(150); // 150 사이클을 기다립니다.
        GPPUD=0; // GPPUD에 기록했던 GPIO의 설정을 제거합니다.
        GPPUDCLK1=0; // GPPUDCLK1에 기록했던 GPIO의 설정을 제거합니다.

        // EMMC_SLOTISR_VER : 버전 정보와 슬롯 인터럽트 상태를 포함한다.
        sd_hv = (EMMC_SLOTISR_VER & HOST_SPEC_NUM) >> HOST_SPEC_NUM_SHIFT;
        uart_puts("EMMC: GPIO set up\n");
        // Reset the card.
        EMMC_CONTROL0 = 0;
        EMMC_CONTROL1 |= C1_SRST_HC; // SRST_HC (Reset the complete host circuit)
        cnt=10000;
        do {
                wait_msec(10);
        } while( (EMMC_CONTROL1 & C1_SRST_HC) && cnt-- );
        // 10000회 확인해서 EMMC_CONTROL1의 값이 C1_SRST_HC와 다르면 (리셋되면 값이 설정됨) reset ok
        if(cnt<=0) {
                uart_puts("ERROR: failed to reset EMMC\n");
                return SD_ERROR;
        }
        uart_puts("EMMC: reset OK\n");
        // Data timeout 단위를 최대로 설정 및 절전을 위한 내부 eMMC Clock의 Clock 활성화
        EMMC_CONTROL1 |= C1_CLK_INTLEN | C1_TOUNIT_MAX;
        wait_msec(10);
        // Set clock to setup frequency.
        if((r=sd_clk(400000)))
                return r;
        // IRPT_EN Register 초기화
        EMMC_INT_EN   = 0xffffffff;
        // IRPT_MASK Register 초기화
        EMMC_INT_MASK = 0xffffffff;
        // sd_scr = SD Card Configuration register
        // sd_rca = Relative Card Address register (SD카드 상대 주소 값)
        // 초기화
        sd_scr[0]=sd_scr[1]=sd_rca=sd_err=0;
        sd_cmd(CMD_GO_IDLE,0); // CMD0
        if(sd_err)
                return sd_err;

        sd_cmd(CMD_SEND_IF_COND,0x000001AA); // CMD8
        if(sd_err)
                return sd_err;
        cnt=6;
        r=0;
        while(!(r&ACMD41_CMD_COMPLETE) && cnt--) {
                wait_cycles(400);
                r=sd_cmd(CMD_SEND_OP_COND,ACMD41_ARG_HC);
                uart_puts("EMMC: CMD_SEND_OP_COND returned ");
                if(r&ACMD41_CMD_COMPLETE)
                        uart_puts("COMPLETE ");
                if(r&ACMD41_VOLTAGE)
                        uart_puts("VOLTAGE ");
                if(r&ACMD41_CMD_CCS)
                        uart_puts("CCS ");
                uart_hex(r>>32);
                uart_hex(r);
                uart_puts("\n");
                if(sd_err!=SD_TIMEOUT && sd_err!=SD_OK ) {
                        uart_puts("ERROR: EMMC ACMD41 returned error\n");
                        return sd_err;
                }
        }
        if(!(r&ACMD41_CMD_COMPLETE) || !cnt )
                return SD_TIMEOUT;
        if(!(r&ACMD41_VOLTAGE))
                return SD_ERROR;
        if(r&ACMD41_CMD_CCS)
                ccs=SCR_SUPP_CCS;

        sd_cmd(CMD_ALL_SEND_CID,0);

        sd_rca = sd_cmd(CMD_SEND_REL_ADDR,0);
        uart_puts("EMMC: CMD_SEND_REL_ADDR returned ");
        uart_hex(sd_rca>>32);
        uart_hex(sd_rca);
        uart_puts("\n");
        if(sd_err)
                return sd_err;

        if((r=sd_clk(25000000)))
                return r;

        sd_cmd(CMD_CARD_SELECT,sd_rca);
        if(sd_err)
                return sd_err;

        if(sd_status(SR_DAT_INHIBIT))
                return SD_TIMEOUT;
        EMMC_BLKSIZECNT = (1<<16) | 8;
        sd_cmd(CMD_SEND_SCR,0);
        if(sd_err)
                return sd_err;
        if(sd_int(INT_READ_RDY))
                return SD_TIMEOUT;

        r=0; cnt=100000; while(r<2 && cnt) {
                if( EMMC_STATUS & SR_READ_AVAILABLE )
                        sd_scr[r++] = EMMC_DATA;
                else
                        wait_msec(1);
        }
        if(r!=2)
                return SD_TIMEOUT;
        if(sd_scr[0] & SCR_SD_BUS_WIDTH_4) {
                sd_cmd(CMD_SET_BUS_WIDTH,sd_rca|2);
                if(sd_err)
                        return sd_err;
                EMMC_CONTROL0 |= C0_HCTL_DWITDH;
        }
        // add software flag
        uart_puts("EMMC: supports ");
        if(sd_scr[0] & SCR_SUPP_SET_BLKCNT)
                uart_puts("SET_BLKCNT ");
        if(ccs)
                uart_puts("CCS ");
        uart_puts("\n");
        sd_scr[0]&=~SCR_SUPP_CCS;
        sd_scr[0]|=ccs;
        return SD_OK;
}
