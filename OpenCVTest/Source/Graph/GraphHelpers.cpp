#include "OpenCVPCH.h"

#include "GraphTypes.h"
#include "GraphHelpers.h"
#include "DijkstraSearchObject.h"

#pragma warning (push)
#pragma warning (disable:4623)
#include "Libraries/Delaunator/delaunator.hpp"
#pragma warning (pop)

void Graph::Clear()
{
    points.clear();
    neighbours.clear();
    weights.clear();
}

bool Graph::PointHasNeighbour(vertIndex point, vertIndex neighbourToTestFor) const
{
    const neighbourList& neighbourList = neighbours[point];
    return neighbourList.find( neighbourToTestFor ) != neighbourList.end();
}

//void Graph::operator+=(const Graph& o)
//{
//}


void IndexGraph::Clear()
{
}

//void IndexGraph::operator+=(const Graph& o)
//{
//}

float GetDifferenceBetweenAngles(float angle1, float angle2)
{
    float angleDiff = fabs( angle1 - angle2 );
    if( angleDiff > PI )
    {
        angleDiff = 2*PI - angleDiff;
    }
    return angleDiff;
}

float GetDifferenceBetweenAngles_Birectional(float angle1, float angle2)
{
    float angleDiff = fabs( angle1 - angle2 );
    if( angleDiff > PI )
    {
        angleDiff = 2*PI - angleDiff;
    }    
    if( angleDiff > PI/2 )
    {
        angleDiff -= PI;
    }
    return angleDiff;
}

delaunator::Delaunator TriangulatePointList(const pointList& points)
{
    size_t numPoints = points.size();
    assert( numPoints >= 3 );

    // Ugh: Convert vec2 vector to doubles for Delaunator.
    std::vector<double> pointsDoubles;
    for( size_t i=0; i<numPoints; i++ )
    {
        pointsDoubles.push_back( points.at(i).x );
        pointsDoubles.push_back( points.at(i).y );
    }

    delaunator::Delaunator d( pointsDoubles );

    return d;
}

