//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodes_Core_H__
#define __OpenCVNodes_Core_H__

#include "OpenCVNodeGraph.h"
#include "Utility/Helpers.h"
#include "Utility/VectorTypes.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"

#include "OpenCVNodes_Base.h"

class ComponentBase;

// OpenCV node types.
class OpenCVNode_File_Input;
class OpenCVNode_File_Output;
class OpenCVNode_Convert_Grayscale;
class OpenCVNode_Convert_Crop;
class OpenCVNode_Filter_Threshold;
class OpenCVNode_Filter_Bilateral;
class OpenCVNode_Filter_Morphological;

#define VSNAddVar ComponentBase::AddVariable_Base
#define VSNAddVarEnum ComponentBase::AddVariableEnum_Base

//====================================================================================================
// OpenCVNode_File_Input
//====================================================================================================
static const char* m_OpenCVNode_File_Input_InputLabels[] = { "None" };
static const char* m_OpenCVNode_File_Input_OutputLabels[] = { "Image Output" };

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

        m_InputTooltips  = m_OpenCVNode_File_Input_InputLabels;
        m_OutputTooltips = m_OpenCVNode_File_Input_OutputLabels;
    }

    ~OpenCVNode_File_Input()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "File_Input"; }
    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    void ChooseFile()
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

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVBaseNode::DrawTitle();
        }
        else
        {
            if( ImGui::Button( "..." ) )
            {
                ChooseFile();
            }
            ImGui::SameLine();
            ImGui::Text( "%s: %.14s", m_Name, m_Filename.c_str() );
            if( ImGui::IsItemHovered() && m_Filename.length() > 18 )
            {
                ImGui::BeginTooltip();
                ImGui::Text( "%s", m_Filename.c_str() );
                ImGui::EndTooltip();
            }
        }
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        if( ImGui::Button( "Choose File..." ) )
        {
            ChooseFile();
        }

        ImGui::Text( "File: %.22s", m_Filename.c_str() );
        if( ImGui::IsItemHovered() && m_Filename.length() > 22 )
        {
            ImGui::BeginTooltip();
            ImGui::Text( "%s", m_Filename.c_str() );
            ImGui::EndTooltip();
        }
        ImGui::Text( "Size: %dx%d", m_Image.cols, m_Image.rows );

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return false;
    }

    virtual void TriggerGlobalRun() override
    {
        //Trigger( nullptr, (TriggerFlags)(TriggerFlags::TF_Recursive | TriggerFlags::TF_TriggeredByParent) );
        Trigger( nullptr, TriggerFlags::TF_Recursive | TriggerFlags::TF_TriggeredByParent );
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Load the file from disk.
        m_Image = cv::imread( m_Filename.c_str() );
        m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

        // Trigger the output nodes.
        TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );

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
        OpenCVBaseNode::ImportFromJSONObject( jNode );
        cJSON* jObj = cJSON_GetObjectItem( jNode, "m_Filename" );
        if( jObj )
            m_Filename.assign( jObj->valuestring );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_File_Output
