//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodeGraphEditorCommands_H__
#define __OpenCVNodeGraphEditorCommands_H__

#include "OpenCVNodeGraph.h"

class OpenCVNodeGraph::OpenCVNode;

class EditorCommand_OpenCVNodeGraph_AddNode;
class EditorCommand_OpenCVNodeGraph_DeleteNodes;
class EditorCommand_OpenCVNodeGraph_CreateLink;
class EditorCommand_OpenCVNodeGraph_DeleteLink;

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_AddNode
//====================================================================================================

class EditorCommand_OpenCVNodeGraph_AddNode : public EditorCommand
{
protected:
    OpenCVNodeGraph* m_pNodeGraph;
    OpenCVNodeGraph::OpenCVNode* m_pNode;
    bool m_DeleteNodeWhenDestroyed;

public:
    EditorCommand_OpenCVNodeGraph_AddNode(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::OpenCVNode* pNode);
    virtual ~EditorCommand_OpenCVNodeGraph_AddNode();

    virtual void Do();
    virtual void Undo();
    virtual EditorCommand* Repeat();
};

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_DeleteNodes
//====================================================================================================

class EditorCommand_OpenCVNodeGraph_DeleteNodes : public EditorCommand
{
protected:
    OpenCVNodeGraph* m_pNodeGraph;
    std::vector<OpenCVNodeGraph::OpenCVNode*> m_pNodes;
    std::vector<OpenCVNodeGraph::OpenCVNodeLink> m_Links;
    bool m_DeleteNodesWhenDestroyed;

public:
    EditorCommand_OpenCVNodeGraph_DeleteNodes(OpenCVNodeGraph* pNodeGraph, ImVector<OpenCVNodeGraph::NodeID>& selectedNodeIDs);
    virtual ~EditorCommand_OpenCVNodeGraph_DeleteNodes();

    virtual void Do();
    virtual void Undo();
    virtual EditorCommand* Repeat();
};

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_CreateLink
//====================================================================================================

class EditorCommand_OpenCVNodeGraph_CreateLink : public EditorCommand
{
protected:
    OpenCVNodeGraph* m_pNodeGraph;
    OpenCVNodeGraph::OpenCVNodeLink m_NodeLink;
    int m_NodeLinkIndex;

public:
    EditorCommand_OpenCVNodeGraph_CreateLink(OpenCVNodeGraph* pNodeGraph, OpenCVNodeGraph::OpenCVNodeLink nodeLink);
    virtual ~EditorCommand_OpenCVNodeGraph_CreateLink();

    virtual void Do();
    virtual void Undo();
    virtual EditorCommand* Repeat();
};

//====================================================================================================
// EditorCommand_OpenCVNodeGraph_DeleteLink
//====================================================================================================

class EditorCommand_OpenCVNodeGraph_DeleteLink : public EditorCommand
{
protected:
    OpenCVNodeGraph* m_pNodeGraph;
    OpenCVNodeGraph::OpenCVNodeLink m_NodeLink;
    int m_NodeLinkIndex;
    bool m_DeleteLinkWhenDestroyed;

public:
    EditorCommand_OpenCVNodeGraph_DeleteLink(OpenCVNodeGraph* pNodeGraph, int nodeLinkIndex);
    virtual ~EditorCommand_OpenCVNodeGraph_DeleteLink();

    virtual void Do();
    virtual void Undo();
    virtual EditorCommand* Repeat();
};

#endif // __OpenCVNodeGraphEditorCommands_H__