fullNeighbourList CreateNeighbourList(const pointList& points, float maxDistanceApart, float minInnerAngle)
{
    if( points.size() < 3 )
        return fullNeighbourList();

    maxDistanceApart = maxDistanceApart*2.5f;

    delaunator::Delaunator d = TriangulatePointList( points );

    // Create neighbour list for each point.
    size_t numPoints = points.size();
    fullNeighbourList neighbours( numPoints );

    {
        size_t numTriangles = d.triangles.size()/3;
        for( size_t i=0; i<numTriangles; i++ )
        {
            vertIndex i1 = d.triangles[i*3+0];
            vertIndex i2 = d.triangles[i*3+1];
            vertIndex i3 = d.triangles[i*3+2];

            if( i == 40 )
                int bp = 1;

            // Don't add long edge from triangles along the perimeter (tris without neighbour tris)
            vertIndex e1 = d.halfedges[i*3+0];
            vertIndex e2 = d.halfedges[i*3+1];
            vertIndex e3 = d.halfedges[i*3+2];
            bool includeE1 = true;
            bool includeE2 = true;
            bool includeE3 = true;
            bool hasNeighboursOnAllSides = true;
            if( e1 == -1 || e2 == -1 || e3 == -1 )
            {
                hasNeighboursOnAllSides = false;
            }

            if( minInnerAngle <= 2*PI )
            {
                vec2 p1 = points[i1];
                vec2 p2 = points[i2];
                vec2 p3 = points[i3];

                // Find longest edge.
                float len1 = p1.DistanceFrom( p2 );
                float len2 = p2.DistanceFrom( p3 );
                float len3 = p3.DistanceFrom( p1 );

                float longest = std::max( len1, std::max( len2, len3 ) );

                float minCosAngle = cos( minInnerAngle );
                float cosAngle1 = (p3 - p1).Dot(p2 - p1) / (len3 * len1);
                float cosAngle2 = (p3 - p2).Dot(p1 - p2) / (len2 * len1);
                float cosAngle3 = (p1 - p3).Dot(p2 - p3) / (len3 * len2);

                // If edge 1 is the longest test with the other 2 verts.
                //if( len1 == longest )
                {
                    if( cosAngle1 > minCosAngle || cosAngle2 > minCosAngle )
                        includeE1 = false;
                }
                //if( len2 == longest )
                {
                    if( cosAngle2 > minCosAngle || cosAngle3 > minCosAngle )
                        includeE2 = false;
                }
                //if( len3 == longest )
                {
                    if( cosAngle1 > minCosAngle || cosAngle3 > minCosAngle )
                        includeE3 = false;
                }
            }

            if( hasNeighboursOnAllSides )
            {
                if( includeE1 )
                {
                    neighbours[i1].insert( i2 );
                    neighbours[i2].insert( i1 );
                }

                if( includeE2 )
                {
                    neighbours[i2].insert( i3 );
                    neighbours[i3].insert( i2 );
                }

                if( includeE3 )
                {
                    neighbours[i3].insert( i1 );
                    neighbours[i1].insert( i3 );
                }
            }
            else
            {
                if( includeE1 )
                {
                    if( e1 != -1 || points[i1].DistanceFrom( points[i2] ) < maxDistanceApart )
                    {
                        neighbours[i1].insert( i2 );
                        neighbours[i2].insert( i1 );
                    }
                }

                if( includeE2 )
                {
                    if( e2 != -1 || points[i2].DistanceFrom( points[i3] ) < maxDistanceApart )
                    {
                        neighbours[i2].insert( i3 );
                        neighbours[i3].insert( i2 );
                    }
                }

                if( includeE3 )
                {
                    if( e3 != -1 || points[i3].DistanceFrom( points[i1] ) < maxDistanceApart )
                    {
                        neighbours[i3].insert( i1 );
                        neighbours[i1].insert( i3 );
                    }
                }
            }
        }
    }

    return neighbours;
}

fullEdgeList CreateEdgeListWithWeights(const Graph& graph, bool randomWeights, int randomSeed, float fixedWeight)
{
    fullEdgeList edgeList;

    if( randomWeights )
    {
        srand( randomSeed );
    }

    for( size_t i=0; i<graph.neighbours.size(); i++ )
    {
        const neighbourList& currentNeighbours = graph.neighbours[i];

        for( const vertIndex nIndex : currentNeighbours )
        {
            // Only add the neighbour to the list if the source index is less than the dest.
            if( i < nIndex )
            {
                float weight;
                if( fixedWeight >= 0 )
                {
                    weight = fixedWeight;
                }
                else if( randomWeights )
                {
                    weight = (float)rand() / RAND_MAX;
                }
                else
                {
                    weight = graph.points[i].DistanceFrom( graph.points[nIndex] );
                }

                size_t lowerIndex = i;
                size_t higherIndex = nIndex;
                edgeList[lowerIndex][higherIndex] = weight;
            }
        }
    }

    return edgeList;
}

fullEdgeList CreateEdgeListWithWeightsUsingVectorField(const Graph& graph, const std::vector<float>& rotations)
{
    assert( false ); // Not really useful since it's coded for bidirectional vectors.
    fullEdgeList edgeList;
    
    if( rotations.size() < graph.points.size() )
        return edgeList;

    for( size_t i=0; i<graph.neighbours.size(); i++ )
    {
        const neighbourList& currentNeighbours = graph.neighbours[i];

        for( const vertIndex nIndex : currentNeighbours )
        {
            // Only add the neighbour to the list if the source index is less than the dest.
            if( i < nIndex )
            {
                vec2 iDir( cos(rotations[i]), sin(rotations[i]) );
                vec2 dirTowardsN = (graph.points[nIndex] - graph.points[i]).GetNormalized();
                float weight = 1.0f - fabs( iDir.Dot( dirTowardsN ) ); // fabs for the bidirectionality.

                size_t lowerIndex = i;
                size_t higherIndex = nIndex;
                edgeList[lowerIndex][higherIndex] = weight;
            }
        }
    }

    return edgeList;
}

