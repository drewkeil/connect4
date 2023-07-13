#include "transpositionTable.h"
#include<iostream>

transpositionTable::transpositionTable()
    :mapCutoff(20), maxDepth(42), deepTable((12)*sectionSize,0), lowTable(100000){}

void transpositionTable::setup(int depth, int cutoff){
    maxDepth=depth;
    mapCutoff=cutoff;
    deepTable.assign((maxDepth-mapCutoff)*sectionSize,0);
    lowTable.assign(lowTableSize,0);
}

int transpositionTable::get(uint64_t key, int depth){
    if((maxDepth-depth)<mapCutoff){
        uint64_t keyAccess=~(255ull<<56);
        uint64_t tableVal=lowTable[get_index(key,depth)];
        if((tableVal&keyAccess)==key)
            return static_cast<int>(tableVal>>56);
        else
            return 0;
    }else{
        uint64_t keyAccess=~(255ull<<56);
        uint64_t tableVal=deepTable[get_index(key, depth)];
        if((tableVal&keyAccess)==key)
            return static_cast<int>(tableVal>>56);
        else
            return 0;
    }
}

void transpositionTable::set(uint64_t key, int depth, uint8_t value){
    if((maxDepth-depth)<mapCutoff){
        lowTable[get_index(key,depth)]=(key|(static_cast<uint64_t>(value)<<56));
    }else
        deepTable[get_index(key, depth)]=(key|(static_cast<uint64_t>(value)<<56));
}

void transpositionTable::clear(){
    deepTable.assign((maxDepth-mapCutoff)*sectionSize,0);
    lowTable.assign(lowTableSize,0);
}

int transpositionTable::get_index(uint64_t key, int depth){
    //  write a psuedo machine learning thing to find the best values and num steps to optimize this
    //  try using multiplication then a big right shift
    key^=key>>32;
    key^=key>>5;
    key^=key>>1;
    if((maxDepth-depth)<mapCutoff){
        return key%lowTableSize;
    }
    return (depth-1)*sectionSize+(key%sectionSize);
}