//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodes_H__
#define __OpenCVNodes_H__

#include "OpenCVNode.h"
#include "Utility/Helpers.h"
#include "Utility/VectorTypes.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"

class ComponentBase;

// OpenCV node types.
class OpenCVNode_File_Input;
class OpenCVNode_File_Output;
class OpenCVNode_Convert_Grayscale;
class OpenCVNode_Convert_Crop;
class OpenCVNode_Filter_Threshold;
class OpenCVNode_Filter_Bilateral;

//====================================================================================================
// OpenCVNode
//====================================================================================================

class OpenCVBaseNode : public OpenCVNodeGraph::OpenCVNode
{
protected:
    double m_LastProcessTime;

public:
    OpenCVBaseNode(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
        : OpenCVNodeGraph::OpenCVNode( pNodeGraph, id, name, pos, inputsCount, outputsCount )
    {
        m_LastProcessTime = 0.0;
    }

    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) { return 0; }

    virtual cv::Mat* GetValueMat() { return nullptr; }
    virtual const char* GetSettingsString() { return nullptr; }

    void QuickRun(bool triggerJustThisNodeIfAutoRunIsOff)
    {
        if( m_pNodeGraph->GetAutoRun() ) 
        {
            // Trigger all nodes recursively.
            Trigger( nullptr, true );
        }
        else if( triggerJustThisNodeIfAutoRunIsOff )
        {
            // Trigger just this node.
            Trigger( nullptr, false );
        }
    }

    void TriggerOutputNodes(MyEvent* pEvent, bool recursive)
    {
        if( recursive )
        {
            int count = 0;
            while( OpenCVNode* pNode = (OpenCVNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
            {
                pNode->Trigger( nullptr, true );
            }
        }
    }
};

#define VSNAddVar ComponentBase::AddVariable_Base
#define VSNAddVarEnum ComponentBase::AddVariableEnum_Base

//====================================================================================================
// OpenCVNode_File_Input
//====================================================================================================

class OpenCVNode_File_Input : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    std::string m_Filename;

public:
    OpenCVNode_File_Input(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 0, 1 )
    {
        m_Filename = "Data/test.png";
        m_pTexture = nullptr;
        //VSNAddVar( &m_VariablesList, "Float", ComponentVariableType_Float, MyOffsetOf( this, &this->m_Float ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_File_Input()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "File_Input"; }
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

        if( ImGui::Button( "Choose File..." ) )
        {
            const char* filename = FileOpenDialog( "Data\\", "Images\0*.png;*.jpg\0All\0*.*\0" );
            if( filename[0] != '\0' )
            {
                char path[MAX_PATH];
                strcpy_s( path, MAX_PATH, filename );
                const char* relativePath = GetRelativePath( path );

                m_Filename = relativePath;
                QuickRun( true );
            }
        }

        ImGui::Text( "File: %s", m_Filename.c_str() );
        ImGui::Text( "Size: %dx%d", m_Image.cols, m_Image.rows );
        if( ImGui::Button( "Zoom to native" ) )
        {
            m_pNodeGraph->SetImageWidth( (float)m_Image.cols );
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth() );
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVNode::Trigger( pEvent );

        // Load the file from disk.
        m_Image = cv::imread( m_Filename.c_str() );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, recursive );

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSON_AddStringToObject( jNode, "m_Filename", m_Filename.c_str() );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
        cJSON* jObj = cJSON_GetObjectItem( jNode, "m_Filename" );
        if( jObj )
            m_Filename.assign( jObj->valuestring );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_File_Output
//====================================================================================================

class OpenCVNode_File_Output : public OpenCVBaseNode
{
protected:
    std::string m_Filename;

public:
    OpenCVNode_File_Output(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 0 )
    {
        m_Filename = "Output/test.png";
        //VSNAddVar( &m_VariablesList, "Float", ComponentVariableType_Float, MyOffsetOf( this, &this->m_Float ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_File_Output()
    {
    }

    const char* GetType() { return "File_Output"; }
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

        if( ImGui::Button( "Choose File..." ) )
        {
            const char* filename = FileSaveDialog( "Data\\", "png files\0*.png" );
            if( filename[0] != '\0' )
            {
                char path[MAX_PATH];
                strcpy_s( path, MAX_PATH, filename );
                const char* relativePath = GetRelativePath( path );

                m_Filename = relativePath;

                Trigger( nullptr, false );
            }
        }

        ImGui::Text( "File: %s", m_Filename.c_str() );

        if( ImGui::Button( "Save" ) )
        {
            Save();
        }
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        return true;
    }

    void Save()
    {
        //OpenCVNode::Trigger( pEvent );

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        std::string tempFilename = m_Filename;

        // Append the setting string returned by the previous node.
        const char* settingsString = pNode->GetSettingsString();
        if( settingsString )
        {
            tempFilename += settingsString;
        }

        // Append a .png.
        const char* relativePath = tempFilename.c_str();
        int len = (int)strlen( relativePath );
        if( len < 4 || strcmp( &relativePath[len-4], ".png" ) != 0 )
        {
            tempFilename += ".png";
        }

        // Write the file to disk.
        cv::imwrite( tempFilename.c_str(), *pImage );
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSON_AddStringToObject( jNode, "m_Filename", m_Filename.c_str() );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
        cJSON* jObj = cJSON_GetObjectItem( jNode, "m_Filename" );
        if( jObj )
            m_Filename.assign( jObj->valuestring );
    }

    //virtual cv::Mat* GetValueMat() override { return &m_Image; }
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

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth() );
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVNode::Trigger( pEvent );

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        // Convert to Grayscale.
        cv::cvtColor( *pImage, m_Image, cv::COLOR_BGR2GRAY );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, recursive );

