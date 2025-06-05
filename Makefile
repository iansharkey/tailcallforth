CC = clang
CFLAGS = -Wno-int-conversion -Os -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables -Wno-int-to-void-pointer-cast -Wno-void-pointer-to-int-cast
TARGET = tailcallforth
SOURCES = core.c crt.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS)

%.o: %.c core.h
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)
