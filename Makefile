CXX = g++
C++FLAGS = -Wall -Werror -O3 -march=native -flto
INC = -Iinc
MAIN = playC4 c4Perft
MAINS = $(MAIN:%=obj/%.o)
SRCS = $(wildcard src/*.cpp)
OBJS = $(filter-out $(MAINS),$(SRCS:src/%.cpp=obj/%.o))

playC4: $(OBJS) obj/playC4.o
	$(CXX) $(C++FLAGS) -fwhole-program $(INC) $^ -o $@

c4Perft: $(OBJS) obj/c4Perft.o
	$(CXX) $(C++FLAGS) -fwhole-program $(INC) $^ -o $@

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(C++FLAGS) $(INC) -c $^ -o $@

clean:
	rm -rf obj c4Perft playC4
