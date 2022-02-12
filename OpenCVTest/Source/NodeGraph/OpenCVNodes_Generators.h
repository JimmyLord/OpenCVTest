//
// Copyright (c) 2021 Jimmy Lord
//
#ifndef __OpenCVNodes_Generators_H__
#define __OpenCVNodes_Generators_H__

#include "OpenCVNodes_Base.h"
#include "Utility/Helpers.h"
#include "Graph/GraphTypes.h"

// OpenCV node types.
class OpenCVNode_Generate_PoissonSampling;

// Implementation of Robert Bridson's "Fast Poisson Disk Sampling in Arbitrary Dimensions".
// https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf
// Implemented solely in 2D, which probably defeats the purpose.
void GenerateSampling(std::vector<vec2>& pointList, float minDistance, int maxSamplesPerPoint, bool startWithExistingPoints);
void DrawSampling(cv::Mat& image, ivec2 imageSize, const std::vector<vec2>& pointList, const colorPalette* palette, const std::vector<size_t> pointListLayerStarts);
// Modification of the above that takes in a grayscale image that controls point density.
void GenerateSamplingWithVaryingPointDensity(std::vector<vec2>& pointList, int maxSamplesPerPoint, cv::Mat& pointDensityImage, float minDistance, float maxDistance);
void GenerateGrid(std::vector<vec2>& pointList, fullNeighbourList& neighbours, ivec2 gridSize, float padding, bool connectDiagonals);

//====================================================================================================
// Node_PointDistribution
//====================================================================================================

class Node_PointDistribution : public OpenCVBaseNode
{
public:
    Node_PointDistribution(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, inputsCount, outputsCount )
    {
    }

    DEFINE_NODE_BASE_TYPE( "Node_PointDistribution" );

    virtual std::vector<vec2>* GetValuePointList() = 0;
    virtual std::vector<size_t>* GetValuePointListLayerStarts() = 0;
    virtual fullNeighbourList* GetValueNeighbourList() { return nullptr; }
    virtual float GetValueR_MinDistance() = 0;
    virtual float GetValueSizeReductionRate() = 0;
};

//====================================================================================================
// OpenCVNode_Generate_PoissonSampling
//====================================================================================================

class OpenCVNode_Generate_PoissonSampling : public Node_PointDistribution
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    std::vector<vec2> m_PointList;
    std::vector<size_t> m_PointListLayerStarts;
    bool m_DisplayColors;

    // Saved parameters.
    ivec2 m_ImageSize;
    bool m_UseFixedSeed;
    int m_Seed;
    float m_r_MinDistanceBetweenSamples;
    float m_r_MaxDistanceBetweenSamples;
    int m_k_SampleLimitBeforeRejection;

    int m_NumLayers;
    float m_SizeReductionRate;

