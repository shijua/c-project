CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic

# define library name generated
LIBOUT = DP.a
# define source file
SOURCES = bitwiseShift.c DPI.c DPR.c
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



