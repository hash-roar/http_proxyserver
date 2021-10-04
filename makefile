src=$(wildcard *.cpp)

obj=$(patsubst %.cpp, %.o, $(src))
target=server
CXXFLAGS=-g

$(target):$(obj)
	g++ -g $(obj)  -lyaml-cpp -lpthread -L/usr/local/lib -I/usr/local/include  -o $(target)

%.o:%.c
	g++ -g $< -c 

.PHONY:clean
clean:
	rm $(target) $(obj)