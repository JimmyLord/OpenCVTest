//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodes_Base_H__
#define __OpenCVNodes_Base_H__

#include "OpenCVNodeGraph.h"
#include "Utility/Helpers.h"
#include "Utility/VectorTypes.h"
#include "Libraries/Engine/MyEngine/SourceEditor/PlatformSpecific/FileOpenDialog.h"

class ComponentBase;

//====================================================================================================
// OpenCVBaseNode
//====================================================================================================

class OpenCVBaseNode : public MyNodeGraph::MyNode
{
    friend class OpenCVNodeGraph;

protected:
    OpenCVNodeGraph* m_pNodeGraph; // Hide the m_pNodeGraph in the MyNode class with a pointer to an OpenCVNodeGraph.
    double m_LastProcessTime;

    int m_KnownImageWidth;
    int m_ImageDisplayWidth;
    int m_MaxImageDisplayWidth;

public:
    OpenCVBaseNode(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
    : MyNodeGraph::MyNode( pNodeGraph, id, name, pos, inputsCount, outputsCount )
    {
        m_pNodeGraph = pNodeGraph;
        m_LastProcessTime = 0.0;

        m_KnownImageWidth = 0;
        m_ImageDisplayWidth = 0; // If 0, use max to display.
        m_MaxImageDisplayWidth = 0;
    }

    virtual cv::Mat* GetValueMat() { return nullptr; }
    virtual std::vector<vec2>* GetValuePointList() { return nullptr; }

    virtual void TriggerGlobalRun() {}

    void QuickRun(bool triggerJustThisNodeIfAutoRunIsOff)
    {
        if( m_pNodeGraph->GetAutoRun() ) 
        {
            // Trigger all nodes recursively.
            Trigger( nullptr, TriggerFlags::TF_Recursive );
        }
        else if( triggerJustThisNodeIfAutoRunIsOff )
        {
            // Trigger just this node.
            Trigger( nullptr, TriggerFlags::TF_None );
        }
    }

    enum TriggerFlags // operators defined below this class in global space.
    {
        TF_None                = 0x00,
        TF_Recursive           = 0x01,
        TF_TriggeredByParent   = 0x02,
        TF_All                 = 0xFF,
    };

    virtual bool Trigger(MyEvent* pEvent = nullptr) { return false; }
    virtual bool Trigger(MyEvent* pEvent = nullptr, TriggerFlags triggerFlags = TriggerFlags::TF_All) { return false; }

    virtual cJSON* ExportAsJSONObject()
    {
        cJSON* jNode = MyNodeGraph::MyNode::ExportAsJSONObject();
        cJSON_AddNumberToObject( jNode, "m_ImageDisplayWidth", m_ImageDisplayWidth );
        return jNode;
    }

    virtual void ImportFromJSONObject(cJSON* jNode)
    {
        MyNodeGraph::MyNode::ImportFromJSONObject( jNode );
        cJSONExt_GetInt( jNode, "m_ImageDisplayWidth", &m_ImageDisplayWidth );
    }

    //virtual std::string GetSettingsString() override;

    void AdjustKnownImageWidth(int actualWidth)
    {
        // Parent class values.
        if( m_KnownImageWidth != actualWidth )
        {
            m_KnownImageWidth = actualWidth;
            m_MaxImageDisplayWidth = actualWidth;

            if( m_ImageDisplayWidth == 0 )
            {
                m_ImageDisplayWidth = actualWidth;
            }
        }
    }

    int GetDisplayWidth()
    {
        int w = m_ImageDisplayWidth == 0 ? m_MaxImageDisplayWidth : m_ImageDisplayWidth;
        int outWidth = (int)(w * m_pNodeGraph->GetGlobalImageScale());

        if( outWidth == 0 )
            outWidth = 256;

        if( outWidth < 16 )
            outWidth = 16;

        return outWidth;
    }

    void TriggerOutputNodes(MyEvent* pEvent, bool recursive)
    {
        if( recursive )
        {
            int count = 0;
            while( OpenCVBaseNode* pNode = (OpenCVBaseNode*)m_pNodeGraph->FindNodeConnectedToOutput( m_ID, 0, count++ ) )
            {
                pNode->Trigger( nullptr, TriggerFlags::TF_All );
            }
        }
    }

    cv::Mat* GetInputImage(uint32 slotID)
    {
        // Get Image from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, slotID ) );
        if( pNode )
        {
            cv::Mat* pImage = pNode->GetValueMat();
            if( pImage->empty() == false )
                return pImage;
        }

        return nullptr;
    }

    std::vector<vec2>* GetInputPointList(uint32 slotID)
    {
        // Get a point list from input node.
        OpenCVBaseNode* pNode = static_cast<OpenCVBaseNode*>( m_pNodeGraph->FindNodeConnectedToInput( m_ID, slotID ) );
        if( pNode )
        {
            std::vector<vec2>* pPointList = pNode->GetValuePointList();
            if( pPointList != nullptr )
                return pPointList;
        }

        return nullptr;
    }
};

inline OpenCVBaseNode::TriggerFlags operator~(OpenCVBaseNode::TriggerFlags o)
{
    return (OpenCVBaseNode::TriggerFlags)( ~(int)o );
}

inline OpenCVBaseNode::TriggerFlags operator|(OpenCVBaseNode::TriggerFlags l, OpenCVBaseNode::TriggerFlags r)
{
    return (OpenCVBaseNode::TriggerFlags)( (int)l | (int)r );
}

inline void operator&=(OpenCVBaseNode::TriggerFlags& l, OpenCVBaseNode::TriggerFlags r)
{
    l = (OpenCVBaseNode::TriggerFlags)( (int)l & (int)r );
}

inline void operator|=(OpenCVBaseNode::TriggerFlags& l, OpenCVBaseNode::TriggerFlags r)
{
    l = (OpenCVBaseNode::TriggerFlags)( (int)l | (int)r );
}

//====================================================================================================
// OpenCVBaseNodeWithAutorun and OpenCVBaseFilter
//====================================================================================================

class OpenCVBaseFilter
{
protected:
    GameCore* m_pGameCore;

