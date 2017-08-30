CC=gcc
CFLAGS=-g -Wall -O3 -I /usr/local/include/modbus/
LDLIBS=-L /usr/local/lib/ -lmodbus 

TARGET=client_util

%: %.c %.h
	$(CC) -c $(CFLAGS) $@.c
	$(CC) -o $@ $(LDLIBS) $@.o

all: $(TARGET)

clean:
	rm *.o $(TARGET)
