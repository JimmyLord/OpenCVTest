//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#include "OpenCVPCH.h"
#include "OpenCVNodeGraph.h"
#include "OpenCVNode.h"

OpenCVNodeGraph::OpenCVNode::OpenCVNode(OpenCVNodeGraph* pNodeGraph, int id, const char* name, const Vector2& pos, int inputsCount, int outputsCount)
: MyNodeGraph::MyNode( pNodeGraph, id, name, pos, inputsCount, outputsCount)
{
    m_pNodeGraph = pNodeGraph;
}

OpenCVNodeGraph::OpenCVNode::~OpenCVNode()
{
}
