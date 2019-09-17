//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodeTypeManager_H__
#define __OpenCVNodeTypeManager_H__

#include "OpenCVNode.h"
#include "OpenCVNodeGraph.h"

class ComponentBase;

//====================================================================================================
// OpenCVNodeTypeManager
//====================================================================================================

class OpenCVNodeTypeManager : public OpenCVBaseNodeTypeManager
{
protected:

public:
    OpenCVNodeTypeManager();

    virtual OpenCVNodeGraph::OpenCVNode* AddCreateNodeItemsToContextMenu(Vector2 pos, OpenCVNodeGraph* pNodeGraph) override;
    virtual OpenCVNodeGraph::OpenCVNode* CreateNode(const char* typeName, Vector2 pos, OpenCVNodeGraph* pNodeGraph) override;
};

#endif //__OpenCVNodeTypeManager_H__