public:
    OpenCVNode_Generate_PoissonSampling(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : Node_PointDistribution( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        
        m_ImageSize.Set( 128, 128 );
        m_UseFixedSeed = false;
        m_Seed = 0;
        m_r_MinDistanceBetweenSamples = 10;
        m_r_MaxDistanceBetweenSamples = 10;
        m_k_SampleLimitBeforeRejection = 30;

        m_NumLayers = 1;
        m_SizeReductionRate = 2.0f;

        m_DisplayColors = false;
    }

    ~OpenCVNode_Generate_PoissonSampling()
    {
        SAFE_RELEASE( m_pTexture );
    }

    DEFINE_NODE_TYPE( "Generate_PoissonSampling" );

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            Node_PointDistribution::DrawTitle();
        else
            ImGui::Text( "%s", m_Name );
    }

    virtual bool DrawContents() override
    {
        bool modified = Node_PointDistribution::DrawContents();

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
            ImGui::DragInt( "Seed", &m_Seed, 1.0f );
            if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
        }

        if( pDensityMask )
        {
            ImGui::DragFloat( "Min Distance", &m_r_MinDistanceBetweenSamples, 1.00f, 1.00f, m_r_MaxDistanceBetweenSamples );
            if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
            ImGui::DragFloat( "Max Distance", &m_r_MaxDistanceBetweenSamples, 1.00f, m_r_MinDistanceBetweenSamples, 100.0f );
            if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
        }
        else
        {
            ImGui::DragFloat( "Distance", &m_r_MinDistanceBetweenSamples, 1.0f, 1.0f, 100.0f );
            if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
            m_r_MaxDistanceBetweenSamples = m_r_MinDistanceBetweenSamples;
        }
        ImGui::DragInt( "Max Samples", &m_k_SampleLimitBeforeRejection, 1.0f, 1, 100 );
        
        ImGui::DragInt( "# Layers", &m_NumLayers, 1, 1, 10 );
        if( m_NumLayers < 1 )
            m_NumLayers = 1;
        if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
        ImGui::DragFloat( "Reduction", &m_SizeReductionRate, 0.1f, 1.1f, 10.0f );
        if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }

        if( ImGui::Checkbox( "Colors", &m_DisplayColors ) ) { QuickRun( false ); }

        if( ImGui::Button( "Generate" ) )
        {
            Trigger( nullptr, TriggerFlags::TF_Recursive );
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );
        //m_pNodeGraph->GetImageWidth()

        return modified;
    }

    virtual void TriggerGlobalRun() override
    {
        Trigger( nullptr, TriggerFlags::TF_Recursive );
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //Node_PointDistribution::Trigger( pEvent );

        cv::Mat* pDensityMask = GetInputImage( 0 );

        colorPalette* pPaletteToUse = nullptr;
        if( m_DisplayColors )
            pPaletteToUse = &m_pNodeGraph->GetPalette();

        // Generate the image.
        if( m_UseFixedSeed )
        {
            srand( m_Seed );
        }

        if( pDensityMask == nullptr )
        {
            m_PointListLayerStarts.clear();

            int layersLeft = m_NumLayers;
            float distance = m_r_MinDistanceBetweenSamples;
            bool firstRun = true;
            m_PointListLayerStarts.push_back( 0 );

            while( layersLeft )
            {
                layersLeft--;

                GenerateSampling( m_PointList, distance, m_k_SampleLimitBeforeRejection, !firstRun );
                m_PointListLayerStarts.push_back( m_PointList.size() );
                distance /= m_SizeReductionRate;

                firstRun = false;
            }
        }
        else
        {
            GenerateSamplingWithVaryingPointDensity( m_PointList, m_k_SampleLimitBeforeRejection, *pDensityMask, m_r_MinDistanceBetweenSamples, m_r_MaxDistanceBetweenSamples );
        }

        m_Image = cv::Mat::zeros( cv::Size(m_ImageSize.x,m_ImageSize.y), CV_8UC3 );
        DrawSampling( m_Image, m_ImageSize, m_PointList, pPaletteToUse, m_PointListLayerStarts );

        // Display it.
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = Node_PointDistribution::ExportAsJSONObject();
        cJSONExt_AddIntArrayToObject( jNode, "m_ImageSize", &m_ImageSize.x, 2 );

        cJSON_AddNumberToObject( jNode, "m_UseFixedSeed", m_UseFixedSeed );
        cJSON_AddNumberToObject( jNode, "m_Seed", m_Seed );

        cJSON_AddNumberToObject( jNode, "m_r_MinDistanceBetweenSamples", m_r_MinDistanceBetweenSamples );
        cJSON_AddNumberToObject( jNode, "m_r_MaxDistanceBetweenSamples", m_r_MaxDistanceBetweenSamples );
        cJSON_AddNumberToObject( jNode, "m_k_SampleLimitBeforeRejection", m_k_SampleLimitBeforeRejection );
        cJSON_AddNumberToObject( jNode, "m_NumLayers", m_NumLayers );
        cJSON_AddNumberToObject( jNode, "m_SizeReductionRate", m_SizeReductionRate );
        cJSON_AddNumberToObject( jNode, "m_DisplayColors", m_DisplayColors );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        Node_PointDistribution::ImportFromJSONObject( jNode );
        cJSONExt_GetIntArray( jNode, "m_ImageSize", &m_ImageSize.x, 2 );

        cJSONExt_GetBool( jNode, "m_UseFixedSeed", &m_UseFixedSeed );
        cJSONExt_GetInt( jNode, "m_Seed", &m_Seed );
        
        cJSONExt_GetFloat( jNode, "m_r_MinDistanceBetweenSamples", &m_r_MinDistanceBetweenSamples );
        cJSONExt_GetFloat( jNode, "m_r_MaxDistanceBetweenSamples", &m_r_MaxDistanceBetweenSamples );
        cJSONExt_GetInt( jNode, "m_k_SampleLimitBeforeRejection", &m_k_SampleLimitBeforeRejection );
        cJSONExt_GetInt( jNode, "m_NumLayers", &m_NumLayers );
        cJSONExt_GetFloat( jNode, "m_SizeReductionRate", &m_SizeReductionRate );
        cJSONExt_GetBool( jNode, "m_DisplayColors", &m_DisplayColors );
    }
    
    virtual std::string GetSettingsString() override
    {
        std::string settingsString;
        settingsString += "-fs" + std::to_string( m_UseFixedSeed );
        settingsString += "-s" + std::to_string( m_Seed );
        settingsString += "-min"; PrintFloatBadlyWithPrecision( settingsString, m_r_MinDistanceBetweenSamples, 2 );
        settingsString += "-max"; PrintFloatBadlyWithPrecision( settingsString, m_r_MaxDistanceBetweenSamples, 2 );
        settingsString += "-sl" + std::to_string( m_k_SampleLimitBeforeRejection );
        settingsString += "-nl" + std::to_string( m_NumLayers );
        settingsString += "-rr"; PrintFloatBadlyWithPrecision( settingsString, m_SizeReductionRate, 2 );

        return settingsString;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
    virtual std::vector<vec2>* GetValuePointList() override { return &m_PointList; }
    virtual std::vector<size_t>* GetValuePointListLayerStarts() { return &m_PointListLayerStarts; }
    virtual float GetValueR_MinDistance() { return m_r_MinDistanceBetweenSamples; }
    virtual float GetValueSizeReductionRate() { return m_SizeReductionRate; }
};

