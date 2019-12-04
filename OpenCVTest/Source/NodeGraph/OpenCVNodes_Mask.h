//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodesMask_H__
#define __OpenCVNodesMask_H__

#include "Utility/Helpers.h"
#include "Utility/VectorTypes.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"
#include "OpenCVNodes_Base.h"

class ComponentBase;

// OpenCV node types.
class OpenCVNode_Filter_Mask;

//====================================================================================================
// OpenCVNode_Filter_Mask
//====================================================================================================

class OpenCVNode_Filter_Mask : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;

public:
    OpenCVNode_Filter_Mask(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 4, 1 )
    {
        m_pTexture = nullptr;
        //VSNAddVar( &m_VariablesList, "Float", ComponentVariableType_Float, MyOffsetOf( this, &this->m_Float ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_Filter_Mask()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Filter_Mask"; }
    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

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

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return modified;
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage1 = GetInputImage( 0 ); // 255
        cv::Mat* pImage2 = GetInputImage( 1 ); // 10 - 245
        cv::Mat* pImage3 = GetInputImage( 2 ); // 0
        cv::Mat* pImageMask = GetInputImage( 3 );

        if( pImageMask && ( pImage1 || pImage2 || pImage3 ) )
        {
            cv::Mat maskOriginal;
            cv::Mat maskHigh;
            cv::Mat maskMid;
            cv::Mat maskLow;

            cv::cvtColor( *pImageMask, maskOriginal, cv::COLOR_BGR2GRAY );
            maskHigh = maskOriginal - 150; // 150-255's become only non-zeroes.
            maskMid = maskOriginal;
            maskLow = 100 - maskOriginal;  // 0-100's become only non-zeroes.

            if( pImage2 )
            {
                cv::Mat image2Color = *pImage2;
                if( image2Color.type() == CV_8UC1 )
                    cv::cvtColor( *pImage2, image2Color, cv::COLOR_GRAY2BGR );
                image2Color.copyTo( m_Image, maskMid ); // Copies Highs and Mids.
            }
            if( pImage1 )
            {
                cv::Mat image1Color = *pImage1;
                if( image1Color.type() == CV_8UC1 )
                    cv::cvtColor( *pImage1, image1Color, cv::COLOR_GRAY2BGR );
                image1Color.copyTo( m_Image, maskHigh ); // Overwrite Highs from Image2.
            }
            if( pImage3 )
            {
                cv::Mat image3Color = *pImage3;
                if( image3Color.type() == CV_8UC1 )
                    cv::cvtColor( *pImage3, image3Color, cv::COLOR_GRAY2BGR );
                image3Color.copyTo( m_Image, maskLow ); // Copy Lows.
            }

            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, recursive );
        }

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

#endif //__OpenCVNodesMask_H__
