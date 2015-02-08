FLAGS = -ansi -pedantic -Wall -Werror

all: bin ls rshell move 

rshell:
	g++ src/rshell.cpp -o rshell $(CPPFLAGS)

ls:
	g++ src/ls.cpp -o ls $(CPPFLAGS)

bin:
	mkdir bin

move: 
	mv rshell bin; mv ls bin

clean:
	rm -r bin
