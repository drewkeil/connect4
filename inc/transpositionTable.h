//  transposition table for use with the connect4 solving program
#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include <unordered_map>
#include <vector>
#include <cstdint>

class transpositionTable{
public:

    transpositionTable();

    void setup(int depth, int cutoff);

    //  returns the value associated with the key, a value of 0 means an entry with thay key was not found
    int get(uint64_t key, int depth);

    //  sets the value associated with the given key, if the key is not already found in the table it will be created
    //  the depth of the key is used to determine where in the table it will go
    void set(uint64_t key, int depth, uint8_t value);

    //  clears all entries
    void clear();

private:
    int mapCutoff;
    const int sectionSize=5500000;
    const int lowTableSize=150000000;
    int maxDepth;
    std::vector<uint64_t> deepTable;
    std::vector<uint64_t> lowTable;
    
    int get_low_index(uint64_t key);
    int get_deep_index(uint64_t key, int depth);
};

#endif