    // Working variables.
    cv::Mat& m_Dest;
    cv::Mat* m_pSource;

public:
    OpenCVBaseFilter(GameCore* pGameCore, cv::Mat& dest)
    : m_Dest( dest )
    {
        m_pGameCore = pGameCore;
        m_pSource = nullptr;
    }
    virtual ~OpenCVBaseFilter() {}

    virtual void Reset(cv::Mat* pSource, int numStagesToRun) = 0;
    virtual bool Step() = 0; // Returns true when finished.
    virtual void Output(int stage) = 0;
};

class OpenCVBaseNodeWithAutorun : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture;

    OpenCVBaseFilter* m_pFilter;

    bool m_Running;
    bool m_AutoRun;

    int m_MaxStages;
    int m_StagesToRun;
    std::string* m_StageNames;
    bool m_ResetOnStageChange;

    bool m_NeedsReset;
    int m_NumIterations;
    int m_PastIterations;
    cv::Size m_OldImageSize;

public:
    OpenCVBaseNodeWithAutorun(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
    : OpenCVBaseNode( pNodeGraph, id, name, pos, inputsCount, outputsCount )
    {
        m_pTexture = nullptr;

        m_pFilter = nullptr;

        m_Running = true;
        m_AutoRun = true;

        m_MaxStages = 0;
        m_StagesToRun = m_MaxStages;
        m_StageNames = nullptr;
        m_ResetOnStageChange = false;

        m_NeedsReset = true;
        m_NumIterations = 1;
        m_PastIterations = 0;
        m_OldImageSize = cv::Size(0,0);
    }

    void Init(OpenCVBaseFilter* pFilter, int maxStages, std::string* stageNames, bool resetOnStageChange)
    {
        m_pFilter = pFilter;

        m_MaxStages = maxStages;
        m_StagesToRun = maxStages;
        m_StageNames = stageNames;
        m_ResetOnStageChange = resetOnStageChange;
    }

    virtual bool HandleInput(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure)
    {
        // Hack, make tilde key the same as 0.
        if( keyCode == 192 )
            keyCode = '0';

        if( keyAction == GCBA_Down && keyCode >= '0' && keyCode < '0'+m_MaxStages )
        {
            m_StagesToRun = keyCode - '0';
            if( m_ResetOnStageChange )
                m_NeedsReset = true;

            QuickRun( true );
            return true;
        }

        return false;
    }

    virtual bool DrawContents() override
    {
        bool changed = false;

        if( ImGui::SliderInt( "Stage:", &m_StagesToRun, 0, m_MaxStages-1 ) )
        {
            changed = true;
            if( m_ResetOnStageChange )
                m_NeedsReset = true;
        }
        ImGui::SameLine();
        ImGui::Text( "%s", m_StageNames[m_StagesToRun].c_str() );

        if( InnerDrawContents() )
        {
            changed = true;
        }

        ImGui::Checkbox( "AutoRun", &m_AutoRun );
        ImGui::SameLine();
        if( ImGui::Button( "Reset" ) ) { m_NeedsReset = true; m_Running = true; QuickRun( false ); }

        if( m_Running )
        {
            ImGui::Text( "Status: Running: %d", m_NumIterations );
            ImGui::SameLine();
            if( ImGui::Button( "Step" ) ) { m_NumIterations++; QuickRun( false ); }
        }
        else
        {
            ImGui::Text( "Status: Done: %d", m_NumIterations );
        }

        ImGui::Text( "Runtime: %f", m_LastProcessTime );

        if( changed )
        {
            QuickRun( false );
        }
        else if( m_Image.empty() == false && m_Running && m_AutoRun )
        {
            m_NumIterations++;
            QuickRun( false );
        }

        DisplayOpenCVMatAndTexture( &m_Image, m_pTexture, GetDisplayWidth(), m_pNodeGraph->GetHoverPixelsToShow() );

        return false;
    }

    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override
    {
        //OpenCVBaseNode::Trigger( pEvent );

        cv::Mat* pImage = GetInputImage( 0 );

        if( pImage )
        {
            if( pImage->size() != m_OldImageSize )
            {
                m_NeedsReset = true;
            }
            if( m_NumIterations < m_PastIterations )
            {
                m_NeedsReset = true;
            }

            int type = pImage->type();
            if( type == CV_8UC3 )
            {
                // Apply the MySLIC filter.
                double timeBefore = MyTime_GetSystemTime();
                if( m_NeedsReset )
                {
                    m_Running = true;
                    m_NeedsReset = false;
                    m_PastIterations = 0;
                    m_NumIterations = 1;
                    m_OldImageSize = pImage->size();
                    Reset( pImage );
                }

                if( m_PastIterations < m_NumIterations )
                {
                    m_PastIterations++;
                    if( Step() == true )
                    {
                        m_Running = false;
                    }
                }
                double timeAfter = MyTime_GetSystemTime();
                m_LastProcessTime = timeAfter - timeBefore;

                Output( m_StagesToRun );

                m_pTexture = CreateOrUpdateTextureDefinitionFromOpenCVMat( &m_Image, m_pTexture );

                // Trigger the ouput nodes.
                TriggerOutputNodes( pEvent, triggerFlags & TriggerFlags::TF_Recursive );
            }
        }

        return false;
    }

    virtual bool InnerDrawContents() = 0;
    
    virtual void Reset(cv::Mat* pImage)
    {
        m_pFilter->Reset( pImage, m_StagesToRun );
    }

    virtual bool Step()
    {
        return m_pFilter->Step();
    }

    virtual void Output(int stages)
    {
        m_pFilter->Output( stages );
    }
};

#endif //__OpenCVNodes_Base_H__
