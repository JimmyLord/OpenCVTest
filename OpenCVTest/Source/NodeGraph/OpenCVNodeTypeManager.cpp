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

VisualScriptNodeTypeManager::VisualScriptNodeTypeManager()
: OpenCVNodeTypeManager()
{
}

OpenCVNodeGraph::OpenCVNode* VisualScriptNodeTypeManager::AddCreateNodeItemsToContextMenu(Vector2 pos, OpenCVNodeGraph* pNodeGraph)
{
    if( ImGui::BeginMenu( "Values" ) )
    {
        if( ImGui::MenuItem( "Float" ) )                { ImGui::EndMenu(); return CreateNode( "Value_Float", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Color" ) )                { ImGui::EndMenu(); return CreateNode( "Value_Color", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "GameObject" ) )           { ImGui::EndMenu(); return CreateNode( "Value_GameObject", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Component" ) )            { ImGui::EndMenu(); return CreateNode( "Value_Component", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Math Operations" ) )
    {
        if( ImGui::MenuItem( "Add" ) )                  { ImGui::EndMenu(); return CreateNode( "MathOp_Add", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Conditions" ) )
    {
        if( ImGui::MenuItem( "GreaterEqual" ) )         { ImGui::EndMenu(); return CreateNode( "Condition_GreaterEqual", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Keyboard" ) )             { ImGui::EndMenu(); return CreateNode( "Condition_Keyboard", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Events" ) )
    {
        if( ImGui::MenuItem( "Keyboard" ) )             { ImGui::EndMenu(); return CreateNode( "Event_Keyboard", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Actions" ) )
    {
        if( ImGui::MenuItem( "Disable GameObject" ) )   { ImGui::EndMenu(); return CreateNode( "Disable_GameObject", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    return nullptr;
}

OpenCVNodeGraph::OpenCVNode* VisualScriptNodeTypeManager::CreateNode(const char* typeName, Vector2 pos, OpenCVNodeGraph* pNodeGraph)
{
    OpenCVNodeGraph::NodeID newNodeID = pNodeGraph->GetNextNodeIDAndIncrement();

#define TypeIs(name) strcmp( typeName, name ) == 0 )

    if( TypeIs( "Value_Float" )             return MyNew OpenCVNode_Value_Float(              pNodeGraph, newNodeID, "Float",             pos, 0.5f );
    if( TypeIs( "Value_Color" )             return MyNew OpenCVNode_Value_Color(              pNodeGraph, newNodeID, "Color",             pos, ColorByte(255, 255, 255, 255) );
    if( TypeIs( "Value_GameObject" )        return MyNew OpenCVNode_Value_GameObject(         pNodeGraph, newNodeID, "GameObject",        pos, nullptr );
    if( TypeIs( "Value_Component" )         return MyNew OpenCVNode_Value_Component(          pNodeGraph, newNodeID, "Component",         pos, nullptr );
    if( TypeIs( "MathOp_Add" )              return MyNew OpenCVNode_MathOp_Add(               pNodeGraph, newNodeID, "Add",               pos );
    if( TypeIs( "Condition_GreaterEqual" )  return MyNew OpenCVNode_Condition_GreaterEqual(   pNodeGraph, newNodeID, "GreaterEqual",      pos );
    if( TypeIs( "Condition_Keyboard" )      return MyNew OpenCVNode_Condition_Keyboard(       pNodeGraph, newNodeID, "If Key",            pos, GCBA_Down, 'Z' );
    if( TypeIs( "Event_Keyboard" )          return MyNew OpenCVNode_Event_Keyboard(           pNodeGraph, newNodeID, "Event Keys",        pos, pNodeGraph->GetEngineCore()->GetManagers()->GetEventManager() );
    if( TypeIs( "Disable_GameObject" )      return MyNew OpenCVNode_Disable_GameObject(       pNodeGraph, newNodeID, "DisableGameObject", pos, nullptr );

#undef TypeIs

    LOGInfo( LOGTag, "EventType not found: %s", typeName );

    return nullptr;
}
