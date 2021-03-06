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
    float m_HoverPixelsToShow;

protected:
    // File IO.
    virtual const char* GetFileExtension() override { return ".opencvnodegraph"; };
    virtual const char* GetDefaultDataFolder() override { return "Data\\NodeGraphs\\"; };
    virtual const char* GetDefaultFileSaveFilter() override { return "OpenCV NodeGraph Files=*.opencvnodegraph"; };

public:
    OpenCVNodeGraph(EngineCore* pEngineCore, OpenCVNodeTypeManager* pNodeTypeManager);
    virtual ~OpenCVNodeGraph();

    // Overrides.
    virtual bool HandleInput(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure) override; // from EditorDocument.

    virtual void Save() override; // from EditorDocument.
    //virtual void Load() override; // from EditorDocument.
    virtual void Run() override; // from EditorDocument.

	virtual void ImportFromJSONObject(cJSON* jNodeGraph) override; // from MyNodeGraph.

    virtual void AddItemsAboveNodeGraphWindow() override;
    virtual void AddAdditionalItemsToNodeContextMenu(MyNodeGraph::MyNode* pNode) override;

    // Getters.
    float GetImageWidth() { return m_ImageWidth; }
    void SetImageWidth(float height) { m_ImageWidth = height; }
    bool GetAutoRun() { return m_AutoRun; }
    float GetHoverPixelsToShow() { return m_HoverPixelsToShow; }
};

//====================================================================================================

class OpenCVBaseNodeTypeManager : public MyNodeTypeManager
{
protected:

public:
    OpenCVBaseNodeTypeManager() {}
};

#endif //__OpenCVNodeGraph_H__
