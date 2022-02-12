#ifndef __GraphTypes_H__
#define __GraphTypes_H__

#include <unordered_set>
#include <queue>

#include "NodeGraph/OpenCVNodes_Base.h"
#include "Utility/Helpers.h"

// Types.
typedef size_t vertIndex;
typedef size_t treeIndex;
typedef float edgeWeight;
typedef std::unordered_set<vertIndex> neighbourList;
typedef std::vector<neighbourList> fullNeighbourList;
typedef std::vector<vec2> pointList;
typedef std::vector<vertIndex> pointIndexList;
typedef std::vector<size_t> labelList;
typedef std::vector<vertIndex> graphPath;
typedef std::vector<vertIndex> vertIndexList;
typedef std::vector<vertIndexList> pointListsGroupedByTree;
typedef std::pair<vertIndex, vertIndex> edge;
typedef std::tuple<edgeWeight, vertIndex, vertIndex> weightedEdge;
enum EdgeTupleOrder {
    EdgeWeight,
    EdgeIndex1,
    EdgeIndex2,
};
typedef std::vector<weightedEdge> linearWeightedEdgeList;
typedef std::map<vertIndex, edgeWeight> vertEdgeList;
typedef std::map<vertIndex, vertEdgeList> fullEdgeList; // Edges only stored in lower vertIndex list.
typedef std::vector<std::tuple<vertIndex, vertIndex, vertIndex>> triIndexList;

// VertexInfo is used by Dijkstra pathfinder and SplitGraph routines.
class VertexInfo
{
public:
    vertIndex ID;

    treeIndex owner; // For SplitGraph, which root node was closest.
    edgeWeight lowestWeight;
    vertIndex parentIndex;
    bool closed;

    int stepsFromBoundary;

public:
    VertexInfo(vertIndex index)
    {
        ID = index;

        lowestWeight = FLT_MAX;
        parentIndex = -1;
        owner = 0;
        closed = false;

        stepsFromBoundary = 0;
    }

    void Clear(bool clearOwner)
    {
        lowestWeight = FLT_MAX;
        parentIndex = -1;
        closed = false;
        stepsFromBoundary = 0;

        if( clearOwner )
        {
            owner = 0;
        }
    }

    //bool operator<(const VertexInfo &rhs) const { return lowestWeight < rhs.lowestWeight; }
};


#endif __GraphTypes_H__
