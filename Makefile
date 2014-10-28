CC	:= gcc
SRC	:= nclife.c
LFLAGS  := -lcurses -lm
CFLAGS	:= -Wall -Wextra -pedantic-errors -std=gnu99 $(LFLAGS)
DFLAGS	:= -g -pg -O0
INSTALL	:= /usr/local/bin/nclife
OUT	:= nclife

all:	std

std:	$(SRC)
	$(CC) $(CFLAGS) -O3 -o $(OUT) $?

clean:	$(SRC)
	rm -f $(OUT) gmon.out

debug:	$(SRC)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUT) $?

install: std
	install $(OUT) $(INSTALL)
	strip $(INSTALL)
