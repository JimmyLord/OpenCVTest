//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVNodeGraphEditorCommands.h"
#include "OpenCVNode.h"
#include "OpenCVNodeGraph.h"
#include "OpenCVNodes.h"

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_AddNode
//====================================================================================================

EditorCommand_OpenCVNodeGraph_AddNode::EditorCommand_OpenCVNodeGraph_AddNode(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::OpenCVNode* pNode)
{
    m_Name = "EditorCommand_OpenCVNodeGraph_AddNode";

    MyAssert( pNode != nullptr );
    MyAssert( pNode->GetNodeGraph() == pNodeGraph );

    m_pNodeGraph = pNodeGraph;
    m_pNode = pNode;
    m_DeleteNodeWhenDestroyed = false;
}

EditorCommand_OpenCVNodeGraph_AddNode::~EditorCommand_OpenCVNodeGraph_AddNode()
{
    if( m_DeleteNodeWhenDestroyed )
    {
        delete m_pNode;
    }
}

void EditorCommand_OpenCVNodeGraph_AddNode::Do()
{
    m_pNodeGraph->AddExistingNode( m_pNode );
    m_DeleteNodeWhenDestroyed = false;
}

void EditorCommand_OpenCVNodeGraph_AddNode::Undo()
{
    m_pNodeGraph->RemoveExistingNode( m_pNode );
    m_DeleteNodeWhenDestroyed = true;
}

EditorCommand* EditorCommand_OpenCVNodeGraph_AddNode::Repeat()
{
    // Do nothing.

    return nullptr;
}

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_DeleteNodes
//====================================================================================================

EditorCommand_OpenCVNodeGraph_DeleteNodes::EditorCommand_OpenCVNodeGraph_DeleteNodes(OpenCVNodeGraph* pNodeGraph, ImVector<OpenCVNodeGraph::NodeID>& selectedNodeIDs)
{
    m_Name = "EditorCommand_OpenCVNodeGraph_DeleteNodes";

    MyAssert( selectedNodeIDs.size() != 0 );

    m_pNodeGraph = pNodeGraph;
    for( int i=0; i<selectedNodeIDs.Size; i++ )
    {
        OpenCVNodeGraph::NodeID nodeID = selectedNodeIDs[i];

        int nodeIndex = m_pNodeGraph->FindNodeIndexByID( nodeID );
        OpenCVNodeGraph::OpenCVNode* pNode = m_pNodeGraph->m_Nodes[nodeIndex];
        MyAssert( pNode->GetNodeGraph() == m_pNodeGraph );

        m_pNodes.push_back( pNode );
    }

    for( uint32 nodeIndex=0; nodeIndex<m_pNodes.size(); nodeIndex++ )
    {
        OpenCVNodeGraph::OpenCVNode* pNode = m_pNodes[nodeIndex];

        for( uint32 slotIndex=0; slotIndex<pNode->m_InputsCount; slotIndex++ )
        {
            int count = 0;
            while( OpenCVNodeGraph::OpenCVNodeLink* pNodeLink = m_pNodeGraph->FindLinkConnectedToInput( pNode->m_ID, slotIndex, count++ ) )
            {
                m_Links.push_back( *pNodeLink );
            }
        }

        for( uint32 slotIndex=0; slotIndex<pNode->m_OutputsCount; slotIndex++ )
        {
            int count = 0;
            while( OpenCVNodeGraph::OpenCVNodeLink* pNodeLink = m_pNodeGraph->FindLinkConnectedToOutput( pNode->m_ID, slotIndex, count++ ) )
            {
                m_Links.push_back( *pNodeLink );
            }
        }
    }

    m_DeleteNodesWhenDestroyed = true;
}

EditorCommand_OpenCVNodeGraph_DeleteNodes::~EditorCommand_OpenCVNodeGraph_DeleteNodes()
{
    if( m_DeleteNodesWhenDestroyed )
    {
        for( uint32 nodeIndex=0; nodeIndex<m_pNodes.size(); nodeIndex++ )
        {
            delete m_pNodes[nodeIndex];
        }
    }
}

