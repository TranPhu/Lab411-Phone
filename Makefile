CC = gcc
send = mcsend.c
recev = mcreceive.c
sources = mcv4l2.c	\
	mcgtk.c		\
	mcsocket.c	\
	mccodec.c	\
	mcudp.c		\
	mcosip.c

object_send 	= lab-phone
object_recev 	= recev

libs := $(shell pkg-config --cflags --libs gtk+-2.0)
libs += $(shell pkg-config --cflags --libs libavcodec)
libs += $(shell pkg-config --cflags --libs libv4l2)
libs += $(shell pkg-config --cflags --libs libswscale libv4lconvert)
libs += -leXosip2

all:
	$(CC) -Wall -O2 $(send) $(sources) -o $(object_send) $(libs)
# 	$(CC) -Wall -O2 $(recev) $(sources) -o $(object_recev) $(libs)
clean:
	rm $(object_send)
	rm *~