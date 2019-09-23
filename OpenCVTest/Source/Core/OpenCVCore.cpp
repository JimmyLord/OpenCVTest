//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVCore.h"
#include "NodeGraph/OpenCVNodeGraph.h"
#include "NodeGraph/OpenCVNodeTypeManager.h"
#include "Settings/Settings.h"
#include "Tests/Tests.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"

OpenCVCore::OpenCVCore()
{
    m_pActiveDocument = nullptr;
    m_pLastActiveDocument = nullptr;

    m_pNodeTypeManager = nullptr;
}

OpenCVCore::~OpenCVCore()
{
    delete m_pNodeTypeManager;
}

//====================================================================================================
// Initialization.
//====================================================================================================

void OpenCVCore::OneTimeInit()
{
    GameCore::OneTimeInit(); // Note: Not calling EngineCore::OneTimeInit().

    // Copied from EngineCore::OneTimeInit().
    {
        m_SingleFrameMemoryStack.Initialize( 5000000 );

        m_pEditorState = MyNew EditorState( this );

        if( m_pImGuiManager == nullptr )
            m_pImGuiManager = MyNew ImGuiManager;

        if( m_pImGuiManager )
        {
            m_pImGuiManager->Init( (float)GetWindowWidth(), (float)GetWindowHeight() );
        }

        // This should be the editor pref load point for ImGui Editor builds.
        m_pEditorPrefs = MyNew EditorPrefs;
        m_pEditorPrefs->Init();
        m_pEditorPrefs->LoadWindowSizePrefs();
        m_pEditorPrefs->LoadPrefs();
    }

    m_pNodeTypeManager = CreateNodeTypeManager();

    // Restore the previously open documents.
    {
        cJSON* jEditorPrefs = m_pEditorPrefs->GetEditorPrefsJSONString();
        cJSON* jOpenDocumentsArray = cJSON_GetObjectItem( jEditorPrefs, "State_OpenDocuments" );
        if( jOpenDocumentsArray )
        {
            for( int i=0; i<cJSON_GetArraySize( jOpenDocumentsArray ); i++ )
            {
                cJSON* jDocument = cJSON_GetArrayItem( jOpenDocumentsArray, i );
                char* relativePath = jDocument->valuestring;
                int len = (int)strlen( relativePath );

                EditorDocument* pNewDocument = nullptr;

                if( strcmp( &relativePath[len-strlen(".opencvnodegraph")], ".opencvnodegraph" ) == 0 )
                {
                    pNewDocument = MyNew OpenCVNodeGraph( this, m_pNodeTypeManager );
                }

                if( pNewDocument != nullptr )
                {
                    pNewDocument->SetRelativePath( relativePath );
                    pNewDocument->Load();
                    m_pActiveDocument = pNewDocument;
                    GetEditorState()->OpenDocument( pNewDocument );
                }
            }
        }
    }
}

//====================================================================================================
// Input/Event handling.
//====================================================================================================
bool OpenCVCore::OnKeys(GameCoreButtonActions action, int keycode, int unicodechar)
{
    if( m_pActiveDocument && m_pActiveDocument->HandleInput( action, keycode, -1, -1, -1, -1, -1 ) )
        return true;

    if( m_pImGuiManager->HandleInput( action, keycode, -1, -1, -1, -1, -1 ) )
        return true;

    return false;
}

bool OpenCVCore::OnTouch(int action, int id, float x, float y, float pressure, float size)
{
    if( m_pImGuiManager->HandleInput( -1, -1, action, id, x, y, pressure ) )
        return true;

    return false;
}

bool OpenCVCore::OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id)
{
    return GameCore::OnButtons( action, id ); // Note: Not calling GameCore::OneTimeInit().
}

//====================================================================================================
// Update/Draw.
//====================================================================================================

void OpenCVCore::OnFocusLost()
{
    GameCore::OnFocusLost(); // Note: Not calling GameCore::OnFocusLost().

    m_pImGuiManager->OnFocusLost();
}