//====================================================================================================
static const char* m_OpenCVNode_File_Output_InputLabels[] = { "Image Input" };
static const char* m_OpenCVNode_File_Output_OutputLabels[] = { "None" };

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

        m_InputTooltips  = m_OpenCVNode_File_Output_InputLabels;
        m_OutputTooltips = m_OpenCVNode_File_Output_OutputLabels;
    }

    ~OpenCVNode_File_Output()
    {
    }

    const char* GetType() { return "File_Output"; }
    //virtual uint32 EmitLua(char* string, uint32 offset, uint32 bytesAllocated, uint32 tabDepth) override;

    virtual void DrawTitle() override
    {
        if( m_Expanded )
            OpenCVBaseNode::DrawTitle();
        else
            ImGui::Text( "%s: %s", m_Name, m_Filename.c_str() );
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        if( ImGui::Button( "Choose File..." ) )
        {
            const char* filename = FileSaveDialog( "Data\\", "png files\0*.png" );
            if( filename[0] != '\0' )
            {
                char path[MAX_PATH];
                strcpy_s( path, MAX_PATH, filename );
                const char* relativePath = GetRelativePath( path );

                m_Filename = relativePath;

                Trigger( nullptr, TriggerFlags::TF_None );
            }
        }

        ImGui::Text( "File: %s", m_Filename.c_str() );

        if( ImGui::Button( "Save" ) )
        {
            Save();
        }

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        return true;
    }

    void Save()
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, 0 ) );
        if( pNode == nullptr )
            return;
        cv::Mat outputImage = *pNode->GetValueMat();
        if( outputImage.empty() == true )
            return;

        Save( outputImage, m_Filename );
    }

    static void Save(cv::Mat& outputImage, std::string filename, OpenCVBaseNode* pNode = nullptr)
    {
        cv::Mat temp;
        int type = outputImage.type();
        if( type == CV_32F )
        {
            double min;
            double max;
            cv::minMaxLoc( outputImage, &min, &max );
            float scale = 255.0f / (float)(max - min);
            outputImage.convertTo( temp, CV_8UC1, scale );
            cv::cvtColor( temp, temp, cv::COLOR_GRAY2RGB );
            //applyColorMap( temp, temp, cv::COLORMAP_JET );
        }
        else
        {
            temp = outputImage;
        }

        std::string tempFilename = filename;

        // Append the setting string returned by the previous node.
        if( pNode )
        {
            std::string settingsString = pNode->GetSettingsString();
            if( settingsString.length() > 0 )
            {
                tempFilename += settingsString;
            }
        }

        // Append a .png.
        const char* relativePath = tempFilename.c_str();
        int len = (int)strlen( relativePath );
        if( len < 4 || strcmp( &relativePath[len-4], ".png" ) != 0 )
        {
            tempFilename += ".png";
        }

        // Write the file to disk.
        cv::imwrite( tempFilename.c_str(), temp );
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSON_AddStringToObject( jNode, "m_Filename", m_Filename.c_str() );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        OpenCVBaseNode::ImportFromJSONObject( jNode );
        cJSON* jObj = cJSON_GetObjectItem( jNode, "m_Filename" );
        if( jObj )
            m_Filename.assign( jObj->valuestring );
    }

    //virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Convert_Grayscale
//====================================================================================================
static const char* m_OpenCVNode_Convert_Grayscale_InputLabels[] = { "Image Input" };
static const char* m_OpenCVNode_Convert_Grayscale_OutputLabels[] = { "Image Output" };

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

        m_InputTooltips  = m_OpenCVNode_Convert_Grayscale_InputLabels;
        m_OutputTooltips = m_OpenCVNode_Convert_Grayscale_OutputLabels;
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
            OpenCVBaseNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            // Convert to Grayscale.
            cv::cvtColor( *pImage, m_Image, cv::COLOR_BGR2GRAY );
            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );
        }

        return false;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Convert_Crop
//====================================================================================================
static const char* m_OpenCVNode_Convert_Crop_InputLabels[] = { "Image Input" };
static const char* m_OpenCVNode_Convert_Crop_OutputLabels[] = { "Cropped Output" };

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

        m_InputTooltips  = m_OpenCVNode_Convert_Crop_InputLabels;
        m_OutputTooltips = m_OpenCVNode_Convert_Crop_OutputLabels;
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
            OpenCVBaseNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
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

            DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );
        }
        else
        {
            ImGui::Text( "No input image." );
        }

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            Validate( pImage );

            // Crop out a subregion of the image.
            //cv::cvtColor( *pImage, m_Image, cv::COLOR_BGR2GRAY );
            cv::Rect cropArea( m_TopLeft.x, m_TopLeft.y, m_Size.x, m_Size.y );
            m_Image = (*pImage)( cropArea );
            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );
        }

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
        OpenCVBaseNode::ImportFromJSONObject( jNode );
        cJSONExt_GetIntArray( jNode, "m_TopLeft", &m_TopLeft.x, 2 );
        cJSONExt_GetIntArray( jNode, "m_Size", &m_Size.x, 2 );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Filter_Threshold
//====================================================================================================
static const char* m_OpenCVNode_Filter_Threshold_InputLabels[] = { "Image Input" };
static const char* m_OpenCVNode_Filter_Threshold_OutputLabels[] = { "Image Output" };

class OpenCVNode_Filter_Threshold : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;
    float m_ThresholdValue;
    int m_ThresholdType;

    const char* ThresholdTypeNames[7] = 
    {
        "Binary",
        "Binary Inverse",
        "Truncate",
        "To Zero",
        "To Zero Inverse",
        "To One",
        "To One Inverse",
    };

    const int ThresholdTypeMax = 7;

