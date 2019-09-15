#include "OpenCVPCH.h"

#include "OpenCVCore.h"
#include "Tests/Tests.h"

OpenCVCore::OpenCVCore()
{
    m_pSprite = nullptr;

    m_Position.Set( 0, 0, 0 );
}

OpenCVCore::~OpenCVCore()
{
    if( m_pSprite != nullptr )
        m_pSprite->Release();
}

//====================================================================================================
// Initialization.
//====================================================================================================

void OpenCVCore::OneTimeInit()
{
    GameCore::OneTimeInit();

    TestSimpleModification();
    //TestDFT();
    //TestVideo();
    //TestMorph();
    //TestThreshold();

    //waitKey();

    //// Set Clear color to dark blue.
    //m_pRenderer->SetClearColor( ColorFloat( 0.0f, 0.0f, 0.2f, 0.0f ) );
    //m_pRenderer->SetClearDepth( 1.0f );

    //// Create a shader, texture and material.
    //TextureDefinition* pErrorTexture = GetManagers()->GetTextureManager()->GetErrorTexture();
    //ShaderGroup* pShader = MyNew ShaderGroup( this, "Data/Shaders/Shader_Texture.glsl", pErrorTexture );
    //TextureDefinition* pTexture = GetManagers()->GetTextureManager()->CreateTexture( "Data/Textures/Clouds.png" );
    //MaterialDefinition* pMaterial = GetManagers()->GetMaterialManager()->CreateMaterial();

    //// Assign the shader and texture to the material.
    //pMaterial->SetShader( pShader );
    //pMaterial->SetTextureColor( pTexture );

    //// Create a sprite, it's small since there's no view or projection matrix down below.
    //m_pSprite = MyNew MySprite();
    //m_pSprite->Create( GetManagers()->GetBufferManager(), 0.1f, 0.1f, 0, 1, 0, 1, Justify_Center, true );
    //m_pSprite->SetMaterial( pMaterial );

    //// Release the shader, texture and material.
    //pMaterial->Release();
    //pTexture->Release();
    //pShader->Release();
}

//====================================================================================================
// Input/Event handling.
//====================================================================================================

bool OpenCVCore::OnTouch(int action, int id, float x, float y, float pressure, float size)
{
    //if( GameCore::OnTouch( action, id, x, y, pressure, size ) )
    //    return true;

    //// Prefer 0,0 at bottom left.
    //y = GetWindowHeight() - y;

    //// Move the square to the mouse position in clip space.
    //m_Position.x = (x / GetWindowWidth()) * 2 - 1;
    //m_Position.y = (y / GetWindowHeight()) * 2 - 1;

    return false;
}

bool OpenCVCore::OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id)
{
    return GameCore::OnButtons( action, id );
}

//====================================================================================================
// Update/Draw.
//====================================================================================================

float OpenCVCore::Tick(float deltaTime)
{
    return GameCore::Tick( deltaTime );
}

void OpenCVCore::OnDrawFrame(unsigned int canvasid)
{
    GameCore::OnDrawFrame( 0 );

    //// Clear the screen.
    //m_pRenderer->ClearBuffers( true, true, false );

    //// Draw the sprite. No camera or projection, so coordinates are in clip space.
    //MyMatrix transform;
    //transform.CreateTranslation( m_Position );

    //m_pSprite->Draw( nullptr, nullptr, &transform );
}
