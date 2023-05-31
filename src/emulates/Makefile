CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

LIBOUT = Util.a
SOURCES = Util.c
OBJECTS = $(SOURCES:.c=.o)
LIBFLAGS = rcs

# .SUFFIXES: .c .o
.PHONY: all clean
all: $(LIBOUT)

$(LIBOUT): $(OBJECTS)
	ar $(LIBFLAGS) $(LIBOUT) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(LIBOUT)

# dependency
# memory.o: memory.c memory.h
# Util.o: Util.c Util.h