fullEdgeList CreateEdgeListWithWeightsFromSourceImage(const Graph& graph, const cv::Mat& weightImage)
{
    fullEdgeList edgeList;

    cv::Vec3b* imageValues = (cv::Vec3b*)weightImage.ptr();
    uint32 imageStride = (uint32)( weightImage.step[0]/weightImage.channels() );
    uint32 imageWidth = weightImage.cols;
    uint32 imageHeight = weightImage.rows;

    for( size_t i=0; i<graph.neighbours.size(); i++ )
    {
        const neighbourList& currentNeighbours = graph.neighbours[i];

        for( const vertIndex nIndex : currentNeighbours )
        {
            // Only add the neighbour to the list if the source index is less than the dest.
            if( i < nIndex )
            {
                float blackWeight = 0.1f;
                float whiteWeight = 1.0f;

                float cPercX = graph.points[i].x / 100.0f;
                float cPercY = graph.points[i].y / 100.0f;
                ivec2 cPos = ivec2( (int)(cPercX * imageWidth), (int)(cPercY * imageHeight) );
                float cValue = imageValues[cPos.y*imageStride + cPos.x][0] * (whiteWeight - blackWeight) + blackWeight;

                float nPercX = graph.points[nIndex].x / 100.0f;
                float nPercY = graph.points[nIndex].y / 100.0f;
                ivec2 nPos = ivec2( (int)(nPercX * imageWidth), (int)(nPercY * imageHeight) );
                float nValue = imageValues[nPos.y*imageStride + nPos.x][0] * (whiteWeight - blackWeight) + blackWeight;

                float weight = (cValue + nValue) / 2;

                size_t lowerIndex = i;
                size_t higherIndex = nIndex;
                edgeList[lowerIndex][higherIndex] = weight;
            }
        }
    }

    return edgeList;
}

graphPath FindShortestPath(const Graph& graph, vertIndex startIndex, vertIndex endIndex)
{
    graphPath path;

    vertIndex currentIndex = startIndex;
    path.push_back( currentIndex );
    vec2 destinationPos = graph.points[endIndex];

    while( currentIndex != endIndex )
    {
        vec2 currentPos = graph.points[currentIndex];
        const neighbourList& currentNeighbours = graph.neighbours[currentIndex];

        edgeWeight lowestWeight = FLT_MAX;
        vertIndex closestIndex = 0;
        for( const vertIndex nIndex : currentNeighbours )
        {
            if( nIndex == endIndex )
            {
                closestIndex = nIndex;
                break;
            }

            vertIndex lowerIndex = currentIndex < nIndex ? currentIndex : nIndex;
            vertIndex higherIndex = currentIndex > nIndex ? currentIndex : nIndex;
            edgeWeight weight = graph.weights.at(lowerIndex).at(higherIndex);

            if( weight < lowestWeight )
            {
                lowestWeight = weight;
                closestIndex = nIndex;
            }
        }

        path.push_back( closestIndex );
        currentIndex = closestIndex;
    }

    return path;
}

graphPath FindShortestPath_Dijkstra(const Graph& graph, vertIndex startIndex, vec2 endPosition)
{
    std::pair<vertIndex, float> pointInfo = FindNearestPoint( graph.points, endPosition );
    vertIndex endIndex = pointInfo.first;
    //float pointDist = pointInfo.second;

    return FindShortestPath_Dijkstra( graph, startIndex, endIndex );
}

