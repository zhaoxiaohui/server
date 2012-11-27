all: hps

hps: main.o server.o connection.o request_handler.o reply.o request_parser.o mime_types.o log.o
	g++ -o hps -lboost_thread -lboost_system -lboost_filesystem -lpthread main.o server.o log.o connection.o request_handler.o reply.o request_parser.o mime_types.o

main.o: main.cpp server.hpp
	g++ -o main.o -c main.cpp

server.o: server.hpp server.cpp connection.hpp request_handler.hpp
	g++ -o server.o -c server.cpp

request_handler.o: request_handler.hpp request_handler.cpp
	g++ -o request_handler.o -c request_handler.cpp

connection.o: connection.hpp connection.cpp reply.hpp request.hpp request_parser.hpp request_handler.hpp log.hpp
	g++ -o connection.o -c connection.cpp

reply.o: reply.hpp reply.cpp header.hpp
	g++ -o reply.o -c reply.cpp

request_parser.o: request_parser.hpp request_parser.cpp mime_types.hpp
	g++ -o request_parser.o -c request_parser.cpp

mime_types.o: mime_types.hpp mime_types.cpp
	g++ -o mime_types.o -c mime_types.cpp

log.o: log.hpp log.cpp
	g++ -o log.o -c log.cpp
