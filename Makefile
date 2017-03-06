
SRCS   	= $(wildcard src/*.c)

CFLAGS 	= -Wall

test : $(SRCS)
	cc $(SRCS) -o test $(CFLAGS)

.PHONY  : clean
clean   :
	-rm test

