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
    m_HoverPixelsToShow = 32.0f;
}

OpenCVNodeGraph::~OpenCVNodeGraph()
{
}

bool OpenCVNodeGraph::HandleInput(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure)
{
    ImGuiIO& io = ImGui::GetIO();

    if( keyAction == GCBA_Down )
    {
        bool N  = !io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper; // No modifiers held
        bool C  =  io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper; // Ctrl
        bool A  = !io.KeyCtrl &&  io.KeyAlt && !io.KeyShift && !io.KeySuper; // Alt
        bool S  = !io.KeyCtrl && !io.KeyAlt &&  io.KeyShift && !io.KeySuper; // Shift
        bool CS =  io.KeyCtrl && !io.KeyAlt &&  io.KeyShift && !io.KeySuper; // Ctrl-Shift

        if( N  && keyCode == VK_F5 ) { EditorDocumentMenuCommand( EditorDocumentMenuCommand_Run ); return true; }
        if( C  && keyCode == ' ' )   { EditorDocumentMenuCommand( EditorDocumentMenuCommand_Run ); return true; }
    }

    return MyNodeGraph::HandleInput( keyAction, keyCode, mouseAction, id, x, y, pressure );
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

void OpenCVNodeGraph::Run()
{
    MyNodeGraph::Run();

    // If nothing is selected, find input nodes and run those, otherwise only run the selected nodes.
    if( m_SelectedNodeIDs.size() == 0 )
    {
        // Find input nodes and run recursively.
        for( int i=0; i<m_Nodes.size(); i++ )
        {
            OpenCVBaseNode* pNode = (OpenCVBaseNode*)m_Nodes[i];
            if( strcmp( pNode->GetType(), "File_Input" ) == 0 )
            {
                pNode->Trigger( nullptr, true );
            }
        }
    }
    else
    {
        // Run selected nodes.
        for( int i=0; i<m_SelectedNodeIDs.size(); i++ )
        {
            int nodeIndex = FindNodeIndexByID( m_SelectedNodeIDs[i] );
            ((OpenCVBaseNode*)m_Nodes[nodeIndex])->Trigger( nullptr, false );
        }
    }
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
    ImGui::DragFloat( "Hover Pixels", &m_HoverPixelsToShow, 1.0f, 1.0f, 64.0f );
    
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
