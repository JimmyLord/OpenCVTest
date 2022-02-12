#ifndef __DijkstraSearchObject_H__
#define __DijkstraSearchObject_H__

#include "Graph/GraphTypes.h"

class Graph;

class DijkstraSearchObject
{
    // Define callback function types.
    using PostInitCallback = std::function<void()>;
    using VertexSelectedFromOpenListCallback = std::function<void(size_t index)>;
    using WeightCalculationCallback = std::function<float(vertIndex currentIndex, vertIndex nIndex, vertIndex parentIndex)>;

public:
    enum DijkstraBitFlags
    {
        DBF_None                = 0,
        DBF_ForceSameOwnership  = 0x01,
        DBF_CheckWeights        = 0x02,
        DBF_ReturnShortestPath  = 0x04,
    };

public:
    DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, vertIndex startIndex, DijkstraBitFlags flags);
    DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, std::vector<vertIndex> startIndices, DijkstraBitFlags flags);
    DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, vertIndex startIndex, vertIndex endIndex, DijkstraBitFlags flags);
    DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, std::vector<vertIndex> startIndices, vertIndex endIndex, DijkstraBitFlags flags);
    vertIndexList Search();

public:
    const Graph& m_Graph;
    std::vector<VertexInfo>& m_VertexInfo;
    pointIndexList m_StartIndices;
    vertIndex m_EndIndex;
    DijkstraBitFlags m_Flags;

    float m_MaxWeight;

    PostInitCallback m_pPostInitCallback;
    VertexSelectedFromOpenListCallback m_pVertexSelectedFromOpenListCallback;
    WeightCalculationCallback m_pWeightCalculationCallback;
};

#endif //__DijkstraSearchObject_H__
