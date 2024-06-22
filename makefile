
main: server.cpp src/socket.cpp
	g++ -o server2 server.cpp -std=c++17
clean: 
	rm server