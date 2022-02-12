#ifndef __GraphHelpers_H__
#define __GraphHelpers_H__

#include "GraphTypes.h"

namespace delaunator
{
    class Delaunator;
}

class Graph
{
public:
    pointList points;
    fullNeighbourList neighbours;
    fullEdgeList weights;

    void Clear();
    //void operator+=(const Graph& o);
    bool PointHasNeighbour(vertIndex point, vertIndex neighbourToTestFor) const;
};

class IndexGraph
{
public:
    pointIndexList pointIndicesInOriginalGraph;
    pointList points;
    fullNeighbourList neighbours;
    fullEdgeList weights;

    void Clear();
    //void operator+=(const Graph& o);
};

// Helper functions.
float GetDifferenceBetweenAngles(float angle1, float angle2);
float GetDifferenceBetweenAngles_Birectional(float angle1, float angle2);

delaunator::Delaunator TriangulatePointList(const pointList& points);
fullNeighbourList CreateNeighbourList(const pointList& points, float maxDistanceApart, float minInnerAngle);
fullEdgeList CreateEdgeListWithWeights(const Graph& graph, bool randomWeights, int randomSeed, float fixedWeight);
fullEdgeList CreateEdgeListWithWeightsUsingVectorField(const Graph& graph, const std::vector<float>& rotations);
fullEdgeList CreateEdgeListWithWeightsFromSourceImage(const Graph& graph, const cv::Mat& weightImage);

graphPath FindShortestPath(const Graph& graph, vertIndex startIndex, vertIndex endIndex);
graphPath FindShortestPath_Dijkstra(const Graph& graph, vertIndex startIndex, vec2 endPosition);
graphPath FindShortestPath_Dijkstra(const Graph& graph, vertIndex startIndex, vertIndex endIndex);

float DistanceFromLineSegment(vec2 start, vec2 end, vec2 point);
linearWeightedEdgeList GenerateMinimumSpanningTree(const Graph& graph);

std::pair<int, float> FindNearestEdge(const pointList& points, const linearWeightedEdgeList& edgeList, vec2 point);
std::pair<vertIndex, float> FindNearestPoint(pointList pointList, vec2 point);

void SplitGraph_BFSFloodFillOwnership(treeIndex owner, vertIndex startIndex, std::vector<VertexInfo>& vertexInfo, const fullNeighbourList& neighbours, const fullEdgeList& edgeList, const linearWeightedEdgeList& activeEdges);
vertIndex FindNearestVertexToPoint(const pointList& points, vec2 point);
linearWeightedEdgeList SplitGraph(std::vector<VertexInfo>& vertexInfo, const Graph& graph, const linearWeightedEdgeList& activeEdges, int numSplits, const pointList& treeRoots);
vertIndex FindNextNeighbourClockwiseFromIndex(const pointList& points, const pointIndexList& validVerts, const neighbourList& neighbours, vec2 pointPos, vec2 previousPos);
pointIndexList BuildBoundaryVertexList(treeIndex treeLabel, const pointList& points, const fullNeighbourList& neighbours, const std::vector<VertexInfo>& vertexInfo);
pointIndexList BuildBoundaryVertexList_Method2(treeIndex treeLabel, const pointList& points, const fullNeighbourList& neighbours, const std::vector<VertexInfo>& vertexInfo, const vertIndexList& vertsInRegion);
vertIndex FindTopmostPointIndex(const pointList& points, const pointIndexList& allowedVerts);

vertIndexList CreateGroupOfVertsNotBlockedByVertices(const Graph& graph, std::vector<VertexInfo>& vertexInfo, const vertIndexList& blockers, const vertIndex startIndex);
vertIndexList CreateListOfVertsWithinStepsOfVertex(const Graph& graph, std::vector<VertexInfo>& vertexInfo, const vertIndex startIndex, float maxWeight);
vertIndexList CreateListOfVertsWithinRadiusOfVertex(const Graph& graph, vertIndex center, float radius);

void Graph_SwapIndex(std::vector<vec2>& pointList, fullNeighbourList& neighbours, vertIndex v1, vertIndex v2);
void Graph_RemovePoint(std::vector<vec2>& pointList, fullNeighbourList& neighbours, vertIndex indexToRemove);

#endif __GraphHelpers_H__
