#include "connect4.h"
#include "c4AI.h"

#include <iostream>
#include <string>

using namespace std;

int main(){
    string again;
    c4AI AI;
    
    do{
        connect4 c4;
        cout<<"\033[2J";
        int depth;
        cout<<"    depth for computer search: ";
        cin>>depth;
        c4.print_board(cout);
        string selectedCol;
        while(c4.on_turn()<42){
            cout<<"    Player "<<c4.on_turn()%2+1<<" please select a column(1-7) or ('e' to exit, 'u' to undo last turn, 'c' for the computer to take a turn)\n\033[2K    ";
            cin>>selectedCol;
            if(selectedCol[0]<'1'||selectedCol[0]>'7'){
                if(selectedCol[0]=='e')
                    return 0;
                else if(selectedCol[0]=='c'){
                    c4.place(AI.next_move(c4,depth));
                    if(c4.check_win()){
                        cout<<"\033[9A";
                        c4.print_board(cout);
                        cout<<"\033[2K    Player "<<(c4.on_turn()-1)%2+1<<" is the winner";
                        break;
                    }
                }else
                    c4.unplace();
            }else{
                c4.place(selectedCol[0]-'1');
                if(c4.check_win()){
                    cout<<"\033[9A";
                    c4.print_board(cout);
                    cout<<"\033[2K    Player "<<(c4.on_turn()-1)%2+1<<" is the winner";
                    break;
                }
            }
            cout<<"\033[9A";
            c4.print_board(cout);
        }
        cout<<"\n    Game is Over\n    play again? (y/n)\n    ";
        cin>>again;
    }while(again[0]=='y');
}