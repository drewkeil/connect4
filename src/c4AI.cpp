#include "c4AI.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <cstdlib>

int c4AI::next_move(connect4& c4, int turn){
	stopped=false;
	int depth=std::min(initializedDepth, 42-turn);
	numSearched=0;
	int alpha=-1;//-(depth-1);
	int beta=1;//depth-1;
	uint8_t moveOrder[7]={3,4,2,1,5,0,6};
	int num=order_moves(c4, moveOrder);
	int bestCol=moveOrder[0];
	for(int i=0;i<num;++i){
		c4.place_legal(moveOrder[i]);
		int score=-evaluate_board(c4, depth-1, -beta, -alpha);
		c4.unplace(moveOrder[i]);
		if(score>alpha){
			alpha=score;
			bestCol=moveOrder[i];
		}
		if(alpha>=beta){
			stopped=true;
			return bestCol;
		}
	}
	stopped=true;
	return bestCol;
}

void c4AI::initialize_search(int depth, connect4& c4){
	stopped=false;
	numSearched=0;
	depth=std::max(depth,1);
	initializedDepth=depth;
	ttable.setup(depth, std::min(depth, 24));
	uint8_t moveOrder[7]={3,4,2,1,5,0,6};
	int num=order_moves(c4, moveOrder);
	int alpha=-1;//-(depth-1);
	int beta=1;//depth-1;
	for(int i=0;i<num;++i){
		c4.place_legal(moveOrder[i]);
		int score=-evaluate_board(c4, depth-1, -beta, -alpha);
		c4.unplace(moveOrder[i]);
		if(score>alpha){
			alpha=score;
		}
		if(alpha>=beta){
			stopped=true;
			return;
		}
	}
	stopped=true;
}

void c4AI::thread_search(connect4 c4, int depth, int threadNum){
	int alpha=-1;//-(depth-1);
	int beta=1;//depth-1;
	uint8_t moveOrder[7]={3,4,2,1,5,0,6};
	thread_order_moves(moveOrder, threadNum);
	for(int i=0;i<7;++i){
		if(stopped)
			return;
		if(c4.place(moveOrder[i])){
			int score=-evaluate_board(c4, depth-1, -beta, -alpha);
			c4.unplace(moveOrder[i]);
			if(score>alpha){
				alpha=score;
			}
			if(alpha>=beta){
				return;
			}
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
	if(val>3){ // score in ttable is lower bound
		val-=5;
		if(val>alpha)
			alpha=val;
		if(alpha>=beta)
			return alpha;
	}else if(val) // score in ttable is upper bound
		return val-2;
	uint8_t moveOrder[7]={3,4,2,1,5,0,6};
	int num=order_moves(c4, moveOrder);
	if(num==0)
		return 0;
	int score=-1;
	for(int i=0;i<num;++i){
		connect4 newc4(c4);
		newc4.place_legal(moveOrder[i]);
		score=std::max(score, -evaluate_board(newc4, depth-1, -beta, -alpha));
		alpha=std::max(score, alpha);
		if(alpha>=beta){
			ttable.set(hash, depth, alpha+5); // needs to be cached differently so program knows it is min score
			return alpha;
		}
	}
	ttable.set(hash, depth, score+2);
	return score;
}

uint64_t c4AI::positions_searched(){
	return numSearched;
}

int c4AI::order_moves(connect4& c4, uint8_t moveOrder[]){
	int scores[7]={};
	int notFull=0;
	bool blockNeeded=false;
	bool ignoredValid=false;
	for(int i=0;i<7;++i){
		if(c4.place(i)){
			if(c4.check_win()){
				scores[i]+=9999;
				notFull=1;
				c4.unplace(i);
				break;
			}
			if(blockNeeded){
				c4.unplace(i);
				continue;
			}
			scores[i]+=static_evaluate(c4);
			if(c4.place(i)){
				if(c4.check_win()){
					scores[i]-=50;
					c4.unplace(i);
					c4.unplace(i);
					ignoredValid=true;
					continue;
				}else
					c4.unplace(i);
			}
			c4.unplace(i);
			uint64_t temp = c4.friends;
			c4.friends = c4.enemies;
			c4.enemies = temp;
			c4.place_legal(i);
			if(c4.check_win()){
				notFull=0;
				blockNeeded=true;
				scores[i]+=999;
			}
			c4.unplace(i);
			temp = c4.friends;
			c4.friends = c4.enemies;
			c4.enemies = temp;
			++notFull;
		}else
			scores[i]=-1000;  
	}
	
	notFull=std::max(notFull,(int)ignoredValid);
	
	for(int i=0;i<6;++i){
		for(int j=i+1;(j>0)&&(scores[moveOrder[j]]>scores[moveOrder[j-1]]);--j){
			uint8_t temp=moveOrder[j-1];
			moveOrder[j-1]=moveOrder[j];
			moveOrder[j]=temp;
		}
	}
	return notFull;
}

int c4AI::static_evaluate(connect4& c4){
	uint64_t empty=~(c4.friends|c4.enemies|(SEVENTH<<1)|255ull|255ull<<56);

	
	//  horizontal
	uint64_t test=(c4.enemies<<2)&(c4.enemies|empty);
	uint64_t potentialWins=(test&(test<<1))&~(empty&(empty<<1));

	//  diagonal 1
	test=(c4.enemies<<14)&(c4.enemies|empty);
	potentialWins|=(test&(test<<7))&~(empty&(empty<<7));

	//  diagonal 2
	test=(c4.enemies<<18)&(c4.enemies|empty);
	potentialWins|=(test&(test<<9))&~(empty&(empty<<9));
	
	//  horizontal
	test=(c4.enemies>>2)&(c4.enemies|empty);
	potentialWins|=(test&(test>>1))&~(empty&(empty>>1));
	
	//  verticle   
	test=(c4.enemies>>16)&(c4.enemies|empty);
	potentialWins|=(test&(test>>8))&~(empty&(empty>>8));

	//  diagonal 1
	test=(c4.enemies>>14)&(c4.enemies|empty);
	potentialWins|=(test&(test>>7))&~(empty&(empty>>7));

	//  diagonal 2
	test=(c4.enemies>>18)&(c4.enemies|empty);
	potentialWins|=(test&(test>>9))&~(empty&(empty>>9));
	
	return __builtin_popcountll(potentialWins);
}

void c4AI::thread_order_moves(uint8_t moveOrder[], int threadNum){ // this can definintly be improved
	uint8_t temp=moveOrder[0];
	moveOrder[0]=moveOrder[threadNum];
	moveOrder[threadNum]=temp;
}
