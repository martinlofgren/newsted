
SRCS   	= $(wildcard src/*.c)

CFLAGS 	= -Wall -g

test : $(SRCS)
	cc $(SRCS) -o test $(CFLAGS) -lm

.PHONY  : clean
clean   :
	-rm test

