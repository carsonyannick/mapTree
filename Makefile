all : mapTree

mapTree: main.o socket.o
	g++  -g main.o socket.o -o mapTree

main.o: main.cpp socket.h
	g++ -c -g main.cpp

socket.o: socket.cpp
	g++ -c -g socket.cpp

clean:
	rm *.o mapTree
