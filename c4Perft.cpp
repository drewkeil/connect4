#include "connect4.h"
#include "c4AI.h"

#include <iostream>
#include <chrono>
#include <string>

using namespace std;


uint64_t perft(int depth, connect4& c4){
    //cout<<"\033[7A";
    //c4.print_board(cout);
    
    if((c4.on_turn()!=0)&&c4.check_win())
        return 0;
    if(depth==0)
        return 1;
    uint64_t total=0;
    for(int i=0;i<7;++i){
        if(c4.place(i)){
            total+=perft(depth-1, c4);
            c4.unplace();
        }
    }
    return total;
}

int main(){
    int depth;
    connect4 c4;
    c4AI AI;
    cout<<"\n\n\n\n  enter depth (negative depth to exit): ";
    cin>>depth;
    while(depth>0){
        string responce;
        cout<<"  perform perft? (y/n)\n  ";
        cin>>responce;
        if(responce[0]=='y'){
            cout<<"  perft at depth of "<<depth<<'\n';
            //c4.print_board(cout);
            auto begin=chrono::high_resolution_clock::now();
            uint64_t found=perft(depth, c4);
            auto end=chrono::high_resolution_clock::now();
            cout<<"  took "<<(chrono::duration_cast<chrono::milliseconds>(end - begin).count()/1000.0)<<" seconds\n  found "<<found<<" leaf positions\n";
        }

        cout<<"  perform alpha beta search? (y/n)\n  ";  //  technicaly it's a negamax search with alpha beta pruning but whatever
        cin>>responce;
        if(responce[0]=='y'){
            cout<<"  alpha beta search to depth of "<<depth<<'\n';
            auto begin=chrono::high_resolution_clock::now();
            AI.next_move(c4, depth);
            auto end=chrono::high_resolution_clock::now();
            cout<<"  took "<<(chrono::duration_cast<chrono::milliseconds>(end - begin).count()/1000.0)<<" seconds\n  searched "<<AI.positions_searched()<<" leaf positions\n\n";
        }
        cout<<"  enter depth (negative depth to exit): ";
        cin>>depth;
    }
}