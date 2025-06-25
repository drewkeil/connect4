#include "connect4.h"
#include "c4AI.h"

#include <iostream>
#include <chrono>
#include <string>

using namespace std;


uint64_t perft(int depth, connect4& c4){
    if(c4.check_win())
        return 0;
    if(depth==0)
        return 1;
    uint64_t total=0;
    for(int i=0;i<7;++i){
	     connect4 newc4(c4);
        if(newc4.place(i)){
            total+=perft(depth-1, newc4);
        }
    }
    return total;
}

void debug_info(c4AI& solver);

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
			cout<<"  took "<<(chrono::duration_cast<chrono::milliseconds>(end - begin).count()/1000.0)<<" seconds\n  searched "<<found<<" nodes\n";
		}

		cout<<"  perform alpha beta search? (y/n)\n  ";  //  technicaly it's a negamax search with alpha beta pruning but whatever
		cin>>responce;
		if(responce[0]=='y'){
			cout<<"  alpha beta search to depth of "<<depth<<"\n";
			auto begin=chrono::high_resolution_clock::now();
			AI.initialize_search(depth, c4);
			auto end=chrono::high_resolution_clock::now();
			cout<<"  took "<<(chrono::duration_cast<chrono::milliseconds>(end - begin).count()/1000.0)<<" seconds\n  searched "<<AI.positions_searched()<<" nodes\n\n";
			debug_info(AI);
		}
		cout<<"  enter depth (negative depth to exit): ";
		cin>>depth;
	}
}

void debug_info(c4AI& solver){
#ifndef NDEBUG
	// ttable stats
	uint64_t used = 0;
	uint64_t collisions = 0;
	uint64_t usedTemp, collisionsTemp;
	std::vector<uint64_t>& low = solver.ttable.lowCounts;
	std::vector<uint64_t>& deep = solver.ttable.deepCounts;
	for(size_t i=0;i<low.size();++i){
		if(low[i]){
			++used;
			collisions += low[i] - 1;
		}
	}
	cout<<"low table usage "<<used<<'/'<<low.size()<<" ("<<((double)used)/low.size()*100<<"%)\n";
	cout<<"low table collisions "<<collisions<<" average "<<((double)collisions) / used<<endl;
	usedTemp = used;
	collisionsTemp = collisions;

	used = 0;
	collisions = 0;
	for(size_t i=0;i<deep.size();++i){
		if(deep[i]){
			++used;
			collisions += deep[i] - 1;
		}
	}
	cout<<"deep table usage "<<used<<'/'<<deep.size()<<" ("<<((double)used)/deep.size()*100<<"%)\n";
	cout<<"deep table collisions "<<collisions<<" average "<<((double)collisions) / used<<endl;

	collisions += collisionsTemp;
	used += usedTemp;
	cout<<"total table usage "<<used<<'/'<<deep.size() + low.size()<<" ("<<((double)used)/(deep.size()+low.size())*100<<"%)\n";
	cout<<"total table collisions "<<collisions<<" average "<<((double)collisions) / used<<endl;

	// pruning stats
	cout<<'\n';
	cout<<"number of cutoffs "<<solver.numCutoffs<<endl;
	cout<<"cutoffs at each index: \n";
	for(int i=0;i<7;++i)
		cout<<i<<": "<<solver.cutoffOrderIndex[i]<<' ';
	cout<<"\nTransposition table cutoffs "<<solver.ttCutoffs<<endl;
#endif
}
