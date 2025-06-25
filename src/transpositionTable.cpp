#include "transpositionTable.h"
#include<iostream>

transpositionTable::transpositionTable()
    :mapCutoff(20), maxDepth(42), deepTable((12)*sectionSize,0), lowTable(lowTableSize){}

void transpositionTable::setup(int depth, int cutoff){
    maxDepth=depth;
    mapCutoff=cutoff;
    deepTable.assign((maxDepth-mapCutoff)*sectionSize,0);
    lowTable.assign(lowTableSize,0);
#ifndef NDEBUG
    deepCounts.assign((maxDepth-mapCutoff)*sectionSize,0);
    lowCounts.assign(lowTableSize,0);
#endif
}

int transpositionTable::get(uint64_t key, int depth){
    uint64_t keyAccess=~(255ull<<56);
    if((maxDepth-depth)<mapCutoff){
        uint64_t tableVal=lowTable[get_low_index(key)];
        if((tableVal&keyAccess)==key)
            return static_cast<int>(tableVal>>56);
    }else{
        uint64_t tableVal=deepTable[get_deep_index(key, depth)];
        if((tableVal&keyAccess)==key)
            return static_cast<int>(tableVal>>56);
    }
	 return 0;
}

void transpositionTable::set(uint64_t key, int depth, uint8_t value){
    if((maxDepth-depth)<mapCutoff){
        lowTable[get_low_index(key)]=(key|(static_cast<uint64_t>(value)<<56));
    }else{
        deepTable[get_deep_index(key, depth)]=(key|(static_cast<uint64_t>(value)<<56));
	 }
#ifndef NDEBUG
    if((maxDepth-depth)<mapCutoff){
		 ++lowCounts[get_low_index(key)];
	 }else{
		 ++deepCounts[get_deep_index(key, depth)];
	 }
#endif
}

void transpositionTable::clear(){
    deepTable.assign((maxDepth-mapCutoff)*sectionSize,0);
    lowTable.assign(lowTableSize,0);
}

int transpositionTable::get_low_index(uint64_t key){
    key^=key>>33;
    key^=key>>5;
    return key%lowTableSize;
}

int transpositionTable::get_deep_index(uint64_t key, int depth){
    key^=key>>33;
    key^=key>>5;
    return (depth-1)*sectionSize+(key%sectionSize);
}

