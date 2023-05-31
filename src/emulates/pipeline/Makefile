CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

# define library name generated
LIBOUT = pipelines.a
# define source file
SOURCES = input.c output.c inputformat.c
# define object file
OBJECTS = $(SOURCES:.c=.o)
LIBSRC = ../Util.a
# define library flags
LIBFLAGS = rcs
# The r flag specifies that the library should be created or updated, the c flag specifies that the library should be created if it does not exist, and the s flag specifies that an index of the symbols in the library should be created.

.PHONY: all clean
all: libs $(LIBOUT)

$(LIBOUT): $(OBJECTS)
	ar $(LIBFLAGS) $(LIBOUT) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(LIBOUT)

libs:
	cd ../; make

# input.o: input.c input.h inputformat.o
# inputformat.o: inputformat.c inputformat.h
# output.o: output.c output.h
