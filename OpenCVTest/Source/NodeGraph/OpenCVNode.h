//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __OpenCVNode_H__
#define __OpenCVNode_H__

#include "OpenCVNodeGraph.h"

class OpenCVNodeGraph::OpenCVNode : public MyNodeGraph::MyNode
{
protected:
    OpenCVNodeGraph* m_pNodeGraph; // Hide the m_pNodeGraph in the MyNode class with a pointer to an OpenCVNodeGraph.

public:
    OpenCVNode(OpenCVNodeGraph* pNodeGraph, int id, const char* name, const Vector2& pos, int inputsCount, int outputsCount);
    virtual ~OpenCVNode();
};

#endif //__OpenCVNode_H__
