//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodes_H__
#define __OpenCVNodes_H__

#include "OpenCVNode.h"
#include "Utility/Helpers.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"

class ComponentBase;

// OpenCV node types.
class OpenCVNode_Input_File;
class OpenCVNode_Convert_Grayscale;
class OpenCVNode_Filter_Threshold;
class OpenCVNode_Filter_Bilateral;

//====================================================================================================
// OpenCVNode
//====================================================================================================

class OpenCVBaseNode : public OpenCVNodeGraph::OpenCVNode
{
public:
    OpenCVBaseNode(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
        : OpenCVNodeGraph::OpenCVNode( pNodeGraph, id, name, pos, inputsCount, outputsCount )
    {
    }

    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) { return 0; }

    virtual cv::Mat* GetValueMat() { return nullptr; }

    void QuickRun()
    {
        if( m_pNodeGraph->GetAutoRun() ) 
        {
            Trigger( nullptr );
        }
    }
};

#define VSNAddVar ComponentBase::AddVariable_Base
#define VSNAddVarEnum ComponentBase::AddVariableEnum_Base

//====================================================================================================
// OpenCVNode_Input_File
//====================================================================================================

class OpenCVNode_Input_File : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    std::string m_Filename;

public:
    OpenCVNode_Input_File(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_Filename = "Data/test.png";
        m_pTexture = nullptr;
        //VSNAddVar( &m_VariablesList, "Float", ComponentVariableType_Float, MyOffsetOf( this, &this->m_Float ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_Input_File()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Input_File"; }
    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            OpenCVNode::DrawTitle();
        else
            ImGui::Text( "%s: %s", m_Name, m_Filename.c_str() );
    }

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        ImGui::Text( "Filename: %s", m_Filename.c_str() );

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageSize() );

        if( ImGui::Button( "Choose File..." ) )
        {
            const char* filename = FileOpenDialog( "Data\\", "Images\0*.png;*.jpg\0All\0*.*\0" );
            if( filename[0] != '\0' )
            {
                char path[MAX_PATH];
                strcpy_s( path, MAX_PATH, filename );
                const char* relativePath = GetRelativePath( path );

                m_Filename = relativePath;
                QuickRun();
            }
        }
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        //OpenCVNode::Trigger( pEvent );

        // Load the file from disk.
        m_Image = cv::imread( m_Filename.c_str() );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
        {
            pNode->Trigger();
        }

        return false;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Convert_Grayscale
//====================================================================================================

class OpenCVNode_Convert_Grayscale : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;

public:
    OpenCVNode_Convert_Grayscale(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        //VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_Convert_Grayscale()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Convert_Grayscale"; }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageSize() );
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        //OpenCVNode::Trigger( pEvent );

        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        // Convert to grayscale.
        cv::cvtColor( *pImage, m_Image, cv::COLOR_BGR2GRAY );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the ouput nodes.
        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
        {
            pNode->Trigger();
        }

        return false;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Filter_Threshold
//====================================================================================================

class OpenCVNode_Filter_Threshold : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    float m_ThresholdValue;
    int m_ThresholdType;

    const char* ThresholdTypeNames[5] = 
    {
        "Binary",
        "Binary Inverse",
        "Truncate",
        "To Zero",
        "To Zero Inverse",
    };

    const int ThresholdTypeMax = 5;

public:
    OpenCVNode_Filter_Threshold(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        m_ThresholdValue = 0;
        m_ThresholdType = 0;
        //VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_Filter_Threshold()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Filter_Threshold"; }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        if( ImGui::DragFloat( "Value", &m_ThresholdValue, 1.0f, 0.0f, 255.0f ) )             { QuickRun(); }
        //if( ImGui::ListBox( "Type", &m_ThresholdType, ThresholdTypeNames, ThresholdTypeMax ) ) { QuickRun(); }

        if( ImGui::BeginCombo( "Type", ThresholdTypeNames[m_ThresholdType] ) )
        {
            for( int n = 0; n < ThresholdTypeMax; n++ )
            {
                bool is_selected = (n == m_ThresholdType);
                if( ImGui::Selectable( ThresholdTypeNames[n], is_selected ) )
                {
                    m_ThresholdType = n;
                    QuickRun();
                }
                if( is_selected )
                {
                    // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageSize() );
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        //OpenCVNode::Trigger( pEvent );

        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        // Apply the threshold filter.
        cv::threshold( *pImage, m_Image, m_ThresholdValue, 255, m_ThresholdType );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the ouput nodes.
        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
        {
            pNode->Trigger();
        }

        return false;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Filter_Bilateral
//====================================================================================================

class OpenCVNode_Filter_Bilateral : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    int m_WindowSize;
    float m_SigmaColor;
    float m_SigmaSpace;

public:
    OpenCVNode_Filter_Bilateral(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        m_WindowSize = 3;
        m_SigmaColor = 150.0f;
        m_SigmaSpace = 150.0f;
        //VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_Filter_Bilateral()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Filter_Bilateral"; }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual void DrawContents() override
    {
        OpenCVNode::DrawContents();

        if( ImGui::DragInt( "Window Size", &m_WindowSize, 1.0f, 1, 30 ) )          { QuickRun(); }
        if( ImGui::DragFloat( "Sigma Color", &m_SigmaColor, 1.0f, 0.0f, 255.0f ) ) { QuickRun(); }
        if( ImGui::DragFloat( "Sigma Space", &m_SigmaSpace, 1.0f, 0.0f, 255.0f ) ) { QuickRun(); }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageSize() );
    }

    virtual bool Trigger(MyEvent* pEvent) override
    {
        //OpenCVNode::Trigger( pEvent );

        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        // Apply the Bilateral filter.
        cv::bilateralFilter( *pImage, m_Image, m_WindowSize, m_SigmaColor, m_SigmaSpace );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the ouput nodes.
        int count = 0;
        while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
        {
            pNode->Trigger();
        }

        return false;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

#endif //__OpenCVNodes_H__
