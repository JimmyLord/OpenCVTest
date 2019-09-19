//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"

#include "OpenCVCore.h"
#include "Tests/Tests.h"
#include "NodeGraph/OpenCVNodeGraph.h"
#include "NodeGraph/OpenCVNodeTypeManager.h"
#include "Classwork/ClassworkNodeTypeManager.h"

OpenCVCore::OpenCVCore()
{
    m_pImGuiManager = nullptr;
    m_pNodeGraph = nullptr;
    m_pNodeTypeManager = nullptr;
}

OpenCVCore::~OpenCVCore()
{
    delete m_pNodeGraph;
    delete m_pNodeTypeManager;
    delete m_pImGuiManager;
}

//====================================================================================================
// Initialization.
//====================================================================================================

void OpenCVCore::OneTimeInit()
{
    GameCore::OneTimeInit();

    if( m_pImGuiManager == nullptr )
        m_pImGuiManager = MyNew ImGuiManager;

    if( m_pImGuiManager )
    {
        m_pImGuiManager->Init( (float)GetWindowWidth(), (float)GetWindowHeight() );
    }

    m_pNodeTypeManager = MyNew ClassworkNodeTypeManager(); //OpenCVNodeTypeManager();
}

//====================================================================================================
// Input/Event handling.
//====================================================================================================

bool OpenCVCore::OnTouch(int action, int id, float x, float y, float pressure, float size)
{
    if( m_pImGuiManager->HandleInput( -1, -1, action, id, x, y, pressure ) )
        return true;

    return false;
}

bool OpenCVCore::OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id)
{
    return GameCore::OnButtons( action, id );
}

//====================================================================================================
// Update/Draw.
//====================================================================================================

void OpenCVCore::OnFocusLost()
{
    GameCore::OnFocusLost();

    m_pImGuiManager->OnFocusLost();
}

float OpenCVCore::Tick(float deltaTime)
{
    GameCore::Tick( deltaTime );

    m_pImGuiManager->StartFrame();
    m_pImGuiManager->StartTick( deltaTime );

    if( ImGui::CollapsingHeader( "Tests", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        if( ImGui::Button( "Test Simple Modification" ) )
            TestSimpleModification();

        if( ImGui::Button( "Test DFT" ) )
            TestDFT();

        if( ImGui::Button( "Test Video" ) )
            TestVideo();

        if( ImGui::Button( "Test Morph" ) )
            TestMorph();

        if( ImGui::Button( "Test Threshold" ) )
            TestThreshold();
    }

    if( ImGui::CollapsingHeader( "Document Test", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        if( ImGui::Button( "New Document" ) )
        {
            m_pNodeGraph = MyNew OpenCVNodeGraph( nullptr, m_pNodeTypeManager );
        }
    }

    return 0;
}

void OpenCVCore::OnDrawFrame(unsigned int canvasid)
{
    GameCore::OnDrawFrame( 0 );

    m_pRenderer->ClearBuffers( true, true, false );

    if( m_pNodeGraph )
    {
        ImGui::SetNextWindowSize( ImVec2(1000, 550), ImGuiSetCond_FirstUseEver );
        bool documentStillOpen = true;
        m_pNodeGraph->CreateWindowAndUpdate( &documentStillOpen );
    }

    // End frame and draw imgui drawlists.
    if( m_pImGuiManager )
    {
        m_pImGuiManager->EndFrame( (float)m_MainViewport.GetWidth(), (float)m_MainViewport.GetHeight(), true );
    }
}
