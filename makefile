all: cofi

cofi: main.o server.o
	g++ main.o server.o -o -lpqxx -lpq cofi

main.o: server.h main.cpp
	g++ -c main.cpp

server.o: server.h server.cpp
	g++ -c server.cpp

clean: 
	rm -rf *o cofi
