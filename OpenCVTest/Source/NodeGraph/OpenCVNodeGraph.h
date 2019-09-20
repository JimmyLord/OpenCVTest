//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodeGraph_H__
#define __OpenCVNodeGraph_H__

#include "Libraries/Engine/Libraries/imgui/imgui.h"
#include "Libraries/Engine/Libraries/imgui/imgui_internal.h"

class OpenCVNodeTypeManager;

class OpenCVNodeGraph : public MyNodeGraph
{
public:
    class OpenCVNode;

protected:
    float m_ImageWidth;
    bool m_AutoRun;

protected:
    // File IO.
    virtual const char* GetFileExtension() override { return ".opencvnodegraph"; };
    virtual const char* GetDefaultDataFolder() override { return "Data\\NodeGraphs\\"; };
    virtual const char* GetDefaultFileSaveFilter() override { return "OpenCV NodeGraph Files=*.opencvnodegraph"; };

public:
    OpenCVNodeGraph(EngineCore* pEngineCore, OpenCVNodeTypeManager* pNodeTypeManager);
    virtual ~OpenCVNodeGraph();

    // Overrides.
    virtual void AddItemsAboveNodeGraphWindow() override;
    virtual void AddAdditionalItemsToNodeContextMenu(MyNodeGraph::MyNode* pNode) override;

    // Getters.
    float GetImageWidth() { return m_ImageWidth; }
    void SetImageWidth(float height) { m_ImageWidth = height; }
    bool GetAutoRun() { return m_AutoRun; }
};

//====================================================================================================

class OpenCVBaseNodeTypeManager : public MyNodeTypeManager
{
protected:

public:
    OpenCVBaseNodeTypeManager() {}
};

#endif //__OpenCVNodeGraph_H__
