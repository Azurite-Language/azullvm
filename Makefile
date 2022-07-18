prellvm:
	g++ *.cpp visitor/*.cpp -o prellvm -g -fsanitize=address `llvm-config --cxxflags --ldflags --system-libs --libs core`

all: prellvm

compile: prellvm
	./prellvm  2>output.ll && llc output.ll && gcc output.s
