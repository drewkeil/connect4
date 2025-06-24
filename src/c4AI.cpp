#include "c4AI.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <cstdlib>

int c4AI::next_move(connect4& c4, int turn, std::vector<int>& scores){
	stopped=false;
	int depth=std::min(initializedDepth, 42-turn);
	numSearched=1;
	for(int i=0;i<7;++i)
		scores[i] = -2;

	for(int i=0;i<7;++i){
		if(c4.is_winning(i)){
			scores[i] = 1;
			return i;
		}
	}

	int alpha=-1;//-(depth-1);
	int beta=1;//depth-1;
	uint8_t moveOrder[7]={3,4,2,1,5,0,6};
	int num=order_moves(c4, moveOrder);
	int bestCol=moveOrder[0];
	for(int i=0;i<num;++i){
		c4.place_legal(moveOrder[i]);
		int score=-evaluate_board(c4, depth-1, -beta, -alpha);
		scores[moveOrder[i]] = score;
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

int c4AI::initialize_search(int depth, connect4& c4){
	stopped=false;
	numSearched=0;
	depth=std::max(depth,1);
	initializedDepth=depth;
	ttable.setup(depth, std::min(depth, 24));
	for(int i=0;i<7;++i){
		if(c4.is_winning(i)){
			return 1;
		}
	}
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
			return alpha;
		}
	}
	stopped=true;
	return alpha;
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
	++numSearched;

	if(depth==0){
		return 0; // if it's a win that should have been checked by the parent
	}

	for(int i=0;i<7;++i){
		if(c4.is_winning(i)){
			return 1;
		}
	}

	int start = 0;
	uint64_t hash=c4.get_hash();
	if(depth > 1){
		int val=ttable.get(hash, depth); 
		if((val & 0xf) > 3){ // score in ttable is lower bound
			start = val >> 4;
			val = (val & 0xf) - 5;
			if(val>alpha)
				alpha=val;
		}else if(val){ // score in ttable is upper bound
			val -= 2;
			if(val < beta)
				beta = val;
		}
		if(alpha>=beta)
			return alpha;
	}
	uint8_t moveOrder[7]={3,4,2,1,5,0,6};
	int num=order_moves(c4, moveOrder);
	if(num==0)
		return 0;
	int score = -1;
	for(int i=start;i<num;++i){
		connect4 newc4(c4);
		newc4.place_legal(moveOrder[i]);
		score=std::max(score, -evaluate_board(newc4, depth-1, -beta, -alpha));
		alpha = std::max(score, alpha);
		if(alpha>=beta){
			ttable.set(hash, depth, (alpha+5) | (i << 4)); // cached differently so program knows it's lower bound
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
	bool ignoredValid=false;

	for(int i=0;i<7;++i){
		if(c4.place(i)){
			if(c4.is_winning(i)){
					scores[i]-=50;
					c4.unplace(i);
					ignoredValid=true;
					continue;
			}
			scores[i]+=static_evaluate(c4);
			c4.unplace(i);
			uint64_t temp = c4.friends;
			c4.friends = c4.enemies;
			if(c4.is_winning(i)){
				notFull=1;
				scores[i]+=999;
				c4.friends = temp;
				break;
			}
			c4.friends = temp;
			++notFull;
		}else
			scores[i]=-1000;  
	}
	
	notFull=std::max(notFull,(int)ignoredValid);
	
	for(int i=0;i<6;++i){
		int j = i + 1;
		uint8_t temp = moveOrder[j];
		for(;(j>0)&&(scores[temp]>scores[moveOrder[j-1]]);--j){
			moveOrder[j]=moveOrder[j-1];
		}
		moveOrder[j] = temp;
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
