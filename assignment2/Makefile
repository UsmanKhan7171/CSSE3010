# Add additional project sources like this:
# SRCS += X.c
#
# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

#location of freertos source files
MYLIB=../mylib
NP2_ROOT=/home/sanchez/Documents/git/np2/np2
FREERTOS_PATH=$(NP2_ROOT)/src/FreeRTOS
CLI_PATH=$(NP2_ROOT)/src/freertosplus/FreeRTOS-Plus-CLI
USB_PATH=$(NP2_ROOT)/src/hal/STM32_USB_Device_Library
HID_PATH=$(USB_PATH)/Class/HID
USB_CORE_PATH=$(USB_PATH)/Core
MQTT_PATH=$(NP2_ROOT)/src/net/apps/mqtt

#Name of project
PROJ_NAME=main

#Your local source files must ONLY be added to SRCS
SRCS = $(PROJ_NAME).c
#SRCS += $(USB_CORE_PATH)/Src/*.c
#SRCS += $(HID_PATH)/Src/*.c
CFLAGS += -I$(MYLIB)
CFLAGS += -I$(HID_PATH)/Inc
CFLAGS += -I$(USB_CORE_PATH)/Inc
CFLAGS += -I$(NP2_ROOT)/src/boards/netduinoplus2/usb/hid
#CFLAGS += -DENABLE_VCP #enable USB Virtual Com Port
REMOVE_VCPSRC = 1
CFLAGS += -DSDEBUG

#Extra files from the src folder must ONLY be added to LIBSRCS
LIBSRCS = $(FREERTOS_PATH)/portable/MemMang/heap_4.c
LIBSRCS += /home/sanchez/Documents/git/np2/np2/src/boards/netduinoplus2/usb/hid/usbd_desc.c
LIBSRCS += $(wildcard $(HID_PATH)/Src/*.c)

###################################################
ROOT=$(shell pwd)

OBJS=$(SRCS:.c=.o)

###################################################

.PHONY: $(shell mkdir -p obj) lib proj all depend
all: proj

include $(CLI_PATH)/Makefile.freertoscli

#Include lwip makefile
include $(NP2_ROOT)/src/net/lwip/Makefile.lwip

#Include net makefile
include $(NP2_ROOT)/src/boards/netduinoplus2/Makefile.net

include $(MQTT_PATH)/Makefile.mqtt

#Include freertos makefile
include $(NP2_ROOT)/src/freertos/Makefile.freertos

#Include np2 makefile - NOTE MUST BE LAST TO INCLUDE
include $(NP2_ROOT)/src/boards/netduinoplus2/Makefile.np2

lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

#compile and link file
$(PROJ_NAME).elf:	$(NP2OBJ) $(OBJS)
	$(CC) $(LDFLAGS) -o $@ -Llib $(OBJS) $(NP2OBJ) $(LD_SYS_LIBS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

prog:
	sudo dfu-util -d 0483:df11 -c 1 -i 0 -a 0 -s 0x08000000 -D $(PROJ_NAME).bin		#Program NP2

pron:
	sudo ~/gitdev/stlink/st-flash write $(PROJ_NAME).bin 0x8000000	#Program Nucleo

qemu:
	np2-qemu $(PROJ_NAME).elf

gdb:
	np2-gdb-qemu $(PROJ_NAME).elf

clean:
	rm -f *.o
	rm -f $(OBJDIR)/*.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	rm -f $(LIBNP2)
	rm -rf .depend
	rmdir $(OBJDIR)


include .depend
