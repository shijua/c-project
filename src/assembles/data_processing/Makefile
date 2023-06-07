CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic

# define library name generated
LIBOUT = data_processing.a
# define source file
SOURCES = data_processing.c
# define object file
OBJECTS = $(SOURCES:.c=.o)
# define library flags
LIBFLAGS = rcs

# .SUFFIXES: .c .o
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