CC=gcc
CFLAGS=-Wall -Wextra -std=c99
targets=sfl

build:$(targets)

run_sfl:sfl.c
	 $(CC) $(CFLAGS) sfl.c -o sfl

clean:
	rm -f $(targets)

pack:
	zip -FSr 312CA_Celescu_Rares_Tema1.zip README Makefile *.c *.h
