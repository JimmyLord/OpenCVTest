//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodeGraph_H__
#define __OpenCVNodeGraph_H__

#include "Libraries/Engine/Libraries/imgui/imgui.h"
#include "Libraries/Engine/Libraries/imgui/imgui_internal.h"
#include "Utility/Helpers.h"

class OpenCVNodeTypeManager;

class OpenCVNodeGraph : public MyNodeGraph
{
public:
    class OpenCVNode;

protected:
    float m_GlobalImageScale;
    bool m_AutoRun;
    float m_HoverPixelsToShow;
    colorPalette m_Palette;

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
    float GetGlobalImageScale() { return m_GlobalImageScale; }
    void SetGlobalImageScale(float scale) { m_GlobalImageScale = scale; }
    bool GetAutoRun() { return m_AutoRun; }
    float GetHoverPixelsToShow() { return m_HoverPixelsToShow; }
    colorPalette& GetPalette() { return m_Palette; }
};

//====================================================================================================

class OpenCVBaseNodeTypeManager : public MyNodeTypeManager
{
protected:

public:
    OpenCVBaseNodeTypeManager() {}
};

#endif //__OpenCVNodeGraph_H__
