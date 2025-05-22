#ifndef CONNECT4_H
#define CONNECT4_H

#include <iostream>
#include <stdint.h>

// bitboard repersentation is a little gross but doesn't really matter because it's still 64 bits either way (i think)

enum column:uint64_t{
	FIRST=282578800148737ull,
	SECOND=FIRST<<1,
	THIRD=SECOND<<1,
	FOURTH=THIRD<<1,
	FIFTH=FOURTH<<1,
	SIXTH=FIFTH<<1,
	SEVENTH=SIXTH<<1
};

class connect4{
public:
	
	// creates an empty game board with red to place
	connect4();

	connect4(const connect4& other);

	// prints the board to os
	void print_board(std::ostream& os, bool redTurn);

	// places a piece in column col, color is determined by whose turn it is, returns true if placing succeds
	// does nothing and returns false if column col is already filled
	// REQUIRES: 0<=col<7
	bool place(int col);

	// undoes the most recent placement in column [col]
	void unplace(int col);

	// returns true if the most recent turn resulted in a win
	bool check_win();

	// does what you think
	uint64_t get_hash();

private:
	uint64_t friends;
	uint64_t enemies;
	uint64_t placeable[7]={1ull<<48,2ull<<48,4ull<<48,8ull<<48,16ull<<48,32ull<<48,64ull<<48};
	
	friend class c4AI;
	
};






#endif