graphPath FindShortestPath_Dijkstra(const Graph& graph, vertIndex startIndex, vertIndex endIndex)
{
    assert( graph.points.size() == graph.neighbours.size() );

    if( startIndex >= graph.points.size() || endIndex >= graph.points.size() )
    {
        return graphPath();
    }

    std::vector<VertexInfo> vertexInfo;
    for( size_t i=0; i<graph.points.size(); i++ )
    {
        vertexInfo.push_back( VertexInfo(i) );
    }

    // Open list, sorted from worst to best.
    pointIndexList openList;
    openList.push_back( startIndex );
    vertexInfo[startIndex].lowestWeight = 0;

    while( openList.size() > 0 )
    {
        // Grab the last element of the open list, which is the lowest weight vertex.
        vertIndex currentIndex = openList[openList.size()-1];
        openList.pop_back();
        vertexInfo[currentIndex].closed = true;

        if( currentIndex == endIndex )
            break;

        // Loop through neighbours.
        for( const vertIndex nIndex : graph.neighbours[currentIndex] )
        {
            // If the neighbour is closed, skip over it.
            if( vertexInfo[nIndex].closed == false )
            {
                vertIndex lowerIndex = currentIndex < nIndex ? currentIndex : nIndex;
                vertIndex higherIndex = currentIndex > nIndex ? currentIndex : nIndex;
                edgeWeight weight = graph.weights.at(lowerIndex).at(higherIndex);

                edgeWeight totalWeight = vertexInfo[currentIndex].lowestWeight + weight;

                // If this is a shorter way to reach this vertex, update it with a new parent/cost.
                if( totalWeight < vertexInfo[nIndex].lowestWeight )
                {
                    vertexInfo[nIndex].lowestWeight = totalWeight;

                    // If we already have a parent, then we have an old cost which was higher.
                    // Remove from open list and re-add it sorted in new position.
                    if( vertexInfo[nIndex].parentIndex != -1 && vertexInfo[nIndex].closed == false )
                    {
                        auto it = std::find( openList.begin(), openList.end(), nIndex );
                        if( it != openList.end() )
                        {
                            openList.erase( it );
                        }
                    }

                    // Insert into open list sorted in descending order.
                    {
                        bool inserted = false;
                        for( size_t i=0; i<openList.size(); i++ )
                        {
                            if( totalWeight > vertexInfo[openList[i]].lowestWeight )
                            {
                                inserted = true;
                                openList.insert( openList.begin() + i, nIndex );
                                break;
                            }
                        }

                        // If the list was empty or we didn't find a shorter path,
                        // insert the element at the end of the list.
                        if( inserted == false )
                        {
                            openList.push_back( nIndex );
                        }
                    }

                    // Set the new parent.
                    vertexInfo[nIndex].parentIndex = currentIndex;
                }
            }
        }
    }

    graphPath path;

    vertIndex i = endIndex;
    while( i != -1 )
    {
        path.push_back( i );
        i = vertexInfo[i].parentIndex;
    }

    return path;
}

linearWeightedEdgeList GenerateMinimumSpanningTree(const Graph& graph)
{
    // Storage for generated list of edges in MST.
    linearWeightedEdgeList mstEdges;

    // Temp heap and parent list.
    std::priority_queue<weightedEdge> activeEdges;
    pointIndexList parents( graph.points.size() );
    std::fill( parents.begin(), parents.end(), -1 );

    // Seed MST with vertex 0.
    vertIndex currentVertex = 0;
    vertIndex nextVertex = currentVertex;

    // Loop while there are activeEdges in the list.
    while( nextVertex != -1 )
    {
        // Assign the parent.
        parents[nextVertex] = currentVertex;
        currentVertex = nextVertex;

        // Add all connected edges into the active list.
        // Sort by weight. Smallest on top of heap by multiplying by -1.
        for( const vertIndex n : graph.neighbours[currentVertex] )
        {
            size_t lowerIndex = currentVertex < n ? currentVertex : n;
            size_t higherIndex = currentVertex > n ? currentVertex : n;
            edgeWeight weight = graph.weights.at(lowerIndex).at(higherIndex);

            activeEdges.push( { weight*-1, currentVertex, n } );
        }

        // Grab the lowest weighted edge that connects to a vertex not already in the MST.
        nextVertex = -1;
        while( activeEdges.empty() == false )
        {
            auto& info = activeEdges.top();
            vertIndex v1 = std::get<EdgeIndex1>( info );
            vertIndex v2 = std::get<EdgeIndex2>( info );
            activeEdges.pop();

            if( parents[v2] == -1 )
            {
                nextVertex = v2;
                mstEdges.push_back( { 1.0f, v1, v2 } ); // TODO: fix the 1.0f.
                break;
            }
        }
    }

    return mstEdges;
}

