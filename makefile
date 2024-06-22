
main: server.cpp src/socket.cpp
	g++ -o server server.cpp -std=c++17
clean: 
	rm server