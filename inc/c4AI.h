#ifndef C4AI_H
#define C4AI_H

#include "connect4.h"
#include "transpositionTable.h"
#include <cstdint>
#include <vector>

#ifndef NDEBUG
	#define resetCounters() do{ \
		numCutoffs = 0; \
		ttCutoffs = 0; \
		for(int __i = 0;__i<7;++__i) \
			cutoffOrderIndex[__i] = 0; \
		}while(false)
	#define cutoff() ++numCutoffs
	#define ttCutoff() ++ttCutoffs
	#define cutoffIndex(idx) ++cutoffOrderIndex[(idx)]
#else
	#define resetCounters()
	#define cutoff()
	#define ttCutoff()
	#define cutoffIndex(idx)
#endif

class c4AI{
public:
	transpositionTable ttable;
	uint64_t numSearched;
#ifndef NDEBUG
	uint64_t numCutoffs;
	uint64_t ttCutoffs;
	uint64_t cutoffOrderIndex[7];
#endif
	
	// attempts to find the optimal next move in a given connect 4 game
	// REQUIRES: the search has been initialized
	int next_move(connect4& c4, int turn, std::vector<int>& scores);

	//  returns the number of leaf nodes evaluated during most recent call of next_move()
	uint64_t positions_searched();

	// sets up the transposition table and does a search to [depth], filling it with useful information to be used in all future searches
	 // also return score for testing purposes
	// WARNING: gives inaccurate results for any depth less than 42, depths less than 42 should be used only for testing purposes
	int initialize_search(int depth, connect4& c4);

private:
	int initializedDepth=0;
	bool stopped=false;
	
	// gives a numerical evaluation representing whether a position is winning, losing, or a draw
	// searches [depth] turns ahead
	int evaluate_board(connect4& c4, int depth, int alpha, int beta);
	
	// orders the moveOrder array to (hopefully) be in order of best to worst
	int order_moves(connect4& c4, uint8_t moveOrder[]);

	//  evaluates the position as it is, from the perspective of the player whose turn it isn't
	int static_evaluate(connect4& c4);

	//  a function to be used by an individual thread to search a position
	//  thread search does not work
	void thread_search(connect4 c4, int depth, int threadNum);

	void thread_order_moves(uint8_t moveOrder[], int threadNum);

};

#endif
