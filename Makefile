SRC_DIR=src
TARGET=bin/azullvm
INCLUDE_DIR=${SRC_DIR}/inc ${SRC_DIR}
OBJ_DIR=obj
SRC=$(wildcard ${SRC_DIR}/**/*.cpp) $(wildcard ${SRC_DIR}/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC:.cpp=.o))

CXXFLAGS=-g -Wall -Wextra `llvm-config --cxxflags --ldflags --system-libs --libs core`
CXXFLAGS+= $(patsubst %,-I %,$(INCLUDE_DIR))

all: folders $(TARGET)

folders:
	mkdir -p bin
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c $< -o $(OBJ_DIR)/$(@F) $(CXXFLAGS)


$(TARGET): $(OBJS)
	g++ $(OBJ_DIR)/*.o -o $(TARGET) $(CXXFLAGS)


compile: $(TARGET)
	./$(TARGET) output.prellvm  2>output.ll && llc output.ll && gcc output.s

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) $(OBJS) $(TARGET) output.prellvm output.ll output.s a.out