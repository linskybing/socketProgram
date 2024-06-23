
main: server.cpp src/socket.cpp
	g++ -O3 -o server server.cpp -std=c++17
clean: 
	rm server