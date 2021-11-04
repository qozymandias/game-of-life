bin/main: bin/main.o 
	g++ -g -o bin/main bin/main.o -std=c++11 -Wall -pedantic -lncurses -pthread

bin/main.o: src/main.cpp src/life.hpp src/frontend.hpp
	mkdir -p bin	
	g++ -g -c -o bin/main.o src/main.cpp

clean:
	rm bin/*
