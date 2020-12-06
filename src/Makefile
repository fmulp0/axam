AMIGA_BASE=/opt/Amiga
RM=rm -f
VBCC_TARGET=$(AMIGA_BASE)/target
NDK_1_3=$(AMIGA_BASE)/NDK_1.3
VBCCM68K=$(AMIGA_BASE)/vbcc/bin/vbccm68k
VASMM68K=$(AMIGA_BASE)/vasm/vasmm68k_mot_win32
VLINK=$(AMIGA_BASE)/vlink/vlink

SRCS=buffer.c command.c console.c crc.c data.c error.c file.c main.c packet.c serial.c stream.c trackdisk.c
OBJS=buffer.o command.o console.o crc.o data.o error.o file.o main.o packet.o serial.o stream.o trackdisk.o
ASMS=buffer.asm command.asm console.asm crc.asm data.asm error.asm file.asm main.asm packet.asm serial.asm stream.asm trackdisk.asm

TARGET=axam-0.6.0

$(TARGET) : $(OBJS)
	$(VLINK) -bamigahunk -x -Bstatic -Cvbcc -Z -mrel "$(VBCC_TARGET)/lib/startup.o" $^ -L"$(VBCC_TARGET)/lib" -lvc -lamiga -lauto -o $@

%.o : %.asm
	$(VASMM68K) -quiet -Fhunk -kick1hunks -phxass -nowarn=62 $< -o $@
	
%.asm : %.c
	$(VBCCM68K) -quiet $< -o=$@ -O=1 -no-cpp-warn -I. -I"$(VBCC_TARGET)/include" -I"$(NDK_1_3)/Includes1.3/include.h"

buffer.o : buffer.asm
command.o : command.asm
console.o : console.asm
crc.o : crc.asm
data.o : data.asm
error.o : error.asm
file.o : file.asm
main.o : main.asm
packet.o : packet.asm
serial.o : serial.asm
stream.o : stream.asm
trackdisk.o : trackdisk.asm

clean : 
	$(RM) $(OBJS) $(TARGET) $(ASMS)
