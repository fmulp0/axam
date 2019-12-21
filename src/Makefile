
#	vc -bamigahunk -x -Bstatic -Cvbcc -nostdlib -Z -mrel -L/home/lkornwinkel/Development/Amiga/target/lib -lamiga $^ -o $@ 
#-I/home/lkornwinkel/Development/Amiga/NDK_1.3/Includes1.3/include.h 
SRCS=trackdisk.c error.c command.c main.c serial.c
OBJS=trackdisk.o error.o command.o main.o serial.o

TARGET=axam-0.4.0

$(TARGET) : $(OBJS)
	vlink -bamigahunk -x -Bstatic -Cvbcc -Z -mrel /home/lkornwinkel/Development/Amiga/target/lib/startup.o $^ -L/home/lkornwinkel/Development/Amiga/target/lib -lvc -lamiga -lauto -o $@

%.o : %.c
	vc -I/home/lkornwinkel/Development/Amiga/target/include -o $@ -c $<

clean : 
	rm -f $(OBJS) $(TARGET)