float DistanceFromLineSegment(vec2 start, vec2 end, vec2 point)
{
    float dist;

    vec2 line = end - start;
    float len = line.Length();
    vec2 lineDir = line / len;

    vec2 lineToPoint = point - start;
    float distFromStart = lineToPoint.Length();
    float perc = lineDir.Dot( lineToPoint );

    if( perc < 0 )
    {
        dist = distFromStart;
    }
    else if( perc > len )
    {
        dist = point.DistanceFrom( end );
    }
    else
    {
        dist = (lineToPoint - lineDir * perc).Length();
    }

    return dist;
}

std::pair<int, float> FindNearestEdge(const pointList& points, const linearWeightedEdgeList& edgeList, vec2 point)
{
    int closestEdgeIndex = -1;
    float closestDist = FLT_MAX;

    for( size_t i=0; i<edgeList.size(); i++ )
    {
        vertIndex i1 = std::get<EdgeIndex1>( edgeList[i] );
        vertIndex i2 = std::get<EdgeIndex2>( edgeList[i] );

        float dist = DistanceFromLineSegment( points[i1], points[i2], point );
        if( dist < closestDist )
        {
            closestDist = dist;
            closestEdgeIndex = (int)i;
        }
    }
    return { closestEdgeIndex, closestDist };
}

std::pair<vertIndex, float> FindNearestPoint(pointList pointList, vec2 point)
{
    vertIndex closestPointIndex = -1;
    float closestDist = FLT_MAX;

    for( size_t i=0; i<pointList.size(); i++ )
    {
        float dist = pointList[i].DistanceFrom( point );
        if( dist < closestDist )
        {
            closestDist = dist;
            closestPointIndex = i;
        }
    }

    return { closestPointIndex, closestDist };
}

void SplitGraph_BFSFloodFillOwnership(treeIndex owner, vertIndex startIndex, std::vector<VertexInfo>& vertexInfo, const fullNeighbourList& neighbours, const fullEdgeList& edgeList, const linearWeightedEdgeList& activeEdges)
{
    // Open list, sorted from worst to best.
    pointIndexList openList;
    openList.push_back( startIndex );
    vertexInfo[startIndex].lowestWeight = 0;
    vertexInfo[startIndex].owner = owner;

    for( size_t i=0; i<vertexInfo.size(); i++ )
    {
        vertexInfo[i].closed = false;
    }

    while( openList.size() > 0 )
    {
        // Grab the last element of the open list, which is the lowest weight vertex.
        vertIndex currentIndex = openList[openList.size()-1];
        openList.pop_back();
        vertexInfo[currentIndex].closed = true;

        //if( currentIndex == endIndex )
        //    break;

        // Loop through neighbours.
        for( const vertIndex nIndex : neighbours[currentIndex] )
        {
            // If the neighbour is closed, skip over it.
            if( vertexInfo[nIndex].closed == false )
            {
                vertIndex lowerIndex = currentIndex < nIndex ? currentIndex : nIndex;
                vertIndex higherIndex = currentIndex > nIndex ? currentIndex : nIndex;
                edgeWeight weight = edgeList.at(lowerIndex).at(higherIndex);

                edgeWeight totalWeight = vertexInfo[currentIndex].lowestWeight + weight;

                // If this is a shorter way to reach this vertex, update it with a new parent/cost.
                if( totalWeight < vertexInfo[nIndex].lowestWeight )
                {
                    vertexInfo[nIndex].lowestWeight = totalWeight;
                    vertexInfo[nIndex].owner = owner;

                    // If we already have a parent, then we have an old cost which was higher.
                    // Remove from open list and re-add it sorted in new position.
                    if( vertexInfo[nIndex].parentIndex != -1 && vertexInfo[nIndex].closed == false )
                    {
                        auto it = std::find( openList.begin(), openList.end(), nIndex );
                        if( it != openList.end() )
                        {
                            openList.erase( it );
                        }
                    }

                    // Insert into open list sorted in descending order.
                    {
                        bool inserted = false;
                        for( size_t i=0; i<openList.size(); i++ )
                        {
                            if( totalWeight > vertexInfo[openList[i]].lowestWeight )
                            {
                                inserted = true;
                                openList.insert( openList.begin() + i, nIndex );
                                break;
                            }
                        }

                        // If the list was empty or we didn't find a shorter path,
                        // insert the element at the end of the list.
                        if( inserted == false )
                        {
                            openList.push_back( nIndex );
                        }
                    }

                    // Set the new parent.
                    vertexInfo[nIndex].parentIndex = currentIndex;
                }
            }
        }
    }
}