EditorDocument* OpenCVCore::AddDocumentMenu(EngineCore* pEngineCore, EditorDocument* pDocument)
{
    EditorDocument* pNewDocument = nullptr;

    if( ImGui::BeginMenu( "Document" ) )
    {
        //if( ImGui::BeginMenu( "New Document" ) )
        {
            //if( ImGui::MenuItem( "Visual Script" ) )
            //{
            //    pNewDocument = MyNew MyNodeGraph( pEngineCore, &g_VisualScriptNodeTypeManager );
            //}
            //if( ImGui::MenuItem( "Heightmap" ) )
            //{
            //    pNewDocument = MyNew EditorDocument_Heightmap( pEngineCore, nullptr );
            //}
            if( ImGui::MenuItem( "New OpenCV Node Graph" ) )
            {
                pNewDocument = MyNew OpenCVNodeGraph( this, m_pNodeTypeManager );
            }
            //ImGui::EndMenu(); // "New Document"
        }

        if( ImGui::MenuItem( "Load Document..." ) )
        {
            pNewDocument = LoadDocument( pEngineCore );
            //pNewDocument = pDocument->EditorDocumentMenuCommand( EditorDocument::EditorDocumentMenuCommand_Load );
        }

        if( ImGui::BeginMenu( "Load Recent Document" ) )
        {
            uint32 numRecentDocuments = pEngineCore->GetEditorPrefs()->Get_Document_NumRecentDocuments();
            if( numRecentDocuments == 0 )
            {
                ImGui::Text( "no recent documents." );
            }

            for( uint32 i=0; i<numRecentDocuments; i++ )
            {
                std::string relativePathStr = pEngineCore->GetEditorPrefs()->Get_Document_RecentDocument( i );
                const char* relativePath = relativePathStr.c_str();

                if( ImGui::MenuItem( relativePath ) )
                {
                    uint32 len = (uint32)strlen( relativePath );

                    //if( strcmp( &relativePath[len-strlen(".myvisualscript")], ".myvisualscript" ) == 0 )
                    //{
                    //    pNewDocument = MyNew MyNodeGraph( pEngineCore, &g_VisualScriptNodeTypeManager );
                    //}

                    //if( strcmp( &relativePath[len-strlen(".myheightmap")], ".myheightmap" ) == 0 )
                    //{
                    //    pNewDocument = MyNew EditorDocument_Heightmap( pEngineCore, nullptr );
                    //}

                    if( strcmp( &relativePath[len-strlen(".opencvnodegraph")], ".opencvnodegraph" ) == 0 )
                    {
                        pNewDocument = MyNew OpenCVNodeGraph( pEngineCore, m_pNodeTypeManager );
                    }

                    if( pNewDocument )
                    {
                        pNewDocument->SetRelativePath( relativePath );
                        pNewDocument->Load();

                        pEngineCore->GetEditorPrefs()->AddRecentDocument( relativePath );
                    }
                    else
                    {
                        LOGError( LOGTag, "Document not found: %s - removing from recent list.", relativePath );
                        pEngineCore->GetEditorPrefs()->RemoveRecentDocument( relativePath );
                        i--;
                        numRecentDocuments--;
                    }
                }
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // Save.
        {
            char tempstr[MAX_PATH + 10];
            if( pDocument )
            {
                if( pDocument->GetFilename()[0] == '\0' )
                {
                    sprintf_s( tempstr, MAX_PATH + 10, "Save Untitled as..." );
                }
                else
                {
                    sprintf_s( tempstr, MAX_PATH + 10, "Save %s", pDocument->GetFilename() );
                }
            }
            else
            {
                sprintf_s( tempstr, MAX_PATH + 10, "Save" );
            }

            if( ImGui::MenuItem( tempstr, "Ctrl-S", false, pDocument != nullptr ) )
            {
                pDocument->EditorDocumentMenuCommand( EditorDocument::EditorDocumentMenuCommand_Save );
            }
        }

        if( ImGui::MenuItem( "Save As...", nullptr, false, pDocument != nullptr ) )
        {
            pDocument->EditorDocumentMenuCommand( EditorDocument::EditorDocumentMenuCommand_SaveAs );
        }

        //if( ImGui::MenuItem( "Save All", "Ctrl-Shift-S", false, pDocument != nullptr ) )
        //{
        //    pDocument->EditorDocumentMenuCommand( EditorDocument::EditorDocumentMenuCommand_SaveAll );
        //}

        ImGui::EndMenu(); // "Document"
    }

    return pNewDocument;
}

EditorDocument* OpenCVCore::LoadDocument(EngineCore* pEngineCore)
{
    char tempFilter[256];
    //sprintf_s( tempFilter, 256, "%s\0All\0*.*\0", GetDefaultFileSaveFilter() );
    sprintf_s( tempFilter, 256, "%s=%s=",
        "OpenCV NodeGraph Files=*.opencvnodegraph",
        "All=*.*"
    );
    uint32 tempFilterLen = (uint32)strlen( tempFilter );
    for( uint32 i=0; i<tempFilterLen; i++ )
    {
        if( tempFilter[i] == '=' )
        {
            tempFilter[i] = '\0';
        }
    }

    const char* filename = FileOpenDialog( "Data\\", tempFilter );
    if( filename[0] != '\0' )
    {
        char path[MAX_PATH];
        strcpy_s( path, MAX_PATH, filename );
        const char* relativePath = ::GetRelativePath( path );

        int len = (int)strlen( relativePath );
        EditorDocument* pNewDocument = nullptr;

        if( strcmp( &relativePath[len-strlen(".opencvnodegraph")], ".opencvnodegraph" ) == 0 )
        {
            pNewDocument = MyNew OpenCVNodeGraph( pEngineCore, m_pNodeTypeManager );
        }        

        if( pNewDocument == nullptr )
        {
            LOGError( LOGTag, "This filetype is not supported.\n" );
            return nullptr;
        }

        pNewDocument->SetRelativePath( relativePath );
        pNewDocument->Load();

        pEngineCore->GetEditorPrefs()->AddRecentDocument( relativePath );

        return pNewDocument;
    }

    return nullptr;
}

float OpenCVCore::Tick(float deltaTime)
{
    GameCore::Tick( deltaTime ); // Note: Not calling EngineCore::Tick().

    m_pImGuiManager->StartFrame();
    m_pImGuiManager->StartTick( deltaTime );

    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::BeginMenu( "Tests" ) )
        {
            if( ImGui::MenuItem( "Simple Modification" ) )
                TestSimpleModification();

            if( ImGui::MenuItem( "DFT" ) )
                TestDFT();

            if( ImGui::MenuItem( "Video" ) )
                TestVideo();

            if( ImGui::MenuItem( "Morph" ) )
                TestMorph();

            if( ImGui::MenuItem( "Threshold" ) )
                TestThreshold();

            ImGui::EndMenu();
        }

        EditorDocument* pNewDocument = AddDocumentMenu( this, m_pLastActiveDocument );
        if( pNewDocument != nullptr )
        {
            m_pActiveDocument = pNewDocument;
            GetEditorState()->OpenDocument( pNewDocument );
        }

        ImGui::EndMainMenuBar();
    }

    return 0;
}