        return false;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Convert_Crop
//====================================================================================================

class OpenCVNode_Convert_Crop : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    ivec2 m_TopLeft;
    ivec2 m_Size;

public:
    OpenCVNode_Convert_Crop(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        m_TopLeft.Set( 0, 0 );
        m_Size.Set( 32, 32 );
        //VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );
    }

    ~OpenCVNode_Convert_Crop()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Convert_Crop"; }

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

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        if( pNode )
        {
            cv::Mat* pImage = pNode->GetValueMat();

            if( pImage->cols > 0 )
            {
                bool valuesChanged = false;

                if( ImGui::DragInt( "Top",    &m_TopLeft.y, 1.0f, 0, pImage->rows - m_Size.y ) ) { valuesChanged = true; }
                if( ImGui::DragInt( "Left",   &m_TopLeft.x, 1.0f, 0, pImage->cols - m_Size.x ) ) { valuesChanged = true; }
                if( ImGui::DragInt( "Width",  &m_Size.x,    1.0f, 1, pImage->cols ) ) { valuesChanged = true; }
                if( ImGui::DragInt( "Height", &m_Size.y,    1.0f, 1, pImage->rows ) ) { valuesChanged = true; }

                if( valuesChanged )
                {
                    Validate( pImage );
                    QuickRun( true );
                }

                DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth() );
            }
            else
            {
                ImGui::Text( "No input image." );
            }
        }
        else
        {
            ImGui::Text( "No input image." );
        }
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVNode::Trigger( pEvent );

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        Validate( pImage );

        // Crop out a subregion of the image.
        cv::cvtColor( *pImage, m_Image, cv::COLOR_BGR2GRAY );
        cv::Rect cropArea( m_TopLeft.x, m_TopLeft.y, m_Size.x, m_Size.y );
        m_Image = (*pImage)( cropArea );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, recursive );

        return false;
    }

    void Validate(cv::Mat* pImage)
    {
        if( m_Size.x > pImage->cols )
            m_Size.x = pImage->cols;
        if( m_Size.y > pImage->rows )
            m_Size.y = pImage->rows;

        if( m_TopLeft.x + m_Size.x > pImage->cols )
            m_TopLeft.x = pImage->cols - m_Size.x;
        if( m_TopLeft.y + m_Size.y > pImage->rows )
            m_TopLeft.y = pImage->rows - m_Size.y;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSONExt_AddIntArrayToObject( jNode, "m_TopLeft", &m_TopLeft.x, 2 );
        cJSONExt_AddIntArrayToObject( jNode, "m_Size", &m_Size.x, 2 );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
        cJSONExt_GetIntArray( jNode, "m_TopLeft", &m_TopLeft.x, 2 );
        cJSONExt_GetIntArray( jNode, "m_Size", &m_Size.x, 2 );
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

        if( ImGui::DragFloat( "Value", &m_ThresholdValue, 1.0f, 0.0f, 255.0f ) )             { QuickRun( false ); }
        //if( ImGui::ListBox( "Type", &m_ThresholdType, ThresholdTypeNames, ThresholdTypeMax ) ) { QuickRun(); }

        if( ImGui::BeginCombo( "Type", ThresholdTypeNames[m_ThresholdType] ) )
        {
            for( int n = 0; n < ThresholdTypeMax; n++ )
            {
                bool is_selected = (n == m_ThresholdType);
                if( ImGui::Selectable( ThresholdTypeNames[n], is_selected ) )
                {
                    m_ThresholdType = n;
                    QuickRun( false );
                }
                if( is_selected )
                {
                    // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth() );
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVNode::Trigger( pEvent );

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        // Apply the threshold filter.
        cv::threshold( *pImage, m_Image, m_ThresholdValue, 255, m_ThresholdType );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, recursive );

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSON_AddNumberToObject( jNode, "m_ThresholdValue", m_ThresholdValue );
        cJSON_AddNumberToObject( jNode, "m_ThresholdType", m_ThresholdType );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
        cJSONExt_GetFloat( jNode, "m_ThresholdValue", &m_ThresholdValue );
        cJSONExt_GetInt( jNode, "m_ThresholdType", &m_ThresholdType );
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
    std::string m_SettingsString;
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

        if( ImGui::DragInt( "Window Size", &m_WindowSize, 1.0f, 1, 30 ) )          { QuickRun( false ); }
        if( ImGui::DragFloat( "Sigma Color", &m_SigmaColor, 1.0f, 0.0f, 255.0f ) ) { QuickRun( false ); }
        if( ImGui::DragFloat( "Sigma Space", &m_SigmaSpace, 1.0f, 0.0f, 255.0f ) ) { QuickRun( false ); }
        ImGui::Text( "Runtime: %f", m_LastProcessTime );

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, m_pNodeGraph->GetImageWidth() );
    }

    virtual bool Trigger(MyEvent* pEvent, bool recursive) override
    {
        //OpenCVNode::Trigger( pEvent );

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        cv::Mat* pImage = pNode->GetValueMat();

        // Apply the Bilateral filter.
        double timeBefore = MyTime_GetSystemTime();
        cv::bilateralFilter( *pImage, m_Image, m_WindowSize, m_SigmaColor, m_SigmaSpace );
        double timeAfter = MyTime_GetSystemTime();
        m_LastProcessTime = timeAfter - timeBefore;

        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, recursive );

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSON_AddNumberToObject( jNode, "m_WindowSize", m_WindowSize );
        cJSON_AddNumberToObject( jNode, "m_SigmaColor", m_SigmaColor );
        cJSON_AddNumberToObject( jNode, "m_SigmaSpace", m_SigmaSpace );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        MyNode::ImportFromJSONObject( jNode );
        cJSONExt_GetInt( jNode, "m_WindowSize", &m_WindowSize );
        cJSONExt_GetFloat( jNode, "m_SigmaColor", &m_SigmaColor );
        cJSONExt_GetFloat( jNode, "m_SigmaSpace", &m_SigmaSpace );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
    virtual const char* GetSettingsString() override
    {
        m_SettingsString = "-w" + std::to_string( m_WindowSize );
        m_SettingsString += "-c";
        PrintFloatBadlyWithPrecision( m_SettingsString, m_SigmaColor, 2 );
        m_SettingsString += "-s";
        PrintFloatBadlyWithPrecision( m_SettingsString, m_SigmaSpace, 2 );

        return m_SettingsString.c_str();
    }
};

#endif //__OpenCVNodes_H__
