
main: server.cpp src/socket.cpp
	g++ -o server server.cpp
clean: 
	rm server