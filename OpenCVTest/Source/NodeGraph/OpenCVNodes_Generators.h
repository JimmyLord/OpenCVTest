//
// Copyright (c) 2021 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodes_Generators_H__
#define __OpenCVNodes_Generators_H__

#include "OpenCVNodes_Base.h"

// OpenCV node types.
class OpenCVNode_Generate_PoissonSampling;

//====================================================================================================
// OpenCVNode_Generate_PoissonSampling
//====================================================================================================

// Implementation of Robert Bridson's "Fast Poisson Disk Sampling in Arbitrary Dimensions".
// https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf
// Implemented solely in 2D, which probably defeats the purpose.
void GenerateSampling(cv::Mat& image, ivec2 imageSize, float r, int k)
{
    // Temp variables.
    std::vector<vec2> activeList;
    float cellSize = r / sqrtf(2);
    ivec2 gridSize( (int)ceil(imageSize.x / cellSize), (int)ceil(imageSize.y / cellSize) );
    std::vector<vec2> pointGrid( gridSize.x*gridSize.y );

    // Initialize image and grid.
    image = cv::Mat::zeros( cv::Size(imageSize.x,imageSize.y), CV_8UC1 );
    for( int y=0; y<gridSize.y; y++ )
    {
        for( int x=0; x<gridSize.x; x++ )
        {
            pointGrid[y*gridSize.x + x].Set( -1, -1 );
        }
    }

    // Pick a random point.
    vec2 pos( randFloat(0,(float)imageSize.x), randFloat(0,(float)imageSize.y) );
    int gx = (int)(pos.x / cellSize);
    int gy = (int)(pos.y / cellSize);

    // Push the sample into the active list and color the pixel.
    activeList.push_back( pos );
    pointGrid[gy*gridSize.x + gx] = pos;
    image.at<uchar>( cv::Point((int)pos.x,(int)pos.y) ) = 255;

    // Loop through active list.
    while( activeList.size() > 0 )
    {
        // Remove this sample from the active list.
        vec2 currentPos = activeList[0];
        activeList[0] = activeList[activeList.size()-1];
        activeList.pop_back();

        // Check a maximum number of samples around our current position.
        for( int i=0; i<k; i++ )
        {
            float angle = randFloat( 0, 2*PI );
            vec2 dir( cos(angle), sin(angle) );
            float dist = randFloat( r, 2*r );

            pos = currentPos + dir * dist;
            int ngx = (int)(pos.x / cellSize);
            int ngy = (int)(pos.y / cellSize);

            // Out of bounds check.
            if( pos.x < 0 || pos.x >= imageSize.x || pos.y < 0 || pos.y >= imageSize.y ||
                ngx < 0 || ngx >= gridSize.x || ngy < 0 || ngy >= gridSize.y )
            {
                continue;
            }

            // Check all neighbouring grid cells.
            bool tooClose = false;
            for( int y=-1; y<=1; y++ )
            {
                for( int x=-1; x<=1; x++ )
                {
                    ivec2 tile( ngx+x, ngy+y );
                    int tileIndex = tile.y*gridSize.x + tile.x;
                    if( tile.x < 0 || tile.x >= gridSize.x ) continue;
                    if( tile.y < 0 || tile.y >= gridSize.y ) continue;

                    if( pointGrid[tileIndex].x != -1 )
                    {
                        float distance = pointGrid[tileIndex].DistanceFrom( pos );

                        if( distance < r )
                        {
                            tooClose = true;
                        }
                    }
                }
            }

            // If we found a spot far enough from all others,
            // push the sample into the active list and color the pixel.
            if( tooClose == false )
            {
                activeList.push_back( pos );
                pointGrid[ngy*gridSize.x + ngx] = pos;
                image.at<uchar>( cv::Point((int)pos.x,(int)pos.y) ) = 255;
            }
        }
    }
};

class OpenCVNode_Generate_PoissonSampling : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;

    // Saved parameters.
    ivec2 m_ImageSize;
    float m_r_MinimumDistanceBetweenSamples;
    int m_k_SampleLimitBeforeRejection;

public:
    OpenCVNode_Generate_PoissonSampling(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_pTexture = nullptr;
        
        m_ImageSize.Set( 128, 128 );
        m_r_MinimumDistanceBetweenSamples = 5;
        m_k_SampleLimitBeforeRejection = 30;
    }

    ~OpenCVNode_Generate_PoissonSampling()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Generate_PoissonSampling"; }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            OpenCVBaseNode::DrawTitle();
        else
            ImGui::Text( "%s", m_Name );
    }

    virtual bool DrawContents() override
    {
        bool modified = OpenCVBaseNode::DrawContents();

        if( ImGui::DragInt2( "Size", &m_ImageSize.x, 1.0f, 1, 4096 ) ) { QuickRun( false ); }
        if( ImGui::DragFloat( "Min Distance", &m_r_MinimumDistanceBetweenSamples, 1.0f, 1.0f, 255.0f ) ) { QuickRun( false ); }
        if( ImGui::DragInt( "Max Samples", &m_k_SampleLimitBeforeRejection, 1.0f, 1, 100 ) ) { QuickRun( false ); }

        if( ImGui::Button( "Generate" ) )
        {
            Trigger( nullptr, true );
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return modified;
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Generate the image.
        GenerateSampling( m_Image, m_ImageSize, m_r_MinimumDistanceBetweenSamples, m_k_SampleLimitBeforeRejection );

        // Display it.
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, recursive );

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSONExt_AddIntArrayToObject( jNode, "m_ImageSize", &m_ImageSize.x, 2 );
        cJSON_AddNumberToObject( jNode, "m_r_MinimumDistanceBetweenSamples", m_r_MinimumDistanceBetweenSamples );
        cJSON_AddNumberToObject( jNode, "m_k_SampleLimitBeforeRejection", m_k_SampleLimitBeforeRejection );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
        cJSONExt_GetIntArray( jNode, "m_ImageSize", &m_ImageSize.x, 2 );
        cJSONExt_GetFloat( jNode, "m_r_MinimumDistanceBetweenSamples", &m_r_MinimumDistanceBetweenSamples );
        cJSONExt_GetInt( jNode, "m_k_SampleLimitBeforeRejection", &m_k_SampleLimitBeforeRejection );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

#endif //__OpenCVNodes_Generators_H__
