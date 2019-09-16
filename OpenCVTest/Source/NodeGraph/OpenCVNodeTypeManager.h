//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __VisualScriptNodeTypeManager_H__
#define __VisualScriptNodeTypeManager_H__

#include "OpenCVNode.h"
#include "OpenCVNodeGraph.h"

class ComponentBase;

// Visual Script node types.
class VisualScriptNode_Value_Float;
class VisualScriptNode_Value_Color;
class VisualScriptNode_Value_GameObject;
class VisualScriptNode_Value_Component;
class VisualScriptNode_MathOp_Add;
class VisualScriptNode_Condition_GreaterEqual;
class VisualScriptNode_Condition_Keyboard;
class VisualScriptNode_Event_Keyboard;
class VisualScriptNode_Disable_GameObject;

//====================================================================================================
// VisualScriptNodeTypeManager
//====================================================================================================

class VisualScriptNodeTypeManager : public OpenCVNodeTypeManager
{
protected:

public:
    VisualScriptNodeTypeManager();

    virtual OpenCVNodeGraph::OpenCVNode* AddCreateNodeItemsToContextMenu(Vector2 pos, OpenCVNodeGraph* pNodeGraph) override;
    virtual OpenCVNodeGraph::OpenCVNode* CreateNode(const char* typeName, Vector2 pos, OpenCVNodeGraph* pNodeGraph) override;
};

#endif //__VisualScriptNodeTypeManager_H__
