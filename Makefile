SRC_DIR=src

INCLUDE_DIR=${SRC_DIR}/inc ${SRC_DIR}
OBJ_DIR=obj
SRC=$(wildcard ${SRC_DIR}/**/*.cpp) $(wildcard ${SRC_DIR}/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC:.cpp=.o))

CXXFLAGS=-g -Wall -Wextra -std=c++17 `llvm-config-10 --cxxflags` `llvm-config-10 --ldflags --libs all`
CXXFLAGS+= $(patsubst %,-I %,$(INCLUDE_DIR))

all: folders prellvm

folders:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c $< -o $(OBJ_DIR)/$(@F) $(CXXFLAGS)


prellvm: $(OBJS)
	g++ $(OBJ_DIR)/*.o -o prellvm $(CXXFLAGS)


compile: prellvm
	./prellvm output.prellvm  2>output.ll && llc output.ll && gcc output.s

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) $(OBJS) prellvm output.prellvm output.ll output.s a.out