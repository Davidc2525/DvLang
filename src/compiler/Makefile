
CC = gcc
CXX = g++
DEBUG=false
DVVM = -L../../out/ -I../ -lDvVm
# -Wall  -Wextra

PROTO_BUF = -pthread -lprotobuf -pthread -lpthread
FLAGS = $(DVVM) -ldl -lffi  -Wall  -Wextra -g -Dcpu_arch=64
CFLAGS = -std=c99 $(FLAGS)
CXXFLAGS = -std=c++17 $(FLAGS)

LIB_TARGET = ../../out/libDvc.a

LIB_OBJ =   src/compiler/generate/symbol_table/symbol_table.o \
			src/compiler/generate/symbol_table/obj.o \
			src/compiler/generate/symbol_table/struct.o \
			src/compiler/generate/code_generator/code_generator.o \
			src/compiler/generate/code_generator/operand.o \
			src/compiler/compiler.o \
			src/parser_params/params.o \
			src/parser_params/Parser.o \
			src/parser_params/Scanner.o \
			src/parser/Parser.o \
			src/parser/Scanner.o

TARGET =  ../../out/Dvc
OBJ =  src/main.o $(LIB_TARGET)

compile: $(TARGET)
	printf "\n\033[1;32mDvCompiler compilado\033[0;0m\n"


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

build_parser:
	cococpp -namespace PARSER -o src/parser -frames resources/frames/  -trace AF resources/DvLang_gramatica.1.atg
	rm src/parser/*.old &

build_parser_es:
	cococpp -namespace PARSER -o src/parser -frames resources/frames/  resources/DvLang_gramatica_es.atg
	rm src/parser/*.old &

build_parser_params:
	cococpp -namespace PARSER_PARAMS -o src/parser_params -frames resources/frames/  resources/params.atg
	rm src/parser_params/*.old &

clear:
	rm src/parser_params/*.old &
	rm src/parser_params/*.o &
	rm src/parser/*.old &
	rm src/parser/*.o &
	rm src/compiler/generate/symbol_table/*.o &
	rm src/compiler/generate/code_generator/*.o &
	rm src/compiler/*.o &
	rm src/*.o &
	#rm src/parser/*.o &
