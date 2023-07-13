C++FLAGS ?= -Wall -Werror -pedantic -g --std=c++11 -Wno-sign-compare -Wno-comment

playC4.exe: playC4.cpp connect4.cpp c4AI.cpp transpositionTable.cpp
	g++ $(C++FLAGS) playC4.cpp connect4.cpp c4AI.cpp transpositionTable.cpp -o playC4.exe

c4Perft.exe: c4Perft.cpp connect4.cpp c4AI.cpp transpositionTable.cpp
	g++ $(C++FLAGS) c4Perft.cpp connect4.cpp c4AI.cpp transpositionTable.cpp -o c4Perft.exe

clean:
	rm *.exe