#!/usr/bin/make -f

CFLAGS = -g -Wall -Wextra -std=c99 -pedantic -Wall -O2 -fpic -shared -Wno-unused-parameter
LDFLAGS = -ldl

.PHONY: clean

donothing.so: donothing.c
	${CC} ${CFLAGS} $< -o $@ ${LDFLAGS}

clean:
	${RM} donothing.so