void EditorCommand_OpenCVNodeGraph_DeleteNodes::Do()
{
    // Remove Nodes.
    for( uint32 nodeIndex=0; nodeIndex<m_pNodes.size(); nodeIndex++ )
    {
        OpenCVNodeGraph::OpenCVNode* pNode = m_pNodes[nodeIndex];

        m_pNodeGraph->RemoveExistingNode( pNode );

        // Remove all connections to this node.
        for( int i=0; i<m_pNodeGraph->m_Links.size(); i++ )
        {
            if( m_pNodeGraph->m_Links[i].m_InputNodeID == pNode->m_ID || m_pNodeGraph->m_Links[i].m_OutputNodeID == pNode->m_ID )
            {
                m_pNodeGraph->m_Links.erase_unsorted( m_pNodeGraph->m_Links.Data + i );
                i--;
            }
        }
    }

    m_DeleteNodesWhenDestroyed = true;
}

void EditorCommand_OpenCVNodeGraph_DeleteNodes::Undo()
{
    // Restore Nodes.
    for( uint32 nodeIndex=0; nodeIndex<m_pNodes.size(); nodeIndex++ )
    {
        m_pNodeGraph->AddExistingNode( m_pNodes[nodeIndex] );
    }

    // Restore all links.
    for( uint32 i=0; i<m_Links.size(); i++ )
    {
        m_pNodeGraph->m_Links.push_back( m_Links[i] );
    }

    m_DeleteNodesWhenDestroyed = false;
}

EditorCommand* EditorCommand_OpenCVNodeGraph_DeleteNodes::Repeat()
{
    // Do nothing.

    return nullptr;
}

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_CreateLink
//====================================================================================================

EditorCommand_OpenCVNodeGraph_CreateLink::EditorCommand_OpenCVNodeGraph_CreateLink(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::OpenCVNodeLink nodeLink)
{
    m_Name = "EditorCommand_OpenCVNodeGraph_CreateLink";

    m_pNodeGraph = pNodeGraph;
    m_NodeLink = nodeLink;
    m_NodeLinkIndex = -1;
}

EditorCommand_OpenCVNodeGraph_CreateLink::~EditorCommand_OpenCVNodeGraph_CreateLink()
{
}

void EditorCommand_OpenCVNodeGraph_CreateLink::Do()
{
    m_NodeLinkIndex = m_pNodeGraph->m_Links.size();
    m_pNodeGraph->m_Links.push_back( m_NodeLink );
}

void EditorCommand_OpenCVNodeGraph_CreateLink::Undo()
{
    m_pNodeGraph->m_Links.erase_unsorted( m_pNodeGraph->m_Links.Data + m_NodeLinkIndex );
}

EditorCommand* EditorCommand_OpenCVNodeGraph_CreateLink::Repeat()
{
    // Do nothing.

    return nullptr;
}

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_DeleteLink
//====================================================================================================

EditorCommand_OpenCVNodeGraph_DeleteLink::EditorCommand_OpenCVNodeGraph_DeleteLink(OpenCVNodeGraph* pNodeGraph, int nodeLinkIndex)
{
    m_Name = "EditorCommand_OpenCVNodeGraph_DeleteLink";

    m_pNodeGraph = pNodeGraph;
    m_NodeLinkIndex = nodeLinkIndex;

    m_NodeLink = m_pNodeGraph->m_Links[nodeLinkIndex];
}

EditorCommand_OpenCVNodeGraph_DeleteLink::~EditorCommand_OpenCVNodeGraph_DeleteLink()
{
}

void EditorCommand_OpenCVNodeGraph_DeleteLink::Do()
{
    m_pNodeGraph->m_Links.erase_unsorted( m_pNodeGraph->m_Links.Data + m_NodeLinkIndex );
}

void EditorCommand_OpenCVNodeGraph_DeleteLink::Undo()
{
    m_pNodeGraph->m_Links.insert( m_pNodeGraph->m_Links.Data + m_NodeLinkIndex, m_NodeLink );
}

EditorCommand* EditorCommand_OpenCVNodeGraph_DeleteLink::Repeat()
{
    // Do nothing.

    return nullptr;
}
