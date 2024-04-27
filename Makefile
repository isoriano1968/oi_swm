CC = gcc
PROGRAM = oi_swm_dbus_7
CFLAGS = `pkg-config --cflags dbus-1 json-c gtk+-3.0`
LIBS = `pkg-config --libs dbus-1 json-c gtk+-3.0`

all:
	$(CC) $(CFLAGS) $(LIBS) $(PROGRAM).c -o $(PROGRAM)
clean:

