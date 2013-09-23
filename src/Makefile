#!/usr/bin/env make

comp_flags=

ifdef CPPFLAGS
comp_flags+=$(CPPFLAGS)
endif

ifdef CFLAGS
comp_flags+=$(CFLAGS)
endif

comp_flags+=-g -Wall -Wextra -Werror -std=c99

ifdef NDEBUG
comp_flags+=-DNDEBUG=1 -O3
else
comp_flags+=-UNDEBUG -O0
endif

link_flags =
sources = $(wildcard *.c)
headers = $(wildcard *.h)
objects = $(sources:.c=.o)

json-parser: $(objects)
	$(CC) $(comp_flags) -o $@ $^ $(link_flags)

%.o: %.c $(headers)
	$(CC) -c -fPIC $(comp_flags) -o $@ $<

clean:
	rm -f *.o json-parser