void OpenCVCore::OnDrawFrame(unsigned int canvasid)
{
    GameCore::OnDrawFrame( 0 ); // Note: Not calling EngineCore::OnDrawFrame().

    m_pRenderer->ClearBuffers( true, true, false );

    // Clear m_pActiveDocument, a new one will be set if a document is in focus.
    m_pActiveDocument = nullptr;

    std::vector<EditorDocument*>* pOpenDocuments = &GetEditorState()->m_pOpenDocuments;
    for( uint32 i=0; i<pOpenDocuments->size(); i++ )
    {
        EditorDocument* pDocument = (*pOpenDocuments)[i];

        if( pDocument )
        {
            ImGui::SetNextWindowSize( ImVec2(1000, 550), ImGuiSetCond_FirstUseEver );
            bool documentStillOpen = true;
            pDocument->CreateWindowAndUpdate( &documentStillOpen );
            if( pDocument->IsWindowFocused() )
            {
                m_pActiveDocument = pDocument;
                m_pLastActiveDocument = pDocument;
            }

            if( documentStillOpen == false )
            {
                if( pDocument->HasUnsavedChanges() )
                {
                    //m_ShowWarning_CloseDocument = true;
                    //m_DocumentIndexToCloseAfterWarning = i;
                }
                else
                {
                    m_pActiveDocument = nullptr;
                    m_pLastActiveDocument = nullptr;
                    delete pDocument;
                    pOpenDocuments->erase( pOpenDocuments->begin() + i );
                }
            }
        }
    }

    // End frame and draw imgui drawlists.
    if( m_pImGuiManager )
    {
        m_pImGuiManager->EndFrame( (float)m_MainViewport.GetWidth(), (float)m_MainViewport.GetHeight(), true );
    }
}
