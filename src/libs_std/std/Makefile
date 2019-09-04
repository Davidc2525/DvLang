CC = gcc
CXX = g++

#EFLAGS = -fpic para compilar libreria std
EFLAGS = -fpic
PROTO_BUF = -pthread -lprotobuf -pthread -lpthread
FLAGS = $(EFLAGS) -I../../ -Wall  -Wextra -g  -Dcpu_arch=64

CFLAGS = -std=c11 $(FLAGS)
CXXFLAGS = -std=c++17 $(FLAGS)
#CXXFLAGS="-std=c++17 $(FLAGS) $(pkg-config --cflags protobuf)"
#LIBS="$(pkg-config --libs protobuf)"

DEBUG=false

LIB_STD_TARGET = libstd.so
LIB_STD_OBJ = std.o \
				../../heap/heap.o \
				../../strings/strings.o \
				../../numbers/numbers.o 



compile_std:$(LIB_STD_TARGET)
	printf "\n\033[1;32mLibstd compilado\033[0;0m\n"

$(LIB_STD_TARGET):$(LIB_STD_OBJ)
	printf "\033[1;33mlibstd.so: \033[1;34m%s\033[0;0m \033[0;0m\n" $@
	$(CXX) -shared -o $@ $^ 

%.o: %.cpp
	printf "\033[1;36mCompilando c++: \033[1;34m%s\033[0;0m -> \033[1;35m%s\033[0;0m  \033[0;0m\n" $^ $@
	$(CXX) -o $@ $(CXXFLAGS) -c $^ -DDEBUG=${DEBUG}

%.o: %.cc
	printf "\033[1;36mCompilando c++: \033[1;34m%s\033[0;0m -> \033[1;35m%s\033[0;0m  \033[0;0m\n" $^ $@
	$(CXX) -o $@ $(CXXFLAGS) -c $^ -DDEBUG=${DEBUG}

%.o: %.c
	printf "\033[1;36mCompilando c: \033[1;34m%s\033[0;0m -> \033[1;35m%s\033[0;0m  \033[0;0m\n" $^ $@
	$(CC) -o $@ $(CFLAGS) -c $^ -DDEBUG=${DEBUG}


clear:
	rm *.o & 
	rm *.so & 
	


