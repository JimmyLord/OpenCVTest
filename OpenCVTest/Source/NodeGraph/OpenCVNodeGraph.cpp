//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVNodeGraph.h"
#include "OpenCVNodes.h"
#include "OpenCVNodeTypeManager.h"

OpenCVNodeGraph::OpenCVNodeGraph(EngineCore* pEngineCore, OpenCVNodeTypeManager* pNodeTypeManager)
: MyNodeGraph( pEngineCore, pNodeTypeManager )
{
    m_ImageWidth = 200;
    m_AutoRun = true;
    m_HoverZoomLevel = 4.0;
}

OpenCVNodeGraph::~OpenCVNodeGraph()
{
}

void OpenCVNodeGraph::Save()
{
    EditorDocument::Save();

    // Save NodeGraph as JSON string.
    {
        const char* filename = GetRelativePath();
        if( filename[0] == '\0' )
        {
            return;
        }

        // Tack on proper extension.
        char newFilename[MAX_PATH];
        bool hasExtension = false;
        uint32 filenameLen = (uint32)strlen( filename );
        uint32 desiredExtLen = (uint32)strlen( GetFileExtension() );
        if( filenameLen > desiredExtLen && strcmp( &filename[filenameLen-desiredExtLen], GetFileExtension() ) == 0 )
        {
            hasExtension = true;
            snprintf_s( newFilename, MAX_PATH, "%s", filename );
        }
        else
        {
            snprintf_s( newFilename, MAX_PATH, "%s%s", filename, GetFileExtension() );
        }

        // Create JSON string.
        cJSON* jNodeGraph = ExportAsJSONObject();

        char* jsonString = cJSON_Print( jNodeGraph );

        cJSON_Delete( jNodeGraph );

        FILE* pFile;
#if MYFW_WINDOWS
        fopen_s( &pFile, newFilename, "wb" );
#else
        pFile = fopen( newFilename, "wb" );
#endif
        fprintf( pFile, "%s", jsonString );
        fclose( pFile );

        cJSONExt_free( jsonString );
    }
}

void OpenCVNodeGraph::Load()
{
    MyNodeGraph::Load();
}

void OpenCVNodeGraph::AddItemsAboveNodeGraphWindow()
{
    ImGui::Text( "Scroll (%.2f,%.2f)", m_ScrollOffset.x, m_ScrollOffset.y );
    
    ImGui::SameLine();
    ImGui::Checkbox( "Auto Run", &m_AutoRun );
    
    ImGui::SameLine();
    ImGui::PushItemWidth( 100 );
    ImGui::DragFloat( "Image Width", &m_ImageWidth );
    
    ImGui::SameLine();
    ImGui::PushItemWidth( 100 );
    ImGui::DragFloat( "Hover Zoom", &m_HoverZoomLevel, 0.1f, 0.1f, 1000.0f );
    
    ImGui::SameLine( ImGui::GetWindowWidth() - 300 );
    ImGui::Checkbox( "Show grid", &m_GridVisible );
}

void OpenCVNodeGraph::AddAdditionalItemsToNodeContextMenu(MyNodeGraph::MyNode* pNode)
{
    // Draw context menu.
    if( ImGui::MenuItem( "Run", nullptr, false ) )
    {
        ((OpenCVBaseNode*)pNode)->Trigger( nullptr, true );
    }

    if( ImGui::MenuItem( "Run this node only", nullptr, false ) )
    {
        ((OpenCVBaseNode*)pNode)->Trigger( nullptr, false );
    }
}
