#pragma once
#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "WorldModel.h"
#include <unordered_map>

class TranspositionTable 
{
public:

    struct Entry
    {
        WorldModel worldModel;
        int depth;
        float cost = 0.0f;
        bool valid = false;

        Entry()
            : depth(std::numeric_limits<int>::max()) {}
    };

    explicit TranspositionTable(size_t size = 1024);

    bool Has(const WorldModel& worldModel) const;
    void Add(const WorldModel& worldModel, int depth, float cost);

private:
    size_t mySize;
    std::vector<Entry> myEntries;
};

#endif 