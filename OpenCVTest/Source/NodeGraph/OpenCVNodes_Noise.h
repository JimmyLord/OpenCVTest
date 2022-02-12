//
// Copyright (c) 2022 Jimmy Lord
//
#ifndef __OpenCVNodes_Noise_H__
#define __OpenCVNodes_Noise_H__

#include "OpenCVNodes_Base.h"
#include "Utility/Helpers.h"
#include "Graph/GraphTypes.h"

// OpenCV node types.
class OpenCVNode_Generate_SimplexNoise;

//====================================================================================================
// OpenCVNode_Generate_SimplexNoise
//====================================================================================================

class OpenCVNode_Generate_SimplexNoise : public OpenCVBaseNode
{
protected:
    cv::Mat m_Image;
    TextureDefinition* m_pTexture = nullptr;

    // Saved parameters.
    ivec2 m_ImageSize = ivec2( 512, 512 );
    bool m_UseFixedSeed = true;
    int m_Seed = 0;

    int m_NumOctaves = 1;
    float m_Frequency = 1/100.0f;
    vec2 m_Offset = vec2( 0.0f, 0.0f );
    bool m_Inverse = false;

public:
    OpenCVNode_Generate_SimplexNoise(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::NodeID id, const char* name, const Vector2& pos);
    ~OpenCVNode_Generate_SimplexNoise();

    DEFINE_NODE_TYPE( "Generate_SimplexNoise" );

    void GenerateNoise();

    virtual void DrawTitle() override;
    virtual bool DrawContents() override;
    virtual void TriggerGlobalRun() override;
    virtual bool Trigger(MyEvent* pEvent, TriggerFlags triggerFlags) override;

    virtual cJSON* ExportAsJSONObject() override;
    virtual void ImportFromJSONObject(cJSON* jNode) override;
    virtual std::string GetSettingsString() override;

    virtual cv::Mat* GetValueMat() override { return &m_Image; }
};

#endif //__OpenCVNodes_Noise_H__
