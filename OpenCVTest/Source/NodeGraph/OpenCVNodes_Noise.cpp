//
// Copyright (c) 2022 Jimmy Lord
//
#include "OpenCVPCH.h"
#include "OpenCVNodes_Noise.h"
#include "Graph/GraphHelpers.h"

OpenCVNode_Generate_SimplexNoise::OpenCVNode_Generate_SimplexNoise(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
    : OpenCVBaseNode( pNodeGraph, id, name, pos, 0, 1 )
{
}

OpenCVNode_Generate_SimplexNoise::~OpenCVNode_Generate_SimplexNoise()
{
    SAFE_RELEASE( m_pTexture );
}

void OpenCVNode_Generate_SimplexNoise::GenerateNoise()
{
    osn_context* noiseContext;
    open_simplex_noise( m_Seed, &noiseContext );

    cv::Vec3b* imageValues = (cv::Vec3b*)m_Image.ptr();
    uint32 imageStride = (uint32)( m_Image.step[0]/m_Image.channels() );

    for( int y = 0; y < m_ImageSize.y; y++ )
    {
        for( int x = 0; x < m_ImageSize.x; x++ )
        {
            vec2 offset = m_Offset;
            float freq = m_Frequency;
            float amplitude = 1.0f; //m_Amplitude;
            float persistance = 0.08f; //m_Persistance;
            float lacunarity = 5.0f; //m_Lacunarity;

            float totalNoise = 0;
            for( int octave = 0; octave < m_NumOctaves; octave++ )
            {
                vec2 pos = vec2((float)x,(float)y) * freq + offset;

                float noise = (float)open_simplex_noise2( noiseContext, pos.x, pos.y );

                totalNoise += noise * amplitude;
                amplitude *= persistance;
                freq *= lacunarity;

                offset.Set( fw::Random::Float(0,10000), fw::Random::Float(0,10000) );
            }
            
            totalNoise = totalNoise * 0.5f + 0.5f;

            uchar noiseChar = (uchar)( totalNoise * 255 );

            if( m_Inverse )
                noiseChar = 255 - noiseChar;

            imageValues[y*imageStride + x] = cv::Vec3b( noiseChar, noiseChar, noiseChar );
        }
    }

    open_simplex_noise_free( noiseContext );
}

void OpenCVNode_Generate_SimplexNoise::DrawTitle()
{
    if( m_Expanded )
        OpenCVBaseNode::DrawTitle();
    else
        ImGui::Text( "%s", m_Name );
}

bool OpenCVNode_Generate_SimplexNoise::DrawContents()
{
    bool modified = OpenCVBaseNode::DrawContents();

    cv::Mat* pDensityMask = GetInputImage( 0 );
    if( pDensityMask )
    {
        m_ImageSize.Set( pDensityMask->cols, pDensityMask->rows );
    }
    else
    {
        ImGui::DragInt2( "Size", &m_ImageSize.x, 1.0f, 1, 4096 );
        if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
    }

    AdjustKnownImageWidth( m_ImageSize.x );

    if( ImGui::Checkbox( "Use Fixed Seed", &m_UseFixedSeed ) ) { QuickRun( false ); }
    if( m_UseFixedSeed )
    {
        ImGui::SameLine();
        if( ImGui::DragInt( "Seed", &m_Seed, 1.0f ) ) { QuickRun( false ); }
    }

    if( ImGui::DragInt( "Octaves", &m_NumOctaves, 1, 1, 10 ) ) { QuickRun( false ); }
    if( ImGui::DragFloat( "Freq", &m_Frequency, 0.001f, 0.001f, 1.0f ) ) { QuickRun( false ); }
    ImGui::SameLine();
    if( ImGui::DragFloat2( "Offset", &m_Offset.x, 0.1f, 0.0f, 1000.0f ) ) { QuickRun( false ); }
    if( ImGui::Checkbox( "Inverse", &m_Inverse ) ) { QuickRun( false ); }

    if( ImGui::Button( "Generate" ) )
    {
        Trigger( nullptr, TriggerFlags::TF_Recursive );
    }

    DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

    return modified;
}

void OpenCVNode_Generate_SimplexNoise::TriggerGlobalRun()
{
    Trigger( nullptr, TriggerFlags::TF_Recursive );
}

bool OpenCVNode_Generate_SimplexNoise::Trigger(MyEvent* pEvent, TriggerFlags triggerFlags)
{
    //OpenCVBaseNode::Trigger( pEvent );

    cv::Mat* pDensityMask = GetInputImage( 0 );

    // Generate the image.
    if( m_UseFixedSeed )
    {
        srand( m_Seed );
    }

    m_Image = cv::Mat::zeros( cv::Size(m_ImageSize.x,m_ImageSize.y), CV_8UC3 );

    // Generate noise.
    GenerateNoise();

    // Display it.
    m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

    // Trigger the output nodes.
    TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );

    return false;
}

cJSON* OpenCVNode_Generate_SimplexNoise::ExportAsJSONObject()
{
    cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
    cJSONExt_AddIntArrayToObject( jNode, "m_ImageSize", &m_ImageSize.x, 2 );

    cJSON_AddNumberToObject( jNode, "m_UseFixedSeed", m_UseFixedSeed );
    cJSON_AddNumberToObject( jNode, "m_Seed", m_Seed );

    cJSON_AddNumberToObject( jNode, "m_NumOctaves", m_NumOctaves );
    cJSON_AddNumberToObject( jNode, "m_Frequency", m_Frequency );
    cJSONExt_AddFloatArrayToObject( jNode, "m_Offset", &m_Offset.x, 2 );
    cJSON_AddNumberToObject( jNode, "m_Inverse", m_Inverse );
    return jNode;
}

void OpenCVNode_Generate_SimplexNoise::ImportFromJSONObject(cJSON* jNode)
{
    OpenCVBaseNode::ImportFromJSONObject( jNode );
    cJSONExt_GetIntArray( jNode, "m_ImageSize", &m_ImageSize.x, 2 );

    cJSONExt_GetBool( jNode, "m_UseFixedSeed", m_UseFixedSeed );
    cJSONExt_GetInt( jNode, "m_Seed", m_Seed );

    cJSONExt_GetInt( jNode, "m_NumOctaves", m_NumOctaves );
    cJSONExt_GetFloat( jNode, "m_Frequency", m_Frequency );
    cJSONExt_GetFloatArray( jNode, "m_Offset", &m_Offset.x, 2 );
    cJSONExt_GetBool( jNode, "m_Inverse", m_Inverse );
}

std::string OpenCVNode_Generate_SimplexNoise::GetSettingsString()
{
    std::string settingsString;
    settingsString += "-fs" + std::to_string( m_UseFixedSeed );
    settingsString += "-s" + std::to_string( m_Seed );
    settingsString += "-fr" + std::to_string( m_Frequency );
    settingsString += "-i" + std::to_string( m_Inverse );

    return settingsString;
}
