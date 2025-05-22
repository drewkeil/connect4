#include "connect4.h"
#include <cassert>
#include <thread>

connect4::connect4():friends(0), enemies(0){}

connect4::connect4(const connect4& other)
	:friends(other.friends), enemies(other.enemies){
	placeable[0]=other.placeable[0];
	placeable[1]=other.placeable[1];
	placeable[2]=other.placeable[2];
	placeable[3]=other.placeable[3];
	placeable[4]=other.placeable[4];
	placeable[5]=other.placeable[5];
	placeable[6]=other.placeable[6];
}

bool connect4::place(int col){
	if(placeable[col]&127ull)
		return false;
	friends|=(placeable[col]);
	placeable[col]>>=8;
	uint64_t temp = friends;
	friends = enemies;
	enemies = temp;
	return true;
}

void connect4::print_board(std::ostream& os, bool redTurn){
	uint64_t& red    = redTurn ? friends:enemies;
	uint64_t& yellow = redTurn ? enemies:friends;

	os<<"    ";
	
	for(int i=8;i<56;++i){
		if(i%8==7){
			os<<"|\n    ";
			continue;
		}
		uint64_t bit=1ull<<i;
		if((bit)&red) //red
			os<<"|\033[91mO\033[0m";
		else if((bit)&yellow) //yellow
			os<<"|\033[93mO\033[0m";
		else //blank
			os<<"| ";
		
	}
	os<<" 1 2 3 4 5 6 7\n";
}

void connect4::unplace(int col){
	uint64_t temp = friends;
	friends = enemies;
	enemies = temp;
	placeable[col]<<=8;
	friends^=placeable[col];
}

bool connect4::check_win(){
	// verticle
	uint64_t test=enemies&(enemies>>16);
	if(test&(test>>8))
		return true;

	// horizontal
	test=enemies&(enemies>>2);
	if(test&(test>>1))
		return true;

	// first diagonal
	test=enemies&(enemies>>14);
	if(test&(test>>7))
		return true;

	// second diagonal
	test=enemies&(enemies>>18);
	if(test&(test>>9))
		return true;

	return false;
}

uint64_t connect4::get_hash(){
	return friends|placeable[0]|placeable[1]|placeable[2]|placeable[3]|placeable[4]|placeable[5]|placeable[6];
}
