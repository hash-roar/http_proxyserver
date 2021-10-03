server:server.cpp http_conn.cpp
		g++ server.cpp http_conn.cpp log.cpp ./tools/tool.cpp -g -lpthread -o server
clean:
	rm server

