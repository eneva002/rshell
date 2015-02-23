FLAGS = -ansi -pedantic -Wall -Werror
STD = -std=c++11

all: bin rshell ls 

rshell:
	g++ $(STD) src/rshell.cpp -o bin/rshell $(CPPFLAGS)

ls:
	g++ $(STD) src/ls.cpp -o bin/ls $(CPPFLAGS)

bin:
	mkdir bin

clean:
	rm -r bin
