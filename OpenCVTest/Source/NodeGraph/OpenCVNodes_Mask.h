//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodesMask_H__
#define __OpenCVNodesMask_H__

#include "Utility/Helpers.h"
#include "Utility/VectorTypes.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"
#include "OpenCVNodes.h"

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
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 3, 1 )
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

    virtual void DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth(), m_pNodeGraph->GetHoverPixelsToShow() );
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage1 = GetInputImage( 0 );
        cv::Mat* pImage2 = GetInputImage( 1 );
        cv::Mat* pImageMask = GetInputImage( 2 );

        if( pImage1 && pImage2 && pImageMask )
        {
            cv::Mat maskGray;
            cv::Mat maskInverse;

            cv::cvtColor( *pImageMask, maskGray, cv::COLOR_BGR2GRAY );
            maskInverse = 255 - maskGray;

            cv::Mat image1Color = *pImage1;
            cv::Mat image2Color = *pImage2;

            if( image1Color.type() == CV_8UC1 )
                cv::cvtColor( *pImage1, image1Color, cv::COLOR_GRAY2BGR );
            if( image2Color.type() == CV_8UC1 )
                cv::cvtColor( *pImage2, image2Color, cv::COLOR_GRAY2BGR );

            image1Color.copyTo( m_Image, maskGray );
            image2Color.copyTo( m_Image, maskInverse );

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
