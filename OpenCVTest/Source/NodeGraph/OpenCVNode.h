//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNode_H__
#define __OpenCVNode_H__

#include "OpenCVNodeGraph.h"

class OpenCVNodeGraph::OpenCVNode : public ComponentVariableCallbackInterface
{
public:
    OpenCVNodeGraph* m_pNodeGraph;

    NodeID m_ID;
    char m_Name[32];
    Vector2 m_Pos;
    uint32 m_InputsCount;
    uint32 m_OutputsCount;
    bool m_Expanded;

    // Node properties.
    TCPPListHead<ComponentVariable*> m_VariablesList;

    // Temp values, no need to save.
    Vector2 m_Size;
    float m_TitleWidth;

public:
    OpenCVNode(OpenCVNodeGraph* pNodeGraph, int id, const char* name, const Vector2& pos, int inputsCount, int outputsCount);
    virtual ~OpenCVNode();

    virtual const char* GetType() = 0;

    OpenCVNodeGraph* GetNodeGraph();

    uint32 OpenCVNode::GetInputSlotCount() const;
    uint32 OpenCVNode::GetOutputSlotCount() const;
    ImVec2 GetInputSlotPos(SlotID slotID) const;
    ImVec2 GetOutputSlotPos(SlotID slotID) const;
    NodeID GetID() { return m_ID; }

    void Draw(ImDrawList* pDrawList, Vector2 offset, bool isSelected, MouseNodeLinkStartPoint* pMouseNodeLink);
    void HandleNodeSlot(ImDrawList* pDrawList, Vector2 slotPos, NodeID nodeID, SlotID slotID, SlotType slotType, MouseNodeLinkStartPoint* pMouseNodeLink);
    bool HandleNodeLinkCreation(Vector2 slotPos, NodeID nodeID, SlotID slotID, SlotType slotType, MouseNodeLinkStartPoint* pMouseNodeLink);

    virtual void DrawTitle();
    virtual void DrawContents();

    virtual bool Trigger(MyEvent* pEvent = nullptr) { return false; }

    virtual uint32 ExportAsLuaVariablesString(char* string, uint32 offset, uint32 bytesAllocated) { return 0; }
    virtual uint32 ExportAsLuaString(char* string, uint32 offset, uint32 bytesAllocated) { return 0; }
    virtual cJSON* ExportAsJSONObject();
    virtual void ImportFromJSONObject(cJSON* jNode);
};

#endif //__OpenCVNode_H__
