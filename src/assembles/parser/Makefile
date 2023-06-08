CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

# define library name generated
LIBOUT = parser.a
# define source file
SOURCES = input.c parser.c ../Util.c ../symboltable.c ../parseformat.c
# define object file
OBJECTS = $(SOURCES:.c=.o)
# define library flags
LIBFLAGS = rcs

.PHONY: all clean
all: libs $(LIBOUT)

$(LIBOUT): $(OBJECTS)
	ar $(LIBFLAGS) $(LIBOUT) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(LIBOUT)

libs:
	cd ../single_data_transfer; make
	cd ../branch; make
	cd ../special; make
	cd ../data_processing; make
	cd ../; make