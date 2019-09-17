//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodeGraph_H__
#define __OpenCVNodeGraph_H__

#include "Libraries/Engine/Libraries/imgui/imgui.h"
#include "Libraries/Engine/Libraries/imgui/imgui_internal.h"

class OpenCVNodeTypeManager;

class OpenCVNodeGraph : public EditorDocument
{
public:
    class OpenCVNode;
    class OpenCVNodeLink;

    friend class OpenCVNode;
    friend class OpenCVNodeLink;
    friend class EditorCommand_OpenCVNodeGraph_AddNode;
    friend class EditorCommand_OpenCVNodeGraph_DeleteNodes;
    friend class EditorCommand_OpenCVNodeGraph_CreateLink;
    friend class EditorCommand_OpenCVNodeGraph_DeleteLink;

    typedef uint32 NodeID;

protected:
    typedef uint32 SlotID;

    static const NodeID NodeID_Undefined = UINT_MAX;
    static const NodeID SlotID_Undefined = UINT_MAX;

    enum SlotType
    {
        SlotType_Input,
        SlotType_Output,
        SlotType_Undefined,
    };

    class MouseNodeLinkStartPoint
    {
    public:
        NodeID m_NodeID;
        SlotID m_SlotID;
        SlotType m_SlotType;
        ImU32 m_Color;

        void Set(NodeID nodeID, SlotID slotID, SlotType slotType) { m_NodeID = nodeID; m_SlotID = slotID; m_SlotType = slotType; }
        void Clear() { Set( NodeID_Undefined, SlotID_Undefined, SlotType_Undefined ); }
        bool InUse() { return m_NodeID != NodeID_Undefined; }
    };

    class OpenCVNodeGraph::OpenCVNodeLink
    {
    public:
        NodeID m_OutputNodeID;
        SlotID m_OutputSlotID;
        NodeID m_InputNodeID;
        SlotID m_InputSlotID;

        OpenCVNodeLink()
        {
            m_OutputNodeID = NodeID_Undefined;
            m_OutputSlotID = SlotID_Undefined;
            m_InputNodeID = NodeID_Undefined;
            m_InputSlotID = SlotID_Undefined;
        }
        OpenCVNodeLink(NodeID outputNodeID, SlotID outputSlotID, NodeID inputNodeID, SlotID inputSlotID)
        {
            m_OutputNodeID = outputNodeID;
            m_OutputSlotID = outputSlotID;
            m_InputNodeID = inputNodeID;
            m_InputSlotID = inputSlotID;
        }
    };

protected:
    OpenCVNodeTypeManager* m_pNodeTypeManager;

    NodeID m_NextNodeID;

    std::vector<OpenCVNode*> m_Nodes;
    ImVector<OpenCVNodeLink> m_Links;
    ImVector<NodeID> m_SelectedNodeIDs;
    int m_SelectedNodeLinkIndex;

    Vector2 m_ScrollOffset;
    bool m_GridVisible;
    float m_ImageSize;
    bool m_AutoRun;

    MouseNodeLinkStartPoint m_MouseNodeLinkStartPoint;

    bool m_ShowingLuaString;
    const char* m_pLuaString;

protected:
    void Clear();

    void DrawGrid(Vector2 offset);
    int FindNodeIndexByID(NodeID nodeID);
    bool IsNodeSlotInUse(NodeID nodeID, SlotID slotID, SlotType slotType);
    void SetExpandedForAllSelectedNodes(bool expand);

    virtual void Save() override; // from EditorDocument.
    virtual void Load() override; // from EditorDocument.

    const char* ExportAsLuaString();
    cJSON* ExportAsJSONObject();
    void ImportFromJSONObject(cJSON* jNodeGraph);

    // Used by EditorCommand_NodeGraph_AddNode and EditorCommand_NodeGraph_DeleteNodes for undo/redo.
    void AddExistingNode(OpenCVNode* pNode);
    void RemoveExistingNode(OpenCVNode* pNode);

    // File IO.
    virtual const char* GetFileExtension() { return ".opencvnodegraph"; };
    virtual const char* GetDefaultDataFolder() { return "Data\\NodeGraphs\\"; };
    virtual const char* GetDefaultFileSaveFilter() { return "OpenCV NodeGraph Files=*.opencvnodegraph"; };

public:
    OpenCVNodeGraph(EngineCore* pEngineCore, OpenCVNodeTypeManager* pNodeTypeManager);
    virtual ~OpenCVNodeGraph();

    // Returns true if in focus.
    virtual void Update() override;

    // Getters.
    OpenCVNodeLink* FindLinkConnectedToInput(NodeID nodeID, SlotID slotID, int resultIndex = 0);
    OpenCVNodeLink* FindLinkConnectedToOutput(NodeID nodeID, SlotID slotID, int resultIndex = 0);

    OpenCVNode* FindNodeConnectedToInput(NodeID nodeID, SlotID slotID, int resultIndex = 0);
    OpenCVNode* FindNodeConnectedToOutput(NodeID nodeID, SlotID slotID, int resultIndex = 0);

    NodeID GetNextNodeIDAndIncrement();

    float GetImageSize() { return m_ImageSize; }
    bool GetAutoRun() { return m_AutoRun; }
};

//====================================================================================================

class OpenCVBaseNodeTypeManager
{
protected:
    OpenCVNodeGraph* m_pNodeGraph;

public:
    OpenCVBaseNodeTypeManager() {}

    virtual OpenCVNodeGraph::OpenCVNode* AddCreateNodeItemsToContextMenu(Vector2 pos, OpenCVNodeGraph* pNodeGraph) = 0;
    virtual OpenCVNodeGraph::OpenCVNode* CreateNode(const char* typeName, Vector2 pos, OpenCVNodeGraph* pNodeGraph) = 0;
};

#endif //__OpenCVNodeGraph_H__