// TODO: This is very inefficient, some space partioning might do wonders.
vertIndex FindNearestVertexToPoint(const pointList& points, vec2 point)
{
    if( points.size() == 0 )
        return -1;

    assert( points.size() > 0 );

    vertIndex closestIndex = 0;
    float closestDist = points[0].DistanceFrom( point );

    for( size_t i=1; i<points.size(); i++ )
    {
        float d = points[i].DistanceFrom( point );
        if( d < closestDist )
        {
            closestIndex = i;
            closestDist = d;
        }
    }

    return closestIndex;
}

linearWeightedEdgeList SplitGraph(std::vector<VertexInfo>& vertexInfo, const Graph& graph, const linearWeightedEdgeList& activeEdges, int numSplits, const pointList& treeRoots)
{
    linearWeightedEdgeList edges;

    assert( graph.points.size() == graph.neighbours.size() );
    size_t numVerts = graph.points.size();

    // Build a neighbour list for active edges.
    fullNeighbourList activeNeighbours( numVerts );

    for( size_t i=0; i<activeEdges.size(); i++ )
    {
        vertIndex v1 = std::get<EdgeIndex1>( activeEdges[i] );
        vertIndex v2 = std::get<EdgeIndex2>( activeEdges[i] );

        activeNeighbours[v1].insert( v2 );
        activeNeighbours[v2].insert( v1 );
    }

    pointIndexList starts;
    if( treeRoots.size() == 0 )
    {
        // Pick some random roots.
        for( int i=0; i<numSplits-1; i++ )
        {
            starts.push_back( randSizeT(0, numVerts-1) );
        }
    }
    else
    {
        // Find nearest vertex to each treeRoot.
        for( size_t i=0; i<treeRoots.size(); i++ )
        {
            vertIndex vi = FindNearestVertexToPoint( graph.points, treeRoots[i] );
            starts.push_back( vi );
        }
    }

    for( size_t i=0; i<starts.size(); i++ )
    {
        SplitGraph_BFSFloodFillOwnership( (int)i+1, starts[i], vertexInfo, activeNeighbours, graph.weights, activeEdges );
    }

    for( size_t i=0; i<graph.points.size(); i++ )
    {
        const neighbourList& currentNeighbours = activeNeighbours[i];

        for( const vertIndex n : currentNeighbours )
        {
            if( i < n )
            {
                if( vertexInfo[i].owner == vertexInfo[n].owner )
                {
                    edges.push_back( { 1.0f, i, n } ); // TODO: Fix the 1.0f.
                }
            }
        }
    }

    return edges;
}

