#ifndef C4AI_H
#define C4AI_H

#include "connect4.h"
#include "transpositionTable.h"

class c4AI{
public:
    
    // attempts to find the optimal next move in a given connect 4 game
    // REQUIRES: the search has been initialized
    int next_move(connect4& c4);

    //  returns the number of leaf nodes evaluated during most recent call of next_move()
    uint64_t positions_searched();

    // sets up the transposition table and does a search to [depth], filling it with useful informationto be used in all future searches
    // WARRNING: gives innacurate results for any depth less than 42, depths less than 42 should be used only for testing purposes
    void initialize_search(int depth, connect4& c4);

private:
    uint64_t numSearched;
    transpositionTable ttable;
    int initializedDepth=0;
    bool stopped=false;
    
    // gives a numerical evaluation repersenting whether a position is winning, losing, or a draw
    // searches [depth] turns ahead
    int evaluate_board(connect4& c4, int depth, int alpha, int beta);
    
    // orders the moveOrder array to (hopefuly) be in order of best to worst
    int order_moves(connect4& c4, int moveOrder[]);

    //  evaluates the position as it is, from the perspective of the player whose turn it isn't
    int static_evaluate(connect4& c4);

    //  a function to be used by an individual thread to search a position
    //  currently slower, more research needed
    void thread_search(connect4 c4, int depth, int threadNum);

    void thread_order_moves(int moveOrder[], int threadNum);

};

#endif