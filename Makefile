CC = gcc
CXX = g++

#EFLAGS = -fpic para compilar libreria std
EFLAGS = #-O1
PROTO_BUF = -pthread -lprotobuf -pthread -lpthread
FLAGS = $(EFLAGS) -ldl -lffi -Wall -lpthread -Wextra -g  -Dcpu_arch=64  -DMAX_HEAP_PROGRAM=1000000

CFLAGS = -std=c99 $(FLAGS)
CXXFLAGS = -std=c++17 $(FLAGS)
#CXXFLAGS="-std=c++17 $(FLAGS) $(pkg-config --cflags protobuf)"
#LIBS="$(pkg-config --libs protobuf)"

DEBUG=false

LIB_TARGET = out/libDvVm.a
LIB_OBJ = 	src/numbers/numbers.o \
			src/process/strings_sets.o \
			src/strings/strings.o \
			src/heap/heap.o \
			src/stack/estack.o \
			src/program/program.o \
			src/process/cpu.o \
			src/process/process.o \
			src/process/lib.o \
			src/process/native.o \
			src/process/natives.o \
			src/DvVm/Vm.o
			#src/proto/objFile.pb.o descomentar para protobuf
			#src/main.o


TARGET = out/DvVm
TARGET_DEBUG = out/DvVmDB

OBJ =  src/main.o $(LIB_TARGET)


compile: $(TARGET)
	printf "\n\033[1;32mDvVm compilado\033[0;0m\n"


$(LIB_TARGET): $(LIB_OBJ)
	printf "\033[1;33mEmpaquetando: \033[1;34m%s\033[0;0m \033[0;0m\n" $@
	ar rcs $@ $^

$(TARGET): $(OBJ)
	printf "\033[1;31mCompilando principal: \033[1;34m%s\033[0;0m \033[0;0m\n" $@
	$(CXX) -o $@ $^ $(CXXFLAGS) -DDEBUG=${DEBUG}

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
	rm src/numbers/numbers.o &
	rm src/program/program.o &
	rm src/strings/strings.o &
	rm src/process/*.o &
	rm src/heap/heap.o &
	rm src/DvVm/Vm.o &
	rm src/stack/estack.o &
	#rm src/proto/*.o &
	#rm src/main.o &



#SRC_FILES = src/numbers/* src/types.h src/program/* src/process/* src/proto/* src/heap/* src/DvVm/*  src/stack/* src/main.cpp

#LIBS =  -Ilib/bfp-master/lib -pthread  -lprotobuf -pthread -lpthread
##-I/usr/local/include -L/usr/local/lib -faggressive-loop-optimizations -O3
#CPP_FLAGS= -Dcpu_arch=64 -Wall  -Wextra -g -std=c++11 -DMAX_HEAP_PROGRAM=1000

#CPP = g++

#compile:
#	${CPP} ${SRC_FILES} ${CPP_FLAGS} ${LIBS} -DDEBUG=false -o out/DvVm

#compile_debug:
#	${CPP} ${SRC_FILES} ${CPP_FLAGS} ${LIBS} -DDEBUG=true -o out/DvVmDB
#
#run:
#	./out/DvVm ${ARGS}

#compile_run:  compile
#	./out/DvVm ${ARGS}

#clear:
#	rm out/*

compile_protos:
	protoc --cpp_out=src/proto objFile.proto

#proto_java:
#	mkdir java_proto
#	protoc --java_out=./java_proto/ objFile.proto
