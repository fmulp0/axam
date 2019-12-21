AMIGA_BASE=/opt/Amiga

VBCC_TARGET=$(AMIGA_BASE)/target
NDK_1_3=$(AMIGA_BASE)/NDK_1.3
VBCCM68K=$(AMIGA_BASE)/vbcc/bin/vbccm68k
VASMM68K=$(AMIGA_BASE)/vasm/vasmm68k_mot
VLINK=$(AMIGA_BASE)/vlink/vlink

SRCS=trackdisk.c error.c command.c main.c serial.c
OBJS=trackdisk.o error.o command.o main.o serial.o
ASMS=trackdisk.asm error.asm command.asm main.asm serial.asm

TARGET=axam-0.4.0

$(TARGET) : $(OBJS)
	$(VLINK) -bamigahunk -x -Bstatic -Cvbcc -Z -mrel $(VBCC_TARGET)/lib/startup.o $^ -L$(VBCC_TARGET)/lib -lvc -lamiga -lauto -o $@

%.o : %.asm
	$(VASMM68K) -quiet -Fhunk -kick1hunks -phxass -nowarn=62 $< -o $@
	
%.asm : %.c
	$(VBCCM68K) -quiet $< -o=$@ -O=1 -no-cpp-warn -I. -I$(VBCC_TARGET)/include -I$(NDK_1_3)/Includes1.3/include.h

trackdisk.o : trackdisk.asm
error.o : error.asm
command.o : command.asm
main.o : main.asm
serial.o : serial.asm

clean : 
	rm -f $(OBJS) $(TARGET) $(ASMS)