vertIndex FindNextNeighbourClockwiseFromIndex(const pointList& points, const pointIndexList& validVerts, const neighbourList& neighbours, vec2 pointPos, vec2 previousPos)
{
    // Get dir and angle to previous point.
    vec2 startDir = previousPos - pointPos;
    float startAngle = atan2f( -startDir.y, startDir.x );
    if( startAngle < 0 )
        startAngle += 2*PI;

    // Find next clockwise neighbour.
    float largestAngle = -FLT_MAX;
    vertIndex largestIndex = -1;
    for( const vertIndex nIndex : neighbours )
    {
        // Ensure the neighbour is in the boundary list.
        auto itBoundary = std::find( validVerts.begin(), validVerts.end(), nIndex );
        if( itBoundary == validVerts.end() )
            continue;

        vec2 dir = points[nIndex] - pointPos;
        float angle = atan2f( -dir.y, dir.x );
        if( angle < 0 )
            angle += 2*PI;
        float angleDiff = angle - startAngle;
        if( angleDiff < 0 )
            angleDiff += 2*PI;

        if( angleDiff >= largestAngle )
        {
            largestAngle = angleDiff;
            largestIndex = nIndex;
        }
    }

    return largestIndex;
}

pointIndexList BuildBoundaryVertexList(treeIndex treeLabel, const pointList& points, const fullNeighbourList& neighbours, const std::vector<VertexInfo>& vertexInfo)
{
    pointIndexList boundaryVerts;
    for( size_t i=0; i<points.size(); i++ )
    {
        if( vertexInfo[i].owner == treeLabel )
        {
            const neighbourList& currentNeighbours = neighbours[i];

            for( const vertIndex nIndex : currentNeighbours )
            {
                // If a neighbour isn't in our tree, this is a boundary vert.
                if( vertexInfo[nIndex].owner != treeLabel )
                {
                    boundaryVerts.push_back( i );
                    break;
                }
            }
        }
    }

    return boundaryVerts;
}

pointIndexList BuildBoundaryVertexList_Method2(treeIndex treeLabel, const pointList& points, const fullNeighbourList& neighbours, const std::vector<VertexInfo>& vertexInfo, const vertIndexList& vertsInRegion)
{
    pointIndexList boundaryVerts;
    for( size_t i=0; i<vertsInRegion.size(); i++ )
    {
        const neighbourList& currentNeighbours = neighbours[vertsInRegion[i]];

        for( const vertIndex nIndex : currentNeighbours )
        {
            // If the neighbour is inside our region, go to the next one.
            if( std::find( vertsInRegion.begin(), vertsInRegion.end(), nIndex ) != vertsInRegion.end() )
                continue;

            // If a neighbour isn't in our tree, this is a boundary vert.
            if( vertexInfo[nIndex].owner != treeLabel )
            {
                boundaryVerts.push_back( vertsInRegion[i] );
                break;
            }
        }
    }

    return boundaryVerts;
}

vertIndex FindTopmostPointIndex(const pointList& points, const pointIndexList& allowedVerts)
{
    // 0,0 is at top left.
    float smallestY = FLT_MAX;
    vertIndex smallestIndex = -1;

    for( size_t i=0; i<allowedVerts.size(); i++ )
    {
        if( points[allowedVerts[i]].y < smallestY )
        {
            smallestY = points[allowedVerts[i]].y;
            smallestIndex = allowedVerts[i];
        }
    }

    return smallestIndex;
}

