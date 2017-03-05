.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

all:
	gcc train.c -o train
	gcc test.c -o test
# type make/make all to compile test_hmm

clean:
	rm -f train
	rm -f test
# type make clean to remove the compiled file
