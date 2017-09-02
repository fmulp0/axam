SC=sc
CFLAGS=ansi

all : axam

serial.o : serial.c serial.h
	sc serial.c ansi

trackdisk.o : trackdisk.c trackdisk.h
	sc trackdisk.c ansi

# cmd_common.o : cmd_common.c sercom.h
# 	sc cmd_common.c ansi

#cmd_get_ftype.o : cmd_get_ftype.c sercom.h
#	sc cmd_get_ftype ansi

# cmd_msg.o : cmd_msg.c sercom.h
# 	sc cmd_msg.c ansi
#
# cmd_put_file.o : cmd_put_file.c sercom.h
# 	sc cmd_put_file.c ansi
#
# cmd_put_image.o : cmd_put_image.c sercom.h
# 	sc cmd_put_image.c ansi

command.o: command.c command.h
	sc command.c ansi

main.o : main.c
	sc main.c ansi

axam : main.o command.o serial.o trackdisk.o
	slink lib:c.o,command.o,main.o,serial.o,trackdisk.o to axam lib lib:sc.lib lib lib:amiga.lib

clean:
	delete axam main.o command.o serial.o trackdisk.o
	
# axam : main.o sercom.o cmd_common.o cmd_get_ftype.o cmd_msg.o cmd_put_file.o
# 	slink lib:c.o,cmd_common.o,cmd_msg.o,cmd_put_file.o,main.o,sercom.o to axam lib lib:sc.lib lib lib:amiga.lib
#	lc link to axfer cmd_common.o,cmd_get_ftype.o,cmd_msg.o,cmd_put_file.o,main.o,sercom.o
