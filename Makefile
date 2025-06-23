CXX = g++
C++FLAGS = -Wall -Werror -O3 -march=native -flto
INC = -Iinc
BINS = playC4 c4Perft profile
MAINS = $(BINS:%=obj/%.o)
SRCS = $(wildcard src/*.cpp)
OBJS = $(filter-out $(MAINS),$(SRCS:src/%.cpp=obj/%.o))

playC4: $(OBJS) obj/playC4.o
	$(CXX) $(C++FLAGS) -fwhole-program $(INC) $^ -o $@

c4Perft: $(OBJS) obj/c4Perft.o
	$(CXX) $(C++FLAGS) -fwhole-program $(INC) $^ -o $@

profile: C++FLAGS += -g3
profile: $(OBJS) obj/profile.o
	$(CXX) $(C++FLAGS) -fwhole-program $(INC) $^ -o $@

run_perf: profile
	perf record -F 499 --call-graph dwarf -b ./profile

obj/%.o: src/%.cpp inc/%.h | obj
	$(CXX) $(C++FLAGS) $(INC) -c $< -o $@

obj/%.o: src/%.cpp | obj
	$(CXX) $(C++FLAGS) $(INC) -c $< -o $@

obj: 
	mkdir -p obj

clean:
	rm -rf $(BINS) obj 
