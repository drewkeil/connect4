#include "connect4.h"
#include <cassert>
#include <thread>

connect4::connect4():red(0), yellow(0), onTurn(0){}

connect4::connect4(connect4& other)
    :red(other.red), yellow(other.yellow), onTurn(other.onTurn){
    for(int i=0;i<7;++i)
        placeable[i]=other.placeable[i];
}

bool connect4::place(int col){
    //assert(col>=0&&col<7);
    if(placeable[col]&127ull)
        return false;
    uint64_t& board=(onTurn%2) ? yellow:red; //TODO: computing this once then passing it around like ethan's does could definintly be faster
    board|=(placeable[col]);
    placeable[col]>>=8;
    ++onTurn;
    return true;
}

void connect4::print_board(std::ostream& os){
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

void connect4::unplace(int col){ //  i always know what most recent col is, colsPlaced array is unnecessary, can have int col argument
    --onTurn;
    uint64_t& board=(onTurn%2) ? yellow:red;
    placeable[col]<<=8;
    board^=placeable[col];
}

int connect4::on_turn(){
    return onTurn;
}

bool connect4::check_win(){
    const uint64_t& board=onTurn%2 ? red:yellow; // red and yellow switched here because i want the player whose turn it isn't
    
    // verticle
    uint64_t test=board&(board>>16);  //TODO: might be faster to make temps and | them instead of all the if's
    if(test&(test>>8))
        return true;

    // horizontal
    test=board&(board>>2);
    if(test&(test>>1))
        return true;

    // first diagonal
    test=board&(board>>14);
    if(test&(test>>7))
        return true;

    // second diagonal
    test=board&(board>>18);
    if(test&(test>>9))
        return true;

    return false;
}

uint64_t connect4::get_hash(){
    //uint64_t hash=;
    //for(int i=0;i<7;++i)  // try lots of | instaad of for (probably not faster), it's like way faster?
    //    hash|=placeable[i];
    //return hash;
    return (onTurn%2 ? yellow:red)|placeable[0]|placeable[1]|placeable[2]|placeable[3]|placeable[4]|placeable[5]|placeable[6];
}
