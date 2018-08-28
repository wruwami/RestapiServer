CC	= g++

RestapiHttpServer	: RestapiHttpServer.o Server.o Acceptor.o Service.o
	g++ -o RestapiHttpServer RestapiHttpServer.o Server.o Acceptor.o Service.o -lboost_system -pthread

RestapiHttpServer.o : RestapiHttpServer.cpp
	g++ -std=c++11 -c RestapiHttpServer.cpp -o RestapiHttpServer.o
	
Server.o :
	g++ -std=c++11 -c Server.cpp -o Server.o

Acceptor.o :
	g++ -std=c++11 -c Acceptor.cpp -o Acceptor.o

Service.o :
	g++ -std=c++11 -c Service.cpp -o Service.o
	
clean :
	rm *.o RestapiHttpServer