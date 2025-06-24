#include "connect4.h"
#include "c4AI.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(){
	string again;
	c4AI AI;
	int turn = 0;
	int placed[42];
	std::vector<int> scores(7, -2);
	bool initialized = false;
	do{
		connect4 c4;
		c4.print_board(cout, !(turn%2));
		std::cout<<'\n';
		string selectedCol;
		while(turn<42){
			cout<<"    Player "<<turn%2+1<<" please select a column(1-7) or ('e' to exit, 'u' to undo last turn, 'c' for the computer to take a turn)\n\033[2K    ";
			cin>>selectedCol;
			if(selectedCol[0]<'1'||selectedCol[0]>'7'){
				if(selectedCol[0]=='e')
					return 0;
				else if(selectedCol[0]=='c'){
					if(!initialized){
						AI.initialize_search(42-turn,c4);
						initialized = true;
					}
					int position = AI.next_move(c4, turn, scores);
					c4.place(position);
					placed[turn++] = position;
					if(c4.check_win()){
						cout<<"\033[10A";
						c4.print_board(cout, !(turn%2));
						cout<<"\033[2K    Player "<<(turn-1)%2+1<<" is the winner";
						break;
					}
				}else if(turn>0){
					c4.unplace(placed[--turn]);
				}
			}else{
				if(c4.place(selectedCol[0]-'1')){
					placed[turn++]=selectedCol[0]-'1';
				}
				if(c4.check_win()){
					cout<<"\033[10A";
					c4.print_board(cout, !(turn%2));
					cout<<"\033[2K    Player "<<(turn-1)%2+1<<" is the winner";
					break;
				}
			}
			cout<<"\033[10A";
			c4.print_board(cout, !(turn%2));
			std::cout<<"     ";
			for(int i=0;i<7;++i){
				if(scores[i] != -2)
					cout<<scores[i]<<' ';
				else
					cout<<"  ";
			}
			std::cout<<'\n';
			scores.assign(7, -2);
		}
		cout<<"\n    Game is Over\n    play again? (y/n)\n    ";
		cin>>again;
	}while(again[0]=='y');
}
