#include "OpenCVPCH.h"

#include "DijkstraSearchObject.h"
#include "Graph/GraphTypes.h"
#include "Graph/GraphHelpers.h"

DijkstraSearchObject::DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, vertIndex startIndex, DijkstraBitFlags flags)
    : DijkstraSearchObject( graph, vertexInfo, startIndex, -1, flags )
{
}

DijkstraSearchObject::DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, std::vector<vertIndex> startIndices, DijkstraBitFlags flags)
    : DijkstraSearchObject( graph, vertexInfo, startIndices, -1, flags )
{
}

DijkstraSearchObject::DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, vertIndex startIndex, vertIndex endIndex, DijkstraBitFlags flags)
    : DijkstraSearchObject( graph, vertexInfo, std::vector<vertIndex>(1, startIndex), endIndex, flags )
{
}

DijkstraSearchObject::DijkstraSearchObject(const Graph& graph, std::vector<VertexInfo>& vertexInfo, std::vector<vertIndex> startIndices, vertIndex endIndex, DijkstraBitFlags flags)
    : m_Graph( graph )
    , m_VertexInfo( vertexInfo )
    , m_EndIndex( endIndex )
    , m_Flags( flags )
    , m_MaxWeight( FLT_MAX )
{
    m_StartIndices = startIndices;
}

vertIndexList DijkstraSearchObject::Search()
{
    vertIndexList connectedVerts;

    if( m_Graph.neighbours.size() == 0 )
        return connectedVerts;

    // Init vertexInfo struct for each vertex.
    for( size_t i=0; i<m_Graph.points.size(); i++ )
    {
        m_VertexInfo[i].closed = false;
        m_VertexInfo[i].lowestWeight = FLT_MAX;
    }

    if( m_pPostInitCallback != nullptr )
    {
        m_pPostInitCallback();
    }

    // Open list.
    pointIndexList openList = m_StartIndices;
    if( (m_Flags & DBF_ReturnShortestPath) == 0 )
    {
        connectedVerts.insert( connectedVerts.end(), m_StartIndices.begin(), m_StartIndices.end() );
    }

    for( vertIndex v : m_StartIndices )
    {
        m_VertexInfo[v].lowestWeight = 0;
    }

    // Loop until we've visited all neighbours,
    //    making a list of all verts connected to startIndex.
    while( openList.size() > 0 )
    {
        // Grab the last element of the open list, which is the lowest weight vertex.
        vertIndex currentIndex = openList[openList.size()-1];
        openList.pop_back();
        m_VertexInfo[currentIndex].closed = true;

        if( m_pVertexSelectedFromOpenListCallback != nullptr )
        {
            m_pVertexSelectedFromOpenListCallback( currentIndex );
        }

        // Loop through neighbours.
        for( const vertIndex nIndex : m_Graph.neighbours[currentIndex] )
        {
            // If the neighbour is closed, skip over it.
            if( m_VertexInfo[nIndex].closed )
                continue;

            if( m_Flags & DijkstraBitFlags::DBF_ForceSameOwnership )
            {
                // If the verts aren't part of the same region.
                if( m_VertexInfo[nIndex].owner != m_VertexInfo[currentIndex].owner )
                    continue;
            }

            // Check weights.
            if( (m_Flags & DijkstraBitFlags::DBF_CheckWeights) == 0 )
            {
                openList.push_back( nIndex );
                if( (m_Flags & DBF_ReturnShortestPath) == 0 )
                {
                    connectedVerts.push_back( nIndex );
                }
            }
            else
            {
                vertIndex lowerIndex = currentIndex < nIndex ? currentIndex : nIndex;
                vertIndex higherIndex = currentIndex > nIndex ? currentIndex : nIndex;
                edgeWeight weight = 0;
                if( m_pWeightCalculationCallback )
                {
                    weight = m_pWeightCalculationCallback( currentIndex, nIndex, m_VertexInfo[currentIndex].parentIndex );
                }
                else
                {
                    weight = m_Graph.weights.at(lowerIndex).at(higherIndex);
                }

                edgeWeight totalWeight = m_VertexInfo[currentIndex].lowestWeight + weight;

                if( totalWeight < m_VertexInfo[nIndex].lowestWeight && totalWeight < m_MaxWeight )
                {
                    m_VertexInfo[nIndex].lowestWeight = totalWeight;

                    if( (m_Flags & DBF_ReturnShortestPath) == 0 )
                    {
                        connectedVerts.push_back( nIndex );
                    }

                    // If we already have a parent, then we have an old cost which was higher.
                    // Remove from open list and re-add it sorted in new position.
                    if( m_VertexInfo[nIndex].parentIndex != -1 && m_VertexInfo[nIndex].closed == false )
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
                            if( totalWeight > m_VertexInfo[openList[i]].lowestWeight )
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
                            if( (m_Flags & DBF_ReturnShortestPath) == 0 )
                            {
                                connectedVerts.push_back( nIndex );
                            }
                        }
                    }

                    // Set the new parent.
                    m_VertexInfo[nIndex].parentIndex = currentIndex;
                    // Copy the owner over.
                    //m_VertexInfo[nIndex].owner = vertexInfo[currentIndex].owner;
                }
            }
        }
    }

    if( m_Flags & DBF_ReturnShortestPath )
    {
        graphPath path;
        vertIndex i = m_EndIndex;
        while( i != -1 )
        {
            path.push_back( i );

            //if( m_VertexInfo[i].lowestWeight <= 0.0f )
            //    break;

            i = m_VertexInfo[i].parentIndex;
        }
        return path;
    }

    return connectedVerts;
}