//====================================================================================================
// OpenCVNode_Generate_RegularGrid
//====================================================================================================

class OpenCVNode_Generate_RegularGrid : public Node_PointDistribution
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    std::vector<vec2> m_PointList;
    std::vector<size_t> m_PointListLayerStarts;
    fullNeighbourList m_NeighbourList;
    bool m_DisplayColors;

    // Saved parameters.
    ivec2 m_ImageSize;
    ivec2 m_GridSize;
    bool m_ConnectDiagonals;

    int m_NumLayers;
    float m_SizeReductionRate;

public:
    OpenCVNode_Generate_RegularGrid(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : Node_PointDistribution( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;

        m_ImageSize.Set( 128, 128 );
        m_GridSize.Set( 4, 4 );
        m_ConnectDiagonals = false;

        m_NumLayers = 1;
        m_SizeReductionRate = 2.0f;

        m_DisplayColors = false;
    }

    ~OpenCVNode_Generate_RegularGrid()
    {
        SAFE_RELEASE( m_pTexture );
    }

    DEFINE_NODE_TYPE( "Generate_RegularGrid" );

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            Node_PointDistribution::DrawTitle();
        else
            ImGui::Text( "%s", m_Name );
    }

    virtual bool DrawContents() override
    {
        bool modified = Node_PointDistribution::DrawContents();

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

        ImGui::DragInt2( "Grid Size", &m_GridSize.x, 1.00f, 1, 100 );
        if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }

        if( ImGui::Checkbox( "Connect Diagonals", &m_ConnectDiagonals ) ) { QuickRun( false ); }

        ImGui::DragInt( "# Layers", &m_NumLayers, 1, 1, 10 );
        if( m_NumLayers < 1 )
            m_NumLayers = 1;
        if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }
        ImGui::DragFloat( "Reduction", &m_SizeReductionRate, 0.1f, 1.1f, 10.0f );
        if( ImGui::IsItemDeactivatedAfterEdit() ) { QuickRun( false ); }

        if( ImGui::Checkbox( "Colors", &m_DisplayColors ) ) { QuickRun( false ); }

        if( ImGui::Button( "Generate" ) )
        {
            Trigger( nullptr, TriggerFlags::TF_Recursive );
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );
        //m_pNodeGraph->GetImageWidth()

        return modified;
    }

    virtual void TriggerGlobalRun() override
    {
        Trigger( nullptr, TriggerFlags::TF_Recursive );
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //Node_PointDistribution::Trigger( pEvent );

        cv::Mat* pDensityMask = GetInputImage( 0 );

        colorPalette* pPaletteToUse = nullptr;
        if( m_DisplayColors )
            pPaletteToUse = &m_pNodeGraph->GetPalette();

        // Generate the image.
        {
            m_PointListLayerStarts.clear();
            m_PointListLayerStarts.push_back( 0 );

            GenerateGrid( m_PointList, m_NeighbourList, m_GridSize, 20, m_ConnectDiagonals );
            m_PointListLayerStarts.push_back( 4 );
            m_PointListLayerStarts.push_back( 4 );
            m_PointListLayerStarts.push_back( m_PointList.size() );
        }

        m_Image = cv::Mat::zeros( cv::Size(m_ImageSize.x,m_ImageSize.y), CV_8UC3 );
        DrawSampling( m_Image, m_ImageSize, m_PointList, pPaletteToUse, m_PointListLayerStarts );

        // Display it.
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = Node_PointDistribution::ExportAsJSONObject();
        cJSONExt_AddIntArrayToObject( jNode, "m_ImageSize", &m_ImageSize.x, 2 );

        cJSONExt_AddIntArrayToObject( jNode, "m_GridSize", &m_GridSize.x, 2 );
        cJSON_AddNumberToObject( jNode, "m_ConnectDiagonals", m_ConnectDiagonals );

        cJSON_AddNumberToObject( jNode, "m_NumLayers", m_NumLayers );
        cJSON_AddNumberToObject( jNode, "m_SizeReductionRate", m_SizeReductionRate );
        cJSON_AddNumberToObject( jNode, "m_DisplayColors", m_DisplayColors );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        Node_PointDistribution::ImportFromJSONObject( jNode );
        cJSONExt_GetIntArray( jNode, "m_ImageSize", &m_ImageSize.x, 2 );

        cJSONExt_GetIntArray( jNode, "m_GridSize", &m_GridSize.x, 2 );
        cJSONExt_GetBool( jNode, "m_ConnectDiagonals", &m_ConnectDiagonals );

        cJSONExt_GetInt( jNode, "m_NumLayers", &m_NumLayers );
        cJSONExt_GetFloat( jNode, "m_SizeReductionRate", &m_SizeReductionRate );
        cJSONExt_GetBool( jNode, "m_DisplayColors", &m_DisplayColors );
    }

    virtual std::string GetSettingsString() override
    {
        std::string settingsString;
        settingsString += "-gs" + std::to_string( m_GridSize.x ) + "x" + std::to_string( m_GridSize.y );
        settingsString += "-d" + std::to_string( m_ConnectDiagonals );
        settingsString += "-nl" + std::to_string( m_NumLayers );
        settingsString += "-rr"; PrintFloatBadlyWithPrecision( settingsString, m_SizeReductionRate, 2 );

        return settingsString;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
    virtual std::vector<vec2>* GetValuePointList() override { return &m_PointList; }
    virtual std::vector<size_t>* GetValuePointListLayerStarts() { return &m_PointListLayerStarts; }
    virtual fullNeighbourList* GetValueNeighbourList() { return &m_NeighbourList; }
    virtual float GetValueSizeReductionRate() { return m_SizeReductionRate; }
    virtual float GetValueR_MinDistance() { return 100; }
};

#endif //__OpenCVNodes_Generators_H__
