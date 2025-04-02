#pragma once

#include <queue>
#include <vector>

template<typename T, typename Priority>
struct PriorityQueue
{
public:

    typedef std::pair<Priority, T> PQElement;

    struct Compare
    {
        bool operator()(const PQElement& a, const PQElement& b) const
        {
            return a.first > b.first;
        }
    };

    std::priority_queue<PQElement, std::vector<PQElement>, Compare> elements;

    inline bool Empty() const { return elements.empty(); }

    inline void Put(T item, Priority priority)
    {
        elements.emplace(priority, item);
    }

    T Get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

