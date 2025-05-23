#include "connect4.h"
#include "c4AI.h"

// performs a full depth search and exits
// this is intended to be used with profiling tools
int main(){
	connect4 c4;
	c4AI AI;
	AI.initialize_search(42, c4);
}
