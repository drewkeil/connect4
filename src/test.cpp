#include "connect4.h"
#include "c4AI.h"

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

int main(int argc, char **argv){
	if(argc < 2){
		std::cerr << "test file not provided\n";
		return 1;
	}
	
	std::ifstream fin;
	fin.open(argv[1]);
	if(!fin.is_open()){
		std::cerr << "could not open " << argv[1] << '\n';
		return 1;
	}

	std::string line;
	uint64_t nodes = 0, time_ms = 0;
	int lines = 0;
	int wrong = 0;
	while(std::getline(fin, line)){
		++lines;
		std::cout << "                                                       \r";
		std::cout << "On line " << lines << ": " << line << '\r' << std::flush;
		int i;
		connect4 c4;
		c4AI solver;
		for(i=0;line[i]!=' ';++i){
			c4.place_legal(line[i] - '1');
		}
		auto begin = std::chrono::high_resolution_clock::now();
		int score = solver.initialize_search(42-i, c4);
		auto end = std::chrono::high_resolution_clock::now();
		time_ms += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		nodes += solver.positions_searched();

		int correct_score = atoi(line.substr(i+1).c_str());
		if(score != correct_score){
			std::cout << "                                                       \r";
			std::cout << "incorrect on line " << lines << " expected " << line.substr(i+1) << " got " << score <<  std::endl;
			++wrong;
		}
	}
	std::cout << "                                                       \r";
	std::cout << "total time:    " << (time_ms / 1000.0) << '\n';
	std::cout << "total nodes:   " << nodes << '\n';
	std::cout << "average time:  " << (time_ms / 1000.0) / lines << '\n';
	std::cout << "average nodes: " << nodes / lines << '\n';
	std::cout << lines - wrong << '/' << lines <<" correct\n";

	return 0;
}
