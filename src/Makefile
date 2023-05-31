CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

LIBSRCEMULATE = emulates/pipeline/pipelines.a emulates/Util.a
# .SUFFIXES: .c .o  # Suffixes for implicit rules so that .o would not be deleted automatically

.PHONY: all clean

all: libemulates emulate assemble 

assemble: assemble.o
# emulate: emulate.o emulates/pipeline/input.o emulates/pipeline/output.o emulates/memory.o emulates/pipeline/inputformat.o emulates/Util.o
emulate: emulate.o $(LIBSRCEMULATE)
	$(CC) $(CFLAGS) -o $@ $^

# .c.o:
# 	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o
	rm -rf **/**/*.a
	rm -rf **/**/*.o
	rm -rf **/*.a
	rm -rf **/*.o

# writing dependencies for emulate
libemulates:
	cd emulates/pipeline; make
	cd emulates; make


# emulate.o: emulate.c emulate.h 
# emulates/pipeline/input.o: emulates/pipeline/input.c emulates/pipeline/input.h emulates/pipeline/inputformat.o emulates/Util.o emulates/memory.o
# emulates/pipeline/inputformat.o: emulates/pipeline/inputformat.c emulates/pipeline/inputformat.h emulates/Util.o emulates/memory.o
# emulates/pipeline/output.o: emulates/pipeline/output.c emulates/pipeline/output.h emulates/memory.o
# emulates/memory.o: emulates/memory.h emulates/memory.c
# emulates/Util.o: emulates/Util.c emulates/Util.h


# # writing dependencies for assemble



