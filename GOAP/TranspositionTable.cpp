#include "TranspositionTable.h"

TranspositionTable::TranspositionTable(size_t size)
    : mySize(size), myEntries(size) {}

bool TranspositionTable::Has(const WorldModel& worldModel) const
{
    // Get the hash value
    size_t hashValue = worldModel.Hash();
    size_t index = hashValue % mySize;

    const Entry& entry = myEntries[index];

    if (!entry.valid) 
    {
        return false;
    }

    return entry.worldModel == worldModel;
}

void TranspositionTable::Add(const WorldModel& worldModel, int depth, float cost) 
{
    size_t hashValue = worldModel.Hash();
    size_t index = hashValue % mySize;

    Entry& entry = myEntries[index];

    if (!entry.valid) 
    {
        entry.worldModel = worldModel;
        entry.depth = depth;
        entry.cost = cost;
        entry.valid = true;
        return;
    }

    if (entry.worldModel == worldModel)
    {
        // If we have a lower depth, use the new one
        if (cost < entry.cost)
        {
            entry.depth = depth;
            entry.cost = cost;
        }
    }
    else
    {
        // Replace the slot if our new cost is lower
        if (cost < entry.cost)
        {
            entry.worldModel = worldModel;
            entry.depth = depth;
            entry.cost = cost;
        }
    }
}