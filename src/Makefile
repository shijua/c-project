CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

LIBSRCEMULATE = emulates/Util.a \
	emulates/pipeline/pipelines.a \
	emulates/single_data_transfer/single_data_transfer.a \
	emulates/special/special.a \
	emulates/branch/branch.a \
	emulates/data_processing/data_processing.a

LIBSRCASSEMBLE = assembles/Util.a \
	assembles/parser/parser.a \
	assembles/single_data_transfer/single_data_transfer.a \
	assembles/special/special.a \
	assembles/branch/branch.a \
	assembles/data_processing/data_processing.a
# .SUFFIXES: .c .o  # Suffixes for implicit rules so that .o would not be deleted automatically

.PHONY: all clean

all: runemulate runassemble

assemble: assemble.o $(LIBSRCASSEMBLE)
emulate: emulate.o $(LIBSRCEMULATE)
runemulate: libemulates emulate
runassemble: libassembles assemble

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o
	rm -rf **/**/*.a
	rm -rf **/**/*.o
	rm -rf **/*.a
	rm -rf **/*.o

copy:
	rm -r ../../armv8_testsuite/solution
	mkdir ../../armv8_testsuite/solution
	cp emulate ../../armv8_testsuite/solution
	cp assemble ../../armv8_testsuite/solution

# writing dependencies for emulate
libemulates:
	cd emulates/pipeline; make
	cd emulates; make

libassembles:
	cd assembles/parser; make
	cd assembles; make