vertIndexList CreateGroupOfVertsNotBlockedByVertices(const Graph& graph, std::vector<VertexInfo>& vertexInfo, const vertIndexList& blockers, const vertIndex startIndex)
{
    vertIndexList connectedVerts;

    // Init vertexInfo struct for each vertex.
    // Only used for closed flag. // TODO: Could easily be more efficient with a bit array.
    for( size_t i=0; i<graph.points.size(); i++ )
    {
        vertexInfo[i].closed = false;
    }

    // Set all blocked verts to be closed.
    for( size_t i=0; i<blockers.size(); i++ )
    {
        vertexInfo[blockers[i]].closed = true;
    }

    // Open list, no sorting.
    pointIndexList openList;
    openList.push_back( startIndex );
    connectedVerts.push_back( startIndex );

    // Loop until we've visited all neighbours,
    //    making a list of all verts connected to startIndex.
    while( openList.size() > 0 )
    {
        // Grab the last element of the open list, which is the lowest weight vertex.
        vertIndex currentIndex = openList[openList.size()-1];
        openList.pop_back();
        vertexInfo[currentIndex].closed = true;

        // Loop through neighbours.
        for( const vertIndex nIndex : graph.neighbours[currentIndex] )
        {
            // If the neighbour is closed, skip over it.
            if( vertexInfo[nIndex].closed )
                continue;

            // If the verts aren't part of the same region.
            if( vertexInfo[nIndex].owner != vertexInfo[currentIndex].owner )
                continue;

            openList.push_back( nIndex );
            connectedVerts.push_back( nIndex );
        }
    }

    return connectedVerts;
}

vertIndexList CreateListOfVertsWithinStepsOfVertex(const Graph& graph, std::vector<VertexInfo>& vertexInfo, const vertIndex startIndex, float maxWeight)
{
    vertIndexList verts;

    int flags = DijkstraSearchObject::DBF_ForceSameOwnership | DijkstraSearchObject::DBF_CheckWeights;
    DijkstraSearchObject dso( graph, vertexInfo, startIndex, (DijkstraSearchObject::DijkstraBitFlags)flags );
    dso.m_MaxWeight = maxWeight;
    verts = dso.Search();

    return verts;
}

vertIndexList CreateListOfVertsWithinRadiusOfVertex(const Graph& graph, vertIndex center, float radius)
{
    assert( center < graph.points.size() );

    vertIndexList verts;

    float radiusSquared = radius*radius;

    for( size_t i=0; i<graph.points.size(); i++ )
    {
        float distSquared = (graph.points[i] - graph.points[center]).LengthSquared();
        if( distSquared < radiusSquared )
        {
            verts.push_back( i );
        }
    }

    return verts;
}

void Graph_SwapIndex(std::vector<vec2>& pointList, fullNeighbourList& neighbours, vertIndex v1, vertIndex v2)
{
    if( v1 == v2 )
        return;

    vec2 temp = pointList[v1];
    pointList[v1] = pointList[v2];
    pointList[v2] = temp;

    neighbourList tempNeighbours = neighbours[v1];
    neighbours[v1] = neighbours[v2];
    neighbours[v2] = tempNeighbours;

    for( size_t i=0; i<neighbours.size(); i++ )
    {
        if( neighbours[i].count(v1) )
        {
            neighbours[i].erase( v1 );
            neighbours[i].insert( v2 );
        }
        else if( neighbours[i].count(v2) )
        {
            neighbours[i].erase( v2 );
            neighbours[i].insert( v1 );
        }
    }
}

void Graph_RemovePoint(std::vector<vec2>& pointList, fullNeighbourList& neighbours, vertIndex indexToRemove)
{
    vertIndex lastVertIndex = pointList.size()-1;

    // Remove the edge from all neighbours to us.
    for( size_t i : neighbours[indexToRemove] )
    {
        neighbours[i].erase( indexToRemove );
    }

    pointList[indexToRemove] = pointList[lastVertIndex];
    neighbours[indexToRemove] = neighbours[lastVertIndex];

    // Point the edge from all neighbours to the new index.
    for( size_t i : neighbours[indexToRemove] )
    {
        neighbours[i].erase( lastVertIndex );
        neighbours[i].insert( indexToRemove );
    }

    //// If any neighbour has this one as a neighbour, fix it.
    //for( size_t i=0; i<neighbours.size(); i++ )
    //{
    //    if( neighbours[i].count(lastVertIndex) )
    //    {
    //        neighbours[i].erase( lastVertIndex );
    //        if( indexToRemove != lastVertIndex )
    //        {
    //            neighbours[i].insert( indexToRemove );
    //        }
    //    }
    //}

    pointList.pop_back();
    neighbours.pop_back();
}
