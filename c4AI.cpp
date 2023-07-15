#include "c4AI.h"
#include <iostream>
#include <algorithm>
#include <thread>


int c4AI::next_move(connect4& c4, int depth){
    ttable.clear();
    depth=std::max(depth,1);
    ttable.setup(depth, std::min(depth, 20));
    numSearched=0;
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
            ttable.clear();
            return bestCol;
        }
    }
    ttable.clear();
    return bestCol;
}

void c4AI::thread_evaluate(connect4 c4,int depth, int* score){
    *score=-evaluate_board(c4, depth, -1, 1);
}

int c4AI::evaluate_board(connect4& c4, int depth, int alpha, int beta){
    if(depth==0){
        ++numSearched;
        return -c4.check_win();
    }
    if(c4.check_win())
        return -1;//return -depth;
    if(alpha==1)//(alpha>=(depth-1))
        return alpha;
    uint64_t hash=c4.get_hash();
    int val=ttable.get(hash, depth); 
    if(val)
        return val-2;
    int moveOrder[7]={3,4,2,1,5,0,6};
    int num=order_moves(c4, moveOrder);
    if(num==0)  //  it was returning alpha when reaching the end of the game at non-zero depth, whoops
        return -c4.check_win();
    for(int i=0;i<num;++i){
        c4.place(moveOrder[i]);
        alpha=std::max(alpha, -evaluate_board(c4, depth-1, -beta, -alpha));
        c4.unplace();
        if(alpha>=beta){
            ttable.set(hash, depth, alpha+2);
            return alpha;
        }
    }
    ttable.set(hash, depth, alpha+2);
    return alpha;
}

uint64_t c4AI::positions_searched(){
    return numSearched;
}

int c4AI::order_moves(connect4& c4, int moveOrder[]){
    int scores[7]={};
    int notFull=0;
    bool blockNeeded=false;
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
            /*if(c4.place(i)){  //  this searches less nodes but slower search overall
                scores[i]-=c4.check_win();  //  can't remove move from move pool as it could lead to computer thinking game has ended in a draw
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
        }else
            scores[i]=-1000;  
    }
    
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