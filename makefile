CXX=g++
Objects=_Socket.o main.o
exe:$(Objects)
	g++  $(Objects) -o exe
_Socket.o:_Socket.cc 
	g++ -c _Socket.cc  -o _Socket.o 
main.o:main.cpp 
	g++ -c main.cpp -o main.o