public:
    OpenCVNode_Filter_Threshold(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        m_ThresholdValue = 0;
        m_ThresholdType = 0;
        //VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );

        m_InputTooltips  = m_OpenCVNode_Filter_Threshold_InputLabels;
        m_OutputTooltips = m_OpenCVNode_Filter_Threshold_OutputLabels;
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
            OpenCVBaseNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

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

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            // Apply the threshold filter.
            if( m_ThresholdType < 5 )
            {
                cv::threshold( *pImage, m_Image, m_ThresholdValue, 255, m_ThresholdType );
            }
            else
            {
                if( m_ThresholdType == 5 )
                {
                    cv::threshold( *pImage, m_Image, m_ThresholdValue, 255, cv::THRESH_TOZERO );
                    m_Image.setTo( 255, m_Image == 0 );
                }
                if( m_ThresholdType == 6 )
                {
                    cv::threshold( *pImage, m_Image, m_ThresholdValue, 255, cv::THRESH_TOZERO_INV );
                    m_Image.setTo( 255, m_Image == 0 );
                }
            }
            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );
        }

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
        OpenCVBaseNode::ImportFromJSONObject( jNode );
        cJSONExt_GetFloat( jNode, "m_ThresholdValue", &m_ThresholdValue );
        cJSONExt_GetInt( jNode, "m_ThresholdType", &m_ThresholdType );
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Filter_Bilateral
//====================================================================================================
static const char* m_OpenCVNode_Filter_Bilateral_InputLabels[] = { "Image Input" };
static const char* m_OpenCVNode_Filter_Bilateral_OutputLabels[] = { "Image Output" };

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

        m_InputTooltips  = m_OpenCVNode_Filter_Bilateral_InputLabels;
        m_OutputTooltips = m_OpenCVNode_Filter_Bilateral_OutputLabels;
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
            OpenCVBaseNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        if( ImGui::DragInt( "Window Size", &m_WindowSize, 1.0f, 1, 30 ) )          { QuickRun( false ); }
        if( ImGui::DragFloat( "Sigma Color", &m_SigmaColor, 1.0f, 0.0f, 255.0f ) ) { QuickRun( false ); }
        if( ImGui::DragFloat( "Sigma Space", &m_SigmaSpace, 1.0f, 0.0f, 255.0f ) ) { QuickRun( false ); }
        ImGui::Text( "Runtime: %f", m_LastProcessTime );

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            // Apply the Bilateral filter.
            double timeBefore = MyTime_GetSystemTime();
            cv::bilateralFilter( *pImage, m_Image, m_WindowSize, m_SigmaColor, m_SigmaSpace );
            double timeAfter = MyTime_GetSystemTime();
            m_LastProcessTime = timeAfter - timeBefore;

            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );
        }

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
        OpenCVBaseNode::ImportFromJSONObject( jNode );
        cJSONExt_GetInt( jNode, "m_WindowSize", &m_WindowSize );
        cJSONExt_GetFloat( jNode, "m_SigmaColor", &m_SigmaColor );
        cJSONExt_GetFloat( jNode, "m_SigmaSpace", &m_SigmaSpace );
    }

    virtual std::string GetSettingsString() override
    {
        std::string settingsString;

        settingsString = "-w" + std::to_string( m_WindowSize );
        settingsString += "-c"; PrintFloatBadlyWithPrecision( settingsString, m_SigmaColor, 2 );
        settingsString += "-s"; PrintFloatBadlyWithPrecision( settingsString, m_SigmaSpace, 2 );

        return settingsString;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

//====================================================================================================
// OpenCVNode_Filter_Morphological
//====================================================================================================
static const char* m_OpenCVNode_Filter_Morphological_InputLabels[] = { "Image Input" };
static const char* m_OpenCVNode_Filter_Morphological_OutputLabels[] = { "Image Output" };

class OpenCVNode_Filter_Morphological : public OpenCVBaseNode
{
public:
    enum class MorphType
    {
        Erode,
        Dilate,
        Open,
        Close,
        Gradient,
        TopHat,
        BlackHat,
        NumTypes,
    };

    inline static std::string MorphTypeNames[(int)MorphType::NumTypes]
    {
        "Erode",
        "Dilate",
        "Open",
        "Close",
        "Gradient",
        "TopHat",
        "BlackHat",
    };

    const int cvMorphTypes[(int)MorphType::NumTypes]
    {
        cv::MORPH_ERODE,
        cv::MORPH_DILATE,
        cv::MORPH_OPEN,
        cv::MORPH_CLOSE,
        cv::MORPH_GRADIENT,
        cv::MORPH_TOPHAT,
        cv::MORPH_BLACKHAT,
    };

    enum class MorphKernel
    {
        Rect,
        Cross,
        Ellipse,
        NumTypes,
    };

    inline static std::string MorphKernelNames[(int)MorphKernel::NumTypes]
    {
        "Rect",
        "Cross",
        "Ellipse",
    };

    const int cvMorphKernels[(int)MorphKernel::NumTypes]
    {
        cv::MORPH_RECT,
        cv::MORPH_CROSS,
        cv::MORPH_ELLIPSE,
    };

protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;

    int m_WindowSize;
    MorphType m_MorphType;
    MorphKernel m_MorphKernel;

public:
    OpenCVNode_Filter_Morphological(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos)
        : OpenCVBaseNode( pNodeGraph, id, name, pos, 1, 1 )
    {
        m_pTexture = nullptr;
        m_WindowSize = 3;
        m_MorphType = MorphType::Erode;
        m_MorphKernel = MorphKernel::Rect;
        //VSNAddVar( &m_VariablesList, "Color", ComponentVariableType_ColorByte, MyOffsetOf( this, &this->m_Color ), true, true, "", nullptr, nullptr, nullptr );

        m_InputTooltips  = m_OpenCVNode_Filter_Morphological_InputLabels;
        m_OutputTooltips = m_OpenCVNode_Filter_Morphological_OutputLabels;
    }

    ~OpenCVNode_Filter_Morphological()
    {
        SAFE_RELEASE( m_pTexture );
    }

    const char* GetType() { return "Filter_Morphological"; }

    virtual void DrawTitle() override
    {
        if( m_Expanded )
        {
            OpenCVBaseNode::DrawTitle();
        }
        else
        {
            ImGui::Text( "%s", m_Name );
        }
    }

    virtual bool DrawContents() override
    {
        OpenCVBaseNode::DrawContents();

        if( ImGui::DragInt( "Window Size", &m_WindowSize, 1.0f, 1, 30 ) )          { QuickRun( false ); }

        if( ImGui::BeginCombo( "Type", MorphTypeNames[(int)m_MorphType].c_str() ) )
        {
            for( int n = 0; n < (int)MorphType::NumTypes; n++ )
            {
                bool is_selected = (n == (int)m_MorphType);
                if( ImGui::Selectable( MorphTypeNames[n].c_str(), is_selected ) )
                {
                    m_MorphType = (MorphType)n;
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

        if( ImGui::BeginCombo( "Kernel Shape", MorphKernelNames[(int)m_MorphKernel].c_str() ) )
        {
            for( int n = 0; n < (int)MorphKernel::NumTypes; n++ )
            {
                bool is_selected = (n == (int)m_MorphKernel);
                if( ImGui::Selectable( MorphKernelNames[n].c_str(), is_selected ) )
                {
                    m_MorphKernel = (MorphKernel)n;
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

        ImGui::Text( "Runtime: %f", m_LastProcessTime );

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        // Get Image from input node.
        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            // Apply the Morphological filter.
            double timeBefore = MyTime_GetSystemTime();
            
            cv::Mat kernel = cv::getStructuringElement( cvMorphKernels[(int)m_MorphKernel],
                                                        cv::Size( 2*m_WindowSize+1, 2*m_WindowSize+1 ),
                                                        cv::Point( m_WindowSize, m_WindowSize ) ) * 255.0f;
            cv::morphologyEx( *pImage, m_Image, cvMorphTypes[(int)m_MorphType], kernel );

            double timeAfter = MyTime_GetSystemTime();
            m_LastProcessTime = timeAfter - timeBefore;

            m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

            // Trigger the output nodes.
            TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );
        }

        return false;
    }

    virtual cJSON* ExportAsJSONObject() override
    {
        cJSON* jNode = OpenCVBaseNode::ExportAsJSONObject();
        cJSON_AddNumberToObject( jNode, "m_WindowSize", m_WindowSize );
        cJSON_AddNumberToObject( jNode, "m_MorphType", (int)m_MorphType );
        cJSON_AddNumberToObject( jNode, "m_MorphKernel", (int)m_MorphKernel );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode) override
    {
        OpenCVBaseNode::ImportFromJSONObject( jNode );
        cJSONExt_GetInt( jNode, "m_WindowSize", &m_WindowSize );
        cJSONExt_GetInt( jNode, "m_MorphType", (int*)&m_MorphType );
        cJSONExt_GetInt( jNode, "m_MorphKernel", (int*)&m_MorphKernel );
    }

    virtual std::string GetSettingsString() override
    {
        std::string settingsString;

        settingsString = "-w" + std::to_string( m_WindowSize );
        //settingsString += "-c"; PrintFloatBadlyWithPrecision( settingsString, m_MorphType, 2 );
        //settingsString += "-s"; PrintFloatBadlyWithPrecision( settingsString, m_MorphKernel, 2 );

        return settingsString;
    }

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

#endif //__OpenCVNodes_Core_H__
