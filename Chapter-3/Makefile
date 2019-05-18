SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = arm-none-eabi
CFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding

all: clean kernel7.img

boot.o: boot.S
	$(TARGET)-gcc $(CFLAGS) -c boot.S -o boot.o

%.o: %.c
	$(TARGET)-gcc $(CFLAGS) -std=gnu99 -c $< -o $@ -O2 -Wall -Wextra

kernel7.img: boot.o $(OBJS)
	$(TARGET)-gcc -T linker.ld -o kernel7.elf -ffreestanding -O2 -nostdlib boot.o $(OBJS)
	$(TARGET)-objdump -D kernel7.elf > kernel7.dump
	$(TARGET)-objcopy kernel7.elf -O binary kernel7.img

clean:
	rm kernel7.dump kernel7.elf kernel7.img *.o >/dev/null 2>/dev/null || true

minicom:
	sudo minicom -b 115200 -D /dev/ttyUSB0
