CXX		:=	clang++
FLAGS	:=	-Wall -Wextra -std=c++20 -O2

INCLUDE	:=	include
SRC		:=	src
BIN		:=	bin

LIBS	:=	-lglfw -lGL -lOpenEXR
EXE		:=	main

DEPS	:=	$(wildcard $(SRC)/*.c*)
OBJ		:=	$(patsubst $(SRC)/%.c*, $(BIN)/%.o, $(DEPS))

$(BIN)/%.o: $(SRC)/%.cpp
	mkdir -p $(BIN)
	$(CXX) -c -o $@ $< $(FLAGS) -I$(INCLUDE) -I/usr/include/Imath

$(BIN)/$(EXE): $(OBJ)
	mkdir -p $(BIN)
	$(CXX) $(FLAGS) -I$(INCLUDE) -I/usr/include/Imath -o $@ $^ $(LIBS) -fuse-ld=mold


run: $(BIN)/$(EXE)
		./$(BIN)/$(EXE)


start:
		clear
		./$(BIN)/$(EXE)

clean:
		rm -rf $(BIN)/*

setup:
	mkdir -p $(BIN)
	mkdir -p $(SRC)
	mkdir -p $(INCLUDE)
	touch $(SRC)/main.cpp
	echo '-Iinclude/' > compile_flags.txt
	echo -e "IndentWidth: 4\nBasedOnStyle: LLVM\nPointerAlignment: Left\nSortIncludes: false" > .clang-format
	clear
