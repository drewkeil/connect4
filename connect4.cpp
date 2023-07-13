#include "connect4.h"
#include <cassert>

connect4::connect4():red(0), yellow(0),placeable(127ull<<48),onTurn(0){}

connect4::connect4(connect4& other)
    :red(other.red), yellow(other.yellow), placeable(other.placeable), onTurn(other.onTurn){
    for(int i=0;i<onTurn;++i)
        colsPlaced[i]=other.colsPlaced[i];
}

bool connect4::place(int col){
    assert(col>=0&&col<7);
    if(!(placeable&(columns[col]&~127ull)))
        return false;
    uint64_t& board=(onTurn%2) ? yellow:red;
    board|=(placeable&columns[col]);
    placeable^=(placeable>>8|placeable)&columns[col];
    colsPlaced[onTurn]=col;
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
    /*
    const uint64_t& us=onTurn%2 ? red:yellow;
    uint64_t empty=~(red|yellow|(columns[6]<<1)|255ull|(255ull<<56));  //  more not empty fixed directly down but it's worse
    
    //  horizontal  this one is good
    uint64_t test=(us<<2)&(us|empty);
    uint64_t potentialWins=(test&(test<<1))&~(empty&(empty<<1));
    
    //  verticle   this one is good, although looking for potential wins directly downward should do nothing so I am concerned
    test=(us<<16)&(us|empty);
    potentialWins|=(test&(test<<8))&~(empty&(empty<<8));

    //  diagonal 1   this one is bad
    test=(us<<14)&(us|empty);
    potentialWins|=(test&(test<<7))&~(empty&(empty<<7));

    //  diagonal 2   this one is bad
    test=(us<<18)&(us|empty);
    potentialWins|=(test&(test<<9))&~(empty&(empty<<9));
    
    //  horizontal   this one is good
    test=(us>>2)&(us|empty);
    potentialWins|=(test&(test>>1))&~(empty&(empty>>1));
    
    //  verticle   this one is good
    test=(us>>16)&(us|empty);
    potentialWins|=(test&(test>>8))&~(empty&(empty>>8));

    //  diagonal 1   this one is bad
    test=(us>>14)&(us|empty);
    potentialWins|=(test&(test>>7))&~(empty&(empty>>7));

    //  diagonal 2   this one is bad
    test=(us>>18)&(us|empty);
    potentialWins|=(test&(test>>9))&~(empty&(empty>>9));
    

    int count=0;
    while(potentialWins){
        potentialWins&=potentialWins-1;
        ++count;
    }
    */
    os<<" 1 2 3 4 5 6 7\n";
}

void connect4::unplace(){
    if(onTurn==0)
        return;
    --onTurn;
    uint64_t& board=(onTurn%2) ? yellow:red;
    board^=(placeable<<8)&columns[colsPlaced[onTurn]];
    placeable^=(placeable<<8|placeable)&columns[colsPlaced[onTurn]];
}

int connect4::on_turn(){
    return onTurn;
}

bool connect4::check_win(){
    const uint64_t& board=onTurn%2 ? red:yellow; // red and yellow switched here because i want the player whose turn it isn't
    
    // verticle
    uint64_t test=board&(board>>16);
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
    return placeable|(onTurn%2 ? yellow:red);
}