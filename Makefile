FLAGS = -ansi -pedantic -Wall -Werror

all: bin rshell move 

rshell:
	g++ src/rshell.cpp -o rshell $(CPPFLAGS)

bin:
	mkdir bin

move: 
	mv rshell bin

clean:
	rm -r bin
