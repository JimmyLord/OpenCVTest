//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVNodeTypeManager.h"

#include "OpenCVNodeGraph.h"
#include "OpenCVNode.h"
#include "OpenCVNodes.h"

#undef AddVar
#define AddVar ComponentBase::AddVariable_Base

OpenCVNodeTypeManager::OpenCVNodeTypeManager()
: OpenCVBaseNodeTypeManager()
{
}

OpenCVNodeGraph::OpenCVNode* OpenCVNodeTypeManager::AddCreateNodeItemsToContextMenu(Vector2 pos, OpenCVNodeGraph* pNodeGraph)
{
    if( ImGui::BeginMenu( "Input" ) )
    {
        if( ImGui::MenuItem( "File" ) )      { ImGui::EndMenu(); return CreateNode( "Input_File", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Convert" ) )
    {
        if( ImGui::MenuItem( "Grayscale" ) ) { ImGui::EndMenu(); return CreateNode( "Convert_Grayscale", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Filter" ) )
    {
        if( ImGui::MenuItem( "Threshold" ) ) { ImGui::EndMenu(); return CreateNode( "Filter_Threshold", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Bilateral" ) ) { ImGui::EndMenu(); return CreateNode( "Filter_Bilateral", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    return nullptr;
}

OpenCVNodeGraph::OpenCVNode* OpenCVNodeTypeManager::CreateNode(const char* typeName, Vector2 pos, OpenCVNodeGraph* pNodeGraph)
{
    OpenCVNodeGraph::NodeID newNodeID = pNodeGraph->GetNextNodeIDAndIncrement();

#define TypeIs(name) strcmp( typeName, name ) == 0 )

    if( TypeIs( "Input_File" )        return MyNew OpenCVNode_Input_File(        pNodeGraph, newNodeID, "File", pos );
    if( TypeIs( "Convert_Grayscale" ) return MyNew OpenCVNode_Convert_Grayscale( pNodeGraph, newNodeID, "Grayscale", pos );
    if( TypeIs( "Filter_Threshold" )  return MyNew OpenCVNode_Filter_Threshold(  pNodeGraph, newNodeID, "Threshold", pos );
    if( TypeIs( "Filter_Bilateral" )  return MyNew OpenCVNode_Filter_Bilateral(  pNodeGraph, newNodeID, "Bilateral", pos );

#undef TypeIs

    LOGInfo( LOGTag, "NodeType not found: %s", typeName );

    return nullptr;
}
