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

MyNodeGraph::MyNode* OpenCVNodeTypeManager::AddCreateNodeItemsToContextMenu(Vector2 pos, MyNodeGraph* pNodeGraph)
{
    if( ImGui::BeginMenu( "File" ) )
    {
        if( ImGui::MenuItem( "Input" ) )      { ImGui::EndMenu(); return CreateNode( "File_Input", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Output" ) )     { ImGui::EndMenu(); return CreateNode( "File_Output", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Convert" ) )
    {
        if( ImGui::MenuItem( "Crop" ) )      { ImGui::EndMenu(); return CreateNode( "Convert_Crop", pos, pNodeGraph ); }
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

MyNodeGraph::MyNode* OpenCVNodeTypeManager::CreateNode(const char* typeName, Vector2 pos, MyNodeGraph* pNodeGraph)
{
    OpenCVNodeGraph::NodeID newNodeID = pNodeGraph->GetNextNodeIDAndIncrement();

#define TypeIs(name) strcmp( typeName, name ) == 0 )

    if( TypeIs( "File_Input" )        return MyNew OpenCVNode_File_Input(        (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Input", pos );
    if( TypeIs( "File_Output" )       return MyNew OpenCVNode_File_Output(       (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Output", pos );
    if( TypeIs( "Convert_Grayscale" ) return MyNew OpenCVNode_Convert_Grayscale( (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Grayscale", pos );
    if( TypeIs( "Convert_Crop" )      return MyNew OpenCVNode_Convert_Crop(      (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Crop", pos );
    if( TypeIs( "Filter_Threshold" )  return MyNew OpenCVNode_Filter_Threshold(  (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Threshold", pos );
    if( TypeIs( "Filter_Bilateral" )  return MyNew OpenCVNode_Filter_Bilateral(  (OpenCVNodeGraph*)pNodeGraph, newNodeID, "Bilateral", pos );

#undef TypeIs

    LOGInfo( LOGTag, "NodeType not found: %s", typeName );

    return nullptr;
}
