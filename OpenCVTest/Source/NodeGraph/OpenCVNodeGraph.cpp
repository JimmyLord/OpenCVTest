//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVNodeGraph.h"
#include "OpenCVNode.h"
#include "OpenCVNodeTypeManager.h"

OpenCVNodeGraph::OpenCVNodeGraph(EngineCore* pEngineCore, OpenCVNodeTypeManager* pNodeTypeManager)
: MyNodeGraph( pEngineCore, pNodeTypeManager )
{
    m_ImageWidth = 200;
    m_AutoRun = true;
}

OpenCVNodeGraph::~OpenCVNodeGraph()
{
}

void OpenCVNodeGraph::AddItemsAboveNodeGraphWindow()
{
    ImGui::Text( "Scroll (%.2f,%.2f)", m_ScrollOffset.x, m_ScrollOffset.y );
    ImGui::SameLine();
    ImGui::Checkbox( "Auto Run", &m_AutoRun );
    ImGui::SameLine();
    ImGui::PushItemWidth( 100 );
    ImGui::DragFloat( "Image Width", &m_ImageWidth );
    ImGui::SameLine( ImGui::GetWindowWidth() - 300 );
    ImGui::Checkbox( "Show grid", &m_GridVisible );
}

void OpenCVNodeGraph::AddAdditionalItemsToNodeContextMenu(MyNodeGraph::MyNode* pNode)
{
    // Draw context menu.
    if( ImGui::MenuItem( "Run", nullptr, false ) )
    {
        pNode->Trigger();
    }
}
