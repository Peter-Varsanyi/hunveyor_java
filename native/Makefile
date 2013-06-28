#CC = gcc
CFLAGS += -Wall 
#LDFLAGS += $(shell xmlrpc-c-config abyss-server --libs)
CFLAGS += -Wall
#LDFLAGS += $(shell libusb-config --libs)



all: RPI

#rpi: RPI.c 
%: %.c
	$(CC) $^ -o $@

clean:
	rm -f rpi

