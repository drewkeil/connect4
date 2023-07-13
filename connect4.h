#ifndef CONNECT4_H
#define CONNECT4_H

#include <iostream>

// bitboard repersentation is a little gross but doesn't really matter because it's still 64 bits either way (i think)

enum column:uint64_t{
    FIRST=282578800148737ull, //  no binary literals in c++11 for some reason
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

    connect4(connect4& other);

    // prints the board to os
    void print_board(std::ostream& os);

    // places a piece in column col, color is determined by whose turn it is, returns true if placing succeds
    // does nothing and returns false if column col is already filled
    // REQUIRES: 0<=col<7 and col is not filled
    bool place(int col);

    // undoes the most recent placement
    void unplace();

    // returns the turn number
    int on_turn();
    
    // returns true if the most recent turn resulted in a win
    bool check_win();

    uint64_t get_hash();

private:
    uint64_t red;
    uint64_t yellow;
    uint64_t placeable;
    const column columns[7]={FIRST,SECOND,THIRD,FOURTH,FIFTH,SIXTH,SEVENTH};
    int onTurn;
    int colsPlaced[42];
    
    friend class c4AI;
    
};






#endif