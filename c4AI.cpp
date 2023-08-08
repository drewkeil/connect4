#include "c4AI.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <cstdlib>

int c4AI::next_move(connect4& c4){
    int depth=std::min(initializedDepth, 42-c4.onTurn);
    numSearched=0;
    for(int i=0;i<7;++i)
        std::thread(&c4AI::thread_search, this, std::ref(c4), initializedDepth).detach(); //*/
    int alpha=-1;//-(depth-1);
    int beta=1;//depth-1;
    int moveOrder[7]={3,4,2,1,5,0,6};
    int num=order_moves(c4, moveOrder);
    int bestCol=moveOrder[0];
    for(int i=0;i<num;++i){
        c4.place(moveOrder[i]);
        int score=-evaluate_board(c4, depth-1, -beta, -alpha);
        c4.unplace();
        if(score>alpha){
            alpha=score;
            bestCol=moveOrder[i];
        }
        if(alpha>=beta){
            return bestCol;
        }
    }
    return bestCol;
}

void c4AI::initialize_search(int depth, connect4& c4){
    numSearched=0;
    depth=std::max(depth,1);
    initializedDepth=depth;
    ttable.setup(depth, std::min(depth, 24));
    srand(23546);
    //  should remove this when implementing improvements so time isn't as effected by randomness
    //  should add a stop so it doesn't use more than 8 total by accident
    //for(int i=0;i<7;++i)  //  faster!!!!!!!!!!!! ZOOOOOOOOMMMMMM
    //    std::thread(&c4AI::thread_search, this, std::ref(c4), initializedDepth).detach();
    int moveOrder[7]={3,4,2,1,5,0,6};
    int num=order_moves(c4, moveOrder);
    int alpha=-1;//-(depth-1);
    int beta=1;//depth-1;
    for(int i=0;i<num;++i){
        c4.place(moveOrder[i]);
        int score=-evaluate_board(c4, depth-1, -beta, -alpha);
        c4.unplace();
        if(score>alpha){
            alpha=score;
        }
        if(alpha>=beta){
            return;
        }
    }
}

void c4AI::thread_search(connect4 c4, int depth){
    int alpha=-1;//-(depth-1);
    int beta=1;//depth-1;
    int moveOrder[7]={3,4,2,1,5,0,6};
    int num=order_moves(c4, moveOrder, true);
    for(int i=0;i<num;++i){
        c4.place(moveOrder[i]);
        int score=-evaluate_board(c4, depth-1, -beta, -alpha);
        c4.unplace();
        if(score>alpha){
            alpha=score;
        }
        if(alpha>=beta){
            return;
        }
    }
    return;
}

int c4AI::evaluate_board(connect4& c4, int depth, int alpha, int beta){
    if(depth==0){
        ++numSearched;
        return -c4.check_win();
    }
    if(c4.check_win())
        return -1;
    uint64_t hash=c4.get_hash();
    int val=ttable.get(hash, depth); 
    if(val)
        return val-2;
    int moveOrder[7]={3,4,2,1,5,0,6};
    int num=order_moves(c4, moveOrder);
    if(num==0)  //  it was returning alpha when reaching the end of the game at non-zero depth, whoops
        return -c4.check_win();
    int score=-1;
    for(int i=0;i<num;++i){
        c4.place(moveOrder[i]);
        score=std::max(score, -evaluate_board(c4, depth-1, -beta, -alpha));
        c4.unplace();
        alpha=std::max(score, alpha);
        if(alpha>=beta){
            ttable.set(hash, depth, alpha+2);
            return alpha;
        }
    }
    ttable.set(hash, depth, score+2);
    return score;
}

uint64_t c4AI::positions_searched(){
    return numSearched;
}

int c4AI::order_moves(connect4& c4, int moveOrder[], bool random){
    int scores[7]={};
    int notFull=0;  //  i should change what this is called but i'm too lazy
    bool blockNeeded=false;
    bool ignoredValid=false;
    for(int i=0;i<7;++i){
        if(c4.place(i)){
            if(c4.check_win()){
                scores[i]+=9999;
                notFull=1;
                c4.unplace();
                break;
            }//*/
            if(blockNeeded){
                c4.unplace();
                continue;
            }
            scores[i]+=static_evaluate(c4);
            if(c4.place(i)){
                if(c4.check_win()){
                    scores[i]-=50;
                    c4.unplace();
                    c4.unplace();
                    ignoredValid=true;
                    continue;
                }else
                    c4.unplace();
            }//*/
            c4.unplace();
            ++c4.onTurn;
            c4.place(i);
            if(c4.check_win()){
                notFull=0;
                blockNeeded=true;
                scores[i]+=999;
            }
            c4.unplace();
            --c4.onTurn;
            ++notFull;
            if(random)
                scores[i]+=rand()%10;
        }else
            scores[i]=-1000;  
    }
    
    notFull=std::max(notFull,1*ignoredValid); // this is probably unnecessary, but im not sure
    
    for(int i=0;i<6;++i){
        for(int j=i+1;(j>0)&&(scores[moveOrder[j]]>scores[moveOrder[j-1]]);--j){
            int temp=moveOrder[j-1];
            moveOrder[j-1]=moveOrder[j];
            moveOrder[j]=temp;
        }
    }
    return notFull;
}

int c4AI::static_evaluate(connect4& c4){
    const uint64_t& us=c4.onTurn%2 ? c4.red:c4.yellow;
    uint64_t empty=~(c4.red|c4.yellow|(c4.columns[6]<<1)|255ull|255ull<<56);

    
    //  horizontal
    uint64_t test=(us<<2)&(us|empty);
    uint64_t potentialWins=(test&(test<<1))&~(empty&(empty<<1));

    //  diagonal 1
    test=(us<<14)&(us|empty);
    potentialWins|=(test&(test<<7))&~(empty&(empty<<7));

    //  diagonal 2
    test=(us<<18)&(us|empty);
    potentialWins|=(test&(test<<9))&~(empty&(empty<<9));
    
    //  horizontal
    test=(us>>2)&(us|empty);
    potentialWins|=(test&(test>>1))&~(empty&(empty>>1));
    
    //  verticle   
    test=(us>>16)&(us|empty);
    potentialWins|=(test&(test>>8))&~(empty&(empty>>8));

    //  diagonal 1
    test=(us>>14)&(us|empty);
    potentialWins|=(test&(test>>7))&~(empty&(empty>>7));

    //  diagonal 2
    test=(us>>18)&(us|empty);
    potentialWins|=(test&(test>>9))&~(empty&(empty>>9));
    

    int count=0;
    while(potentialWins){
        potentialWins&=potentialWins-1;
        ++count;
    }

    return count;
}