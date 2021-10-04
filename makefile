# server:server.o http_conn.o log.o tool.o
# 	g++ server.o http_conn.o log.o tool.o  -lyaml-cpp -lpthread -L/usr/local/lib -I/usr/local/include  -g  -o server

# serevr.o:server.cpp
# 	g++ server.cpp -c -g

# http_conn.o:http_conn.cpp
# 	g++ http_conn.cpp -c -g

# log.o:log.cpp
# 	g++ log.cpp -c -g

# tool.o:./tools/tool.cpp
# 	g++ ./tools/tool.cpp -c -g


server:server.cpp log.cpp ./tools/tool.cpp http_conn.cpp
	g++ -g server.cpp log.cpp ./tools/tool.cpp http_conn.cpp -lyaml-cpp -lpthread -L/usr/local/lib -I/usr/local/include    -o server

clean:
	rm server 