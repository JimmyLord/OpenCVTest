//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNodeTypeManager_H__
#define __OpenCVNodeTypeManager_H__

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

    virtual MyNodeGraph::MyNode* AddCreateNodeItemsToContextMenu(Vector2 pos, MyNodeGraph* pNodeGraph) override;
    virtual MyNodeGraph::MyNode* CreateNode(const char* typeName, Vector2 pos, MyNodeGraph* pNodeGraph) override;
};

#endif //__OpenCVNodeTypeManager_H__
