VERSION ?= dev
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c99 -DTERMV_VERSION=\"$(VERSION)\"
LDFLAGS = -lncurses
SRCDIR  = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/board.c $(SRCDIR)/piece.c \
          $(SRCDIR)/game.c $(SRCDIR)/render.c $(SRCDIR)/input.c \
          $(SRCDIR)/theme.c
TARGET  = termv

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
